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
#include <set>

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
      
      typedef dtEntity::EntitySystem BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      ScriptSystem(dtEntity::EntityManager& em);
      ~ScriptSystem();

      dtEntity::ComponentType GetComponentType() const { return TYPE; }

      virtual void Finished();

      virtual void OnAddedToEntityManager(dtEntity::EntityManager& em);
      virtual void OnRemoveFromEntityManager(dtEntity::EntityManager& em);
      
      void OnSceneLoaded(const dtEntity::Message& msg);
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

      v8::Handle<v8::String> GetEntityIdString() const 
      {
         return v8::Handle<v8::String>::New(v8::Isolate::GetCurrent(), *mpEntityIdString);
         //return mEntityIdString; 
      }


      v8::Handle<v8::String> GetPropertyNamesString() const { return v8::Handle<v8::String>::New(v8::Isolate::GetCurrent(), *mpPropertyNamesString); }

      v8::Handle<v8::FunctionTemplate> GetTemplateBySID(dtEntity::StringId) const;
      void SetTemplateBySID(dtEntity::StringId, v8::Handle<v8::FunctionTemplate>);

   private:
      void SetupContext();
      void FetchGlobalTickFunction();
      
      dtEntity::MessageFunctor mSceneLoadedFunctor;
      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mLoadScriptFunctor;
      
      dtEntity::BoolProperty mDebugEnabled;
      bool mDebugPortOpened;
      dtEntity::ArrayProperty mScripts;
      dtEntity::UIntProperty mDebugPort;

      // Ugly trick needed for MSVC to build - 
      // DLL export complain about v8::Persistent NOT to be copyable
      // so using pointer in the interface. 
      // One could also use copiable traits, but does not seems to be a correct fix.
      // For similar issue see: https://github.com/joyent/node/issues/6410
      v8::Persistent<v8::Context>* mpGlobalContext;
      v8::Persistent<v8::Function>* mpGlobalTickFunction;
      std::set<std::string> mIncludedFiles;

      typedef std::map<std::pair<dtEntity::ComponentType, dtEntity::EntityId>, v8::Persistent<v8::Object>* > ComponentMap;
      ComponentMap mComponentMap;
      v8::Persistent<v8::String>* mpEntityIdString;
      v8::Persistent<v8::String>* mpPropertyNamesString;

      typedef std::map<dtEntity::StringId, v8::Persistent<v8::FunctionTemplate>* > TemplateMap;
      TemplateMap mTemplateMap;
   };

}

