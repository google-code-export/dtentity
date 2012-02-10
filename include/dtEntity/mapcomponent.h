#pragma once

/* -*-c++-*-
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

#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/entityid.h>
#include <dtEntity/mapencoder.h>
#include <dtEntity/message.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/property.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>

namespace dtEntity
{
  
   /**
    * The map component holds information about the entity and how to
    * serialize it.
    */
   class DT_ENTITY_EXPORT MapComponent : public Component
   {

   public:
      
      static const ComponentType TYPE;
      static const StringId EntityNameId;
      static const StringId SpawnerNameId;
      static const StringId MapNameId;
      static const StringId UniqueIdId;
      static const StringId SaveWithMapId;
      static const StringId VisibleInEntityListId;
      
      MapComponent();
      virtual ~MapComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void OnAddedToEntity(Entity& entity) { mOwner = &entity; }

      /**
       * string identifier of entity. Does not need to be unique. Mainly used
       * for debugging purposes
       */
      std::string GetEntityName() const { return mEntityName.Get(); }
      void SetEntityName(const std::string& v) { mEntityName.Set(v); }

      /**
       * If entity was created from a spawner then this is set to the spawners name.
       * If string is empty then entity was not created by spawner.
       */
      std::string GetSpawnerName() const;
      void SetSpawnerName(const std::string& v);
   
      /**
       * name of map that entity was loaded from and that it will be stored to
       * if map is saved
       */
      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v) { mMapName.Set(v); }

      /**
       * A unique identifier identifying the entity. May be a random string.
       */
      std::string GetUniqueId() const { return mUniqueIdStr; }
      void SetUniqueId(const std::string& v);

      void SetSaveWithMap(bool v) { mSaveWithMap.Set(v); }
      bool GetSaveWithMap() const { return mSaveWithMap.Get(); }

      void SetVisibleInEntityList(bool v) { mVisibleInEntityList.Set(v); }
      bool GetVisibleInEntityList() const { return mVisibleInEntityList.Get(); }

   private:
      StringProperty mEntityName;
      StringProperty mMapName;
      DynamicStringProperty mSpawnerNameProp;
      DynamicStringProperty mUniqueId;
      Spawner* mSpawner;
      std::string mUniqueIdStr;
      BoolProperty mSaveWithMap;
      BoolProperty mVisibleInEntityList;
      Entity* mOwner;

   };

   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT MapSystem
      : public DefaultEntitySystem<MapComponent>
      , public EntityManager::EntitySystemRequestCallback
   {
   public:

      typedef DefaultEntitySystem<MapComponent> BaseClass;
      typedef std::map<std::string, osg::ref_ptr<Spawner> > SpawnerStorage;


      MapSystem(EntityManager& em);
      ~MapSystem();

      // overridden to check for uniqueness of unique id
      virtual bool CreateComponent(EntityId eid, Component*& component);
      virtual bool DeleteComponent(EntityId eid);

      virtual void OnAddedToEntityManager(dtEntity::EntityManager& em);


      /**
       * Get entity id by unique id. Useful for communicating with the outside world
       */
      EntityId GetEntityIdByUniqueId(const std::string& name) const;

      /**
       * If entity with this name exists, assign its entity object to &entity and return true.
       * Else return false.
       */
      bool GetEntityByUniqueId(const std::string& name, Entity*& entity) const;

      // reacts to SpawnEntityMessage
      void OnSpawnEntity(const Message& msg);

      // reacts to DeleteEntityMessage
      void OnDeleteEntity(const Message& msg);

      // reacts to StopSystemMessage by removing map system from entity manager
      void OnStopSystem(const Message& msg);


      /**
       * Causes a message EntityAddedToSceneMessage to be fired.
       * Layer system reacts to this by adding assigned node to
       * scene graph
       * @param eid Id of entity to add to scene
       * @return true if success
       */
      bool AddToScene(EntityId eid);

      /**
       * Causes a EntityRemovedFromSceneMessage to be fired.
       * Layer system removes attached node from scene graph.
       * @param eid Id of entity to remove from scene
       * @return true if success
       */
      bool RemoveFromScene(EntityId eid);

      /**
       * Register spawner. EntityManager takes ownership of spawner.
       * @threadsafe
       */
      void AddSpawner(Spawner& spawner);

      /**
      * Remove spawner registered by this name and delete it.
      * @return true if spawner was registered under this name
      * @threadsafe
      */
      bool DeleteSpawner(const std::string& name);

      /**
      * if spawner is registered under this name, assign it to
      * @param spawner receives spawner
      * @return true if success
      * @threadsafe
      */
      bool GetSpawner(const std::string& name, Spawner*& spawner) const;

      /**
      * fill up a map with all spawners indexed by their name
      * @threadsafe
      */
      void GetAllSpawners(std::map<std::string, Spawner*>& toFill) const;

      /**
      * fill up a list with names of all registered spawners
      * @threadsafe
      */
      void GetAllSpawnerNames(std::vector<std::string>& toFill) const;

      /**
      * Call spawner to let it set up entity
      * @param name Name of spawner
      * @param spawned A valid, created entity
      * @return true if success
      * @threadsafe
      */
      bool Spawn(const std::string& name, Entity& spawned) const;

      void GetSpawnerCreatedEntities(const std::string& spawnername, std::vector<EntityId>& ids) const;

      /**
       * Load scene from map file, start and configure entity systems
       * as appropriate, load maps
       */
      bool LoadScene(const std::string& path);
      
      /**
       * Unload all maps
       */
      bool UnloadScene();

      /**
       * Unload current scene and create new, empty scene
       */
      bool CreateScene(const std::string& datapath, const std::string& scenepath);

      /**
       * Get path of currently loaded scene
       */
      std::string GetCurrentScene() const { return mCurrentScene; }

      /**
       * Save entity system configurations to current scene file,
       * @param saveAllMaps if set, save all currently loaded maps to map files
       */
      bool SaveCurrentScene(bool saveAllMaps = true);

      /**
       * Save entity system configurations to scene file,
       * @param path Save scene file to this path
       * @param saveAllMaps if set, save all currently loaded maps to map files
       */
      bool SaveScene(const std::string& path, bool saveAllMaps = true);

      /**
       * Return true if map is found in file system
       */
      bool MapExists(const std::string& path);

      /**
       * Load a single map
       */
      bool LoadMap(const std::string& path);

      /**
       * Unload a single map
       */
      bool UnloadMap(const std::string& path);

      /**
       * Delete instances that were created from given map
       */
      bool DeleteEntitiesByMap(const std::string& mapName);

      /**
       * Save a single map
       */
      bool SaveMap(const std::string& path);

      /**
       * Save a single map as a copy
       */
      bool SaveMapAs(const std::string& path, const std::string& copyname);
      
      /**
        * Add an empty, named map to system
        */
      bool AddEmptyMap(const std::string& dataPath, const std::string& mapname);

      /**
       * Check if a map of that name exists
      */
      bool IsMapLoaded(const std::string& mapName) const;

      /**
       * Get a list of all maps in system
       */
      std::vector<std::string> GetLoadedMaps() const;

      /**
       * Get all entities that are stored to this map
       */
      void GetEntitiesInMap(const std::string& mapname, std::vector<EntityId>& toFill) const;
      
      /**
       * Plugin manager is responsible for starting entity systems from shared libraries
       */
      ComponentPluginManager& GetPluginManager() { return mPluginManager; }

      /**
       *called by component. Don't call, please set the property on 
       * the component instead
       */
      void OnEntityChangedUniqueId(EntityId id, const std::string& oldUniqueId, const std::string& newUniqueid);

      MessageFactory& GetMessageFactory() { return mMessageFactory; }


      // implementation of EntityManager::EntitySystemRequestCallback interface
      virtual bool CreateEntitySystem(EntityManager* em, ComponentType t);

   private:

      void EmitSpawnerDeleteMessages(MapSystem::SpawnerStorage& spawners, const std::string& path);

      typedef std::set<std::pair<std::string, std::string> > LoadedMaps;
      LoadedMaps mLoadedMaps;

      // store spawners in a map
      SpawnerStorage mSpawners;

      // functors for registering to messages
      MessageFunctor mSpawnEntityFunctor;
      MessageFunctor mDeleteEntityFunctor;
      MessageFunctor mResetSystemFunctor;
      MessageFunctor mStopSystemFunctor;

      MessageFactory mMessageFactory;

      ComponentPluginManager mPluginManager;

      std::map<std::string, EntityId> mEntitiesByUniqueId;

      std::string mCurrentScene;
      std::string mCurrentSceneDataPath;

      osg::ref_ptr<MapEncoder> mMapEncoder;
   };
}
