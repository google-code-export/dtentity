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

#include <dtEntity/componentpluginmanager.h>

#include <dtEntity/entitymanager.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/stringid.h>
#include <osgDB/FileUtils>
#include <osgDB/DynamicLibrary>
#include <dtEntity/log.h>

#include <iostream>
#include <sstream>


namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   ComponentPluginManager::ComponentPluginManager(EntityManager& em, MessageFactory& mf)
      : mEntityManager(&em)
      , mMessageFactory(&mf)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentPluginManager::~ComponentPluginManager()
   {
      UnloadAllPlugins();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPluginManager::AddFactory(ComponentPluginFactory* factory)
   {
      mFactories[factory->GetType()] = factory;
   }

   ////////////////////////////////////////////////////////////////////////////////
   /** load all dlls in dir and check for plugin factories */
   void ComponentPluginManager::LoadPluginsInDir(const std::string& path)
   {
      
      if(!osgDB::fileExists(path) || osgDB::fileType(path) != osgDB::DIRECTORY)
      {
         return;
      }

      // find out library extension for this system
      // take care of debug/release library stuff on windows
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#if defined (_DEBUG)       
      std::string libExtension = "d.dll";
#else
      std::string libExtension = ".dll";
#endif
#else
         std::string libExtension = ".so";
#endif

      // get libs from directory
      osgDB::DirectoryContents files = osgDB::getDirectoryContents(path);

      // for each library in dir
      osgDB::DirectoryContents::const_iterator i;
      for(i = files.begin(); i != files.end(); ++i)
      {
         std::string fileName = *i;
         
         if(fileName.size() <= libExtension.size()) continue;

         std::string ending = fileName.substr(fileName.size() - libExtension.size(), fileName.size());
         if(ending.compare(libExtension) != 0)
         {
            continue;
         }
       
         std::ostringstream libpath;
         libpath << path << "/" << fileName;
         AddPlugin(libpath.str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPluginManager::AddPlugin(const std::string& abspath)
   {
     std::list<osg::ref_ptr<ComponentPluginFactory> > factories;
     LoadPluginFactories(abspath, factories);

     if(!factories.empty())
     {
        std::list<osg::ref_ptr<ComponentPluginFactory> >::iterator i;
        for(i = factories.begin(); i != factories.end(); ++i)
        {
           osg::ref_ptr<ComponentPluginFactory> factory = *i;
           assert(factory.valid());

           ComponentType ctype = factory->GetType();

           // check if a plugin with this name already exists
           if(mEntityManager->HasEntitySystem(ctype))
           {
              LOG_ERROR("Unable to load entity system from path " << abspath <<":"
                << " A plugin with type " + GetStringFromSID(ctype) << " was already loaded!");
              continue;
           }

           LOG_DEBUG("Registered entity system " + dtEntity::GetStringFromSID(ctype));
           // insert factory into factory list
           AddFactory(factory);
        }
     }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPluginManager::LoadPluginFactories(const std::string& path, std::list<osg::ref_ptr<ComponentPluginFactory> >& factories)
   {
      // use library sharing manager to do the actual library loading
      
      osg::ref_ptr<osgDB::DynamicLibrary> dynlib = osgDB::DynamicLibrary::loadLibrary(path);
      if(dynlib == NULL)
      {
         LOG_WARNING("Could not load library " + path);
         return;
      }
      osgDB::DynamicLibrary::PROC_ADDRESS messagesaddr = dynlib->getProcAddress("RegisterMessages");
      
      if (messagesaddr)
      {
         // typedef for function pointer to get factory from library
         typedef void (*RegisterMessagesFn)(MessageFactory&);


         RegisterMessagesFn fn = (RegisterMessagesFn) messagesaddr;

         // let plugin create its factories
         fn(*mMessageFactory);

      }

      osgDB::DynamicLibrary::PROC_ADDRESS pluginaddr = dynlib->getProcAddress("CreatePluginFactories");

      //Make sure the plugin actually implemented these functions and they
      //have been exported.
      if (pluginaddr)
      {
         // typedef for function pointer to get factory from library
         typedef void (*CreatePluginFactoriesFn)(std::list<ComponentPluginFactory*>&);

         CreatePluginFactoriesFn fn = (CreatePluginFactoriesFn) pluginaddr;

         std::list<ComponentPluginFactory*> faclist;
         // let plugin create its factories
         fn(faclist);
         
         // store ref_ptr to library in factory, just so that it does not get
         // deleted
         if(!faclist.empty())
         {
            std::list<ComponentPluginFactory*>::iterator i;
            for(i = faclist.begin(); i != faclist.end(); ++i)
            {
               osg::ref_ptr<ComponentPluginFactory> factory = *i;
               factory->SetLibrary(dynlib);
               
               // cannot cleanly unload library yet
               dynlib->ref();
               factories.push_back(factory);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentPluginFactory* ComponentPluginManager::GetPluginFactory(ComponentType ctype)
   {
      PluginFactoryMap::iterator i = mFactories.find(ctype);
      if(i == mFactories.end())
      {
         return NULL;
      }
      return i->second;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ComponentPluginManager::FactoryExists(ComponentType ctype)
   {
      return (mFactories.find(ctype) != mFactories.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ComponentPluginManager::StartEntitySystem(ComponentType ctype)
   {
      if(mEntityManager->HasEntitySystem(ctype))
      {
         return true;
      }
      std::string tname = GetStringFromSID(ctype);
      LOG_DEBUG("Starting entity system " + tname);
      ComponentPluginFactory* factory = GetPluginFactory(ctype);
      
      if(factory == NULL)
      {
         LOG_DEBUG("Cannot start entity system " 
            + tname + std::string(": no factory found"));
         return false;
      }
      // start all plugins this plugin depends on
      std::list<ComponentType> deps;
      factory->GetDependencies(deps);

      while(!deps.empty())
      {
         ComponentType dependency = deps.front();
         deps.pop_front();

         if(mEntityManager->HasEntitySystem(dependency))
         {
            continue;
         }
         // check if dependency can be fulfilled
         if(!FactoryExists(dependency))
         {
            LOG_ERROR("Cannot start plugin " << tname << ": It depends on plugin "
             << GetStringFromSID(dependency) << " which was not found.");
            return false;
         }

         // only start dependency if it is not running now
         if(!mEntityManager->HasEntitySystem(dependency))
         {
            StartEntitySystem(dependency);
         }
      }

      // use factory to create the plugin

      EntitySystem* es;
      bool success = factory->Create(mEntityManager, es);

      if(success)
      {
         // call, although no properties were set yet
         es->Finished();
         LOG_DEBUG("Created entity system of type " + GetStringFromSID(ctype));
         mEntityManager->AddEntitySystem(*es);
         return true;
      }
      else
      {
         LOG_ERROR("Error starting entity system " + tname);
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPluginManager::UnloadAllPlugins()
   {
      while(!mFactories.empty())
      {
         ComponentType ctype = mFactories.begin()->first;
         if(mEntityManager->HasEntitySystem(ctype))
         {
            dtEntity::EntitySystem* es = mEntityManager->GetEntitySystem(ctype);
            if(es == NULL || !mEntityManager->RemoveEntitySystem(*es))
            {
               LOG_ERROR("Could not cleanly remove entity system " + GetStringFromSID(ctype));
            }
         }

         mFactories.erase(mFactories.begin());
      }
   }
}
