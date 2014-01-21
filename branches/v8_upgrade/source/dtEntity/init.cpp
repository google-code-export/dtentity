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

#include <dtEntity/init.h>

#include <dtEntity/componentfactories.h>
#include <dtEntity/core.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/systeminterface.h>

#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif


namespace dtEntity
{
    
   ////////////////////////////////////////////////////////////////////////////////
   // a simple log handler that forwards log messages to osg log system
    void ConsoleLogHandler::LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                   const std::string& msg)
   {
       GetSystemInterface()->LogMessage(level, filename, methodname, linenumber, msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SetupDataPaths(int argc, char** argv, bool checkPaths)
   {
      std::string projectassets = "";
      std::string baseassets = "";

   #ifdef _WIN32
      std::string cwd = _getcwd(NULL, 0);
   #else
      std::string cwd = getcwd(NULL, 0);
   #endif


    #if defined(WIN32) && !defined(__CYGWIN__)
        char pathsep = '\\';
    #else
        char pathsep =  '/';
    #endif
    
      if(GetSystemInterface()->FileExists("ProjectAssets"))
      {
         projectassets = cwd + pathsep + "ProjectAssets";
      }

      if(GetSystemInterface()->FileExists("BaseAssets"))
      {
         baseassets = cwd + pathsep + "BaseAssets";
      }

      const char* env_projectassets = getenv("DTENTITY_PROJECTASSETS");
      if(env_projectassets != NULL)
      {
         projectassets = env_projectassets;
      }
      const char* env_baseassets = getenv("DTENTITY_BASEASSETS");
      if(env_baseassets != NULL)
      {
         baseassets = env_baseassets;
      }

      int curArg = 1;

      while (curArg < argc)
      {
         std::string curArgv = argv[curArg];
         if (!curArgv.empty())
         {
            if (curArgv == "--projectAssets")
            {
               ++curArg;
               if (curArg < argc)
               {
                  projectassets = argv[curArg];
               }

            }
            else if (curArgv == "--baseAssets")
            {
               ++curArg;
               if (curArg < argc)
               {
                  baseassets = argv[curArg];
               }
            }
         }
         ++curArg;
      }

      if((projectassets == "" || baseassets == "") && checkPaths)
      {
         LOG_ERROR("Please give argument --projectAssets and --baseAssets with path to project assets dir!");
         return false;
      }

   #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
      const char separator = ';';
   #else
      const char separator = ':';
   #endif

      // split project assets string by ";" or ":" and add as separate paths
      if(!projectassets.empty())
      {
         std::vector<std::string> pathsplit = dtEntity::split(projectassets, separator);
         for(unsigned int i = 0; i < pathsplit.size(); ++i)
         {
             dtEntity::GetSystemInterface()->AddDataFilePath(pathsplit[i]);            
         }
      }

      if(!baseassets.empty())
      {
         dtEntity::GetSystemInterface()->AddDataFilePath(baseassets);
      }

      return true;

   }

   //////////////////////////////////////////////////////////////////////////
   void AddDefaultEntitySystemsAndFactories(int argc, char** argv, EntityManager& em)
   {

      // this is a required component system, so add it immediately
      MapSystem* mapSystem = new MapSystem(em);
      em.AddEntitySystem(*mapSystem);

      // add factories for aditional entity systems, they will be started lazily
      RegisterStandardFactories(ComponentPluginManager::GetInstance());

   }

}
