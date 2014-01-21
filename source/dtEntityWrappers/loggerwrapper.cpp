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
#include <dtEntity/threadsafequeue.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/wrappers.h>
#include <iostream>

using namespace v8;

namespace dtEntityWrappers
{

   dtEntity::StringId s_loggerWrapper = dtEntity::SID("LoggerWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   void LogDebug(const FunctionCallbackInfo<Value>& args)
   {
      LOG_DEBUG(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogAlways(const FunctionCallbackInfo<Value>& args)
   {
      LOG_ALWAYS(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogError(const FunctionCallbackInfo<Value>& args)
   {
      LOG_ERROR(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogInfo(const FunctionCallbackInfo<Value>& args)
   {
      LOG_INFO(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogWarning(const FunctionCallbackInfo<Value>& args)
   {
      LOG_WARNING(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   class LogListenerHolder
         : public dtEntity::LogListener
   {

      struct Msg
      {
         dtEntity::LogLevel::e mLevel;
         std::string mFilename;
         std::string mMethodname;
         int mLinenumber;
         std::string mMsg;
      };

      dtEntity::ThreadSafeQueue<Msg> mMessages;
   public:

      LogListenerHolder(Handle<Function> func)
      {
         Isolate* isolate = v8::Isolate::GetCurrent();

         mFunction.Reset(isolate, func);
         mDebug.Reset(isolate, String::NewFromUtf8(isolate, "DEBUG"));
         mAlways.Reset(isolate, String::NewFromUtf8(isolate, "ALWAYS"));
         mError.Reset(isolate, String::NewFromUtf8(isolate, "ERROR"));
         mInfo.Reset(isolate, String::NewFromUtf8(isolate, "INFO"));
         mWarning.Reset(isolate, String::NewFromUtf8(isolate, "WARNING"));
       }

      ~LogListenerHolder()
      {
         mDebug.Reset();
         mAlways.Reset();
         mError.Reset();
         mInfo.Reset();
         mWarning.Reset();
         mFunction.Reset();
      }


      void Process()
      {
         Isolate* isolate = Isolate::GetCurrent();
         HandleScope scope(isolate);
   
         v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, mFunction);

         Context::Scope context_scope(func->CreationContext());

         Handle<String> loglevel;

         while(!mMessages.Empty())
         {
            Msg msg = mMessages.Pop();
            switch(msg.mLevel)
            {
            case dtEntity::LogLevel::LVL_ALWAYS:  loglevel = v8::Local<v8::String>::New(isolate, mAlways); break;
            case dtEntity::LogLevel::LVL_DEBUG:   loglevel = v8::Local<v8::String>::New(isolate, mDebug); break;
            case dtEntity::LogLevel::LVL_ERROR:   loglevel = v8::Local<v8::String>::New(isolate, mError); break;
            case dtEntity::LogLevel::LVL_INFO :   loglevel = v8::Local<v8::String>::New(isolate, mInfo); break;
            case dtEntity::LogLevel::LVL_WARNING: loglevel = v8::Local<v8::String>::New(isolate, mWarning); break;
            }

   #if defined (_DEBUG)
            bool is_debug = true;
   #else
            bool is_debug = false;
   #endif
            Handle<Value> argv[6] = {
               loglevel,
               ToJSString(msg.mFilename),
               ToJSString(msg.mMethodname),
               Integer::New(isolate, msg.mLinenumber),
               ToJSString(msg.mMsg),
               Boolean::New(isolate, is_debug)
            };


            TryCatch try_catch;
            Handle<Value> result = func->Call(func, 6, argv);

            if(result.IsEmpty())
            {
               ReportException(&try_catch);
            }
         }
      }


      virtual void LogMessage(dtEntity::LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg)
      {
         Msg lmsg;
         lmsg.mLevel = level;
         lmsg.mFilename = filename;
         lmsg.mMethodname = methodname;
         lmsg.mLinenumber = linenumber;
         lmsg.mMsg = msg;

         mMessages.Push(lmsg);
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
   void LogAddListener(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 1 || !args[0]->IsFunction())
      {
         ThrowError("Usage: addLogListener(function(LogLevel, filename, methodname, linenumber, message))");
         return;
      }

      HandleScope scope(Isolate::GetCurrent());
      Handle<Function> func = Handle<Function>::Cast(args[0]);
      LogListenerHolder* h = new LogListenerHolder(func);
      dtEntity::LogManager::GetInstance().AddListener(h);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogProcessListeners(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::LogManager& lm = dtEntity::LogManager::GetInstance();
      unsigned int num = lm.GetNumListeners();
      for(unsigned int i = 0; i < num; ++i)
      {
         dtEntity::LogListener* listener = lm.GetListener(i);
         LogListenerHolder* lh = dynamic_cast<LogListenerHolder*>(listener);
         if(lh)
         {
            lh->Process();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapLogger(Handle<Context> context)
   {
      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope handle_scope(isolate);
      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_loggerWrapper);
      if(templt.IsEmpty())
      {
        templt = FunctionTemplate::New(isolate);
        templt->SetClassName(String::NewFromUtf8(isolate, "Log"));

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set(String::NewFromUtf8(isolate, "always"), FunctionTemplate::New(isolate, LogAlways));
        proto->Set(String::NewFromUtf8(isolate, "debug"), FunctionTemplate::New(isolate, LogDebug));
        proto->Set(String::NewFromUtf8(isolate, "error"), FunctionTemplate::New(isolate, LogError));
        proto->Set(String::NewFromUtf8(isolate, "info"), FunctionTemplate::New(isolate, LogInfo));
        proto->Set(String::NewFromUtf8(isolate, "warning"), FunctionTemplate::New(isolate, LogWarning));
        proto->Set(String::NewFromUtf8(isolate, "addLogListener"), FunctionTemplate::New(isolate, LogAddListener));
        proto->Set(String::NewFromUtf8(isolate, "processLogListeners"), FunctionTemplate::New(isolate, LogProcessListeners));
        GetScriptSystem()->SetTemplateBySID(s_loggerWrapper, templt);
      }
      Local<Object> instance = templt->GetFunction()->NewInstance();
      return handle_scope.Escape(instance);
   }

}
