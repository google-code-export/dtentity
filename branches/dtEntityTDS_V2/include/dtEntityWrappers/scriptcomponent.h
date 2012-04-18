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

#include <v8.h>

#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/property.h>
#include <osgViewer/View>

namespace dtEntityWrappers
{
     
   ////////////////////////////////////////////////////////////////////////////
   class DTENTITY_WRAPPERS_EXPORT ScriptSystem
      : public dtEntity::EntitySystem
      , public dtEntity::EntityManager::ComponentDeletedCallback
   {
      static const dtEntity::StringId ScriptsId;
      static const dtEntity::StringId DebugPortId;
      static const dtEntity::StringId DebugEnabledId;
      
   public:

      static const dtEntity::ComponentType TYPE;

      ScriptSystem(dtEntity::EntityManager& em);
      ~ScriptSystem();

      dtEntity::ComponentType GetComponentType() const { return TYPE; }

      virtual void Finished();

      virtual void OnAddedToEntityManager(dtEntity::EntityManager& em);
      virtual void OnRemoveFromEntityManager(dtEntity::EntityManager& em);
      
      void OnSceneLoaded(const dtEntity::Message& msg);
      void OnResetSystem(const dtEntity::Message& msg);
      void OnLoadScript(const dtEntity::Message& msg);
      void Tick(const dtEntity::Message& msg);

      

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


      // look in given directory for scripts ending with "*.js",
      // execute them and try to add entity system with name
      // equal to the file name
      void LoadAutoStartScripts(const std::string& path);

      virtual bool StorePropertiesToScene() const { return true; }

      v8::Handle<v8::Context> GetGlobalContext();

      /* Methods to store and retrieve component wrappers.
       * Component wrappers are stored to prevent creating different
       * wrappers for the same component, also for being able to invalidate
       * wrappers once the wrapped component is deleted
       */
      void AddToComponentMap(dtEntity::ComponentType, dtEntity::EntityId, v8::Handle<v8::Object>);
      v8::Handle<v8::Object> GetFromComponentMap(dtEntity::ComponentType, dtEntity::EntityId) const;
      bool RemoveFromComponentMap(dtEntity::ComponentType, dtEntity::EntityId);


      // implementation of dtEntity::ComponentDeletedCallback
      virtual void ComponentDeleted(dtEntity::ComponentType t, dtEntity::EntityId id);

   private:
      void SetupContext();
      void FetchGlobalTickFunction();
      
      dtEntity::MessageFunctor mSceneLoadedFunctor;
      dtEntity::MessageFunctor mResetSystemFunctor;
      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mLoadScriptFunctor;
      
      dtEntity::BoolProperty mDebugEnabled;
      bool mDebugPortOpened;
      dtEntity::ArrayProperty mScripts;
      dtEntity::UIntProperty mDebugPort;

      v8::Persistent<v8::Context> mGlobalContext;
      v8::Persistent<v8::Function> mGlobalTickFunction;
      std::set<std::string> mIncludedFiles;

      typedef std::map<std::pair<dtEntity::ComponentType, dtEntity::EntityId>, v8::Persistent<v8::Object> > ComponentMap;
      ComponentMap mComponentMap;
      
   };

}
