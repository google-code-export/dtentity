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

#include <dtEntityWrappers/entitysystemjs.h>

#include <dtEntity/dtentity_config.h>
#include <dtEntity/log.h>
#include <dtEntity/property.h>
#include <dtEntityWrappers/jsproperty.h>
#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>
#include <iostream>

namespace dtEntityWrappers
{
   using namespace v8;


   ////////////////////////////////////////////////////////////////////////////////
   void PropertyGetter(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      ComponentJS* comp = static_cast<ComponentJS*>(Handle<External>::Cast(info.Data())->Value());
      std::string propnamestr = ToStdString(propname);
      dtEntity::Property* prop = comp->Get(dtEntity::SIDHash(propnamestr));
      if(!prop)
         info.GetReturnValue().Set( Undefined(Isolate::GetCurrent()));

      info.GetReturnValue().Set(ConvertPropertyToValue(info.Holder()->CreationContext(), prop));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertySetter(Local<String> propname,
                               Local<Value> value,
                               const PropertyCallbackInfo<void>& info)
   {
      ComponentJS* comp = static_cast<ComponentJS*>(Handle<External>::Cast(info.Data())->Value());
      std::string propnamestr = ToStdString(propname);
      dtEntity::StringId propnamesid = dtEntity::SIDHash(propnamestr);
      dtEntity::Property* prop = comp->Get(propnamesid);
      if(prop)
         SetPropertyFromValue(value, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentJS::ComponentJS(dtEntity::ComponentType componentType, Handle<Object> obj)
      : mComponentType(componentType)
      , mComponent(new RefPersistent<Object>(Isolate::GetCurrent(), obj))
   {

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);


      Handle<Array> propnames = obj->GetPropertyNames();

      isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(ComponentJS));

      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Local<Value> p = propnames->Get(Integer::New(isolate, i));
         Local<String> propname = Local<String>::Cast(p);
         std::string propname_str = ToStdString(propname);

         // don't expose properties beginning with "__"
         if(propname_str.size() > 1 && propname_str[0] == '_' && propname_str[1] == '_')
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SID(propname_str);
         Handle<Value> val = obj->Get(propname);

         // don't convert functions to component properties
         if(val->IsFunction())
         {
#if CALL_ONPROPERTYCHANGED_METHOD == 0
            if(propname_str == "onPropertyChanged")
            {
               LOG_ERROR("onPropertyChanged method is no longer called when a propery is changed!");
            }
#endif
         }
         else
         {
#if CALL_ONPROPERTYCHANGED_METHOD == 0
            // handle properties created with createProperty* functions
            Handle<Value> hidden = obj->GetHiddenValue(propname);
            if(!hidden.IsEmpty())
            {


               Handle<External> ext = Handle<External>::Cast(hidden);
               void* ptr = ext->Value();
               PropertyGetterSetter* prop = reinterpret_cast<PropertyGetterSetter*>(ptr);

               dtEntity::PropertyGroup::iterator j = mValue.find(propname_sid);
               if(j != mValue.end())
               {
                 delete j->second;
                 mValue.erase(j);
               }
               mValue[propname_sid] = prop;

            }
#else

            dtEntity::Property* prop = ConvertValueToProperty(val);
            if(prop)
            {
               dtEntity::PropertyGroup::iterator j = mValue.find(propname_sid);
               if(j != mValue.end())
               {
                 delete j->second;
                 mValue.erase(j);
               }
               mValue[propname_sid] =  prop;
            }

            Handle<External> ext = v8::External::New(isolate, static_cast<void*>(this));
            obj->SetAccessor(propname, PropertyGetter, PropertySetter, ext);
#endif
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentJS::~ComponentJS()
   {
      Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(-(int)sizeof(ComponentJS));
      for(dtEntity::PropertyGroup::iterator i = mValue.begin(); i != mValue.end(); ++i)
      {
         delete i->second;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::OnAddedToEntity(dtEntity::Entity& entity)
   {
   }

#if CALL_ONPROPERTYCHANGED_METHOD
   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::OnPropertyChanged(dtEntity::StringId propnamesid, dtEntity::Property& prop)
   {
      std::string propname = dtEntity::GetStringFromSID(propnamesid);
      HandleScope scope(Isolate::GetCurrent());
         
      Handle<String> s = String::NewFromUtf8(v8::Isolate::GetCurrent(), "onPropertyChanged");
      Local<Object> comp = mComponent->GetLocal();
      Handle<Value> cb = comp->Get(s);
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(comp->CreationContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         Handle<String> propstr = ToJSString(propname);
         Handle<Value> argv[1] = { propstr };
         Handle<Value> ret = func->Call(comp, 1, argv);

         if(ret.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }      
   }
#endif


   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::Finished()
   {
      BaseClass::Finished();
      HandleScope scope(Isolate::GetCurrent());

      // get local handle
      Local<Object> comp = mComponent->GetLocal();

      Handle<String> strfin = String::NewFromUtf8(v8::Isolate::GetCurrent(), "finished");
      Handle<Value> cb = comp->Get(strfin);

      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(comp->CreationContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());

         Handle<Value> ret = func->Call(comp, 0, NULL);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////

#if 0
   ////////////////////////////////////////////////////////////////////////////////
   void ComponentDestructor(v8::Persistent<Value> v, void*)
   {
      HandleScope scope(Isolate::GetCurrent());
      Handle<Object> obj = Handle<Object>::Cast(v);
      Handle<String> strcomp = String::NewFromUtf8(v8::Isolate::GetCurrent(), "__COMPONENT__");
      Handle<Value> hidden = obj->GetHiddenValue(strcomp);
      if(!hidden.IsEmpty())
      {
         void* ptr = Handle<External>::Cast(hidden)->Value();
         delete static_cast<dtEntity::Component*>(ptr);
      }
      v.Dispose();
   }
#endif

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Component* GetOrCreateComponentJS(dtEntity::ComponentType componentType, Handle<Object> obj)
   {
      HandleScope scope(Isolate::GetCurrent());
      Handle<String> strcomp = String::NewFromUtf8(v8::Isolate::GetCurrent(), "__COMPONENT__");
      Handle<Value> v = obj->GetHiddenValue(strcomp);
      if(v.IsEmpty())
      {
         ComponentJS* compjs = new ComponentJS(componentType, obj);
         
         Handle<External> ext = External::New(Isolate::GetCurrent(), compjs);

         obj->SetHiddenValue(strcomp, ext);

         // Ricky on 1/20/2014 - why this persistent should be needed?
         // Persistent<v8::Object> pobj = v8::Persistent<v8::Object>::New(obj);
         // pobj.MakeWeak(NULL, &ComponentDestructor);

         return compjs;
      }
      else
      {
         Handle<External> ext = Handle<External>::Cast(v);
         return static_cast<dtEntity::Component*>(ext->Value());         
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   Local<Function> ExtractFun(Handle<Object> obj, const char* funname)
   {
      EscapableHandleScope scope(Isolate::GetCurrent());
      Handle<String> strFunName = String::NewFromUtf8(v8::Isolate::GetCurrent(), funname);
      Local<Value> val = obj->Get(strFunName);
      if(val.IsEmpty() || !val->IsFunction())
      {
         LOG_ERROR("Entity System method not found: " + std::string(funname));
         return Local<Function>();
      }
      
      return scope.Escape( Local<Function>::Cast(val) ); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntitySystemJS::EntitySystemJS(dtEntity::ComponentType id, dtEntity::EntityManager& em, Handle<Object> obj)
      : BaseClass(em)
      , mComponentType(id)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Context::Scope context_scope(obj->CreationContext());
      mSystem.Reset(isolate, obj);
      mHasCompFun.Reset(isolate, ExtractFun(obj, "hasComponent"));
      mGetCompFun.Reset(isolate, ExtractFun(obj, "getComponent"));
      mCreateCompFun.Reset(isolate, ExtractFun(obj, "createComponent"));
      mDelCompFun.Reset(isolate, ExtractFun(obj, "deleteComponent"));
      mGetESFun.Reset(isolate, ExtractFun(obj, "getEntitiesInSystem"));
      
      mStringGetComponent.Reset(isolate, String::NewFromUtf8(isolate, "getComponent"));
      mStringFinished.Reset(isolate, String::NewFromUtf8(isolate, "finished"));
      mStringOnPropertyChanged.Reset(isolate, String::NewFromUtf8(isolate, "onPropertyChanged"));
      mStringStoreComponentToMap.Reset(isolate, String::NewFromUtf8(isolate, "storeComponentToMap"));
      mStringAllowComponentCreationBySpawner.Reset(isolate, String::NewFromUtf8(isolate, "allowComponentCreationBySpawner"));
      mStringStorePropertiesToScene.Reset(isolate, String::NewFromUtf8(isolate, "storePropertiesToScene"));


      Handle<Array> propnames = obj->GetPropertyNames();
      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(isolate, i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);
         if(propname_str == "componentType")
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SID(propname_str);
         Handle<Value> val = obj->Get(propname);

         //don't convert functions to entity system properties, only primitive values
         if(!val->IsFunction())
         {
#if CALL_ONPROPERTYCHANGED_METHOD == 0
            // handle properties created with createProperty* functions
            Handle<Value> hidden = obj->GetHiddenValue(propname);
            if(!hidden.IsEmpty())
            {
               void* ptr = Handle<External>::Cast(hidden)->Value();
               PropertyGetterSetter* prop = reinterpret_cast<PropertyGetterSetter*>(ptr);

               dtEntity::PropertyGroup::iterator j = mValue.find(propname_sid);
               if(j != mValue.end())
               {
                 delete j->second;
                 mValue.erase(j);
               }
               mValue[propname_sid] = prop;

            }
#else
            dtEntity::Property* prop = ConvertValueToProperty(val);
            if(prop)
            {
               dtEntity::PropertyGroup::iterator j = mValue.find(propname_sid);
               if(j != mValue.end())
               {
                 delete j->second;
                 mValue.erase(j);
               }
               mValue[propname_sid] = prop;

            }
#endif
         }
      }
   }

#if CALL_ONPROPERTYCHANGED_METHOD
   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::OnPropertyChanged(dtEntity::StringId propnamesid, dtEntity::Property& prop)
   {
      std::string propname = dtEntity::GetStringFromSID(propnamesid);
      HandleScope scope(Isolate::GetCurrent());

      Handle<Object> system = Handle<Object>::New(Isolate::GetCurrent(), mSystem);

      Handle<Value> newval = ConvertPropertyToValue(system->CreationContext(), &prop);
      Handle<String> propnamestr = ToJSString(propname);
      system->Set(propnamestr, newval);

      Handle<String> onPropChanged = Handle<String>::New(Isolate::GetCurrent(), mStringOnPropertyChanged);
      Handle<Value> cb = system->Get(onPropChanged);
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(system->CreationContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());
         Handle<Value> argv[2] = { propnamestr, newval};
         Handle<Value> ret = func->Call(system, 2, argv);

         if(ret.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }      
   }
#endif

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::Finished()
   {
      BaseClass::Finished();
      HandleScope scope(Isolate::GetCurrent());

      // get handles
      Handle<Object> system = Handle<Object>::New(Isolate::GetCurrent(), mSystem);
      Handle<String> finishedStr = Handle<String>::New(Isolate::GetCurrent(), mStringFinished);


      Handle<Value> cb = system->Get(finishedStr);
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(system->CreationContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());

         Handle<Value> ret = func->Call(system, 0, NULL);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntitySystemJS::~EntitySystemJS()
   {
      mSystem.Reset();
      mHasCompFun.Reset();
      mGetCompFun.Reset();
      mCreateCompFun.Reset();
      mDelCompFun.Reset();
      mGetESFun.Reset();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::OnRemovedFromEntityManager(dtEntity::EntityManager& em)
   {
      HandleScope scope(Isolate::GetCurrent());

      Handle<Object> system = Handle<Object>::New(Isolate::GetCurrent(), mSystem);
         
      Handle<String> s = String::NewFromUtf8(v8::Isolate::GetCurrent(), "onRemovedFromEntityManager");
      Handle<Value> cb = system->Get(s);

      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(system->CreationContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         Handle<Value> ret = func->Call(system, 0, NULL);

         if(ret.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }     
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::HasComponent(dtEntity::EntityId eid) const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);

      Context::Scope context_scope(system->CreationContext());
      TryCatch try_catch;
      Handle<Value> argv[1] = { Integer::New(isolate, eid) };

      Handle<Function> hasCompFunc = Handle<Function>::New(isolate, mHasCompFun);
      Handle<Value> ret = hasCompFunc->Call(system, 1, argv);

      if(ret.IsEmpty()) 
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }    

   

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::GetComponent(dtEntity::EntityId eid, dtEntity::Component*& component)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);

      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;
      
      Handle<Value> argv[1] = { Integer::New(isolate, eid) };
      Handle<Function> getCompFunc = Handle<Function>::New(isolate, mGetCompFun);
      Handle<Value> ret = getCompFunc->Call(system, 1, argv);

      if(ret.IsEmpty()) 
      {
         ReportException(&try_catch);
         return false;
      }

      if(!ret->IsObject()) 
      {
         return false;
      }
      Handle<Object> obj = Handle<Object>::Cast(ret);
      
      component = GetOrCreateComponentJS(GetComponentType(), obj);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::GetComponent(dtEntity::EntityId eid, const dtEntity::Component*& component) const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(isolate, eid) };
      Handle<Function> getCompFunc = Handle<Function>::New(isolate, mGetCompFun);
      Handle<Value> ret = getCompFunc->Call(system, 1, argv);

      if(ret.IsEmpty() || !ret->IsObject()) 
      {
         ReportException(&try_catch);
         return false;
      }
      if(!ret->IsObject()) 
      {
         return false;
      }
      Handle<Object> obj = Handle<Object>::Cast(ret);

      component = GetOrCreateComponentJS(GetComponentType(), obj);
      return true;
   }      

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::CreateComponent(dtEntity::EntityId eid, dtEntity::Component*& component)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(isolate, eid) };
      Handle<Function> createCompFunc = Handle<Function>::New(isolate, mCreateCompFun);
      Handle<Value> ret = createCompFunc->Call(system, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }

      if(!ret->IsObject())
      {
         return false;
      }
      Handle<Object> obj = Handle<Object>::Cast(ret);
      component = GetOrCreateComponentJS(GetComponentType(), obj);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::DeleteComponent(dtEntity::EntityId eid)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(isolate, eid) };
      Handle<Function> delCompFunc = Handle<Function>::New(isolate, mDelCompFun);
      Handle<Value> ret = delCompFunc->Call(system, 1, argv);

      if(ret.IsEmpty()) 
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }      

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::GetEntitiesInSystem(std::list<dtEntity::EntityId>& toFill) const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;
      Handle<Function> getESFunc = Handle<Function>::New(isolate, mGetESFun);
      Handle<Value> ret = getESFunc->Call(system, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return;
      }
      if(!ret->IsArray())
      {
         LOG_ERROR("getEntitiesInSystem should return an array of entity ids!");
         return;
      }
      Handle<Array> arr = Handle<Array>::Cast(ret);

      for(unsigned int i = 0; i < arr->Length(); ++i)
      {
         toFill.push_back((dtEntity::EntityId) arr->Get(Integer::New(isolate, i))->Uint32Value());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::GroupProperty EntitySystemJS::GetProperties() const
   {
      dtEntity::GroupProperty ret;

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Context::Scope context_scope(system->CreationContext());

      Handle<Array> propnames = system->GetPropertyNames();
      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(isolate, i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);
         if(propname_str == "componentType")
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SIDHash(propname_str);
         Handle<Value> val = system->Get(propname);
         dtEntity::Property* prop = ConvertValueToProperty(val);
         if(prop)
         {
            dtEntity::PropertyGroup::const_iterator j = mValue.find(propname_sid);
            if(j != mValue.end())
            {
              j->second->SetFrom(*prop);
              ret.Add(propname_sid, j->second->Clone());
            }
         }
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::StoreComponentToMap(dtEntity::EntityId id) const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);

      Handle<String> storeComptoMapStr = Handle<String>::New(isolate, mStringStoreComponentToMap);
      if(!system->Has(storeComptoMapStr))
      {
         return true;
      }

      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;
      Handle<Value> argv[1] = { Uint32::New(isolate, id)};
      Local<Function> fun = Local<Function>::Cast(system->Get(storeComptoMapStr));
      Handle<Value> ret = fun->Call(system, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::AllowComponentCreationBySpawner() const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Handle<String> allowCompCreationBySpawnStr = Handle<String>::New(isolate, mStringAllowComponentCreationBySpawner);
      
      if(!system->Has(allowCompCreationBySpawnStr))
      {
         return true;
      }

      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;
      Local<Function> fun = Local<Function>::Cast(system->Get(allowCompCreationBySpawnStr));
      Handle<Value> ret = fun->Call(system, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::StorePropertiesToScene() const
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> system = Handle<Object>::New(isolate, mSystem);
      Handle<String> storePropToMapStr = Handle<String>::New(isolate, mStringStorePropertiesToScene);

      if(!system->Has(storePropToMapStr))
      {
         return true;
      }

      Context::Scope context_scope(system->CreationContext());

      TryCatch try_catch;
      Local<Function> fun = Local<Function>::Cast(system->Get(storePropToMapStr));
      Handle<Value> ret = fun->Call(system, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }
 
}
