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

#include "rocketsystemwrapper.h"
#include "rocketcomponent.h"
#include "contextwrapper.h"
#include "elementwrapper.h"
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <Rocket/Core/Factory.h>
#include <Rocket/Core/FontDatabase.h>

using namespace v8;
using namespace dtEntityWrappers;

namespace dtEntityRocket
{

   Persistent<FunctionTemplate> s_rocketSystemTemplate;



   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RSToString(const Arguments& args)
   {
      return String::New("<libRocket>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RSGetContext(const Arguments& args)
   {
      if(args.Length() > 0)
      {
         dtEntity::EntityId id = args[0]->Uint32Value();
         RocketSystem* rs = UnwrapRocketSystem(args.Holder());
         RocketComponent* rc = rs->GetComponent(id);
         if(rc && rc->GetRocketContext() != NULL)
         {
            return WrapContext(rc->GetRocketContext());
         }
      }
      return Null();      
   }   
   
   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapRocketSystem(RocketSystem* v)
   {
      
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(GetGlobalContext());

      Local<Object> instance = s_rocketSystemTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));

      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   RocketSystem* UnwrapRocketSystem(v8::Handle<v8::Value> val)
   {
      RocketSystem* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RSLoadFontFace(const Arguments& args)
   {
      if(args.Length() > 0)
      {
         std::string path = ToStdString(args[0]);
         Rocket::Core::FontDatabase::LoadFontFace(path.c_str());  
      }
      return Undefined();      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RSInstanceElement(const Arguments& args)
   {
      if(args.Length() > 0)
      {
         
         if(args[0]->IsNull())
         {
            return ThrowError("First arg has to be an element, is NULL!");
         }
         Rocket::Core::Element* parent = UnwrapElement(args[0]);
         if(args[0]->IsNull() || parent == NULL)
         {
            return ThrowError("First arg has to be an element!");
         }
         Rocket::Core::XMLAttributes attrs;

         if(args.Length() > 3)
         {
            Handle<Object> obj = Handle<Object>::Cast(args[3]);
            if(!obj.IsEmpty())
            {
               Handle<Array> names = obj->GetPropertyNames();
               for(unsigned int i = 0; i < names->Length(); ++i)
               {
                  Handle<Value> key = names->Get(i);
                  attrs.Set(ToRocketString(key), ValToRocketVariant(obj->Get(key)));
               }
            }
         }
         // Element* parent, const String& instancer, const String& tag, const XMLAttributes& attributes
         Rocket::Core::Element* el = Rocket::Core::Factory::InstanceElement(
            parent,
            ToRocketString(args[1]),
            ToRocketString(args[2]),
            attrs);
         if(el == NULL)
         {
            return Null();
         }
         else
         {
            return WrapElement(el);
         }
      }
      return Undefined();      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitRocketSystemWrapper()
   {
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(GetGlobalContext());
      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      s_rocketSystemTemplate = Persistent<FunctionTemplate>::New(templt);
      templt->SetClassName(String::New("Rocket"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();
      proto->Set("toString", FunctionTemplate::New(RSToString));
      proto->Set("getContext", FunctionTemplate::New(RSGetContext));
      proto->Set("instanceElement", FunctionTemplate::New(RSInstanceElement));
      proto->Set("loadFontFace", FunctionTemplate::New(RSLoadFontFace));
      dtEntityWrappers::RegisterEntitySystempWrapper(RocketComponent::TYPE, templt);
   }
}
