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

#include <dtEntityWrappers/entitymanagerwrapper.h>

#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/message.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/stringid.h>
#include <dtEntityWrappers/componentwrapper.h>
#include <dtEntityWrappers/entitysystemjs.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/wrappers.h>
#include <dtEntityWrappers/RefPersistent.h>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{

   dtEntity::StringId s_entityManagerWrapper = dtEntity::SID("EntityManagerWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   class StringCache
   {
      typedef std::map<dtEntity::StringId, osg::ref_ptr< RefPersistent<v8::String> > > StringMap;

   public:
      v8::Handle<v8::String> GetString(dtEntity::StringId v)
      {
         StringMap::iterator i = mStrings.find(v);
         if(i != mStrings.end())
         {
            return i->second->GetLocal();
         }
         EscapableHandleScope scope(Isolate::GetCurrent());
         std::string str = dtEntity::GetStringFromSID(v);
         Local<String> jsstr = ToJSString(str);
         mStrings[v] = new RefPersistent<String>(Isolate::GetCurrent(), jsstr);
         return scope.Escape(jsstr);
      }

   private:
      StringMap mStrings;
   };

   static StringCache s_stringCache;


   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::String> GetString(dtEntity::StringId v)
   {
      return s_stringCache.GetString(v);
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConvertMessageToJS(Handle<Context> context, const dtEntity::Message& msg)
   {
      const dtEntity::PropertyGroup& params = msg.Get();

      EscapableHandleScope scope(Isolate::GetCurrent());
     
      Local<Object> obj = Object::New(Isolate::GetCurrent());
      dtEntity::PropertyGroup::const_iterator i;
      for(i = params.begin(); i != params.end(); ++i)
      {
         dtEntity::StringId n = i->first;
         const dtEntity::Property* p = i->second;
         Handle<Value> v = ConvertPropertyToValue(context, p);
         Handle<String> str = GetString(n);
         obj->Set(str, v);
      }
      return scope.Escape(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct MessageFunctorHolder
   {
      MessageFunctorHolder(dtEntity::MessageType msgtype, Handle<Function> func)
         : mFunctor(dtEntity::MessageFunctor(this, &MessageFunctorHolder::Call))
         , mMessageType(msgtype)
      {
         mFunction = new RefPersistent<Function>(Isolate::GetCurrent(), func);
         Handle<String> str = GetString(msgtype);
         mMessageTypeStr = new RefPersistent<String>(Isolate::GetCurrent(), str);
      }

      ~MessageFunctorHolder() {}

      void Call(const dtEntity::Message& msg)
      {
         HandleScope scope(Isolate::GetCurrent());
         Handle<Function> function = mFunction->GetLocal();

         Handle<Context> context = function->CreationContext();
         Context::Scope context_scope(context);
   
         TryCatch try_catch;

         Handle<String> messageTypeStr = mMessageTypeStr->GetLocal();
         Handle<Value> argv[2] = { messageTypeStr, ConvertMessageToJS(context, msg) };

         Handle<Value> result = function->Call(function, 2, argv);

         if(result.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }

      osg::ref_ptr<RefPersistent<Function> > mFunction;
      dtEntity::MessageFunctor mFunctor;
      osg::ref_ptr<RefPersistent<String> > mMessageTypeStr;
      dtEntity::MessageType mMessageType;
   };

   ////////////////////////////////////////////////////////////////////////////////
   struct MessageFunctorStorage
   {
      std::list<MessageFunctorHolder*> mHolders;
   };

   ////////////////////////////////////////////////////////////////////////////////
   void EMToString(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "<EntityManager>"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMCloneEntity(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), em->CloneEntity(args[0]->Uint32Value(), args[1]->Uint32Value())) );
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void EMCreateEntity(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());

      dtEntity::Entity* entity;
      bool success = em->CreateEntity(entity);
      if(!success) {
         args.GetReturnValue().Set( Null(Isolate::GetCurrent()) );
         return;
      }

      args.GetReturnValue().Set( Integer::New(Isolate::GetCurrent(), entity->GetId()) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMGetEntityIds(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());

      std::vector<dtEntity::EntityId> ids;
      em->GetEntityIds(ids);

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Array> arr = Array::New(isolate, ids.size());

      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(Integer::New(isolate, i), Integer::New(isolate, ids[i]));
      }

      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMHasEntity(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), em->HasEntity(args[0]->Uint32Value())) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ConvertJSToMessage(Handle<Value> val, dtEntity::Message* msg)
   {
      HandleScope scope(Isolate::GetCurrent());
      Handle<Object> o = Handle<Object>::Cast(val);
      Local<Array> propnames = o->GetPropertyNames();
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> propn = propnames->Get(i);
         std::string propname = ToStdString(propn);
         dtEntity::Property* prop = msg->Get(dtEntity::SID(propname));
         if(!prop)
         {
            LOG_ERROR("Error setting message values: Message "
             << dtEntity::GetStringFromSID(msg->GetType())
             << " has no property with name " << propname);
         }
         else
         {
            SetPropertyFromValue(o->Get(propn), prop);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMEmitMessage(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      std::string namestr = ToStdString(args[0]);
      dtEntity::StringId pname = dtEntity::SIDHash(namestr);

      dtEntity::Message* msg = NULL; 
      bool success = dtEntity::MessageFactory::GetInstance().CreateMessage(pname, msg);
      
      if(!success)
      {
         ThrowError("Could not create message of type " + namestr);
         return;
      }

      ConvertJSToMessage(args[1], msg);
      em->EmitMessage(*msg);
      delete msg;
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void EMEnqueueMessage(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      
      dtEntity::StringId pname = dtEntity::SIDHash(ToStdString(args[0]));

      dtEntity::Message* msg = NULL; 
      bool success = dtEntity::MessageFactory::GetInstance().CreateMessage(pname, msg);
      
      if(!success)
      {
         ThrowError("Could not create message of type " + ToStdString(args[0]));
         return;
      }

      ConvertJSToMessage(args[1], msg);
      em->EnqueueMessage(*msg, args[2]->NumberValue());
      delete msg;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMKillEntity(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      bool success = em->KillEntity(args[0]->Int32Value());
      if(success)
         args.GetReturnValue().Set(True(Isolate::GetCurrent()) );
      else
         args.GetReturnValue().Set( False(Isolate::GetCurrent()) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMAddPlugin(const FunctionCallbackInfo<Value>& args)
   {  
      std::string path = ToStdString(args[0]);
      std::string name = ToStdString(args[1]);

      dtEntity::ComponentPluginManager& pm = dtEntity::ComponentPluginManager::GetInstance();
      std::set<dtEntity::ComponentType> handled = pm.AddPlugin(path, name, false);

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Array> arr = Array::New(isolate);

      int idx = 0;
      for(std::set<dtEntity::ComponentType>::iterator i = handled.begin(); i != handled.end(); ++i)
      {
         arr->Set(Integer::New(isolate, idx++), String::NewFromUtf8(isolate, dtEntity::GetStringFromSID(*i).c_str()));
      }

      args.GetReturnValue().Set( arr );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMAddToScene(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      if(!args[0]->IsInt32()) 
      {
         ThrowError("usage: addToScene(int32 entityId)");
         return;
      }

      bool success = em->AddToScene(args[0]->Int32Value());
      if(success)
         args.GetReturnValue().Set(True(Isolate::GetCurrent()));
      else
         args.GetReturnValue().Set(False(Isolate::GetCurrent()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMRegisterForMessages(const FunctionCallbackInfo<Value>& args)
   {  
      if(!args[0]->IsString() || !args[1]->IsFunction())
      {
         ThrowError("Usage: registerForMessages(string msgname, function, [options])");
         return;
      }

      std::string msgname = ToStdString(args[0]);
      
      Handle<Function> func = Handle<Function>::Cast(args[1]);
      std::string funcname = ToStdString(func->GetName());
      if(funcname == "")
      {
         std::ostringstream os; 
         os << ToStdString(func->GetScriptOrigin().ResourceName());
         os << ", line " << func->GetScriptLineNumber();
         funcname = os.str();
      }
      
      MessageFunctorHolder* fh = new MessageFunctorHolder(dtEntity::SIDHash(msgname), func);
      
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());

      MessageFunctorStorage* storage;
      GetInternal(args.This(), 1, storage);
      storage->mHolders.push_back(fh);

      unsigned int options = 1;
      if(args.Length() > 2)
      {
         options = args[2]->Uint32Value();
      }
      em->RegisterForMessages(dtEntity::SIDHash(msgname), fh->mFunctor, options, funcname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMUnregisterForMessages(const FunctionCallbackInfo<Value>& args)
   {  
      if(!args[0]->IsString() || !args[1]->IsFunction())
      {
         ThrowError("Usage: unregisterForMessages(string msgname, function)");
         return;
      }

      std::string msgname = ToStdString(args[0]);
      dtEntity::StringId msgnamesid = dtEntity::SIDHash(msgname);
      Handle<Function> func = Handle<Function>::Cast(args[1]);
      
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());

      MessageFunctorStorage* storage;
      GetInternal(args.This(), 1, storage);
      std::list<MessageFunctorHolder*>::iterator i;
      for(i = storage->mHolders.begin(); i != storage->mHolders.end(); ++i)
      {
         MessageFunctorHolder* holder = *i;
         if(holder->mFunction->GetLocal() == func && holder->mMessageType == msgnamesid)
         {
            em->UnregisterForMessages(holder->mMessageType, holder->mFunctor);
            storage->mHolders.erase(i);
            delete holder;
            return;
         }
      }

      ThrowError("Could not unregister from message, no registrant found: " + msgname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMRemoveFromScene(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      if(!args[0]->IsInt32()) 
      {
         ThrowError("usage: removeFromScene(int32 entityId)");
         return;
      }

      bool success = em->RemoveFromScene(args[0]->Int32Value());
      
      if(success)
         args.GetReturnValue().Set(True(Isolate::GetCurrent()));
      else
         args.GetReturnValue().Set(False(Isolate::GetCurrent()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMHasEntitySystem(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      if(!args[0]->IsString()) 
      {
         ThrowError("usage: hasEntitySystem(string)");
         return;
      }

      dtEntity::ComponentType t = dtEntity::SIDHash(ToStdString(args[0]));
      if(em->HasEntitySystem(t))
      {
         args.GetReturnValue().Set(True(Isolate::GetCurrent()));
         return;
      }
      else
      {
         bool success = dtEntity::ComponentPluginManager::GetInstance().FactoryExists(t);
         if(success)
         {
            args.GetReturnValue().Set(True(Isolate::GetCurrent()));
            return;
         }
      }

      args.GetReturnValue().Set(False(Isolate::GetCurrent()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMAddEntitySystem(const FunctionCallbackInfo<Value>& args)
   {  
      if(IsEntitySystem(args[0]))
      {
         dtEntity::EntitySystem* es = UnwrapEntitySystem(args[0]);

         dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
         em->AddEntitySystem(*es);
         args.GetReturnValue().Set( Undefined( Isolate::GetCurrent()));
         return;
      }

      HandleScope scope(Isolate::GetCurrent());
      Handle<Object> obj = Handle<Object>::Cast(args[0]);
      if(!obj.IsEmpty() &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "componentType")) &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "hasComponent")) &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getComponent")) &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "createComponent")) &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "deleteComponent")) &&
         obj->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getEntitiesInSystem"))
      )
      {
         dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
         std::string compType = ToStdString(obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "componentType")));
         dtEntity::ComponentType ct = (dtEntity::ComponentType) dtEntity::SID(compType);
         EntitySystemJS* es = new EntitySystemJS(ct, *em, obj);
         em->AddEntitySystem(*es);
         args.GetReturnValue().Set( Undefined( Isolate::GetCurrent()));
         return;
      }

      ThrowError("Not an entity system! Has to have componentType, "
                  "hasComponent(), getComponent(), createComponent(), deleteComponent() and getEntitiesInSystem()");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EMGetEntitySystem(const FunctionCallbackInfo<Value>& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.This());
      if(!args[0]->IsString()) 
      {
         ThrowError("usage: getEntitySystem(string)");
         return;
      }

      std::string str = ToStdString(args[0]);
      dtEntity::ComponentType t = dtEntity::SIDHash(str);
      dtEntity::EntitySystem* es = em->GetEntitySystem(t);
      if(es == NULL)
      {
         // try to start from factory
         bool success = dtEntity::ComponentPluginManager::GetInstance().StartEntitySystem(*em, t);
         if(success)
         {
            es = em->GetEntitySystem(t);
            if(es == NULL)
            {
               ThrowError("Could not start entity system " + str);
               return;
            }
         }
         else
         {
            args.GetReturnValue().Set( Null( Isolate::GetCurrent()));
            return;
         }
      }

      ScriptSystem* ss;
      em->GetEntitySystem(ScriptSystem::TYPE, ss);
      args.GetReturnValue().Set( WrapEntitySystem(ss, es) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapPriorities()
   {
     Isolate* isolate = Isolate::GetCurrent();
     EscapableHandleScope scope(isolate);
     Local<Object> obj = Object::New(isolate);

     obj->Set(String::NewFromUtf8(isolate, "default"), Uint32::New(isolate, dtEntity::FilterOptions::ORDER_DEFAULT));
     obj->Set(String::NewFromUtf8(isolate, "lowest"),  Uint32::New(isolate, dtEntity::FilterOptions::PRIORITY_LOWEST));
     obj->Set(String::NewFromUtf8(isolate, "higher"), Uint32::New(isolate, dtEntity::FilterOptions::PRIORITY_HIGHER));
     obj->Set(String::NewFromUtf8(isolate, "highest"),  Uint32::New(isolate, dtEntity::FilterOptions::PRIORITY_HIGHEST));
     obj->Set(String::NewFromUtf8(isolate, "late"),  Uint32::New(isolate, dtEntity::FilterOptions::ORDER_LATE));
     obj->Set(String::NewFromUtf8(isolate, "early"), Uint32::New(isolate, dtEntity::FilterOptions::ORDER_EARLY));
     obj->Set(String::NewFromUtf8(isolate, "earliest"),  Uint32::New(isolate, dtEntity::FilterOptions::ORDER_EARLIEST));
     obj->Set(String::NewFromUtf8(isolate, "singleshot"), Uint32::New(isolate, dtEntity::FilterOptions::SINGLE_SHOT));

     return scope.Escape(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapEntityManager(ScriptSystem* ss, dtEntity::EntityManager* v)
   {
      s_stringCache = StringCache();


      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope scope(isolate);

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_entityManagerWrapper);

      if(templt.IsEmpty())
      {
        templt = FunctionTemplate::New(isolate);
        templt->SetClassName(String::NewFromUtf8(isolate, "EntityManager"));
        templt->InstanceTemplate()->SetInternalFieldCount(2);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set(String::NewFromUtf8(isolate, "addEntitySystem"), FunctionTemplate::New(isolate, EMAddEntitySystem));
        proto->Set(String::NewFromUtf8(isolate, "addToScene"), FunctionTemplate::New(isolate, EMAddToScene));
        proto->Set(String::NewFromUtf8(isolate, "cloneEntity"), FunctionTemplate::New(isolate, EMCloneEntity));
        proto->Set(String::NewFromUtf8(isolate, "createEntity"), FunctionTemplate::New(isolate, EMCreateEntity));
        proto->Set(String::NewFromUtf8(isolate, "getEntityIds"), FunctionTemplate::New(isolate, EMGetEntityIds));
        proto->Set(String::NewFromUtf8(isolate, "hasEntity"), FunctionTemplate::New(isolate, EMHasEntity));
        proto->Set(String::NewFromUtf8(isolate, "emitMessage"), FunctionTemplate::New(isolate, EMEmitMessage));
        proto->Set(String::NewFromUtf8(isolate, "enqueueMessage"), FunctionTemplate::New(isolate, EMEnqueueMessage));
        proto->Set(String::NewFromUtf8(isolate, "getEntitySystem"), FunctionTemplate::New(isolate, EMGetEntitySystem));
        proto->Set(String::NewFromUtf8(isolate, "hasEntitySystem"), FunctionTemplate::New(isolate, EMHasEntitySystem));
        proto->Set(String::NewFromUtf8(isolate, "killEntity"), FunctionTemplate::New(isolate, EMKillEntity));
        proto->Set(String::NewFromUtf8(isolate, "addPlugin"), FunctionTemplate::New(isolate, EMAddPlugin));
        proto->Set(String::NewFromUtf8(isolate, "registerForMessages"), FunctionTemplate::New(isolate, EMRegisterForMessages));
        proto->Set(String::NewFromUtf8(isolate, "unregisterForMessages"), FunctionTemplate::New(isolate, EMUnregisterForMessages));
        proto->Set(String::NewFromUtf8(isolate, "removeFromScene"), FunctionTemplate::New(isolate, EMRemoveFromScene));
        proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, EMToString));

        GetScriptSystem()->SetTemplateBySID(s_entityManagerWrapper, templt);

      }

      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(isolate, v));
      instance->SetInternalField(1, External::New(isolate, new MessageFunctorStorage()));

      return scope.Escape(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityManager* UnwrapEntityManager(v8::Handle<v8::Value> val)
   {
      dtEntity::EntityManager* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

}
