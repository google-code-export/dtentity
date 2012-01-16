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
#include <dtEntity/entitymanager.h>
#include <dtEntity/profile.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <osgDB/FileUtils>

using namespace v8;

namespace dtEntityWrappers
{
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> PrintLN(const Arguments& args)
   {
      for (int i = 0; i < args.Length(); i++)
      {
         String::Utf8Value val(args[i]);
         fprintf(stdout, "%s", *val);
      }      
      std::cout << "\n";
      fflush(stdout);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> Print(const Arguments& args)
   {
      for (int i = 0; i < args.Length(); i++)
      {
         String::Utf8Value val(args[i]);
         fprintf(stdout, "%s", *val);
      }
      fflush(stdout);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> FindDataFile(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         return ThrowError("usage: findDataFile(string path)");
      }
      std::string path = ToStdString(args[0]);
      std::string result = osgDB::findDataFile(path);
      return String::New(result.c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> Include(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         return ThrowError("usage: include(string path)");
      }
      std::string path = ToStdString(args[0]);
      return WrapperManager::GetInstance().ExecuteFile(path);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IncludeOnce(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         return ThrowError("usage: include_once(string path)");
      }

      std::string path = ToStdString(args[0]);
      WrapperManager::GetInstance().ExecuteFileOnce(path);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SetDataFilePathList(const Arguments& args)
   {
      if(args.Length() == 0 || ! args[0]->IsArray())
      {
         return ThrowError("Usage: setDataFilePathList(array)");
      }
      osgDB::FilePathList pl;

      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(args[0]);
      for(unsigned int i = 0; i < arr->Length(); ++i)
      {
         pl.push_back(ToStdString(arr->Get(i)));
      }
      osgDB::setDataFilePathList(pl);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GetDataFilePathList(const Arguments& args)
   {
      osgDB::FilePathList pl = osgDB::getDataFilePathList();
      HandleScope scope;
      Handle<Array> arr = Array::New();

      for(unsigned int i = 0; i < pl.size(); ++i)
      {
         arr->Set(Integer::New(i), String::New(pl[i].c_str()));
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> StartProfile(const Arguments& args)
   {
      CProfileManager::Start_Profile(dtEntity::SID(ToStdString(args[0])));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> StopProfile(const Arguments& args)
   {
      CProfileManager::Stop_Profile();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterGlobalFunctions(Handle<Context> context)
   {
      HandleScope handle_scope;
      Context::Scope context_scope(context);
      
      context->Global()->Set(String::New("findDataFile"), FunctionTemplate::New(FindDataFile)->GetFunction());
      context->Global()->Set(String::New("include"), FunctionTemplate::New(Include)->GetFunction());
      context->Global()->Set(String::New("include_once"), FunctionTemplate::New(IncludeOnce)->GetFunction());
      context->Global()->Set(String::New("print"), FunctionTemplate::New(Print)->GetFunction());
      context->Global()->Set(String::New("println"), FunctionTemplate::New(PrintLN)->GetFunction());
      context->Global()->Set(String::New("getDataFilePathList"), FunctionTemplate::New(GetDataFilePathList)->GetFunction());
      context->Global()->Set(String::New("setDataFilePathList"), FunctionTemplate::New(SetDataFilePathList)->GetFunction());
      context->Global()->Set(String::New("startProfile"), FunctionTemplate::New(StartProfile)->GetFunction());
      context->Global()->Set(String::New("stopProfile"), FunctionTemplate::New(StopProfile)->GetFunction());
   }
}
