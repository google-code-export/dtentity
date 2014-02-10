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
   struct MapData
   {
      std::string mMapPath;
      std::string mDataPath;
      unsigned int mSaveOrder;

      MapData(const std::string& mappath, const std::string& datapath, unsigned int order)
         : mMapPath(mappath)
         , mDataPath(datapath)
         , mSaveOrder(order)
      {
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
  
   /**
    * The map component holds information about the entity and how to
    * serialize it.
    */
   class DT_ENTITY_EXPORT MapComponent : public Component
   {

   public:
      
      static const ComponentType TYPE;
      static const StringId EntityNameId;
      static const StringId EntityDescriptionId;
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
      std::string GetEntityName() const;
      void SetEntityName(const std::string& v);

      /**
       * string description of entity
       */
      std::string GetEntityDescription() const;
      void SetEntityDescription(const std::string& v);

      /**
       * If entity was created from a spawner then this is set to the spawners name.
       * If string is empty then entity was not created by spawner.
       */
      std::string GetSpawnerName() const;
      void SetSpawnerName(const std::string& v);

      /**
       * @return the spawner the entity was created from or NULL
       * if entity was not created from spawner
       */
      Spawner* GetSpawner() const;
   
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
      DynamicStringProperty mEntityName;
      DynamicStringProperty mEntityDescription;
      StringProperty mMapName;
      DynamicStringProperty mSpawnerNameProp;
      DynamicStringProperty mUniqueId;
      Spawner* mSpawner;
      std::string mUniqueIdStr;
      std::string mEntityNameStr;
      std::string mEntityDescStr;
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

      friend class MapComponent;
      static const ComponentType TYPE;
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

      void GetSpawnerCreatedEntities(const std::string& spawnername, std::vector<EntityId>& ids, bool recursive = true) const;

      /**
       * @return true if entity exists and was spawned by spawner or one of its child spawners
       */
      bool IsSpawnOf(EntityId, const std::string& spawnername) const;

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
        *Get an index representing the order in which maps should be loaded / saved
        * Maps with lowest values are loaded first
        */
      unsigned int GetMapSaveOrder(const std::string& path);
      void SetMapSaveOrder(const std::string& path, int);

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

      // implementation of EntityManager::EntitySystemRequestCallback interface
      virtual bool CreateEntitySystem(EntityManager* em, ComponentType t);

      void AddMapEncoder(MapEncoder* ec);

      MapEncoder* GetEncoderForMap(const std::string& extension) const;
      MapEncoder* GetEncoderForScene(const std::string& extension) const;

   private:

      /**
       * called by component. Don't call, please set the property on
       * the component instead
       * returns true if change succeeded, false if an entity with this id already exists
       */
      bool OnEntityChangedUniqueId(EntityId id, const std::string& oldUniqueId, const std::string& newUniqueid);


      /**
       * Handles SetComponentPropertiesMessage
       */
      void OnSetComponentProperties(const Message& m);

      /**
       * Handles SetSystemPropertiesMessage
       */
      void OnSetSystemProperties(const Message& m);

      void EmitSpawnerDeleteMessages(MapSystem::SpawnerStorage& spawners, const std::string& path);

      typedef std::vector<MapData> LoadedMaps;
      LoadedMaps mLoadedMaps;

      // store spawners in a map
      SpawnerStorage mSpawners;

      // functors for registering to messages
      MessageFunctor mSpawnEntityFunctor;
      MessageFunctor mDeleteEntityFunctor;
      MessageFunctor mResetSystemFunctor;
      MessageFunctor mStopSystemFunctor;
      MessageFunctor mSetComponentPropertiesFunctor;
      MessageFunctor mSetSystemPropertiesFunctor;
      ComponentPluginManager mPluginManager;

      std::map<std::string, EntityId> mEntitiesByUniqueId;

      typedef std::vector<MapEncoder*> MapEncoders;
      MapEncoders mMapEncoders;
   };
}
