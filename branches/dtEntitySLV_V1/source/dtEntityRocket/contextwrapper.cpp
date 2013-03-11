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

#include "contextwrapper.h"
#include "rocketcomponent.h"
#include "elementdocumentwrapper.h"

#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <Rocket/Core/Context.h>
#include <Rocket/Core/ElementDocument.h>

using namespace v8;
using namespace dtEntityWrappers;

namespace dtEntityRocket
{
   dtEntity::StringId s_contextWrapper = dtEntity::SID("ContextWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RCToString(const Arguments& args)
   {
      return String::New("<Rocket Context>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RCLoadMouseCursor(const Arguments& args)
   {
      if(args.Length() > 0)
      {
         std::string path = ToStdString(args[0]);

         Rocket::Core::Context* rs = UnwrapContext(args.This());
         Rocket::Core::ElementDocument* doc = rs->LoadMouseCursor(path.c_str());
         return WrapElementDocument(args.This()->CreationContext(), doc);
      }
      return Undefined();      
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RCLoadDocument(const Arguments& args)
   {
      if(args.Length() > 0)
      {
         std::string path = ToStdString(args[0]);

         Rocket::Core::Context* rs = UnwrapContext(args.This());
         Rocket::Core::ElementDocument* doc = rs->LoadDocument(path.c_str());
         if(doc != NULL)
         {
            return WrapElementDocument(args.This()->CreationContext(), doc);
         }
      }
      return Null();
   }

	////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RCUnloadDocument(const Arguments& args)
   {
      if(args.Length() > 0)
      {
			Rocket::Core::ElementDocument* doc = UnwrapElementDocument(args[0]);
			if(doc == NULL)
			{
				return ThrowError("Cannot unload document: not a document!");
			}
         
         Rocket::Core::Context* rs = UnwrapContext(args.This());
         rs->UnloadDocument(doc);
      }
      return Undefined();      
   }
   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapContext(Handle<Context> context, Rocket::Core::Context* v)
   {
      
      v8::HandleScope handle_scope;

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_contextWrapper);
      if(templt.IsEmpty())
      {

        templt = FunctionTemplate::New();

        templt->SetClassName(String::New("RocketContext"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(RCToString));
        proto->Set("loadMouseCursor", FunctionTemplate::New(RCLoadMouseCursor));
        proto->Set("loadDocument", FunctionTemplate::New(RCLoadDocument));
		  proto->Set("unloadDocument", FunctionTemplate::New(RCUnloadDocument));

        GetScriptSystem()->SetTemplateBySID(s_contextWrapper, templt);
      }

      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::Context* UnwrapContext(v8::Handle<v8::Value> val)
   {
      Rocket::Core::Context* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

}
