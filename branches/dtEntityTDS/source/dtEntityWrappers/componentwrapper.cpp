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
      return scope.Close(PropToVal(info.Holder()->CreationContext(), prop));
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
      ValToProp(value, prop);      
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
         obj->Set(ToJSString(propname), PropToVal(args.Holder()->CreationContext(), prop));
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
   Handle<Object> WrapComponent(ScriptSystem* scriptsys, dtEntity::EntityId eid, dtEntity::Component* v)
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
      instance->SetHiddenValue(String::New("__entityid__"), Uint32::New(eid));

      const dtEntity::PropertyContainer::PropertyMap& props = v->GetAllProperties();

      dtEntity::PropertyContainer::PropertyMap::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         dtEntity::Property* prop = i->second;
         Handle<External> ext = v8::External::New(static_cast<void*>(prop));
         std::string propname = dtEntity::GetStringFromSID(i->first);
         instance->SetAccessor(ToJSString(propname),
                         COPropertyGetter, COPropertySetter,
                         ext);
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
