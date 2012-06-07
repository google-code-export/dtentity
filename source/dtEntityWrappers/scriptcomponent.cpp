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



#include <dtEntity/applicationcomponent.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/inputhandler.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityWrappers/componentwrapper.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/messages.h>
#include <dtEntityWrappers/screenwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <iostream>
#include <sstream>
#include <v8.h>
#include <v8-debug.h>

#include <osg/Version>

#define OSG_2_9_VER (defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR >= 2 && OSG_VERSION_MINOR > 8)

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

      mResetSystemFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnResetSystem);
      GetEntityManager().RegisterForMessages(dtEntity::ResetSystemMessage::TYPE, mResetSystemFunctor,
                             dtEntity::FilterOptions::ORDER_EARLY, "ScriptSystem::OnResetSystem");

      mTickFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::Tick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor, "ScriptSystem::Tick");

      mLoadScriptFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnLoadScript);
      em.RegisterForMessages(ExecuteScriptMessage::TYPE, mLoadScriptFunctor, "ScriptSystem::OnLoadScript");

      HandleScope scope;
      mEntityIdString = Persistent<String>::New(String::New("__entityid__"));
      mPropertyNamesString = Persistent<String>::New(String::New("__propertynames__"));
   }  

   ////////////////////////////////////////////////////////////////////////////
   ScriptSystem::~ScriptSystem()
   {
      //V8::RemoveGCPrologueCallback(GCStartCallback);
      //V8::RemoveGCEpilogueCallback(GCEndCallback);

      for(ComponentMap::iterator i = mComponentMap.begin(); i != mComponentMap.end(); ++i)
      {
         i->second.Dispose();
      }
      mComponentMap.clear();

      for(TemplateMap::iterator i = mTemplateMap.begin(); i != mTemplateMap.end(); ++i)
      {
         i->second.Dispose();
      }
      mTemplateMap.clear();

      mGlobalContext.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::SetupContext()
   {
      HandleScope handle_scope;
      if(!mGlobalContext.IsEmpty())
      {
         mGlobalContext.Dispose();
      }

      // create a template for the global object
      Handle<ObjectTemplate> global = ObjectTemplate::New();

      // create persistent global context
      mGlobalContext = Persistent<Context>::New(Context::New(NULL, global));

      // store pointer to script system into isolate data to have it globally available in javascript
      Isolate::GetCurrent()->SetData(this);

      RegisterGlobalFunctions(this, mGlobalContext);

      InitializeAllWrappers(GetEntityManager());

      Handle<Context> context = GetGlobalContext();
      Context::Scope context_scope(context);
      Handle<FunctionTemplate> tmplt = FunctionTemplate::New();
      tmplt->InstanceTemplate()->SetInternalFieldCount(2);

      tmplt->SetClassName(String::New("ScriptSystem"));

      dtEntity::ApplicationSystem* as;
      GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, as);
      osgViewer::GraphicsWindow* window = as->GetPrimaryWindow();

      dtEntity::InputHandler* input = &as->GetWindowManager()->GetInputHandler();

      context->Global()->Set(String::New("Axis"), WrapAxes(input));
      context->Global()->Set(String::New("Input"), WrapInputHandler(GetGlobalContext(), input));
      context->Global()->Set(String::New("Key"), WrapKeys(input));
      context->Global()->Set(String::New("MouseWheelState"), WrapMouseWheelStates());
      context->Global()->Set(String::New("Screen"), WrapScreen(this, as->GetPrimaryView(), window));
      context->Global()->Set(String::New("TouchPhase"), WrapTouchPhases());
      context->Global()->Set(String::New("Priority"), WrapPriorities());
      context->Global()->Set(String::New("Order"), WrapPriorities());


   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::Finished()
   {
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
         v8::HandleScope scope;
         ExecuteFile(script);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnResetSystem(const dtEntity::Message& msg)
   {
      UnregisterJavaScriptFromMessages(this);
      SetupContext();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnLoadScript(const dtEntity::Message& m)
   {
      const ExecuteScriptMessage& msg = static_cast<const ExecuteScriptMessage&>(m);

      HandleScope scope;
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
      return mGlobalContext;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::FetchGlobalTickFunction()
   {
      HandleScope scope;
      Handle<Context> context = GetGlobalContext();

      mGlobalTickFunction.Clear();

      Handle<String> funcname = String::New("__executeTimeOuts");
      if(context->Global()->Has(funcname))
      {
         Handle<Value> func = context->Global()->Get(funcname);
         if(!func.IsEmpty())
         {
            Handle<Function> f = Handle<Function>::Cast(func);
            if(!f.IsEmpty())
            {
               mGlobalTickFunction = Persistent<Function>::New(f);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::Tick(const dtEntity::Message& m)
   {
      if(mGlobalTickFunction.IsEmpty())
      {
         return;
      }

      HandleScope scope;  
      Context::Scope context_scope(GetGlobalContext());

      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      TryCatch try_catch;
      Handle<Value> argv[3] = {
         Number::New(msg.GetDeltaSimTime()),
         Number::New(msg.GetSimulationTime()),
         Uint32::New(osg::Timer::instance()->time_m())
      };

      Handle<Value> ret = mGlobalTickFunction->Call(mGlobalTickFunction, 3, argv);

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

      HandleScope handle_scope;
      Context::Scope context_scope(GetGlobalContext());
      TryCatch try_catch;
      Local<Script> compiled_script = Script::Compile(ToJSString(code), ToJSString(path));

      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
         return Handle<Script>();
      }

      return handle_scope.Close(compiled_script);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ScriptSystem::ExecuteJS(const std::string& code, const std::string& path)
   {
       // Init JavaScript context
      HandleScope handle_scope;
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

      return handle_scope.Close(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Local<Value> ScriptSystem::ExecuteFile(const std::string& path)
   {
      HandleScope handle_scope;

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
         return handle_scope.Close(ret);
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

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::LoadAutoStartScripts(const std::string& path)
   {
      HandleScope handle_scope;
      Handle<Context> global = GetGlobalContext();
      Context::Scope context_scope(global);

      std::set<std::string> executed;

      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      for(osgDB::FilePathList::iterator i = paths.begin(); i != paths.end(); ++i)
      {
         std::ostringstream autostartpath;
         autostartpath << *i << "/" << path;
         
         osgDB::FilePathList currentPathList;
         currentPathList.push_back(autostartpath.str());
         const std::string absPath = osgDB::findDataFile(autostartpath.str());

         if(!osgDB::fileExists(absPath))
         {
            continue;
         }         
         
         osgDB::DirectoryContents contents = osgDB::getDirectoryContents(absPath);

         osgDB::DirectoryContents::const_iterator j;
         for(j = contents.begin(); j != contents.end(); ++j)
         {
            std::string filepath = *j;
            if(osgDB::getFileExtension(filepath).compare("js") != 0) continue;
            if(executed.find(filepath) == executed.end())
            {
               ExecuteFile(path + "/" + filepath);
               executed.insert(filepath);
            }
         }
      }      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentWrapperDestructor(v8::Persistent<Value> v, void* scriptsysnull)
   {      
      dtEntity::Component* component = UnwrapComponent(v);
      if(component != NULL)
      {
         ScriptSystem* scriptsys = static_cast<ScriptSystem*>(Isolate::GetCurrent()->GetData());
         HandleScope scope;
         Handle<Object> o = Handle<Object>::Cast(v);
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
      HandleScope scope;
      Persistent<Object> pobj = Persistent<Object>::New(obj);
      pobj.MakeWeak(this, &ComponentWrapperDestructor);
      V8::AdjustAmountOfExternalAllocatedMemory(sizeof(dtEntity::Component));
      mComponentMap[std::make_pair(ct, eid)] = pobj;
   }

   ////////////////////////////////////////////////////////////////////////////
   Handle<Object> ScriptSystem::GetFromComponentMap(dtEntity::ComponentType ct, dtEntity::EntityId eid) const
   {
      ComponentMap::const_iterator it = mComponentMap.find(std::make_pair(ct, eid));
      if(it == mComponentMap.end())
      {
         return Handle<Object>();
      }
      return it->second;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ScriptSystem::RemoveFromComponentMap(dtEntity::ComponentType ct, dtEntity::EntityId eid)
   {
      ComponentMap::iterator it = mComponentMap.find(std::make_pair(ct, eid));
      if(it == mComponentMap.end())
      {
         return false;
      }
      HandleScope scope;
      Persistent<Object> obj = it->second;
      assert(!obj.IsEmpty() && obj->IsObject());
      // invalidate component wrapper
      obj->SetInternalField(0, External::New(0));
      obj.Dispose();
      mComponentMap.erase(it);
      V8::AdjustAmountOfExternalAllocatedMemory(-(int)sizeof(dtEntity::Component));
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
      return i->second;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::SetTemplateBySID(dtEntity::StringId v, v8::Handle<v8::FunctionTemplate> tpl)
   {
      mTemplateMap[v] = Persistent<FunctionTemplate>::New(tpl);
   }
}
