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

#include <dtEntity/core.h>
#include <dtEntity/fileutils.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/uniqueid.h>
#include <dtEntity/rapidxmlmapencoder.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/dtentity_config.h>
#include <assert.h>
#include <dtEntity/spawner.h>
#include <sstream>
#include <OpenThreads/ScopedLock>
#include <fstream>
#include <climits>

#if PROTOBUF_FOUND
#include <dtEntity/protobufmapencoder.h>
#endif


namespace dtEntity
{
   const StringId MapComponent::TYPE(dtEntity::SID("Map"));   
   const StringId MapComponent::EntityNameId(dtEntity::SID("EntityName"));  
   const StringId MapComponent::EntityDescriptionId(dtEntity::SID("EntityDescription"));  
   const StringId MapComponent::MapNameId(dtEntity::SID("MapName"));  
   const StringId MapComponent::SpawnerNameId(dtEntity::SID("SpawnerName"));  
   const StringId MapComponent::UniqueIdId(dtEntity::SID("UniqueId"));  
   const StringId MapComponent::SaveWithMapId(dtEntity::SID("SaveWithMap"));
   const StringId MapComponent::VisibleInEntityListId(dtEntity::SID("VisibleInEntityList"));
   
   
   ////////////////////////////////////////////////////////////////////////////
   MapComponent::MapComponent()
      : mEntityName(
        DynamicStringProperty::SetValueCB(this, &MapComponent::SetEntityName),
        DynamicStringProperty::GetValueCB(this, &MapComponent::GetEntityName)
        )
      , mEntityDescription(
        DynamicStringProperty::SetValueCB(this, &MapComponent::SetEntityDescription),
        DynamicStringProperty::GetValueCB(this, &MapComponent::GetEntityDescription)
        )
      , mSpawnerNameProp(
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
      Register(EntityDescriptionId, &mEntityDescription);
      Register(MapNameId, &mMapName);
      Register(SpawnerNameId, &mSpawnerNameProp);
      Register(UniqueIdId, &mUniqueId);
      Register(SaveWithMapId, &mSaveWithMap);
      Register(VisibleInEntityListId, &mVisibleInEntityList);
      mSaveWithMap.Set(true);
      mVisibleInEntityList.Set(true);

      mUniqueIdStr = CreateUniqueIdString();

   }
    
   ////////////////////////////////////////////////////////////////////////////
   MapComponent::~MapComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string MapComponent::GetEntityName() const
   {
      return mEntityNameStr;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::SetEntityName(const std::string& v)
   {
      if(v != mEntityNameStr)
      {
         mEntityNameStr = v;
         EntityNameUpdatedMessage msg;
         msg.SetAboutEntityId(mOwner->GetId());
         msg.SetEntityName(v);
         msg.SetUniqueId(mUniqueId.Get());
         mOwner->GetEntityManager().EmitMessage(msg);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string MapComponent::GetEntityDescription() const
   {
      return mEntityDescStr;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::SetEntityDescription(const std::string& v)
   {
      if(v != mEntityDescStr)
      {
         mEntityDescStr = v;
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
   Spawner* MapComponent::GetSpawner() const
   {
      return mSpawner;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MapComponent::SetUniqueId(const std::string& v) { 

      std::string olduid = mUniqueIdStr;
      if(olduid == v) return;

      if(mOwner != NULL)
      {
         MapSystem* ms;
         mOwner->GetEntityManager().GetEntitySystem(TYPE, ms);

         int inc = 0;
         bool success = false;

         while(!success)
         {
            std::ostringstream os;
            os << v;
            if(inc != 0)
            {
               os << "_" << inc;
            }
            ++inc;
            success = ms->OnEntityChangedUniqueId(mOwner->GetId(), olduid, os.str());
            if(success)
            {
               mUniqueIdStr = os.str();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId MapSystem::TYPE(dtEntity::SID("Map"));

   ////////////////////////////////////////////////////////////////////////////
   MapSystem::MapSystem(EntityManager& em)
      : DefaultEntitySystem<MapComponent>(em)
   {

      mSpawnEntityFunctor = MessageFunctor(this, &MapSystem::OnSpawnEntity);
      em.RegisterForMessages(SpawnEntityMessage::TYPE, mSpawnEntityFunctor, "MapSystem::OnSpawnEntity");
      mDeleteEntityFunctor = MessageFunctor(this, &MapSystem::OnDeleteEntity);
      em.RegisterForMessages(DeleteEntityMessage::TYPE, mDeleteEntityFunctor, "MapSystem::OnDeleteEntity");

      mStopSystemFunctor = MessageFunctor(this, &MapSystem::OnStopSystem);
      em.RegisterForMessages(StopSystemMessage::TYPE, mStopSystemFunctor, "MapSystem::OnStopSystem");

      mSetComponentPropertiesFunctor = MessageFunctor(this, &MapSystem::OnSetComponentProperties);
      em.RegisterForMessages(SetComponentPropertiesMessage::TYPE, mSetComponentPropertiesFunctor, "MapSystem::OnSetComponentProperties");

      mSetSystemPropertiesFunctor = MessageFunctor(this, &MapSystem::OnSetSystemProperties);
      em.RegisterForMessages(SetSystemPropertiesMessage::TYPE, mSetSystemPropertiesFunctor, "MapSystem::OnSetSystemPropertie");

      RegisterCommandMessages(MessageFactory::GetInstance());
      RegisterSystemMessages(MessageFactory::GetInstance());
   }

   ////////////////////////////////////////////////////////////////////////////
   MapSystem::~MapSystem()
   {
      for(MapEncoders::iterator i = mMapEncoders.begin(); i != mMapEncoders.end(); ++i)
      {
         delete *i;
      }

      GetEntityManager().UnregisterForMessages(SpawnEntityMessage::TYPE, mSpawnEntityFunctor);
      GetEntityManager().UnregisterForMessages(DeleteEntityMessage::TYPE, mDeleteEntityFunctor);
      GetEntityManager().UnregisterForMessages(StopSystemMessage::TYPE, mStopSystemFunctor);
      GetEntityManager().UnregisterForMessages(SetComponentPropertiesMessage::TYPE, mSetComponentPropertiesFunctor);
      GetEntityManager().UnregisterForMessages(SetSystemPropertiesMessage::TYPE, mSetSystemPropertiesFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnAddedToEntityManager(dtEntity::EntityManager& em)
   {
      em.AddEntitySystemRequestCallback(this);
#if PROTOBUF_FOUND
      AddMapEncoder(new ProtoBufMapEncoder(em));
#endif
      AddMapEncoder(new RapidXMLMapEncoder(em));

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
         size_t s = mEntitiesByUniqueId.erase(i->second->GetUniqueId());
         assert(s == 1);
      }
      return BaseClass::DeleteComponent(eid);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::OnEntityChangedUniqueId(EntityId id, const std::string& oldUniqueId, const std::string& newUniqueId)
   {
      if(oldUniqueId == newUniqueId)
      {
         return true;
      }

      typedef std::map<std::string, EntityId> UIMap;
      MapComponent* comp = GetComponent(id);
      assert(comp != NULL);

      UIMap::iterator j = mEntitiesByUniqueId.find(newUniqueId);
      if(j != mEntitiesByUniqueId.end())
      {
         return false;
      }  

      UIMap::iterator i = mEntitiesByUniqueId.find(oldUniqueId);
      if(i != mEntitiesByUniqueId.end())
      {
         mEntitiesByUniqueId.erase(i);
      }

      mEntitiesByUniqueId[newUniqueId] = id;


      EntityNameUpdatedMessage msg;
      msg.SetAboutEntityId(id);
      msg.SetEntityName(comp->GetEntityName());
      msg.SetUniqueId(newUniqueId);
      GetEntityManager().EmitMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::LoadScene(const std::string& path)
   {
      // get data path containing this map
      
      std::string abspath = GetSystemInterface()->FindDataFile(path);
      if(abspath == "")
      {
         LOG_ERROR("Cannot find scene file: " << path);
         return false;
      }

      std::string scenedatapath = GetSystemInterface()->GetDataFilePathFromFilePath(abspath);

      
      if(scenedatapath == "")
      {
         LOG_ERROR("No data path for scene found!");
         return false;
      }

      MapEncoder* enc = GetEncoderForScene(GetFileExtension(path));
      if(!enc)
      {
         LOG_ERROR("Could not load scene: Loader not found for extension " << GetFileExtension(path));
         return false;
      }

      bool success = enc->LoadSceneFromFile(path);
      
      SceneLoadedMessage msg;
      msg.SetSceneName(path);
      GetEntityManager().EmitMessage(msg);
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

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::SaveScene(const std::string& path, bool saveAllMaps)
   {
      MapEncoder* enc = GetEncoderForScene(GetFileExtension(path));
      if(!enc)
      {
         LOG_ERROR("Could not save scene: Loader not found for extension " << GetFileExtension(path));
         return false;
      }
     bool success = enc->SaveSceneToFile(path);

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
      return (GetSystemInterface()->FindDataFile(path) != "");
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

      MapEncoder* enc = GetEncoderForMap(GetFileExtension(path));
      if(!enc)
      {
         LOG_ERROR("Could not load map: Loader not found for extension " << GetFileExtension(path));
         return false;
      }

      // get data path containing this map
      std::string abspath = GetSystemInterface()->FindDataFile(path);
      std::string mapdatapath = dtEntity::GetSystemInterface()->GetDataFilePathFromFilePath(abspath);
      
      assert(mapdatapath != "");

      LoadedMaps::size_type mapsaveorder = mLoadedMaps.size();


      MapBeginLoadMessage msg;
      msg.SetMapPath(path);
      msg.SetDataPath(mapdatapath);
      msg.SetSaveOrder(mapsaveorder);
      GetEntityManager().EmitMessage(msg);

      bool success = enc->LoadMapFromFile(path);
      if(success)
      {
         mLoadedMaps.push_back(MapData(path, mapdatapath, static_cast<unsigned int>(mLoadedMaps.size())));

         MapLoadedMessage msg1;
         msg1.SetMapPath(path);
         msg1.SetDataPath(mapdatapath);
         msg1.SetSaveOrder(mapsaveorder);
         GetEntityManager().EmitMessage(msg1);
      }
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
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
      MapEncoder* enc = GetEncoderForMap(GetFileExtension(mappath));
      if(!enc)
      {
         LOG_ERROR("Could not save map: Loader not found for extension " << GetFileExtension(mappath));
         return false;
      }

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
      bool success = enc->SaveMapToFile(mappath, os.str());
      
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

      MapEncoder* enc = GetEncoderForMap(GetFileExtension(copypath));
      if(!enc)
      {
         LOG_ERROR("Could not save map: Loader not found for extension " << GetFileExtension(copypath));
         return false;
      }

      bool success = enc->SaveMapToFile(path, copypath);
      
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MapSystem::AddEmptyMap(const std::string& dataPath, const std::string& mapname)
   {
      if(IsMapLoaded(mapname))
      {
         return false;
      }
      if(GetSystemInterface()->FindDataFile(mapname) != "")
      {
         mLoadedMaps.push_back(MapData(mapname, dataPath, static_cast<unsigned int>(mLoadedMaps.size())));
         return false;
      }

      std::string mappathrel = GetFilePath(mapname);
      std::ostringstream os; os << dataPath << "/" << mappathrel;
      std::string mappathabs = os.str();

      if(!GetSystemInterface()->FileExists(mappathabs))
      {
         LOG_ERROR("Cannot create map in directory " << mappathabs << "! Does it exist?");
         return false;
      }

      MapBeginLoadMessage msg;
      msg.SetMapPath(mapname);
      GetEntityManager().EmitMessage(msg);
      mLoadedMaps.push_back(MapData(mapname, dataPath, static_cast<unsigned int>(mLoadedMaps.size())));
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

      EntitySpawnedMessage smsg;
      smsg.SetSpawnerName(spawnerName);
      smsg.SetAboutEntityId(entity->GetId());
      GetEntityManager().EmitMessage(smsg);

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
   void MapSystem::OnSetComponentProperties(const Message& m)
   {
      const SetComponentPropertiesMessage& msg = static_cast<const SetComponentPropertiesMessage&>(m);

      ComponentType ctype = dtEntity::SIDHash(msg.GetComponentType());
      std::string uniqueid = msg.GetEntityUniqueId();

      MapSystem* ms;
      GetEntityManager().GetEntitySystem(MapComponent::TYPE, ms);
      EntityId id = ms->GetEntityIdByUniqueId(uniqueid);

      if(id == 0)
      {
         LOG_ERROR("Entity not found for SetComponentPropertiesMessage!");
         return;
      }

      Component* component;
      bool found = GetEntityManager().GetComponent(id, ctype, component);

      if(!found)
      {
         LOG_WARNING("Cannot process SetComponentProperties message. Component not found: "
            + msg.GetComponentType());
         return;
      }
      const PropertyGroup& props = msg.GetComponentProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = component->Get(i->first);
         if(!target)
         {
            LOG_ERROR(
                "Cannot process SetComponentProperties message. Component "
                << msg.GetComponentType()
                << " has no property named "
                << GetStringFromSID(i->first)
            );
            continue;
         }
         target->SetFrom(*i->second);
#if CALL_ONPROPERTYCHANGED_METHOD
         component->OnPropertyChanged(i->first, *target);
#endif
      }
      component->Finished();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::OnSetSystemProperties(const Message& m)
   {
      const SetSystemPropertiesMessage& msg = static_cast<const SetSystemPropertiesMessage&>(m);
      EntitySystem* sys = GetEntityManager().GetEntitySystem(SIDHash(msg.GetComponentType()));
      if(sys == NULL)
      {
         LOG_WARNING("Cannot process SetSystemProperties message. Entity system not found: "
            << msg.GetComponentType());
         return;
      }
      const PropertyGroup& props = msg.GetSystemProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = sys->Get(i->first);
         if(!target)
         {
            LOG_ERROR("Cannot process SetSystemProperties message. Entity system "
                << msg.GetComponentType()
                << " has no property named "
                << GetStringFromSID(i->first));
            continue;
         }
         target->SetFrom(*i->second);
#if CALL_ONPROPERTYCHANGED_METHOD
         sys->OnPropertyChanged(i->first, *target);
#endif
      }
      sys->Finished();

   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapSystem::GetSpawnerCreatedEntities(const std::string& spawnername, std::vector<EntityId>& ids, bool recursive) const
   {
      ComponentStore::const_iterator i;
      for(i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         MapComponent* component = i->second;         
         Spawner* spwn = component->GetSpawner();

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
   bool MapSystem::IsSpawnOf(EntityId id, const std::string& spawnername) const
   {
      const MapComponent* comp = GetComponent(id);
      if(!comp)
      {
         return false;
      }
      const Spawner* s = comp->GetSpawner();
      while(s != NULL)
      {
         if(s->GetName() == spawnername)
         {
            return true;
         }
         s = s->GetParent();
      }
      return false;
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
         msg.SetVisibleInEntityList(mc->GetVisibleInEntityList());
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
         msg.SetVisibleInEntityList(mc->GetVisibleInEntityList());
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
   void MapSystem::AddMapEncoder(MapEncoder* ec)
   {
      mMapEncoders.push_back(ec);
   }

   ////////////////////////////////////////////////////////////////////////////
   MapEncoder* MapSystem::GetEncoderForMap(const std::string& extension) const
   {
      for(MapEncoders::const_iterator i = mMapEncoders.begin(); i != mMapEncoders.end(); ++i)
      {
         if((*i)->AcceptsMapExtension(extension))
         {
            return *i;
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   MapEncoder* MapSystem::GetEncoderForScene(const std::string& extension) const
   {
      for(MapEncoders::const_iterator i = mMapEncoders.begin(); i != mMapEncoders.end(); ++i)
      {
         if((*i)->AcceptsSceneExtension(extension))
         {
            return *i;
         }
      }
      return NULL;
   }
}
