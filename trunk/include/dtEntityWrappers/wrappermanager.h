#pragma once

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

#include <dtEntityWrappers/export.h>
#include <dtEntity/singleton.h>
#include <map>
#include <v8.h>
#include <assert.h>
#include <string>
#include <set>

namespace dtEntityWrappers
{
   class DTENTITY_WRAPPERS_EXPORT WrapperManager
      : public dtEntity::Singleton<WrapperManager>
   {
   public:
      
      WrapperManager();

      ~WrapperManager();

      /**
        * get global v8 execution context. All scripts are executed in 
        * the same context. This means if one script modifies or 
        * adds a global object, all other scripts will be affected
        * by this.
        */
      v8::Handle<v8::Context> GetGlobalContext();

      /** 
         Load and compile script file, return script handle.
         Uses caching if usecache = true.
         */
      v8::Handle<v8::Script> GetScriptFromFile(const std::string& path);
      

      /** execute JavaScript, returns output from script (last variable in code)*/
      v8::Handle<v8::Value> ExecuteJS(const std::string& code, const std::string& path = "<eval>");
      
      /** load and execute a javascript file */
      v8::Local<v8::Value> ExecuteFile(const std::string& path);

      void ExecuteFileOnce(const std::string& path);
      
      /**
       * Setup a new global context object. All loaded scripts and varibles
       * are discarded!
       */
      void ResetGlobalContext();

   private:
      std::set<std::string> mIncludedFiles;
      
   };
}
