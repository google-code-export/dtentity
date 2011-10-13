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


#include <dtEntityWrappers/loggerwrapper.h>
#include <dtEntity/log.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/wrappers.h>
#include <iostream>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_loggerTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogDebug(const Arguments& args)
   {
      LOG_DEBUG(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogAlways(const Arguments& args)
   {
      LOG_ALWAYS(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogError(const Arguments& args)
   {
      LOG_ERROR(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogInfo(const Arguments& args)
   {
      LOG_INFO(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogWarning(const Arguments& args)
   {
      LOG_WARNING(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapLogger()
   {
      v8::HandleScope handle_scope;
      Handle<Context> context = GetGlobalContext();
      v8::Context::Scope context_scope(context);

      if(s_loggerTemplate.IsEmpty())
      {
        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_loggerTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("Log"));
        
        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("always", FunctionTemplate::New(LogAlways));
        proto->Set("debug", FunctionTemplate::New(LogDebug));
        proto->Set("error", FunctionTemplate::New(LogError));
        proto->Set("info", FunctionTemplate::New(LogInfo));
        proto->Set("warning", FunctionTemplate::New(LogWarning));

      }
      Local<Object> instance = s_loggerTemplate->GetFunction()->NewInstance();
      return handle_scope.Close(instance);
   }

}
