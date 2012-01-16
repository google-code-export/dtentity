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

#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappermanager.h>
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
      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      return scope.Close(PropToVal(prop));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void COPropertySetter(Local<String> propname,
                               Local<Value> value,
                               const AccessorInfo& info)
   {
      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      ValToProp(value, prop);      
      dtEntity::Component* component = UnwrapComponent(info.Holder());
      assert(component != NULL);
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
      return String::New(dtEntity::GetStringFromSID(component->GetType()).c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COProperties(const Arguments& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.Holder());
      HandleScope scope;
      Handle<Object> obj = Object::New();

      const dtEntity::PropertyContainer::PropertyMap& props = component->GetAllProperties();
      dtEntity::PropertyContainer::PropertyMap::const_iterator i;

      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string propname = dtEntity::GetStringFromSID(i->first);
         const dtEntity::Property* prop = i->second;
         obj->Set(String::New(propname.c_str()), PropToVal(prop));
      }

      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> COFinished(const Arguments& args)
   {
      dtEntity::Component* component = UnwrapComponent(args.Holder());
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
   void ComponentWrapperDestructor(v8::Persistent<Value> v, void*)
   {
      V8::AdjustAmountOfExternalAllocatedMemory(-(int)sizeof(dtEntity::Component));
      v.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapComponent(dtEntity::Component* v)
   {

     v8::HandleScope handle_scope;
     v8::Context::Scope context_scope(GetGlobalContext());

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

     const dtEntity::PropertyContainer::PropertyMap& props = v->GetAllProperties();

     dtEntity::PropertyContainer::PropertyMap::const_iterator i;
     for(i = props.begin(); i != props.end(); ++i)
     {
        Handle<External> ext = v8::External::New(static_cast<void*>(i->second));
        std::string propname = dtEntity::GetStringFromSID(i->first);
        instance->SetAccessor(String::New(propname.c_str()),
                         COPropertyGetter, COPropertySetter,
                         ext);
     }
     Persistent<v8::Object> pobj = v8::Persistent<v8::Object>::New(instance);
     pobj.MakeWeak(NULL, &ComponentWrapperDestructor);
     V8::AdjustAmountOfExternalAllocatedMemory(sizeof(dtEntity::Component));
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
