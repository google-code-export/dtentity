/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <dtEntityWrappers/scriptcomponent.h>


#include <dtEntity/core.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/inputinterface.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/systemmessages.h>
#include <dtEntityWrappers/componentwrapper.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/jsproperty.h>
#include <dtEntityWrappers/messages.h>
#include <dtEntityWrappers/screenwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>


#include <v8.h>
#include <v8-debug.h>


#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <iostream>
#include <sstream>

using namespace v8;

namespace dtEntityWrappers
{
   
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   void GCStartCallback(GCType type, GCCallbackFlags flags)
   {
      //LOG_DEBUG("V8 garbage collection started");
   };

   ////////////////////////////////////////////////////////////////////////////
   void GCEndCallback(GCType type, GCCallbackFlags flags)
   {
      //LOG_DEBUG("V8 garbage collection ended");
      /*HeapStatistics stats;
      V8::GetHeapStatistics(&stats);
      std::cout << "Total: " << stats.total_heap_size() << " Used: " << stats.used_heap_size() <<"\n";
      */
   };

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ScriptSystem::TYPE(dtEntity::SID("Script")); 
   const dtEntity::StringId ScriptSystem::ScriptsId(dtEntity::SID("Scripts"));
   const dtEntity::StringId ScriptSystem::DebugPortId(dtEntity::SID("DebugPort"));
   const dtEntity::StringId ScriptSystem::DebugEnabledId(dtEntity::SID("DebugEnabled"));

   ScriptSystem::ScriptSystem(dtEntity::EntityManager& em)
      : dtEntity::EntitySystem(em)
      , mDebugPortOpened(false)
   {      

      V8::Initialize();
      Register(ScriptsId, &mScripts);
      Register(DebugPortId, &mDebugPort);
      Register(DebugEnabledId, &mDebugEnabled);
      mDebugPort.Set(9222);
      
      //V8::AddGCPrologueCallback(GCStartCallback);
      //V8::AddGCEpilogueCallback(GCEndCallback);

      mSceneLoadedFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnSceneLoaded);
      GetEntityManager().RegisterForMessages(dtEntity::SceneLoadedMessage::TYPE, mSceneLoadedFunctor, "ScriptSystem::OnSceneLoaded");

      mTickFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::Tick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor, "ScriptSystem::Tick");

      mLoadScriptFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnLoadScript);
      em.RegisterForMessages(ExecuteScriptMessage::TYPE, mLoadScriptFunctor, "ScriptSystem::OnLoadScript");

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      //mpEntityIdString = new Persistent<String>;
      //mpEntityIdString->Reset(isolate, String::NewFromUtf8(isolate, "__entityid__"));
      mpEntityIdString = new RefPersistent<v8::String>(isolate, String::NewFromUtf8(isolate, "__entityid__"));
      //mpPropertyNamesString = new Persistent<String>;
      //mpPropertyNamesString->Reset(isolate, String::NewFromUtf8(isolate, "__propertynames__"));
      mpPropertyNamesString = new RefPersistent<v8::String>(isolate, String::NewFromUtf8(isolate, "__propertynames__"));
   }  

   ////////////////////////////////////////////////////////////////////////////
   ScriptSystem::~ScriptSystem()
   {
      //V8::RemoveGCPrologueCallback(GCStartCallback);
      //V8::RemoveGCEpilogueCallback(GCEndCallback);

      // RefPersistent will reset automatically when they get destroyed by the map
      /*
      for(ComponentMap::iterator i = mComponentMap.begin(); i != mComponentMap.end(); ++i)
      {
         // reset the persistent and delete the persistent pointer itself
         i->second->Reset();
         delete i->second;
      }
      */

      // same auto-cleanup goes for the template map

      mComponentMap.clear();      
      mTemplateMap.clear();
      

      GetEntityManager().UnregisterForMessages(dtEntity::SceneLoadedMessage::TYPE, mSceneLoadedFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
      GetEntityManager().UnregisterForMessages(ExecuteScriptMessage::TYPE, mLoadScriptFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::SetupContext()
   {
      Isolate* isolate = Isolate::GetCurrent();

      // create the pointers
      if(!mpGlobalContext.valid())
         mpGlobalContext = new RefPersistent<Context>;

      if(!mpGlobalTickFunction.valid())
         mpGlobalTickFunction = new RefPersistent<Function>;

      // handle scope
      HandleScope handle_scope(isolate);      

      // reset the global context if not empty
      if(!mpGlobalContext->GetPersistent().IsEmpty())
      {
         mpGlobalContext->GetPersistent().Reset();
      }

      // create a template for the global object
      Handle<ObjectTemplate> global = ObjectTemplate::New();

      // create persistent global context
      Local<Context> context = Context::New(isolate, NULL, global);
      mpGlobalContext->GetPersistent().Reset(isolate, context);

      // store pointer to script system into isolate data field 0 to have it globally available in javascript
      isolate->SetData(0, this);

      RegisterGlobalFunctions(this, context);
      RegisterPropertyFunctions(this, context);

      InitializeAllWrappers(GetEntityManager());

      Context::Scope context_scope(context);
      Handle<FunctionTemplate> tmplt = FunctionTemplate::New(isolate);
      tmplt->InstanceTemplate()->SetInternalFieldCount(2);

      tmplt->SetClassName(String::NewFromUtf8(isolate, "ScriptSystem"));

      context->Global()->Set(String::NewFromUtf8(isolate, "Screen"), WrapScreen(this));

      dtEntity::InputInterface* ipiface = dtEntity::GetInputInterface();
      if(ipiface)
      {
         context->Global()->Set(String::NewFromUtf8(isolate, "Input"), WrapInputInterface(GetGlobalContext(), ipiface));
         context->Global()->Set(String::NewFromUtf8(isolate, "Axis"), WrapAxes(ipiface));
         context->Global()->Set(String::NewFromUtf8(isolate, "Key"), WrapKeys(ipiface));
      }

      context->Global()->Set(String::NewFromUtf8(isolate, "TouchPhase"), WrapTouchPhases());
      context->Global()->Set(String::NewFromUtf8(isolate, "Priority"), WrapPriorities());
      context->Global()->Set(String::NewFromUtf8(isolate, "Order"), WrapPriorities());

   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::Finished()
   {
      BaseClass::Finished();
      if(mDebugEnabled.Get() && !mDebugPortOpened)
      {
         mDebugPortOpened = true;
         unsigned int dbgport = mDebugPort.Get();
         std::string flags = "--debugger-agent";
         v8::V8::SetFlagsFromString(flags.c_str(), flags.length());
         v8::Debug::EnableAgent("DtScript", dbgport);
         LOG_DEBUG("Enabling JavaScript debugging on port " << dbgport);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnAddedToEntityManager(dtEntity::EntityManager& em)
   {
      SetupContext();      
      em.AddDeletedCallback(this);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
      em.RemoveDeletedCallback(this);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnSceneLoaded(const dtEntity::Message& msg)
   {
      dtEntity::PropertyArray scripts = mScripts.Get();
      dtEntity::PropertyArray::const_iterator i;
      for(i = scripts.begin(); i != scripts.end(); ++i)
      {
         std::string script = (*i)->StringValue();
         v8::HandleScope scope(Isolate::GetCurrent());
         ExecuteFile(script);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnLoadScript(const dtEntity::Message& m)
   {
      const ExecuteScriptMessage& msg = static_cast<const ExecuteScriptMessage&>(m);

      HandleScope scope(Isolate::GetCurrent());
      Handle<Context> context = GetGlobalContext();      
      Context::Scope context_scope(context);
      
      if(msg.GetIncludeOnce())
      {
         ExecuteFileOnce(msg.GetPath());
      }
      else
      {
         ExecuteFile(msg.GetPath());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Context> ScriptSystem::GetGlobalContext()
   {  
      //return v8::Handle<v8::Context>::New(v8::Isolate::GetCurrent(), *mpGlobalContext);
      // return local handle from the persistent instance
      return mpGlobalContext->GetLocal();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::FetchGlobalTickFunction()
   {
      HandleScope scope(Isolate::GetCurrent());
      Handle<Context> context = GetGlobalContext();

      mpGlobalTickFunction->GetPersistent().Reset();

      Local<String> funcname = String::NewFromUtf8(v8::Isolate::GetCurrent(), "__executeTimeOuts");      
      if(context->Global()->Has(funcname))
      {
         Handle<Value> func = context->Global()->Get(funcname);
         if(!func.IsEmpty())
         {
            Handle<Function> f = Handle<Function>::Cast(func);
            if(!f.IsEmpty())
            {
               mpGlobalTickFunction->GetPersistent().Reset(Isolate::GetCurrent(), f);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::Tick(const dtEntity::Message& m)
   {
      if(mpGlobalTickFunction->GetPersistent().IsEmpty())
         return;

      Isolate* isolate = Isolate::GetCurrent();

      HandleScope scope(isolate);  
      Context::Scope context_scope(GetGlobalContext());

      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      TryCatch try_catch;
      Handle<Value> argv[3] = {
         Number::New(isolate, msg.GetDeltaSimTime()),
         Number::New(isolate, msg.GetSimulationTime()),
         Uint32::New(isolate, osg::Timer::instance()->time_m())
      };

      //Local<Function> globalTick = Local<Function>::New(isolate, *mpGlobalTickFunction);
      Local<Function> globalTick = mpGlobalTickFunction->GetLocal(isolate);
      Handle<Value> ret = globalTick->Call(globalTick, 3, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Script> ScriptSystem::GetScriptFromFile(const std::string& path)
   {
     
      std::string code;
      bool success = GetFileContents(path, code);
      if(!success)
      {
         LOG_ERROR("Could not load script file from " + path);
         return Handle<Script>();
      }

      EscapableHandleScope handle_scope(Isolate::GetCurrent());
      Context::Scope context_scope(GetGlobalContext());
      TryCatch try_catch;
      Local<Script> compiled_script = Script::Compile(ToJSString(code), ToJSString(path));

      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
         return Handle<Script>();
      }

      return handle_scope.Escape(compiled_script);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ScriptSystem::ExecuteJS(const std::string& code, const std::string& path)
   {
       // Init JavaScript context
      EscapableHandleScope handle_scope(Isolate::GetCurrent());
      Context::Scope context_scope(GetGlobalContext());

      // We're just about to compile the script; set up an error handler to
      // catch any exceptions the script might throw.
      TryCatch try_catch;

      // Compile the source code.
      Local<Script> compiled_script = Script::Compile(ToJSString(code), ToJSString(path));

      // if an exception occured
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
   //      return try_catch;
      }

      // Run the script!
      Local<Value> ret = compiled_script->Run();
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
   //      return try_catch;
      }

      FetchGlobalTickFunction();

      return handle_scope.Escape(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Local<Value> ScriptSystem::ExecuteFile(const std::string& path)
   {
      EscapableHandleScope handle_scope(Isolate::GetCurrent());

      Handle<Script> script = GetScriptFromFile(path);

      if(!script.IsEmpty())
      {
         v8::Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;
         Local<Value> ret = script->Run();

         FetchGlobalTickFunction();

         if(try_catch.HasCaught())
         {
            ReportException(&try_catch);
            return Local<Value>();
         }
         return handle_scope.Escape(ret);
      }
      return Local<Value>();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::ExecuteFileOnce(const std::string& path)
   {
      if(mIncludedFiles.find(path) == mIncludedFiles.end())
      {
         mIncludedFiles.insert(path);
         ExecuteFile(path);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentWrapperDestructor(v8::Persistent<Value> v, void* scriptsysnull)
   {
      HandleScope scope(Isolate::GetCurrent());

      Local<Value> val = Local<Value>::New(Isolate::GetCurrent(), v);
      dtEntity::Component* component = UnwrapComponent(val);

      if(component != NULL)
      {
         ScriptSystem* scriptsys = GetScriptSystem();
         Handle<Object> o = Handle<Object>::Cast(val);
         assert(!o.IsEmpty());
         Handle<Value> val = o->GetHiddenValue(scriptsys->GetEntityIdString());
         assert(!val.IsEmpty());
         dtEntity::EntityId id = val->Uint32Value();
         scriptsys->RemoveFromComponentMap(component->GetType(), id);         
      } 

      // is already called in RemoveFromComponentMap:
      //v.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::AddToComponentMap(dtEntity::ComponentType ct, dtEntity::EntityId eid, v8::Handle<v8::Object> obj)
   {
      Isolate* isolate = Isolate::GetCurrent();
      //HandleScope scope(isolate);

      Persistent<Object>* pobj = new Persistent<Object>(isolate, obj);

      // TODO review make weak mechanims after bumping v8 release

      //pobj.MakeWeak(this, &ComponentWrapperDestructor);

      //AdjustAmountOfExternalAllocatedMemory(sizeof(dtEntity::Component));

      mComponentMap[std::make_pair(ct, eid)] = new RefPersistent<Object>(isolate, obj);
   }

   ////////////////////////////////////////////////////////////////////////////
   Handle<Object> ScriptSystem::GetFromComponentMap(dtEntity::ComponentType ct, dtEntity::EntityId eid) const
   {
      ComponentMap::const_iterator it = mComponentMap.find(std::make_pair(ct, eid));
      if(it == mComponentMap.end())
      {
         return Handle<Object>();
      }
      
      // return a local handle
      return it->second->GetLocal();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ScriptSystem::RemoveFromComponentMap(dtEntity::ComponentType ct, dtEntity::EntityId eid)
   {
      ComponentMap::iterator it = mComponentMap.find(std::make_pair(ct, eid));
      if(it == mComponentMap.end())
      {
         return false;
      }

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      //TODO (ricky) what is this used for??      
      Local<Object> obj = it->second->GetLocal();
      assert(!obj.IsEmpty() && obj->IsObject());
      // invalidate component wrapper
      obj->SetInternalField(0, External::New(isolate, 0));      

      // remove the entry from the map
      mComponentMap.erase(it);
      isolate->AdjustAmountOfExternalAllocatedMemory(-(int)sizeof(dtEntity::Component));
      return true;

   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::ComponentDeleted(dtEntity::ComponentType t, dtEntity::EntityId id)
   {
      RemoveFromComponentMap(t, id);
   }

   ////////////////////////////////////////////////////////////////////////////
   Handle<FunctionTemplate> ScriptSystem::GetTemplateBySID(dtEntity::StringId v) const
   {
      TemplateMap::const_iterator i = mTemplateMap.find(v);
      if(i == mTemplateMap.end())
      {
         return Handle<FunctionTemplate>();
      }
      
      return i->second->GetLocal();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::SetTemplateBySID(dtEntity::StringId v, v8::Handle<v8::FunctionTemplate> tpl)
   {  
      mTemplateMap[v] = new RefPersistent<FunctionTemplate>(Isolate::GetCurrent(), tpl);
   }
}
