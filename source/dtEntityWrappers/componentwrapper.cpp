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
#include <dtEntity/dtentity_config.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{
   dtEntity::StringId s_componentWrapper = dtEntity::SID("ComponenttmWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   void COPropertyGetter(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      dtEntity::Component* component = UnwrapComponent(info.Holder());
      if(component == NULL)
      {
         ThrowError("Trying to access deleted component!");
         return;
      }

      HandleScope scope(Isolate::GetCurrent());

      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());

      switch(prop->GetDataType())
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
         Handle<Value> v;// = info.Holder()->GetHiddenValue(propname);
         if(true)//v.IsEmpty())
         {
            v = ConvertPropertyToValue(info.Holder()->CreationContext(), prop);
           // info.Holder()->SetHiddenValue(propname, v);
         }
         else
         {
            Handle<Value> ret = SetValueFromProperty(prop, v);
            if(ret->BooleanValue() == false) {
               ThrowError("Internal error: Did property change type on the fly?");
               return;
            }
         }
         info.GetReturnValue().Set(v);
         return;
      }

      default:
         info.GetReturnValue().Set( ConvertPropertyToValue(info.Holder()->CreationContext(), prop) );
         return;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COPropertySetter(Local<String> propname,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info)
   {

      assert(!info.Holder().IsEmpty());
      dtEntity::Component* component = UnwrapComponent(info.Holder());
      if(component == NULL)
      {
         LOG_ERROR("Trying to access deleted component!");
         return;
      }

      HandleScope scope(Isolate::GetCurrent());
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      assert(prop);
      SetPropertyFromValue(value, prop);

#if CALL_ONPROPERTYCHANGED_METHOD
      component->OnPropertyChanged(dtEntity::SIDHash(ToStdString(propname)), *prop);
#endif

   }

   ////////////////////////////////////////////////////////////////////////////////
   void COToString(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set( String::NewFromUtf8(v8::Isolate::GetCurrent(), "<Component>") );
   }


   ////////////////////////////////////////////////////////////////////////////////
   void COGetType(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.This());
      if(component == NULL)
      {
         ThrowError("Accessing a deleted component!");
         return;
      }

      args.GetReturnValue().Set( ToJSString(dtEntity::GetStringFromSID(component->GetType())) );;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void COProperties(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.This());
      if(component == NULL)
      {
         ThrowError("Accessing a deleted component!");
         return;
      }

      HandleScope scope(Isolate::GetCurrent());
      Handle<Object> obj = Object::New(Isolate::GetCurrent());

      const dtEntity::PropertyGroup& props = component->Get();
      dtEntity::PropertyGroup::const_iterator i;

      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string propname = dtEntity::GetStringFromSID(i->first);
         const dtEntity::Property* prop = i->second;
         obj->Set(ToJSString(propname), ConvertPropertyToValue(args.This()->CreationContext(), prop));
      }

      args.GetReturnValue().Set(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COFinished(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.This());
      if(component == NULL)
      {
         ThrowError("Accessing a deleted component!");
         return;
      }

      component->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ConstructCO(const v8::FunctionCallbackInfo<Value>& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::Component* co = static_cast<dtEntity::Component*>(ext->Value());
      args.This()->SetInternalField(0, External::New(Isolate::GetCurrent(), co));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COEquals(const FunctionCallbackInfo<Value>& args)
   {
      if(!IsComponent(args[0]))
      {
         args.GetReturnValue().Set( False(Isolate::GetCurrent()) );
         return;
      }

      dtEntity::Component* component = UnwrapComponent(args.This());
      dtEntity::Component* other = UnwrapComponent(args[0]);
      if(other == component)
         args.GetReturnValue().Set( True(Isolate::GetCurrent()));
      else
         args.GetReturnValue().Set( False(Isolate::GetCurrent()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COCopyPropertyValues(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::Component* co = UnwrapComponent(args.This());
      dtEntity::StringId propname = UnwrapSID(args[0]);
      dtEntity::Property* prop = co->Get(propname);
      if(prop == NULL)
      {
         ThrowError("Component has no property named " + dtEntity::GetStringFromSID(UnwrapSID(args[0])));
         return;
      }

      args.GetReturnValue().Set( SetValueFromProperty(prop, args[1]) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Object> WrapComponent(Handle<Object> wrappedes, ScriptSystem* scriptsys, dtEntity::EntityId eid, dtEntity::Component* v)
   {

      EscapableHandleScope scope(Isolate::GetCurrent());

      Local<Object> wrapped = scriptsys->GetFromComponentMap(v->GetType(), eid);
      if(!wrapped.IsEmpty())
      {
         return scope.Escape(wrapped);
      }

      Isolate* isolate = Isolate::GetCurrent();

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_componentWrapper);
      if(templt.IsEmpty())
      {  
         templt = FunctionTemplate::New(isolate);

         templt->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Component"));
         templt->InstanceTemplate()->SetInternalFieldCount(1);

         Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

         proto->Set(String::NewFromUtf8(isolate, "equals"), FunctionTemplate::New(isolate, COEquals));
         proto->Set(String::NewFromUtf8(isolate, "getType"), FunctionTemplate::New(isolate, COGetType));
         proto->Set(String::NewFromUtf8(isolate, "properties"), FunctionTemplate::New(isolate, COProperties));
         proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, COToString));
         proto->Set(String::NewFromUtf8(isolate, "finished"), FunctionTemplate::New(isolate, COFinished));
         proto->Set(String::NewFromUtf8(isolate, "copyPropertyValues"), FunctionTemplate::New(isolate, COCopyPropertyValues));

         GetScriptSystem()->SetTemplateBySID(s_componentWrapper, templt);
      }


      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(isolate, v));
      instance->SetHiddenValue(scriptsys->GetEntityIdString(), Uint32::New(isolate, eid));

      // GetStringFromSID and conversion to v8::String is costly, create a 
      // hidden value in entity system wrapper that stores
      // strings and their string ids as name=>value pairs
      Handle<Value> propnamesval = wrappedes->GetHiddenValue(scriptsys->GetPropertyNamesString());
      if(propnamesval.IsEmpty())
      {
         Handle<Object> names = Object::New(isolate);
         dtEntity::PropertyGroup::const_iterator i;
         const dtEntity::PropertyGroup& props = v->Get();
         for(i = props.begin(); i != props.end(); ++i)
         {
            dtEntity::StringId sid = i->first;
            std::string propname = dtEntity::GetStringFromSID(sid);
            names->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), propname.c_str()), WrapSID(sid));
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
         Handle<External> ext = v8::External::New(isolate, static_cast<void*>(prop));
         instance->SetAccessor(str, COPropertyGetter, COPropertySetter, ext);
      }
      
      // store wrapped component to script system
      scriptsys->AddToComponentMap(v->GetType(), eid, instance);
      return scope.Escape(instance);
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
     HandleScope scope(Isolate::GetCurrent());
     Handle<FunctionTemplate> tmplt = GetScriptSystem()->GetTemplateBySID(s_componentWrapper);
     if(tmplt.IsEmpty())
     {
        return false;
     }
     return tmplt->HasInstance(val);
   }

}
