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

#include "eventlistener.h"

#include "rocketcomponent.h"
#include "messages.h"
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntity/log.h>
#include "elementwrapper.h"
#include <sstream>
#include <iostream>

namespace dtEntityRocket
{
   using namespace v8;
   using namespace dtEntityWrappers;

   ////////////////////////////////////////////////////////////////////////////////
   Persistent<String> s_BubbleString;
   Persistent<String> s_CaptureString; 
   Persistent<String> s_TargetString;
   Persistent<String> s_UnknownString;
   bool s_StringInit = false;

   ////////////////////////////////////////////////////////////////////////////////
   EventListener::EventListener(Handle<Function> func)
      : mFunc(Persistent<Function>::New(func))        
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventListener::EventListener(const std::string& code)
      : mCode(code)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventListener::~EventListener()
   {
      mFunc.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   static Handle<String> Phase(Rocket::Core::Event::EventPhase p)
   {
      if(!s_StringInit)
      {
         HandleScope scope;
         s_BubbleString = Persistent<String>::New(String::New("Bubble"));
         s_CaptureString = Persistent<String>::New(String::New("Capture"));
         s_TargetString = Persistent<String>::New(String::New("Target"));
         s_UnknownString = Persistent<String>::New(String::New("Unknown"));
      }

      switch(p)
      {
      case Rocket::Core::Event::PHASE_BUBBLE : return s_BubbleString;
      case Rocket::Core::Event::PHASE_CAPTURE : return s_CaptureString;
      case Rocket::Core::Event::PHASE_TARGET : return s_TargetString;
      default: return s_UnknownString;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EventListener::ExecuteCode() 
   {
      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

		std::ostringstream os;
		os << "(function (name, parameters){";
		os << mCode;
		os << "})";
      TryCatch try_catch;
      Local<Script> compiled_script = Script::Compile(String::New(os.str().c_str()), String::New("__Rocket_Inline__"));

      // if an exception occured
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
      }

      // Run the script!
      Local<Value> ret = compiled_script->Run();
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
      }

      if(ret.IsEmpty() || ret->IsNull())
      {
         LOG_ERROR("Cannot parse event handler, has to be a function!");
         return false;
      }
      Handle<Function> func = Handle<Function>::Cast(ret);
      assert(!func.IsEmpty());

      if(!mFunc.IsEmpty()) 
      {
         mFunc.Dispose();
      }
      mFunc = Persistent<Function>::New(func);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventListener::ProcessEvent(Rocket::Core::Event& ev) 
   {
      if(mFunc.IsEmpty() || mFunc->IsNull())
      {
         if(mCode.empty())
         {
            LOG_ERROR("Invalid event handler!");
            return;
         }
         bool success = ExecuteCode();
         if(!success)
         {
            LOG_ERROR("Cannot parse event handler!");
            return;
         }
      }

      HandleScope scope;
      Context::Scope context_scope(GetGlobalContext());

      const Rocket::Core::Dictionary* dict = ev.GetParameters();
      
      Handle<Object> params = Object::New();

      int index = 0;
      Rocket::Core::String key;
      Rocket::Core::Variant* value;
      while (dict->Iterate(index, key, value))
      {
         params->Set(String::New(key.CString()), RocketVariantToVal(*value));            
      }

      Handle<Object> current; 
      if(ev.GetCurrentElement())
      {
         current = WrapElement(ev.GetCurrentElement());
      }

      Handle<Object> target;
      if(ev.GetTargetElement())
      {
         target = WrapElement(ev.GetTargetElement());
      }
      
      TryCatch try_catch;
      Handle<Value> argv[5] = { 
         String::New(ev.GetType().CString()),
         params,
         Phase(ev.GetPhase()),
         current, 
         target
      };
      
      Handle<Value> ret = mFunc->Call(current, 5, argv);
      if(ret.IsEmpty()) 
      {
         ReportException(&try_catch);
      }
   }         
}
