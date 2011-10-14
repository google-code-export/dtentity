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

#include <dtEntityWrappers/wrappermanager.h>

#include <dtEntity/log.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/wrappers.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace v8;

namespace dtEntityWrappers
{
   // stores the global JavaScript context
   Persistent<Context> s_Context;

   ////////////////////////////////////////////////////////////////////////////////
   WrapperManager::WrapperManager()
   {
      ResetGlobalContext();
   }

   ////////////////////////////////////////////////////////////////////////////////
   WrapperManager::~WrapperManager()
   {
      s_Context.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Context> WrapperManager::GetGlobalContext() 
   {
      return s_Context; 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WrapperManager::ResetGlobalContext()
   {
      HandleScope handle_scope;
      if(!s_Context.IsEmpty())
      {
         s_Context.Dispose();
      }

      // create a template for the global object
      Handle<ObjectTemplate> global = ObjectTemplate::New();

      // create persistent global context
      s_Context = Persistent<Context>::New(Context::New(NULL, global));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Script> WrapperManager::GetScriptFromFile(const std::string& path)
   {
     
      std::string code;
      bool success = GetFileContents(path, code);
      if(!success)
      {
         LOG_ERROR("Could not load script file from " + path);
         return Handle<Script>();
      }

      HandleScope handle_scope;
      Context::Scope context_scope(s_Context);
      TryCatch try_catch;
      Local<Script> compiled_script = Script::Compile(String::New(code.c_str()), String::New(path.c_str()));

      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
         return Handle<Script>();
      }

      return handle_scope.Close(compiled_script);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> WrapperManager::ExecuteJS(const std::string& code, const std::string& path)
   {
       // Init JavaScript context
      HandleScope handle_scope;
      Context::Scope context_scope(s_Context);

      // We're just about to compile the script; set up an error handler to
      // catch any exceptions the script might throw.
      TryCatch try_catch;

      // Compile the source code.
      Local<Script> compiled_script = Script::Compile(String::New(code.c_str()), String::New(path.c_str()));

      // if an exception occured
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
   //      return try_catch;
      }

      // Run the script!
      Local<Value> ret = compiled_script->Run();
      if(try_catch.HasCaught())
      {
         ReportException(&try_catch);
   //      return try_catch;
      }

      return handle_scope.Close(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Local<Value> WrapperManager::ExecuteFile(const std::string& path)
   {
      HandleScope handle_scope;

      Handle<Script> script = GetScriptFromFile(path);

      if(!script.IsEmpty())
      {
         v8::Context::Scope context_scope(GetGlobalContext());
         TryCatch try_catch;
         Local<Value> ret = script->Run();
         if(try_catch.HasCaught())
         {
            ReportException(&try_catch);
            return Local<Value>();
         }
         return handle_scope.Close(ret);
      }
      return Local<Value>();
   }
}
