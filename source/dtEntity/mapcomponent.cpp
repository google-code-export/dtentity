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

#include <dtEntity/mapcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/rapidxmlmapencoder.h>
#include <assert.h>
#include <dtEntity/spawner.h>
#include <sstream>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <OpenThreads/ScopedLock>
#include <fstream>


#ifdef WIN32
   #include <Rpc.h>
   #include <Rpcdce.h>
#else
#include <uuid/uuid.h>
   #include <sys/stat.h>
   #include <sys/types.h>

#endif

namespace dtEntity
{
   const StringId MapComponent::TYPE(SID("Map"));   
   const StringId MapComponent::EntityNameId(SID("EntityName"));  
   const StringId MapComponent::MapNameId(SID("MapName"));  
   const StringId MapComponent::SpawnerNameId(SID("SpawnerName"));  
   const StringId MapComponent::UniqueIdId(SID("UniqueId"));  
   const StringId MapComponent::SaveWithMapId(SID("SaveWithMap"));
   const StringId MapComponent::VisibleInEntityListId(SID("VisibleInEntityList"));
   
   
   ////////////////////////////////////////////////////////////////////////////
   MapComponent::MapComponent()
      : mSpawnerNameProp(
           DynamicStringProperty::SetValueCB(this, &MapComponent::SetSpawnerName),
           DynamicStringProperty::GetValueCB(this, &MapComponent::GetSpawnerName)
        )
      , mUniqueId(
           DynamicStringProperty::SetValueCB(this, &MapComponent::SetUniqueId),
           DynamicStringProperty::GetValueCB(this, &MapComponent::GetUniqueId)
        )
      , mSpawner(NULL)
      , mOwner(NULL)
   {
      Register(EntityNameId, &mEntityName);
      Register(MapNameId, &mMapName);
      Register(SpawnerNameId, &mSpawnerNameProp);
      Register(UniqueIdId, &mUniqueId);
      Register(SaveWithMapId, &mSaveWithMap);
      Register(VisibleInEntityListId, &mVisibleInEntityList);
      mSaveWithMap.Set(true);
      mVisibleInEntityList.Set(true);

      mUniqueIdStr = MapSystem::CreateUniqueIdString();

   }
    
   ////////////////////////////////////////////////////////////////////////////
   MapComponent::~MapComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(propname == UniqueIdId)
      {
         SetUniqueId(mUniqueId.Get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string MapComponent::GetSpawnerName() const
   {
      return mSpawner == NULL ? "" : mSpawner->GetName();
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::SetSpawnerName(const std::string& name)
   {
      if(name == "")
      {
         mSpawner = NULL;
      }
      else
      {
         MapSystem* ms;
         mOwner->GetEntityManager().GetEntitySystem(TYPE, ms);
         ms->GetSpawner(name, mSpawner);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::SetUniqueId(const std::string& v) { 

      std::string olduid = mUniqueIdStr;
      if(olduid == v) return;

      mUniqueIdStr = v;
      if(mOwner != NULL)
      {
         MapSystem* ms;
         mOwner->GetEntityManager().GetEntitySystem(TYPE, ms);
         ms->OnEntityChangedUniqueId(mOwner->GetId(), olduid, v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////

   const StringId MapSystem::TYPE(SID("Map"));   

   MapSystem::MapSystem(EntityManager& em)
      : DefaultEntitySystem<MapComponent>(em)
      , mCurrentScene("")
   {

      mSpawnEntityFunctor = MessageFunctor(this, &MapSystem::OnSpawnEntity);
      em.RegisterForMessages(SpawnEntityMessage::TYPE, mSpawnEntityFunctor, "MapSystem::OnSpawnEntity");
      mDeleteEntityFunctor = MessageFunctor(this, &MapSystem::OnDeleteEntity);
      em.RegisterForMessages(DeleteEntityMessage::TYPE, mDeleteEntityFunctor, "MapSystem::OnDeleteEntity");

      mStopSystemFunctor = MessageFunctor(this, &MapSystem::OnStopSystem);
      em.RegisterForMessages(StopSystemMessage::TYPE, mStopSystemFunctor, "MapSystem::OnStopSystem");

      RegisterCommandMessages(MessageFactory::GetInstance());
      RegisterSystemMessages(MessageFactory::GetInstance());
   }

   ////////////////////////////////////////////////////////////////////////////
   MapSystem::~MapSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnAddedToEntityManager(dtEntity::EntityManager& em)
   {
      em.AddEntitySystemRequestCallback(this);

      mMapEncoder = new RapidXMLMapEncoder(em);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::CreateComponent(EntityId eid, Component*& component)
   {
      bool ret = BaseClass::CreateComponent(eid, component);
      if(ret)
      {
         MapComponent* mapcomp = static_cast<MapComponent*>(component);
         std::string uid = mapcomp->GetUniqueId();
         if(uid != "")
         {
            if(mEntitiesByUniqueId.find(uid) != mEntitiesByUniqueId.end())
            {
               LOG_ERROR("Entity with this unique id already exists!");
               DeleteComponent(eid);
               return false;
            }

            mEntitiesByUniqueId[uid] = eid;
         }
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::DeleteComponent(EntityId eid)
   {
      ComponentStore::iterator i = mComponents.find(eid);
      if(i != mComponents.end())
      {
         mEntitiesByUniqueId.erase(i->second->GetUniqueId());
      }
      return BaseClass::DeleteComponent(eid);
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnEntityChangedUniqueId(EntityId id, const std::string& oldUniqueId, const std::string& newUniqueId)
   {
      typedef std::map<std::string, EntityId> UIMap;
      MapComponent* comp = GetComponent(id);
      assert(comp != NULL);

      UIMap::iterator i = mEntitiesByUniqueId.find(oldUniqueId);
      if(i != mEntitiesByUniqueId.end())
      {
         if(oldUniqueId == newUniqueId)
         {
            return;
         }
         mEntitiesByUniqueId.erase(i);
      }

      UIMap::iterator j = mEntitiesByUniqueId.find(newUniqueId);
      if(j != mEntitiesByUniqueId.end())
      {
         LOG_ERROR("An entity with unique id " << newUniqueId << " already exists!");
      }  
      else
      {
         mEntitiesByUniqueId[newUniqueId] = id;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::CreateScene(const std::string& datapath, const std::string& mapname)
   {
      UnloadScene();
      SceneLoadedMessage msg;
      msg.SetSceneName(mapname);
      GetEntityManager().EmitMessage(msg);
      mCurrentScene = mapname;
      mCurrentSceneDataPath = datapath;
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::LoadScene(const std::string& path)
   {
      // get data path containing this map
      std::string scenedatapath = "";
      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      std::string abspath = osgDB::findDataFile(path);
      if(abspath == "")
      {
         LOG_ERROR("Cannot find scene file: " << path);
         return false;
      }

      for(osgDB::FilePathList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         std::string datapath = osgDB::convertFileNameToNativeStyle(*i);
         if(osgDB::equalCaseInsensitive(datapath, abspath.substr(0, datapath.length())))
         {
            scenedatapath = *i;
            break;
         }
      }
      if(scenedatapath == "")
      {
         LOG_ERROR("No data path for scene found!");
         return false;
      }

      bool success = mMapEncoder->LoadSceneFromFile(path);
      
      SceneLoadedMessage msg;
      msg.SetSceneName(path);
      GetEntityManager().EmitMessage(msg);
      mCurrentScene = path;
      mCurrentSceneDataPath = scenedatapath;
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::UnloadScene()
   {
      SceneUnloadedMessage msg;
      GetEntityManager().EmitMessage(msg);

      while(!mLoadedMaps.empty())
      {
         UnloadMap(mLoadedMaps.front().mMapPath);
      }
      mCurrentScene = "";
      mCurrentSceneDataPath = "";
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::SaveCurrentScene(bool saveAllMaps)
   {
      std::ostringstream os;
      os << mCurrentSceneDataPath << "/" << mCurrentScene;
      return SaveScene(os.str(), saveAllMaps);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::SaveScene(const std::string& path, bool saveAllMaps)
   {
     bool success = mMapEncoder->SaveSceneToFile(path);

      if(success && saveAllMaps)
      {
         for(LoadedMaps::const_iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
         {
            bool success = SaveMap(i->mMapPath);
            if(!success)
            {
               LOG_ERROR("Could not save map file " << i->mMapPath);
            }
         }
      }

      return success;
   }


   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::MapExists(const std::string& path)
   {
      return (osgDB::findDataFile(path) != "");
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::LoadMap(const std::string& path)
   {
      if(IsMapLoaded(path))
      {
         LOG_ERROR("Map already loaded: " + path);
         return false;
      }

      if(!MapExists(path))
      {
         LOG_ERROR("Map not found: " + path);
         return false;
      }

      // get data path containing this map
      std::string mapdatapath = "";
      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      std::string abspath = osgDB::findDataFile(path);
      for(osgDB::FilePathList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         std::string datapath = osgDB::convertFileNameToNativeStyle(*i);
         if(osgDB::equalCaseInsensitive(datapath, abspath.substr(0, datapath.length())))
         {
            mapdatapath = *i;
            break;
         }
      }
      unsigned int mapsaveorder = mLoadedMaps.size();

      assert(mapdatapath != "");

      MapBeginLoadMessage msg;
      msg.SetMapPath(path);
      msg.SetDataPath(mapdatapath);
      msg.SetSaveOrder(mapsaveorder);
      GetEntityManager().EmitMessage(msg);

      bool success = mMapEncoder->LoadMapFromFile(path);
      if(success)
      {
         mLoadedMaps.push_back(MapData(path, mapdatapath, mLoadedMaps.size()));

         MapLoadedMessage msg1;
         msg1.SetMapPath(path);
         msg1.SetDataPath(mapdatapath);
         msg1.SetSaveOrder(mapsaveorder);
         GetEntityManager().EmitMessage(msg1);
      }
      return success;
   }

   MapSystem::SpawnerStorage GetChildren(MapSystem::SpawnerStorage& spawners, const std::string& spawnername)
   {
      MapSystem::SpawnerStorage ret;
      MapSystem::SpawnerStorage::iterator i;
      for(i = spawners.begin(); i != spawners.end(); ++i)
      {
         Spawner* parent = i->second->GetParent();
         if((spawnername == "" && parent == NULL) ||
            (parent != NULL && parent->GetName() == spawnername))
         {
            ret[i->first] = i->second;
         }
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::EmitSpawnerDeleteMessages(MapSystem::SpawnerStorage& spawners, const std::string& path)
   {
      MapSystem::SpawnerStorage::iterator i;
      for(i = spawners.begin(); i != spawners.end(); ++i)
      {
         Spawner* spawner = i->second;

         if(spawner->GetMapName() == path)
         {
            MapSystem::SpawnerStorage children = GetChildren(mSpawners, spawner->GetName());
            EmitSpawnerDeleteMessages(children, path);
            SpawnerRemovedMessage msg;
            msg.SetName(i->first);
            msg.SetMapName(spawner->GetMapName());
            msg.SetCategory(spawner->GetGUICategory());
            GetEntityManager().EmitMessage(msg);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::UnloadMap(const std::string& path)
   {
      if(!IsMapLoaded(path))
      {
         LOG_ERROR("Cannot unload map: not loaded! " + path);
         return false;
      }
      MapBeginUnloadMessage msg;
      msg.SetMapPath(path);
      GetEntityManager().EmitMessage(msg);

      // first collect entity ids of all entities in this map
      std::vector<EntityId> ids;
      GetEntitiesInMap(path, ids);

      std::vector<EntityId>::const_iterator j;
      for(j = ids.begin(); j != ids.end(); ++j)
      {
         GetEntityManager().RemoveFromScene(*j);
      }

      for(j = ids.begin(); j != ids.end(); ++j)
      {
         GetEntityManager().KillEntity(*j);
      }


      MapSystem::SpawnerStorage children = GetChildren(mSpawners, "");
      EmitSpawnerDeleteMessages(children, path);

      SpawnerStorage::iterator k = mSpawners.begin();
      while(k != mSpawners.end())
      {
         Spawner* spawner = k->second;
         if(spawner->GetMapName() == path)
         {
            mSpawners.erase(k++);
         }
         else
         {
            ++k;
         }
      }

      MapUnloadedMessage msg1;
      msg1.SetMapPath(path);

      for(LoadedMaps::iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         if(i->mMapPath == path)
         {
            mLoadedMaps.erase(i);
            break;
         }
      }

      GetEntityManager().EmitMessage(msg1);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned int MapSystem::GetMapSaveOrder(const std::string& path)
   {
      for(LoadedMaps::iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         if(i->mMapPath == path)
         {
            return i->mSaveOrder;
         }
      }
      return INT_MAX;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::SetMapSaveOrder(const std::string& path, int order)
   {
      for(LoadedMaps::iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         if(i->mMapPath == path)
         {
            MapData data = *i;
            mLoadedMaps.erase(i);
            data.mSaveOrder = order;
            mLoadedMaps.push_back(data);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::DeleteEntitiesByMap(const std::string& mapName)
   {
      if(!IsMapLoaded(mapName))
      {
         LOG_ERROR("Cannot unload map: not loaded! " + mapName);
         return false;
      }
     
      // first collect entity ids of all entities in this map
      std::vector<EntityId> ids;
      GetEntitiesInMap(mapName, ids);

      std::vector<EntityId>::const_iterator j;
      for(j = ids.begin(); j != ids.end(); ++j)
      {
         GetEntityManager().RemoveFromScene(*j);
      }

      for(j = ids.begin(); j != ids.end(); ++j)
      {
         GetEntityManager().KillEntity(*j);
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::SaveMap(const std::string& mappath)
   {
      std::string datapath = "";
      for(LoadedMaps::const_iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         if(i->mMapPath == mappath)
         {
            datapath = i->mDataPath;
            break;
         }
      }
      if(datapath.empty())
      {
         LOG_ERROR("Cannot save map: No map of this name exists!");
         return false;
      }

      std::ostringstream os;
      os << datapath << "/" << mappath;
      bool success = mMapEncoder->SaveMapToFile(mappath, os.str());
      
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::SaveMapAs(const std::string& path, const std::string& copypath)
   {
      if(!IsMapLoaded(path))
      {
         LOG_ERROR("Cannot save map as: No map of this name exists!");
         return false;
      }
      bool success = mMapEncoder->SaveMapToFile(path, copypath);
      
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::AddEmptyMap(const std::string& dataPath, const std::string& mapname)
   {
      if(IsMapLoaded(mapname))
      {
         return false;
      }
      if(osgDB::findDataFile(mapname) != "")
      {
         mLoadedMaps.push_back(MapData(mapname, dataPath, mLoadedMaps.size()));
         return false;
      }

      std::string mappathrel = osgDB::getFilePath(mapname);
      std::ostringstream os; os << dataPath << "/" << mappathrel;
      std::string mappathabs = os.str();

      if(!osgDB::fileExists(mappathabs))
      {
         LOG_ERROR("Cannot create map in directory " << mappathabs << "! Does it exist?");
         return false;
      }

      MapBeginLoadMessage msg;
      msg.SetMapPath(mapname);
      GetEntityManager().EmitMessage(msg);
      mLoadedMaps.push_back(MapData(mapname, dataPath, mLoadedMaps.size()));
      MapLoadedMessage msg2;
      msg2.SetMapPath(mapname);
      GetEntityManager().EmitMessage(msg2);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::IsMapLoaded(const std::string& path) const
   {
      for(LoadedMaps::const_iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         if(i->mMapPath == path)
         {
            return true;
         }
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> MapSystem::GetLoadedMaps() const
   {
      std::vector<std::string> ret;

      for(LoadedMaps::const_iterator i = mLoadedMaps.begin(); i != mLoadedMaps.end(); ++i)
      {
         ret.push_back(i->mMapPath);
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::GetEntitiesInMap(const std::string& mapname, std::vector<EntityId>& toFill) const
   {
      ComponentStore::const_iterator i;
      for(i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         MapComponent* component = i->second;
         if(component->GetMapName() == mapname)
         {
            toFill.push_back(i->first);
         }         
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::AddSpawner(Spawner& spawner)
   {
      SpawnerAddedMessage msg;

      mSpawners[spawner.GetName()] = &spawner;

      msg.SetName(spawner.GetName());
      if(spawner.GetParent() != NULL)
      {
         msg.SetParentName(spawner.GetParent()->GetName());
      }
      msg.SetMapName(spawner.GetMapName());

      GetEntityManager().EmitMessage(msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MapSystem::DeleteSpawner(const std::string& name)
   {
      if(mSpawners.find(name) != mSpawners.end()) {
         SpawnerRemovedMessage msg;

         SpawnerStorage::iterator i = mSpawners.find(name);
         if(i == mSpawners.end()) return false;

         for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
         {
            MapComponent* mc = i->second;
            if(mc->GetSpawnerName() == name)
            {
               mc->SetSpawnerName("");
            }
         }
         msg.SetName(name);

         msg.SetMapName(i->second->GetMapName());
         msg.SetCategory(i->second->GetGUICategory());
         mSpawners.erase(i);

         GetEntityManager().EmitMessage(msg);
         return true;
      }
      else
      {
         return false;
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MapSystem::GetSpawner(const std::string& name, Spawner*& spawner) const
   {
      SpawnerStorage::const_iterator i = mSpawners.find(name);
      if(i == mSpawners.end()) {
         spawner = NULL;
         return false;
      }
      spawner = i->second.get();
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::GetAllSpawners(std::map<std::string, Spawner*>& toFill) const
   {
      SpawnerStorage::const_iterator i = mSpawners.begin();
      for(; i != mSpawners.end(); ++i)
      {
         toFill[i->first] = i->second.get();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::GetAllSpawnerNames(std::vector<std::string>& toFill) const
   {
      SpawnerStorage::const_iterator i = mSpawners.begin();
      for(; i != mSpawners.end(); ++i)
      {
         toFill.push_back(i->first);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MapSystem::Spawn(const std::string& name, Entity& spawned) const
   {
      Spawner* spawner;
      if(!GetSpawner(name, spawner))
      {
         return false;
      }
      return spawner->Spawn(spawned);
   }

   ///////////////////////////////////////////////////////////////////////////////
   EntityId MapSystem::GetEntityIdByUniqueId(const std::string& uniqueId) const
   {
      std::map<std::string, EntityId>::const_iterator i = mEntitiesByUniqueId.find(uniqueId);
      if(i == mEntitiesByUniqueId.end())
         return 0;
      else 
         return i->second;         
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MapSystem::GetEntityByUniqueId(const std::string& name, Entity*& entity) const
   {
      EntityId id = GetEntityIdByUniqueId(name);
      if(id > 0)
      {
         GetEntityManager().GetEntity(id, entity);
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnSpawnEntity(const Message& msg)
   {
      const SpawnEntityMessage& m = static_cast<const SpawnEntityMessage&>(msg);
      Entity* entity;
      GetEntityManager().CreateEntity(entity);

      std::string spawnerName = m.GetSpawnerName();

      bool success = Spawn(spawnerName, *entity);
      if(!success)
      {
         LOG_ERROR("Could not spawn entity: spawner not found: " + spawnerName);
         return;
      }

      MapComponent* comp;
      if(!entity->GetComponent(comp))
      {
         entity->CreateComponent(comp);
      }
      comp->SetUniqueId(m.GetUniqueId());
      comp->SetEntityName(m.GetEntityName());
      if(m.GetAddToScene())
      {
         GetEntityManager().AddToScene(entity->GetId());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnDeleteEntity(const Message& msg)
   {
      const DeleteEntityMessage& m = static_cast<const DeleteEntityMessage&>(msg);

      Entity* entity;
      bool success = GetEntityByUniqueId(m.GetUniqueId(), entity);
      if(!success)
      {
         LOG_ERROR("Cannot delete: Entity with unique id not found: " + m.GetUniqueId());
         return;
      }
      GetEntityManager().RemoveFromScene(entity->GetId());
      GetEntityManager().KillEntity(entity->GetId());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnStopSystem(const Message& msg)
   {
      ComponentPluginManager::GetInstance().UnloadAllPlugins(GetEntityManager());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::GetSpawnerCreatedEntities(const std::string& spawnername, std::vector<EntityId>& ids, bool recursive) const
   {
      ComponentStore::const_iterator i;
      for(i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         MapComponent* component = i->second;
         std::string cspawnername = component->GetSpawnerName();
         if(cspawnername.empty())
         {
            continue;
         }

         Spawner* spwn;
         if(!GetSpawner(cspawnername, spwn))
         {
             continue;
         }

         while(spwn != NULL)
         {
            if(spwn->GetName() == spawnername)
            {
               ids.push_back(i->first);
               break;
            }
            spwn = spwn->GetParent();
            if(!recursive)
            {
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MapSystem::AddToScene(EntityId eid)
   {
      if(!GetEntityManager().EntityExists(eid))
      {
         LOG_ERROR("Cannot add to scene: Entity with this ID not found!");
         return false;
      }
      EntityAddedToSceneMessage msg;
      msg.SetUInt(EntityAddedToSceneMessage::AboutEntityId, eid);

      MapComponent* mc = GetComponent(eid);
      if(mc)
      {
         msg.SetMapName(mc->GetMapName());
         msg.SetEntityName(mc->GetEntityName());
         msg.SetUniqueId(mc->GetUniqueId());
      }

      GetEntityManager().EmitMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MapSystem::RemoveFromScene(EntityId eid)
   {
      if(!GetEntityManager().EntityExists(eid))
      {
         LOG_ERROR("Cannot remove from scene: Entity with this ID not found!");
         return false;
      }
      EntityRemovedFromSceneMessage msg;
      msg.SetUInt(EntityRemovedFromSceneMessage::AboutEntityId, eid);
      MapComponent* mc = GetComponent(eid);
      if(mc)
      {
         msg.SetMapName(mc->GetMapName());
         msg.SetEntityName(mc->GetEntityName());
         msg.SetUniqueId(mc->GetUniqueId());
      }
      GetEntityManager().EmitMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MapSystem::CreateEntitySystem(EntityManager* em, ComponentType t)
   {
      if(ComponentPluginManager::GetInstance().FactoryExists(t))
      {
         ComponentPluginManager::GetInstance().StartEntitySystem(GetEntityManager(), t);
         if(!em->HasEntitySystem(t))
         {
            LOG_ERROR("Factory error: Factory is registered for type but "
                      "does not create entity system with that type");
            return false;
         }
         return true;
      }
      else
      {
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string MapSystem::CreateUniqueIdString()
   {
#ifdef WIN32
   GUID guid;

   if( UuidCreate( &guid ) == RPC_S_OK )
   {
      unsigned char* guidChar;

      if( UuidToString( const_cast<UUID*>(&guid), &guidChar ) == RPC_S_OK )
      {
         std::string str = reinterpret_cast<const char*>(guidChar);
         if(RpcStringFree(&guidChar) != RPC_S_OK)
         {
            LOG_ERROR("Could not free memory.");
         }
         return str;
      }
      else
      {
         LOG_WARNING("Could not convert UniqueId to std::string." );
         return "ERROR";
      }
   }
   else
   {
      LOG_WARNING("Could not generate UniqueId." );
      return "ERROR";
   }
#else
   uuid_t uuid;
   uuid_generate( uuid );

   char buffer[37];
   uuid_unparse(uuid, buffer);

   return buffer;
#endif
   }
}
