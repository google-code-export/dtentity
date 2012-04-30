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
#include <osgDB/FileUtils>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_entityManagerTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   class StringCache
   {
      typedef std::map<dtEntity::StringId, v8::Persistent<v8::String> > StringMap;
   public:
      v8::Handle<v8::String> GetString(dtEntity::StringId v)
      {
         StringMap::iterator i = mStrings.find(v);
         if(i != mStrings.end())
         {
            return i->second;
         }
         HandleScope scope;
         std::string str = dtEntity::GetStringFromSID(v);
         Handle<String> jsstr = ToJSString(str);
         mStrings[v] = Persistent<String>::New(jsstr);
         return scope.Close(jsstr);
      }

   private:
      std::map<dtEntity::StringId, v8::Persistent<v8::String> > mStrings;
   };

   static StringCache s_stringCache;

   v8::Handle<v8::String> GetString(dtEntity::StringId v)
   {
      return s_stringCache.GetString(v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConvertMessageToJS(Handle<Context> context, const dtEntity::Message& msg)
   {
      const dtEntity::PropertyContainer::PropertyMap& params = msg.GetAllProperties();

      HandleScope scope;
     
      Handle<Object> o = Object::New();
      dtEntity::PropertyContainer::PropertyMap::const_iterator i;
      for(i = params.begin(); i != params.end(); ++i)
      {
         dtEntity::StringId n = i->first;
         const dtEntity::Property* p = i->second;
         Handle<Value> v = ConvertPropertyToValue(context, p);
         Handle<String> str = GetString(n);
         o->Set(str, v);
      }
      return scope.Close(o);
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct MessageFunctorHolder
   {
      MessageFunctorHolder(dtEntity::MessageType msgtype, Handle<Function> func)
         : mFunction(Persistent<Function>::New(func))
         , mFunctor(dtEntity::MessageFunctor(this, &MessageFunctorHolder::Call))
         , mMessageType(msgtype)
      {
         Handle<String> str = GetString(msgtype);
         mMessageTypeStr = Persistent<String>::New(str);
      }

      ~MessageFunctorHolder()
      {
         mMessageTypeStr.Dispose();
      }

      void Call(const dtEntity::Message& msg)
      {
         HandleScope scope;
         Handle<Context> context = mFunction->CreationContext();
         Context::Scope context_scope(context);
   
         TryCatch try_catch;

         Handle<Value> argv[2] = { mMessageTypeStr, ConvertMessageToJS(context, msg) };

         Handle<Value> result = mFunction->Call(mFunction, 2, argv);

         if(result.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }

      Persistent<Function> mFunction;
      dtEntity::MessageFunctor mFunctor;
      Persistent<String> mMessageTypeStr;
      dtEntity::MessageType mMessageType;
   };

   ////////////////////////////////////////////////////////////////////////////////
   struct MessageFunctorStorage
   {
      std::list<MessageFunctorHolder*> mHolders;
   };

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMToString(const Arguments& args)
   {
      return String::New("<EntityManager>");
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMCreateEntity(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());

      dtEntity::Entity* entity;
      bool success = em->CreateEntity(entity);
      if(!success) return Null();
      return Integer::New(entity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMGetEntityIds(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());

      std::vector<dtEntity::EntityId> ids;
      em->GetEntityIds(ids);

      HandleScope scope;
      Handle<Array> arr = Array::New(ids.size());

      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(Integer::New(i), Integer::New(ids[i]));
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ConvertJSToMessage(Handle<Value> val, dtEntity::Message* msg)
   {
      HandleScope scope;
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
   Handle<Value> EMEmitMessage(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      std::string namestr = ToStdString(args[0]);
      dtEntity::StringId pname = dtEntity::SIDHash(namestr);

      dtEntity::Message* msg = NULL; 
      bool success = dtEntity::MessageFactory::GetInstance().CreateMessage(pname, msg);
      
      if(!success)
      {
         return ThrowError("Could not create message of type " + namestr);
      }
      ConvertJSToMessage(args[1], msg);
      em->EmitMessage(*msg);
      delete msg;
      return Undefined();
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMEnqueueMessage(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      
      dtEntity::StringId pname = dtEntity::SIDHash(ToStdString(args[0]));

      dtEntity::Message* msg = NULL; 
      bool success = dtEntity::MessageFactory::GetInstance().CreateMessage(pname, msg);
      
      if(!success)
      {
         return ThrowError("Could not create message of type " + ToStdString(args[0]));
      }

      ConvertJSToMessage(args[1], msg);
      em->EnqueueMessage(*msg, args[2]->NumberValue());
      delete msg;
      return Undefined();
   }  

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMKillEntity(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      bool success = em->KillEntity(args[0]->Int32Value());
      return success ? True() : False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMAddPlugin(const v8::Arguments& args)
   {  
      std::string path = ToStdString(args[0]);
      std::string name = ToStdString(args[1]);

      dtEntity::ComponentPluginManager& pm = dtEntity::ComponentPluginManager::GetInstance();
      std::set<dtEntity::ComponentType> handled = pm.AddPlugin(path, name, false);
      HandleScope scope;
      Handle<Array> arr = Array::New();

      int idx = 0;
      for(std::set<dtEntity::ComponentType>::iterator i = handled.begin(); i != handled.end(); ++i)
      {
         arr->Set(Integer::New(idx++), String::New(dtEntity::GetStringFromSID(*i).c_str()));
      }
      return arr;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMAddToScene(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      if(!args[0]->IsInt32()) return ThrowError("usage: addToScene(int32 entityId)");
      bool success = em->AddToScene(args[0]->Int32Value());
      return success ? True() : False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMRegisterForMessages(const v8::Arguments& args)
   {  
      if(!args[0]->IsString() || !args[1]->IsFunction())
      {
         return ThrowError("Usage: registerForMessages(string msgname, function, [options])");
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
      
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());

      MessageFunctorStorage* storage;
      GetInternal(args.Holder(), 1, storage);
      storage->mHolders.push_back(fh);

      unsigned int options = 0;
      if(args.Length() > 2)
      {
         options = args[2]->Uint32Value();
      }
      em->RegisterForMessages(dtEntity::SIDHash(msgname), fh->mFunctor, options, funcname);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMUnregisterForMessages(const v8::Arguments& args)
   {  
      if(!args[0]->IsString() || !args[1]->IsFunction())
      {
         return ThrowError("Usage: unregisterForMessages(string msgname, function)");
      }
      std::string msgname = ToStdString(args[0]);
      dtEntity::StringId msgnamesid = dtEntity::SIDHash(msgname);
      Handle<Function> func = Handle<Function>::Cast(args[1]);
      
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());

      MessageFunctorStorage* storage;
      GetInternal(args.Holder(), 1, storage);
      std::list<MessageFunctorHolder*>::iterator i;
      for(i = storage->mHolders.begin(); i != storage->mHolders.end(); ++i)
      {
         MessageFunctorHolder* holder = *i;
         if(holder->mFunction == func && holder->mMessageType == msgnamesid)
         {
            em->UnregisterForMessages(holder->mMessageType, holder->mFunctor);
            storage->mHolders.erase(i);
            delete holder;
            return Undefined();
         }
      }
      return ThrowError("Could not unregister from message, no registrant found: " + msgname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void UnregisterJavaScriptFromMessages(ScriptSystem* scriptSystem)
   {  
      HandleScope scope;

      Handle<Context> context = scriptSystem->GetGlobalContext();
      Context::Scope context_scope(context);
      Handle<Object> emh = Handle<Object>::Cast(context->Global()->Get(String::New("EntityManager")));

      dtEntity::EntityManager* em = UnwrapEntityManager(emh);

      MessageFunctorStorage* storage;
      GetInternal(emh, 1, storage);
      std::list<MessageFunctorHolder*>::iterator i;
      for(i = storage->mHolders.begin(); i != storage->mHolders.end(); ++i)
      {
         MessageFunctorHolder* holder = *i;
          
         em->UnregisterForMessages(holder->mMessageType, holder->mFunctor);
         delete holder;
      }
      storage->mHolders.clear();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMRemoveFromScene(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      if(!args[0]->IsInt32()) return ThrowError("usage: removeFromScene(int32 entityId)");
      bool success = em->RemoveFromScene(args[0]->Int32Value());
      return success ? True() : False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMHasEntitySystem(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      if(!args[0]->IsString()) return ThrowError("usage: hasEntitySystem(string)");
      dtEntity::ComponentType t = dtEntity::SIDHash(ToStdString(args[0]));
      if(em->HasEntitySystem(t))
      {
         return True();
      }
      else
      {
         bool success = dtEntity::ComponentPluginManager::GetInstance().FactoryExists(t);
         if(success)
         {
            return True();
         }
      }
      return False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMAddEntitySystem(const v8::Arguments& args)
   {  
      if(IsEntitySystem(args[0]))
      {
         dtEntity::EntitySystem* es = UnwrapEntitySystem(args[0]);

         dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
         em->AddEntitySystem(*es);
         return Undefined();
      }
      HandleScope scope;
      Handle<Object> obj = Handle<Object>::Cast(args[0]);
      if(!obj.IsEmpty() &&
         obj->Has(String::New("componentType")) &&
         obj->Has(String::New("hasComponent")) &&
         obj->Has(String::New("getComponent")) &&
         obj->Has(String::New("createComponent")) &&
         obj->Has(String::New("deleteComponent")) &&
         obj->Has(String::New("getEntitiesInSystem"))
      )
      {
         dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
         std::string compType = ToStdString(obj->Get(String::New("componentType")));
         dtEntity::ComponentType ct = (dtEntity::ComponentType) dtEntity::SID(compType);
         EntitySystemJS* es = new EntitySystemJS(ct, *em, obj);
         em->AddEntitySystem(*es);
         return Undefined();
      }

      return ThrowError("Not an entity system! Has to have componentType, "
         "hasComponent(), getComponent(), createComponent(), deleteComponent() and getEntitiesInSystem()");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EMGetEntitySystem(const v8::Arguments& args)
   {  
      dtEntity::EntityManager* em = UnwrapEntityManager(args.Holder());
      if(!args[0]->IsString()) return ThrowError("usage: getEntitySystem(string)");
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
            assert(es != NULL);
         }
         else
         {
            return Null();
         }
      }
      ScriptSystem* ss;
      em->GetEntitySystem(ScriptSystem::TYPE, ss);
      return WrapEntitySystem(ss, es);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapPriorities()
   {
     HandleScope scope;
     Handle<Object> obj = Object::New();

     obj->Set(String::New("default"), Uint32::New(dtEntity::FilterOptions::ORDER_DEFAULT));
     obj->Set(String::New("lowest"),  Uint32::New(dtEntity::FilterOptions::PRIORITY_LOWEST));
     obj->Set(String::New("higher"), Uint32::New(dtEntity::FilterOptions::PRIORITY_HIGHER));
     obj->Set(String::New("highest"),  Uint32::New(dtEntity::FilterOptions::PRIORITY_HIGHEST));
     obj->Set(String::New("late"),  Uint32::New(dtEntity::FilterOptions::ORDER_LATE));
     obj->Set(String::New("early"), Uint32::New(dtEntity::FilterOptions::ORDER_EARLY));
     obj->Set(String::New("earliest"),  Uint32::New(dtEntity::FilterOptions::ORDER_EARLIEST));
     obj->Set(String::New("singleshot"), Uint32::New(dtEntity::FilterOptions::SINGLE_SHOT));
     return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapEntityManager(ScriptSystem* ss, dtEntity::EntityManager* v)
   {

      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(ss->GetGlobalContext());

      if(s_entityManagerTemplate.IsEmpty())
      {
        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_entityManagerTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("EntityManager"));
        templt->InstanceTemplate()->SetInternalFieldCount(2);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("addEntitySystem", FunctionTemplate::New(EMAddEntitySystem));
        proto->Set("addToScene", FunctionTemplate::New(EMAddToScene));
        proto->Set("createEntity", FunctionTemplate::New(EMCreateEntity));
        proto->Set("getEntityIds", FunctionTemplate::New(EMGetEntityIds));
        proto->Set("emitMessage", FunctionTemplate::New(EMEmitMessage));
        proto->Set("enqueueMessage", FunctionTemplate::New(EMEnqueueMessage));
        proto->Set("getEntitySystem", FunctionTemplate::New(EMGetEntitySystem));
        proto->Set("hasEntitySystem", FunctionTemplate::New(EMHasEntitySystem));
        proto->Set("killEntity", FunctionTemplate::New(EMKillEntity));
        proto->Set("addPlugin", FunctionTemplate::New(EMAddPlugin));
        proto->Set("registerForMessages", FunctionTemplate::New(EMRegisterForMessages));
        proto->Set("unregisterForMessages", FunctionTemplate::New(EMUnregisterForMessages));
        proto->Set("removeFromScene", FunctionTemplate::New(EMRemoveFromScene));
        proto->Set("toString", FunctionTemplate::New(EMToString));
      }
      Local<Object> instance = s_entityManagerTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      instance->SetInternalField(1, External::New(new MessageFunctorStorage()));

      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityManager* UnwrapEntityManager(v8::Handle<v8::Value> val)
   {
      dtEntity::EntityManager* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

}
