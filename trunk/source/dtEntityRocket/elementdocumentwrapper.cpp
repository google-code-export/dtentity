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

#include "elementdocumentwrapper.h"

#include "elementwrapper.h"
#include <dtEntityWrappers/v8helpers.h>
#include <Rocket/Core/ElementDocument.h>

using namespace v8;
using namespace dtEntityWrappers;

namespace dtEntityRocket
{

   Persistent<FunctionTemplate> s_elementDocumentTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EDToString(const Arguments& args)
   {
      return String::New("<Rocket ElementDocument>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EDShow(const Arguments& args)
   {  
      Rocket::Core::ElementDocument* v = UnwrapElementDocument(args.Holder());
      v->Show();   
      return Undefined();      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EDHide(const Arguments& args)
   {
      Rocket::Core::ElementDocument* v = UnwrapElementDocument(args.Holder());
      v->Hide();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> EDClose(const Arguments& args)
   {
      Rocket::Core::ElementDocument* v = UnwrapElementDocument(args.Holder());
      v->Close();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapElementDocument(Rocket::Core::ElementDocument* v)
   {
      
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(GetGlobalContext());

      if(s_elementDocumentTemplate.IsEmpty())
      {
        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        templt->Inherit(GetElementTemplate());
        s_elementDocumentTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("ElementDocument"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);
		  templt->InstanceTemplate()->SetNamedPropertyHandler(
           ELNamedPropertyGetter,
           ELNamedPropertySetter,
           ELNamedPropertyQuery,
           ELNamedPropertyDeleter,
           ELNamedPropertyEnumerator
        );

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(EDToString));
        proto->Set("show", FunctionTemplate::New(EDShow));
        proto->Set("hide", FunctionTemplate::New(EDHide));
        proto->Set("close", FunctionTemplate::New(EDClose));
      }
      Local<Object> instance = s_elementDocumentTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::ElementDocument* UnwrapElementDocument(v8::Handle<v8::Value> val)
   {
      Rocket::Core::ElementDocument* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }
}
