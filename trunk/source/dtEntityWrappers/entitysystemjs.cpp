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
#include <dtEntity/log.h>
#include <dtEntity/property.h>
#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>
#include <iostream>

namespace dtEntityWrappers
{
   using namespace v8;


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> PropertyGetter(Local<String> propname, const AccessorInfo& info)
   {
      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      ComponentJS* comp = static_cast<ComponentJS*>(ext->Value());
      std::string propnamestr = ToStdString(propname);
      dtEntity::Property* prop = comp->Get(dtEntity::SIDHash(propnamestr));
      if(!prop)
      {
         return scope.Close(Undefined());
      }
      return scope.Close(PropToVal(prop));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertySetter(Local<String> propname,
                               Local<Value> value,
                               const AccessorInfo& info)
   {
      
      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      ComponentJS* comp = static_cast<ComponentJS*>(ext->Value());
      std::string propnamestr = ToStdString(propname);
      dtEntity::StringId propnamesid = dtEntity::SIDHash(propnamestr);
      dtEntity::Property* prop = comp->Get(propnamesid);
      if(prop)
      {
         ValToProp(value, prop);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentJS::ComponentJS(dtEntity::ComponentType componentType, Handle<Object> obj)
      : mComponentType(componentType)
      , mComponent(Persistent<Object>::New(obj))
   {

      HandleScope scope;
      Handle<Array> propnames = obj->GetPropertyNames();

      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);

         // don't expose properties beginning with "__"
         if(propname_str.size() > 1 && propname_str[0] == '_' && propname_str[1] == '_')
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SIDHash(propname_str);
         Handle<Value> val = obj->Get(propname);

         // don't convert functions to component properties
         if(!val->IsFunction())
         {
            dtEntity::Property* prop = Convert(val);
            if(prop)
            {
               PropertyMap::iterator j = mProperties.find(propname_sid);
               if(j != mProperties.end())
               {
		           delete j->second;
                 mProperties.erase(j);
	            }        
               mProperties[propname_sid] = prop;     
            }      
            
            Handle<External> ext = v8::External::New(static_cast<void*>(this));
            obj->SetAccessor(propname, PropertyGetter, PropertySetter, Persistent<External>::New(ext));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentJS::~ComponentJS()
   {
      for(PropertyMap::iterator i = mProperties.begin(); i != mProperties.end(); ++i)
      {
         delete i->second;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::OnAddedToEntity(dtEntity::Entity& entity)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::OnPropertyChanged(dtEntity::StringId propnamesid, dtEntity::Property& prop)
   {
      std::string propname = dtEntity::GetStringFromSID(propnamesid);
      HandleScope scope;
         
      /*
      Handle<Value>newval = PropToVal(&prop);
      mComponent->Set(String::New(propname.c_str()), newval);
      */
      Handle<Value> cb = mComponent->Get(String::New("onPropertyChanged"));
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         Handle<Value> argv[1] = { String::New(propname.c_str())};
         Handle<Value> ret = func->Call(mComponent, 1, argv);

         if(ret.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentJS::Finished()
   {
      HandleScope scope;

      Handle<Value> cb = mComponent->Get(String::New("finished"));
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());

         Handle<Value> ret = func->Call(mComponent, 0, NULL);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentDestructor(v8::Persistent<Value> v, void*)
   {
      HandleScope scope;
      Handle<Object> obj = Handle<Object>::Cast(v);
      Handle<Value> hidden = obj->GetHiddenValue(String::New("__COMPONENT__"));
      if(!hidden.IsEmpty())
      {
         void* ptr = Handle<External>::Cast(hidden)->Value();
         delete static_cast<dtEntity::Component*>(ptr);
      }
      v.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Component* GetOrCreateComponentJS(dtEntity::ComponentType componentType, Handle<Object> obj)
   {
      HandleScope scope;
      Handle<Value> v = obj->GetHiddenValue(String::New("__COMPONENT__"));
      if(v.IsEmpty())
      {
         ComponentJS* compjs = new ComponentJS(componentType, obj);
         
         Handle<External> ext = External::New(compjs);

         obj->SetHiddenValue(String::New("__COMPONENT__"), ext);

         Persistent<v8::Object> pobj = v8::Persistent<v8::Object>::New(obj);
         pobj.MakeWeak(NULL, &ComponentDestructor);
         return compjs;
      }
      else
      {
         Handle<External> ext = Handle<External>::Cast(v);
         return static_cast<dtEntity::Component*>(ext->Value());         
      }

   }

   ////////////////////////////////////////////////////////////////////////////////

   Persistent<Function> ExtractFun(Handle<Object> obj, const char* funname)
   {
      HandleScope scope;
      Handle<Value> val = obj->Get(String::New(funname));
      if(val.IsEmpty() || !val->IsFunction())
      {
         LOG_ERROR("Entity System method not found: " + std::string(funname));
         return Persistent<Function>();
      }
      return Persistent<Function>::New(Handle<Function>::Cast(val));
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntitySystemJS::EntitySystemJS(dtEntity::ComponentType id, dtEntity::EntityManager& em, Handle<Object> obj)
      : BaseClass(id, em)
   {
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());
      mSystem = Persistent<Object>::New(obj);
      mHasCompFun = ExtractFun(mSystem, "hasComponent");
      mGetCompFun = ExtractFun(mSystem, "getComponent");
      mCreateCompFun = ExtractFun(mSystem, "createComponent");
      mDelCompFun = ExtractFun(mSystem, "deleteComponent");
      mGetESFun = ExtractFun(mSystem, "getEntitiesInSystem");
      
      Handle<Array> propnames = obj->GetPropertyNames();
      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);
         if(propname_str == "componentType")
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SIDHash(propname_str);
         Handle<Value> val = obj->Get(propname);

         //don't convert functions to entity system properties, only primitive values
         if(!val->IsFunction())
         {
            dtEntity::Property* prop = Convert(val);
            if(prop)
            {
               PropertyMap::iterator j = mProperties.find(propname_sid);
               if(j != mProperties.end())
               {
		           delete j->second;
                 mProperties.erase(j);
	            }        
               mProperties[propname_sid] = prop;     
            }         
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::OnPropertyChanged(dtEntity::StringId propnamesid, dtEntity::Property& prop)
   {
      std::string propname = dtEntity::GetStringFromSID(propnamesid);
      HandleScope scope;
         
      Handle<Value>newval = PropToVal(&prop);
      Handle<String> propnamestr = String::New(propname.c_str());
      mSystem->Set(propnamestr, newval);

      Handle<Value> cb = mSystem->Get(String::New("onPropertyChanged"));
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());
         Handle<Value> argv[2] = { propnamestr, newval};
         Handle<Value> ret = func->Call(mSystem, 2, argv);

         if(ret.IsEmpty()) 
         {
            ReportException(&try_catch);
         }
      }      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::Finished()
   {
      HandleScope scope;

      Handle<Value> cb = mSystem->Get(String::New("finished"));
      if(!cb.IsEmpty() && cb->IsFunction())
      {
         Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;

         Handle<Function> func = Handle<Function>::Cast(cb);
         assert(!func.IsEmpty());

         Handle<Value> ret = func->Call(mSystem, 0, NULL);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntitySystemJS::~EntitySystemJS()
   {
      mSystem.Dispose();
      mHasCompFun.Dispose();
      mGetCompFun.Dispose();
      mCreateCompFun.Dispose();
      mDelCompFun.Dispose();
      mGetESFun.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::HasComponent(dtEntity::EntityId eid) const
   {
      
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());
      TryCatch try_catch;
      Handle<Value> argv[1] = { Integer::New(eid) };
      Handle<Value> ret = mHasCompFun->Call(mSystem, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;
      
      Handle<Value> argv[1] = { Integer::New(eid) };
      Local<Function> fun = Local<Function>::Cast(mSystem->Get(String::New("getComponent")));
      Handle<Value> ret = fun->Call(mSystem, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(eid) };
      Handle<Value> ret = mGetCompFun->Call(mSystem, 1, argv);

      if(ret.IsEmpty() || !ret->IsObject()) 
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
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(eid) };
      Handle<Value> ret = mCreateCompFun->Call(mSystem, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;

      Handle<Value> argv[1] = { Integer::New(eid) };
      Handle<Value> ret = mDelCompFun->Call(mSystem, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());
      TryCatch try_catch;
      Handle<Value> ret = mGetESFun->Call(mSystem, 0, NULL);

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
         toFill.push_back((dtEntity::EntityId) arr->Get(Integer::New(i))->Uint32Value());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::DynamicPropertyContainer EntitySystemJS::GetComponentProperties() const
   {
      
      dtEntity::Component* comp;
      bool success = const_cast<EntitySystemJS*>(this)->CreateComponent(0, comp);
      assert(success);
      ConstPropertyMap m;
      comp->GetProperties(m);
      dtEntity::DynamicPropertyContainer c;
      c.SetProperties(m);
      const_cast<EntitySystemJS*>(this)->DeleteComponent(0);
      return c;
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::GetProperties(PropertyMap& toFill)
   {
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());
      Handle<Array> propnames = mSystem->GetPropertyNames();
      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);
         if(propname_str == "componentType")
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SIDHash(propname_str);
         Handle<Value> val = mSystem->Get(propname);
         dtEntity::Property* prop = Convert(val);
         if(prop)
         {
            PropertyMap::iterator j = mProperties.find(propname_sid);
            if(j != mProperties.end())
            {
		        j->second->SetFrom(*prop);
              toFill[propname_sid] = j->second;
	         }        
            delete prop;
         }         
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntitySystemJS::GetProperties(ConstPropertyMap& toFill) const
   {
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());
      Handle<Array> propnames = mSystem->GetPropertyNames();
      // loop through all properties of object
      for(unsigned int i = 0; i < propnames->Length(); ++i)
      {
         Handle<Value> p = propnames->Get(Integer::New(i));
         Handle<String> propname = Handle<String>::Cast(p);
         std::string propname_str = ToStdString(propname);
         if(propname_str == "componentType")
         {
            continue;
         }
         dtEntity::StringId propname_sid = dtEntity::SIDHash(propname_str);
         Handle<Value> val = mSystem->Get(propname);
         dtEntity::Property* prop = Convert(val);
         if(prop)
         {
            PropertyMap::const_iterator j = mProperties.find(propname_sid);
            if(j != mProperties.end())
            {
		        j->second->SetFrom(*prop);
              toFill[propname_sid] = j->second;
	         }        
            delete prop;
         }         
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntitySystemJS::StoreComponentToMap(dtEntity::EntityId id) const
   {
      HandleScope scope;
      if(!mSystem->Has(String::New("storeComponentToMap")))
      {
         return true;
      }

      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;
      Handle<Value> argv[1] = { Uint32::New(id)};
      Local<Function> fun = Local<Function>::Cast(mSystem->Get(String::New("storeComponentToMap")));
      Handle<Value> ret = fun->Call(mSystem, 1, argv);

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
      HandleScope scope;
      if(!mSystem->Has(String::New("allowComponentCreationBySpawner")))
      {
         return true;
      }

      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;
      Local<Function> fun = Local<Function>::Cast(mSystem->Get(String::New("allowComponentCreationBySpawner")));
      Handle<Value> ret = fun->Call(mSystem, 0, NULL);

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
      HandleScope scope;
      if(!mSystem->Has(String::New("storePropertiesToScene")))
      {
         return true;
      }

      Context::Scope context_scope(GetGlobalContext());

      TryCatch try_catch;
      Local<Function> fun = Local<Function>::Cast(mSystem->Get(String::New("storePropertiesToScene")));
      Handle<Value> ret = fun->Call(mSystem, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }
      return ret->BooleanValue();
   }
 
}
