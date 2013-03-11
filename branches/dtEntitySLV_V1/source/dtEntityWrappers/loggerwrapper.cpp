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


      void Process()
      {
         HandleScope scope;
         Context::Scope context_scope(mFunction->CreationContext());

         Handle<String> loglevel;

         while(!mMessages.Empty())
         {
            Msg msg = mMessages.Pop();
            switch(msg.mLevel)
            {
            case dtEntity::LogLevel::LVL_ALWAYS:  loglevel = mAlways; break;
            case dtEntity::LogLevel::LVL_DEBUG:   loglevel = mDebug; break;
            case dtEntity::LogLevel::LVL_ERROR:   loglevel = mError; break;
            case dtEntity::LogLevel::LVL_INFO :   loglevel = mInfo; break;
            case dtEntity::LogLevel::LVL_WARNING: loglevel = mWarning; break;
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
               Integer::New(msg.mLinenumber),
               ToJSString(msg.mMsg),
               Boolean::New(is_debug)
            };


            TryCatch try_catch;
            Handle<Value> result = mFunction->Call(mFunction, 6, argv);

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
   Handle<Value> LogProcessListeners(const Arguments& args)
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
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapLogger(Handle<Context> context)
   {
      v8::HandleScope handle_scope;
      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_loggerWrapper);
      if(templt.IsEmpty())
      {
        templt = FunctionTemplate::New();
        templt->SetClassName(String::New("Log"));

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("always", FunctionTemplate::New(LogAlways));
        proto->Set("debug", FunctionTemplate::New(LogDebug));
        proto->Set("error", FunctionTemplate::New(LogError));
        proto->Set("info", FunctionTemplate::New(LogInfo));
        proto->Set("warning", FunctionTemplate::New(LogWarning));
        proto->Set("addLogListener", FunctionTemplate::New(LogAddListener));
        proto->Set("processLogListeners", FunctionTemplate::New(LogProcessListeners));
        GetScriptSystem()->SetTemplateBySID(s_loggerWrapper, templt);
      }
      Local<Object> instance = templt->GetFunction()->NewInstance();
      return handle_scope.Close(instance);
   }

}
