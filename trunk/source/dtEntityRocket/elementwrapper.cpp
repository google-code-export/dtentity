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

#include "elementwrapper.h"
#include "eventlistenerwrapper.h"
#include "eventlistener.h"
#include "rocketcomponent.h"
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <Rocket/Core/Element.h>


using namespace v8;
using namespace dtEntityWrappers;

namespace dtEntityRocket
{

   dtEntity::StringId s_elementWrapper = dtEntity::SID("ElementWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELToString(const Arguments& args)
   {
      return String::New("<Rocket ElementDocument>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELAddEventListener(const Arguments& args)
   {  
      if(args.Length() < 2)
      {
         return ThrowError("Usage: addEventListener(string eventname, function, [phase]");
      }
      std::string eventname = ToStdString(args[0]);
      Handle<Function> func = Handle<Function>::Cast(args[1]);
      
      bool in_capture_phase = false;
      if(args.Length() > 2 && args[2]->BooleanValue()) {
         in_capture_phase = true;
      }
      EventListener* el = new EventListener(func);
      Rocket::Core::Element* v = UnwrapElement(args.This());
      if(v == NULL)
      {
         return ThrowError("Not a valid Rocket Element!");
      }

      v->AddEventListener(eventname.c_str(), el, in_capture_phase);

      Handle<String> listeners = String::New("__LISTENERS__");
      if(!func->Has(listeners))
      {
         func->Set(listeners, Array::New());
      }
      Handle<Array> listenersarr = Handle<Array>::Cast(func->Get(listeners));
      Handle<External> ext = External::New(el);
      listenersarr->Set(Integer::New(listenersarr->Length()), ext);
      
      return Undefined();      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELAppendChild(const Arguments& args)
   {  
      Rocket::Core::Element* element = UnwrapElement(args.This());
      Rocket::Core::Element* child = UnwrapElement(args[0]);  
      if(!child) 
      {
         return ThrowError("Not an element!");
      }
      bool dom_element = true;
      if(args.Length() > 1 && args[1]->BooleanValue() == false)
      {
         dom_element = false;
      }
      element->AppendChild(child, dom_element);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetAddress(const Arguments& args)
   {
      Rocket::Core::Element* element = UnwrapElement(args.This());
      return String::New(element->GetAddress(args[0]->BooleanValue()).CString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELRemoveChild(const Arguments& args)
   {
      Rocket::Core::Element* element = UnwrapElement(args.This());
      Rocket::Core::Element* child = UnwrapElement(args[0]);
      if(!child)
      {
         return ThrowError("Not an element!");
      }
      return Boolean::New(element->RemoveChild(child));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetAttribute(const Arguments& args)
   {  
      if(args.Length() < 1)
      {
         return ThrowError("Usage: getAttribute(string name)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      
      std::string name = ToStdString(args[0]);
      Rocket::Core::Variant* var = element->GetAttribute(name.c_str());
      return RocketVariantToVal(var);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetElementById(const Arguments& args)
   {  
      if(args.Length() < 1)
      {
         return ThrowError("Usage: getElementById(string name)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      
      Rocket::Core::Element* el = element->GetElementById(ToRocketString(args[0]));
      if(el == NULL)
      {
         return Null();
      }
      else
      {
         return WrapElement(args.This()->CreationContext(), el);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetId(const Arguments& args)
   {
      Rocket::Core::Element* element = UnwrapElement(args.This());
      return String::New(element->GetId().CString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELSetId(const Arguments& args)
   {
      if(args.Length() < 1)
      {
         return ThrowError("Usage: setId(string id)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      element->SetId(ToStdString(args[0]).c_str());
      return Undefined();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetInnerRML(const Arguments& args)
   {  
      Rocket::Core::Element* element = UnwrapElement(args.This());
      Rocket::Core::String s;
      element->GetInnerRML(s);
      return String::New(s.CString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetParentNode(const Arguments& args)
   {  
      Rocket::Core::Element* element = UnwrapElement(args.This());
      return WrapElement(args.This()->CreationContext(), element->GetParentNode());
   }

	////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELGetProperty(const Arguments& args)
   {  
      if(args.Length() < 1)
      {
         return ThrowError("Usage: getProperty(string name)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      
      std::string name = ToStdString(args[0]);
		const Rocket::Core::Property* prop = element->GetProperty(name.c_str());
		if(prop == NULL) 
		{
			return Null();
		}
		return String::New(prop->ToString().CString());      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELUpdate(const Arguments& args)
   {

      Rocket::Core::Element* element = UnwrapElement(args.This());
      element->Update();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELSetAttribute(const Arguments& args)
   {  
      if(args.Length() < 2)
      {
         return ThrowError("Usage: setAttribute(string name, value)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      
      std::string name = ToStdString(args[0]);

      Rocket::Core::Variant var = ValToRocketVariant(args[1]);
      element->SetAttribute(name.c_str(), var);
      return RocketVariantToVal(var);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELSetInnerRML(const Arguments& args)
   {
      if(args.Length() < 1)
      {
         return ThrowError("Usage: setInnerRML(string val)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      std::string rml = ToStdString(args[0]);
      element->SetInnerRML(rml.c_str());
      return Undefined();

   }

	////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELSetProperty(const Arguments& args)
   {  
      if(args.Length() < 2)
      {
         return ThrowError("Usage: setProperty(string name, value)");
      }
      Rocket::Core::Element* element = UnwrapElement(args.This());
      
      std::string name = ToStdString(args[0]);
		std::string val = ToStdString(args[1]);

		element->SetProperty(name.c_str(), val.c_str());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELRemoveEventListener(const Arguments& args)
   {  
      if(args.Length() < 2)
      {
         return ThrowError("Usage: removeEventListener(string eventname, function, [phase]");
      }
      std::string eventname = ToStdString(args[0]);
      Handle<Function> func = Handle<Function>::Cast(args[1]);
      bool in_capture_phase = false;
      if(args.Length() > 2 && args[2]->BooleanValue()) {
         in_capture_phase = true;
      }

      Handle<String> listeners = String::New("__LISTENERS__");
      if(func->Has(listeners))
      {
         Handle<Array> listenersarr = Handle<Array>::Cast(func->Get(listeners));
         int l = listenersarr->Length();
         for(int i = 0; i < l; ++i)
         {
            Handle<External> ext = Handle<External>::Cast(listenersarr->Get(i));
            if(!ext.IsEmpty())
            {
               EventListener* el = static_cast<EventListener*>(ext->Value());
               Rocket::Core::Element* v = UnwrapElement(args.This());
               v->RemoveEventListener(eventname.c_str(), el, in_capture_phase);
               return Undefined();
            }
         }
      }
      return ThrowError("Could not remove event listener!");
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELNamedPropertyGetter(Local<String> namestr, const AccessorInfo& info)
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);

      Rocket::Core::Variant* prop = element->GetAttribute(propname);
      if(prop == NULL)
      {
         return Handle<Value>();
      }
      else
      {
         return RocketVariantToVal(*prop);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Boolean> ELNamedPropertyDeleter(Local<String> namestr, const AccessorInfo& info)
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);
      if(element->GetAttribute(propname))
      {
         element->RemoveAttribute(propname);
         return True();
      }
      else
      {
         return False();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELNamedPropertySetter(Local<String> namestr, Local<Value> value, const AccessorInfo& info)
   {      
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);
      element->SetAttribute(propname, ToRocketString(value));
      return value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Integer> ELNamedPropertyQuery(Local<String> namestr,const AccessorInfo& info)   
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);

      const Rocket::Core::Variant* prop = element->GetAttribute(propname);
      if(prop == NULL)
      {
         return Integer::New(None);
      }
      else
      {
         return Integer::New(ReadOnly);
      }
   }
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Array> ELNamedPropertyEnumerator(const AccessorInfo& info)   
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      
      HandleScope scope;
      Handle<Array> arr = Array::New();

      int index = 0;
      Rocket::Core::String name;
      Rocket::Core::String prop;
      while(element->IterateAttributes(index, name, prop))
      {
         const char* str = name.CString();
         arr->Set(Integer::New(index), String::New(str));
      }
      return scope.Close(arr);
   }
   /*

////////////////////////////////////////////////////////////////////////////////
   static Handle<Value> ELNamedPropertyGetter(Local<String> namestr, const AccessorInfo& info)
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);

      const Rocket::Core::Property* prop = element->GetProperty(propname);
      if(prop == NULL)
      {
         return Handle<Value>();;
      }
      else
      {
         return RocketVariantToVal(prop->value);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   static Handle<Boolean> ELNamedPropertyDeleter(Local<String> namestr, const AccessorInfo& info)
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);
      if(element->GetProperty(propname))
      {
         element->RemoveProperty(propname);
         return True();
      }
      else
      {
         return False();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELNamedPropertySetter(Local<String> namestr, Local<Value> value, const AccessorInfo& info)
   {
      
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);
      element->SetProperty(propname, ToRocketString(value));
      return value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Integer> ELNamedPropertyQuery(Local<String> namestr,const AccessorInfo& info)   
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      Rocket::Core::String propname = ToRocketString(namestr);

      const Rocket::Core::Property* prop = element->GetProperty(propname);
      if(prop == NULL)
      {
         return Integer::New(None);
      }
      else
      {
         return Integer::New(ReadOnly);
      }
   }
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Array> ELNamedPropertyEnumerator(const AccessorInfo& info)   
   {
      Rocket::Core::Element* element = UnwrapElement(info.Holder());      
      
      HandleScope scope;
      Handle<Array> arr = Array::New();

      int index = 0;
      Rocket::Core::PseudoClassList pseudoclasses;
      Rocket::Core::String name;
      const Rocket::Core::Property* prop;
      while(element->IterateProperties(index, pseudoclasses, name, prop))
      {
         arr->Set(Integer::New(index), String::New(name.CString()));
      }
      return scope.Close(arr);
   }
*/
   ////////////////////////////////////////////////////////////////////////////////
   void CreateTemplate()
   {
      HandleScope scope;

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_elementWrapper);
      if(templt.IsEmpty())
      {
        templt = FunctionTemplate::New();

        templt->SetClassName(String::New("ElementDocument"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(ELToString));
        proto->Set("addEventListener", FunctionTemplate::New(ELAddEventListener));
        proto->Set("appendChild", FunctionTemplate::New(ELAppendChild));
        proto->Set("getAddress", FunctionTemplate::New(ELGetAddress));
        proto->Set("getAttribute", FunctionTemplate::New(ELGetAttribute));
        proto->Set("getElementById", FunctionTemplate::New(ELGetElementById));
        proto->Set("getId", FunctionTemplate::New(ELGetId));
        proto->Set("getInnerRML", FunctionTemplate::New(ELGetInnerRML));
        proto->Set("getParentNode", FunctionTemplate::New(ELGetParentNode));
		  proto->Set("getProperty", FunctionTemplate::New(ELGetProperty));
        proto->Set("update", FunctionTemplate::New(ELUpdate));
        proto->Set("removeChild", FunctionTemplate::New(ELRemoveChild));
		  proto->Set("removeEventListener", FunctionTemplate::New(ELRemoveEventListener));
        proto->Set("setAttribute", FunctionTemplate::New(ELSetAttribute));
        proto->Set("setId", FunctionTemplate::New(ELSetId));
        proto->Set("setInnerRML", FunctionTemplate::New(ELSetInnerRML));
		  proto->Set("setProperty", FunctionTemplate::New(ELSetProperty));

        
        templt->InstanceTemplate()->SetNamedPropertyHandler(
           ELNamedPropertyGetter,
           ELNamedPropertySetter,
           ELNamedPropertyQuery,
           ELNamedPropertyDeleter,
           ELNamedPropertyEnumerator
        );

        GetScriptSystem()->SetTemplateBySID(s_elementWrapper, templt);

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<FunctionTemplate> GetElementTemplate()
   {
      CreateTemplate();
      return GetScriptSystem()->GetTemplateBySID(s_elementWrapper);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapElement(Handle<Context> context, Rocket::Core::Element* v)
   {      
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(context);

      CreateTemplate();

      Local<Object> instance = GetScriptSystem()->GetTemplateBySID(s_elementWrapper)->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::Element* UnwrapElement(v8::Handle<v8::Value> val)
   {
      Rocket::Core::Element* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }
}
