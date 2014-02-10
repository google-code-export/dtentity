#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <dtEntity/export.h>
#include <dtEntity/singleton.h>
#include <dtEntity/componentplugin.h>
#include <dtEntity/dtentity_config.h>
#include <dtEntity/entityid.h>
#include <list>
#include <map>
#include <set>
#include <string>

namespace dtEntity
{
   class MessageFactory;

   void DT_ENTITY_EXPORT AddToList(std::list<dtEntity::ComponentPluginFactory*>& lst, int count, ...);

   // typedef for function pointer to get message factory from library
   typedef void (*RegisterMessagesFn)(MessageFactory&);

   // typedef for function pointer to get entity system factory from library
   typedef void (*CreatePluginFactoriesFn)(std::list<ComponentPluginFactory*>&);

   /**
    * The plugin manager is responsible for loading entity system factories from plugins,
    * keep them around until they are needed, and start and stop the entity systems.
    */
   class DT_ENTITY_EXPORT ComponentPluginManager
      : public Singleton<ComponentPluginManager>
   {
      
   public:

      ComponentPluginManager();
      ~ComponentPluginManager();

      typedef std::map<ComponentType, ComponentPluginFactory*> PluginFactoryMap;
      typedef std::set<ComponentType> ActivePlugins;

      /**
       * is there a factory for an entity system of this type?
       */
      bool FactoryExists(ComponentType ctype);

      /**
       * load all libraries in dir and add component factories to registry
       */
      void LoadPluginsInDir(const std::string& path);

      /**
       * If entity of this type is registered, start it and add it to entity manager.
       * @param name Name of plugin to start
       * @return true if success
       */
      bool StartEntitySystem(EntityManager&, ComponentType ctype);

      /**
       * stop all entity systems loaded from plugins and then unref the plugins
       */
      void UnloadAllPlugins(EntityManager&);

      /**
       * Directly add a factory for starting an entity system (instead of loading
       * it from a plugin
       */
      bool AddFactory(ComponentPluginFactory* factory);

      /**
       * Load a shared library / dll from abs path and register
       * the contained entity system factories
       * A library can also be stored in the scene file, if desired (this can make it easier
       * for applications to know what specific plugins to load).
       * @abspath File name with path of plugin to load
       * @saveWithScene Flag to tell if library will be saved with the scene (false by default)
       *
       * @return The list of new component types loaded from the plugin
       */
      std::set<ComponentType> AddPlugin(const std::string& abspath, bool saveWithScene = false);

      /**
       * same as before, but doesn't expect a full path to plugin, instead takes
        * a plugin directory and a library name without extension.
       */
      std::set<ComponentType> AddPlugin(const std::string& path, const std::string& libname, bool saveWithScene = false);

      /// Retrieves the names of the currently loaded plugins (with no path and no extension)
      const std::map<std::string, bool>& GetLoadedPlugins() const { return mLoadedPlugins; }

      PluginFactoryMap& GetFactories() { return mFactories; }

      // get extension appended to lib name to point to shared library name
      static std::string GetLibExtension();

      // get library file name from plugin name, platform dependent
      std::string GetSharedLibNameFromPluginName(const std::string& pluginName);

   private:

      /** Get PluginFactory for Plugin with this name.
       * @param name The name of the Plugin/PluginFactory to get
       */
      ComponentPluginFactory* GetPluginFactory(ComponentType ctype);

      /** load all plugin factories from libraries found in path */
      void LoadPluginFactories(const std::string& pluginname, const std::string& path, std::list<ComponentPluginFactory*>& factories);

      /** map from plugin name -> plugin factory */
      PluginFactoryMap mFactories;

      /// List of libraries (plugins) currently loaded
      /**
      *  We assume the library name is the same as the file name, with
      *  no path nor extension. We therefore just store the library name
      *
         The map stores each name with a flag that indicates if the library must be saved to
         scene. It is up to the code filling this list to decide what libs must go to file.
      */
      std::map<std::string, bool> mLoadedPlugins;
   };

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
  #define DTE_EXPORT_MACRO __declspec(dllexport)
#else
   #define DTE_EXPORT_MACRO
#endif

#define REGISTER_DTENTITYPLUGIN(pluginname, count, ...) \
   extern "C" DTE_EXPORT_MACRO void dtEntity_##pluginname(std::list<dtEntity::ComponentPluginFactory*>& list) \
   {dtEntity::AddToList(list, count, __VA_ARGS__);} \

   struct DT_ENTITY_EXPORT PluginFunctionProxy
   {
       PluginFunctionProxy(CreatePluginFactoriesFn plgfunction, RegisterMessagesFn msgfunction) ;
   };
}

#if DTENTITY_LIBRARY_STATIC

   #define USE_DTENTITYPLUGIN(pluginname)  \
      extern "C" void dtEntity_##pluginname(std::list<dtEntity::ComponentPluginFactory*>&); \
      extern "C" void dtEntityMessages_##pluginname(dtEntity::MessageFactory& mf); \
      static dtEntity::PluginFunctionProxy proxy_##pluginname(dtEntity_##pluginname, dtEntityMessages_##pluginname);

#else
   #define USE_DTENTITYPLUGIN(pluginname)
#endif


