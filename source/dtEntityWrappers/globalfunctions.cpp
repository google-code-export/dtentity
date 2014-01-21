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

#include <dtEntityWrappers/globalfunctions.h>

#include <dtEntity/core.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/profile.h>
#include <dtEntity/systeminterface.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <osgDB/FileNameUtils>
#include <iostream>
#include <fstream>
#include <sstream>
#include <osgDB/FileUtils>

using namespace v8;

namespace dtEntityWrappers
{
   
   ////////////////////////////////////////////////////////////////////////////////
   void PrintLN(const FunctionCallbackInfo<Value>& args)
   {
      for (int i = 0; i < args.Length(); i++)
      {
         std::cout << ToStdString(args[i]);
      }      
      std::cout << "\n";
      fflush(stdout);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Print(const FunctionCallbackInfo<Value>& args)
   {
      for (int i = 0; i < args.Length(); i++)
      {
         std::cout << ToStdString(args[i]);
      }
      fflush(stdout);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FindDataFile(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         ThrowError("usage: findDataFile(string path)");
         return;
      }
      std::string path = ToStdString(args[0]);
      std::string result = dtEntity::GetSystemInterface()->FindDataFile(path);
      args.GetReturnValue().Set( ToJSString(result) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Include(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         ThrowError("usage: include(string path)");
         return;
      }
      std::string path = ToStdString(args[0]);
      args.GetReturnValue().Set( GetScriptSystem()->ExecuteFile(path));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IncludeOnce(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         ThrowError("usage: include_once(string path)");
         return;
      }

      std::string path = ToStdString(args[0]);
      GetScriptSystem()->ExecuteFileOnce(path);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetDataFilePathList(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() == 0 || ! args[0]->IsArray())
      {
         ThrowError("Usage: setDataFilePathList(array)");
         return;
      }

      osgDB::FilePathList pl;

      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Handle<Array>::Cast(args[0]);
      for(unsigned int i = 0; i < arr->Length(); ++i)
      {
         pl.push_back(osgDB::convertFileNameToUnixStyle(ToStdString(arr->Get(i))));
      }
      osgDB::setDataFilePathList(pl);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetDataFilePathList(const FunctionCallbackInfo<Value>& args)
   {
      Isolate* isolate = Isolate::GetCurrent();
      osgDB::FilePathList pl = osgDB::getDataFilePathList();
      HandleScope scope(isolate);
      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());

      for(unsigned int i = 0; i < pl.size(); ++i)
      {
         arr->Set(Integer::New(isolate, i), ToJSString(pl[i]));
      }
      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StartProfile(const FunctionCallbackInfo<Value>& args)
   {
      CProfileManager::Start_Profile(dtEntity::SID(ToStdString(args[0])));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StopProfile(const FunctionCallbackInfo<Value>& args)
   {
      CProfileManager::Stop_Profile();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SID(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set( WrapSID(dtEntity::SID(ToStdString(args[0]))));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetStringFromSID(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set( String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::GetStringFromSID(UnwrapSID(args[0])).c_str()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterGlobalFunctions(ScriptSystem* ss, Handle<Context> context)
   {
      Isolate* isolate = Isolate::GetCurrent();
      //HandleScope handle_scope;
      Context::Scope context_scope(context);
      context->Global()->Set(String::NewFromUtf8(isolate, "findDataFile"), FunctionTemplate::New(isolate, FindDataFile)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "include"), FunctionTemplate::New(isolate, Include)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "include_once"), FunctionTemplate::New(isolate, IncludeOnce)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "print"), FunctionTemplate::New(isolate, Print)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "println"), FunctionTemplate::New(isolate, PrintLN)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "getDataFilePathList"), FunctionTemplate::New(isolate, GetDataFilePathList)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "setDataFilePathList"), FunctionTemplate::New(isolate, SetDataFilePathList)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "sid"), FunctionTemplate::New(isolate, SID)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "getStringFromSid"), FunctionTemplate::New(isolate, GetStringFromSID)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "startProfile"), FunctionTemplate::New(isolate, StartProfile)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "stopProfile"), FunctionTemplate::New(isolate, StopProfile)->GetFunction());
   }
}
