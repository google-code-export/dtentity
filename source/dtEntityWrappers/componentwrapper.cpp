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


#include <dtEntityWrappers/componentwrapper.h>

#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntity/component.h>

#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_componentTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COPropertyGetter(Local<String> propname, const AccessorInfo& info)
   {
      dtEntity::Component* component = UnwrapComponent(info.Holder());
      if(component == NULL)
      {
         return ThrowError("Trying to access deleted component!");
      }

      HandleScope scope;

      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      case dtEntity::DataType::VEC2:
      case dtEntity::DataType::VEC2D:
      case dtEntity::DataType::VEC3:
      case dtEntity::DataType::VEC3D:
      case dtEntity::DataType::VEC4:
      case dtEntity::DataType::VEC4D:
      case dtEntity::DataType::GROUP:
      case dtEntity::DataType::MATRIX:
      case dtEntity::DataType::QUAT:
      {
         Handle<Value> v = info.Holder()->GetHiddenValue(propname);
         if(v.IsEmpty())
         {
            v = ConvertPropertyToValue(info.Holder()->CreationContext(), prop);
            info.Holder()->SetHiddenValue(propname, v);
         }
         else
         {
            Handle<Value> ret = SetValueFromProperty(prop, v);
            if(ret->BooleanValue() == false) {
               return ThrowError("Internal error: Did property change type on the fly?");
            }
         }
         return scope.Close(v);
      }
      default:
         return scope.Close(ConvertPropertyToValue(info.Holder()->CreationContext(), prop));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COPropertySetter(Local<String> propname,
                               Local<Value> value,
                               const AccessorInfo& info)
   {

      assert(!info.Holder().IsEmpty());
      dtEntity::Component* component = UnwrapComponent(info.Holder());
      if(component == NULL)
      {
         LOG_ERROR("Trying to access deleted component!");
         return;
      }

      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      assert(prop);
      SetPropertyFromValue(value, prop);
      component->OnPropertyChanged(dtEntity::SIDHash(ToStdString(propname)), *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COToString(const Arguments& args)
   {
      return String::New("<Component>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COGetType(const Arguments& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.Holder());
      if(component == NULL)
      {
         return ThrowError("Accessing a deleted component!");
      }
      return ToJSString(dtEntity::GetStringFromSID(component->GetType()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COProperties(const Arguments& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.Holder());
      if(component == NULL)
      {
         return ThrowError("Accessing a deleted component!");
      }

      HandleScope scope;
      Handle<Object> obj = Object::New();

      const dtEntity::PropertyContainer::PropertyMap& props = component->GetAllProperties();
      dtEntity::PropertyContainer::PropertyMap::const_iterator i;

      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string propname = dtEntity::GetStringFromSID(i->first);
         const dtEntity::Property* prop = i->second;
         obj->Set(ToJSString(propname), ConvertPropertyToValue(args.Holder()->CreationContext(), prop));
      }

      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COFinished(const Arguments& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.Holder());
      if(component == NULL)
      {
         return ThrowError("Accessing a deleted component!");
      }

      component->Finished();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConstructCO(const v8::Arguments& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::Component* co = static_cast<dtEntity::Component*>(ext->Value());
      args.Holder()->SetInternalField(0, External::New(co));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COEquals(const Arguments& args)
   {
      if(!IsComponent(args[0]))
      {
         return False();
      }
      dtEntity::Component* component = UnwrapComponent(args.Holder());
      dtEntity::Component* other = UnwrapComponent(args[0]);
      return (other == component) ? True() : False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Object> WrapComponent(Handle<Object> wrappedes, ScriptSystem* scriptsys, dtEntity::EntityId eid, dtEntity::Component* v)
   {

      HandleScope handle_scope;

      Handle<Object> wrapped = scriptsys->GetFromComponentMap(v->GetType(), eid);
      if(!wrapped.IsEmpty())
      {
         return wrapped;
      }

      Context::Scope context_scope(scriptsys->GetGlobalContext());

      if(s_componentTemplate.IsEmpty())
      {
         Handle<FunctionTemplate> templt = FunctionTemplate::New();
         s_componentTemplate = Persistent<FunctionTemplate>::New(templt);
         templt->SetClassName(String::New("Component"));
         templt->InstanceTemplate()->SetInternalFieldCount(1);

         Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

         proto->Set("equals", FunctionTemplate::New(COEquals));
         proto->Set("getType", FunctionTemplate::New(COGetType));
         proto->Set("properties", FunctionTemplate::New(COProperties));
         proto->Set("toString", FunctionTemplate::New(COToString));
         proto->Set("finished", FunctionTemplate::New(COFinished));
      }

      Local<Object> instance = s_componentTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      instance->SetHiddenValue(scriptsys->GetEntityIdString(), Uint32::New(eid));

      // GetStringFromSID and conversion to v8::String is costly, create a 
      // hidden value in entity system wrapper that stores
      // strings and their string ids as name=>value pairs
      Handle<Value> propnamesval = wrappedes->GetHiddenValue(scriptsys->GetPropertyNamesString());
      if(propnamesval.IsEmpty())
      {
         Handle<Object> names = Object::New();
         dtEntity::PropertyContainer::PropertyMap::const_iterator i;
         const dtEntity::PropertyContainer::PropertyMap& props = v->GetAllProperties();
         for(i = props.begin(); i != props.end(); ++i)
         {
            dtEntity::StringId sid = i->first;
            std::string propname = dtEntity::GetStringFromSID(sid);
            names->Set(String::New(propname.c_str()), WrapSID(sid));
         }
         wrappedes->SetHiddenValue(scriptsys->GetPropertyNamesString(), names);
         propnamesval = names;
      }

      Handle<Object> propnames = Handle<Object>::Cast(propnamesval);
      Handle<Array> keys = propnames->GetPropertyNames();
      for(unsigned int i = 0; i < keys->Length(); ++i)
      {
         Handle<String> str = keys->Get(i)->ToString();
         dtEntity::StringId sid = UnwrapSID(propnames->Get(str));
         dtEntity::Property* prop = v->Get(sid);
         if(prop == NULL)
         {
            LOG_ERROR("Could not find property in component: " << ToStdString(str));
            continue;
         }
         Handle<External> ext = v8::External::New(static_cast<void*>(prop));
         instance->SetAccessor(str, COPropertyGetter, COPropertySetter, ext);
      }
      
      // store wrapped component to script system
      scriptsys->AddToComponentMap(v->GetType(), eid, instance);
      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Component* UnwrapComponent(v8::Handle<v8::Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      dtEntity::Component* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsComponent(v8::Handle<v8::Value> val)
   {
     if(s_componentTemplate.IsEmpty())
     {
       return false;
     }
     return s_componentTemplate->HasInstance(val);
   }

}
