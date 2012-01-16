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

#include "eventlistenerwrapper.h"

#include <dtEntityWrappers/v8helpers.h>
#include <Rocket/Core/EventListener.h>

using namespace v8;
using namespace dtEntityWrappers;

namespace dtEntityRocket
{

   Persistent<FunctionTemplate> s_eventListenerTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ELIToString(const Arguments& args)
   {
      return String::New("<Rocket EventListener>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   /*Handle<Value> ELIShow(const Arguments& args)
   {  
      Rocket::Core::EventListener* v = UnwrapEventListener(args.Holder());
       
      return Undefined();      
   }*/

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapEventListener(Handle<Context> context, Rocket::Core::EventListener* v)
   {      
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(context);

      if(s_eventListenerTemplate.IsEmpty())
      {
        v8::HandleScope handle_scope;
        
        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_eventListenerTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("ElementDocument"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(ELIToString));
       // proto->Set("show", FunctionTemplate::New(ELIShow));
      }

      Local<Object> instance = s_eventListenerTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::EventListener* UnwrapEventListener(v8::Handle<v8::Value> val)
   {
      Rocket::Core::EventListener* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }
}
