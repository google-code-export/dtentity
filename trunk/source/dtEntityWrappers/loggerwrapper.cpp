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
   class LogListenerHolder
         : public dtEntity::LogListener
   {

   public:

      LogListenerHolder(Handle<Function> func)
         : mFunction(Persistent<Function>::New(func))
         , mDebug(Persistent<String>::New(String::New("DEBUG")))
         , mAlways(Persistent<String>::New(String::New("ALWAYS")))
         , mError(Persistent<String>::New(String::New("ERROR")))
         , mInfo(Persistent<String>::New(String::New("INFO")))
         , mWarning(Persistent<String>::New(String::New("WARNING")))
      {
      }

      ~LogListenerHolder()
      {
         mDebug.Dispose();
         mAlways.Dispose();
         mError.Dispose();
         mInfo.Dispose();
         mWarning.Dispose();
         mFunction.Dispose();
      }

      virtual void LogMessage(dtEntity::LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg) const
      {
         HandleScope scope;

         Handle<String> loglevel;
         switch(level)
         {
         case dtEntity::LogLevel::LVL_ALWAYS:  loglevel = mAlways; break;
         case dtEntity::LogLevel::LVL_DEBUG:   loglevel = mDebug; break;
         case dtEntity::LogLevel::LVL_ERROR:   loglevel = mError; break;
         case dtEntity::LogLevel::LVL_INFO :   loglevel = mInfo; break;
         case dtEntity::LogLevel::LVL_WARNING: loglevel = mWarning; break;
         }

         Handle<Value> argv[5] = {
            loglevel,
            String::New(filename.c_str()),
            String::New(methodname.c_str()),
            Integer::New(linenumber),
            String::New(msg.c_str())
         };

         Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;
         Handle<Value> result = mFunction->Call(mFunction, 5, argv);

         if(result.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }

      Persistent<Function> mFunction;
      Persistent<String> mDebug;
      Persistent<String> mAlways;
      Persistent<String> mError;
      Persistent<String> mInfo;
      Persistent<String> mWarning;
      dtEntity::MessageFunctor mFunctor;
      Persistent<String> mMessageTypeStr;
      dtEntity::MessageType mMessageType;
   };

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> LogAddListener(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsFunction())
      {
         return ThrowError("Usage: addLogListener(function(LogLevel, filename, methodname, linenumber, message))");
      }
      HandleScope scope;
      Handle<Function> func = Handle<Function>::Cast(args[0]);
      LogListenerHolder* h = new LogListenerHolder(func);
      dtEntity::LogManager::GetInstance().AddListener(h);
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
        proto->Set("addLogListener", FunctionTemplate::New(LogAddListener));

      }
      Local<Object> instance = s_loggerTemplate->GetFunction()->NewInstance();
      return handle_scope.Close(instance);
   }

}
