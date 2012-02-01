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
#include <dtEntity/entitymanager.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/message.h>
#include <dtEntity/log.h>
#include <float.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   EntityManager::EntityManager()
      : mMessagePump(new MessagePump())
   {     
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityManager::~EntityManager() 
   {
      // send and delete all outstanding messages
      EmitQueuedMessages(FLT_MAX);
      delete mMessagePump;
      mMessagePump = NULL;

      // first remove map system so that it can unload plugins
      EntitySystem* mapsys = GetEntitySystem(MapComponent::TYPE);
      if(mapsys) 
      {
         RemoveEntitySystem(*mapsys);
      }

      for(EntitySystemStore::iterator i = mEntitySystemStore.begin();
         i != mEntitySystemStore.end(); ++i)
      {
         i->second->OnRemoveFromEntityManager(*this);
      }
      // delete all entity objects
      while(!mEntities.empty())
      {
         std::pair<EntityId, Entity*> p = *mEntities.begin();
         mEntities.erase(mEntities.begin());
      }

      mEntitySystemStore.clear();
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::CreateEntity(Entity*& entity)
   {
      dtEntity::EntityId eid = this->GetNextAvailableID();
      entity = new dtEntity::Entity(*this, eid);

      OpenThreads::ScopedWriteLock lock(mEntityMutex);
      mEntities.insert(std::pair<EntityId, osg::ref_ptr<Entity> >(eid, entity));

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::AddToScene(EntityId eid)
   {
      if(mEntities.find(eid) == mEntities.end())
      {
         LOG_ERROR("Cannot add to scene: Entity with this ID not found!");
         return false;
      }
      EntityAddedToSceneMessage msg;
      msg.SetUInt(EntityAddedToSceneMessage::AboutEntityId, eid);

      MapComponent* mc;
      if(GetComponent(eid, mc))
      {
         msg.SetMapName(mc->GetMapName());
         msg.SetEntityName(mc->GetEntityName());
         msg.SetUniqueId(mc->GetUniqueId());
      }

      EmitMessage(msg);
      return true;
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveFromScene(EntityId eid)
   {
      if(mEntities.find(eid) == mEntities.end())
      {
         LOG_ERROR("Cannot remove from scene: Entity with this ID not found!");
         return false;
      }
      EntityRemovedFromSceneMessage msg;
      msg.SetUInt(EntityRemovedFromSceneMessage::AboutEntityId, eid);
      MapComponent* mc;
      if(GetComponent(eid, mc))
      {
         msg.SetMapName(mc->GetMapName());
         msg.SetEntityName(mc->GetEntityName());
         msg.SetUniqueId(mc->GetUniqueId());
      }
      EmitMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityId EntityManager::GetNextAvailableID() 
   {
      static EntityId counter = 0;
      return ++counter;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::GetEntity(EntityId id, Entity*& entity)
   {
      OpenThreads::ScopedReadLock lock(mEntityMutex);
      EntityMap::const_iterator it = mEntities.find(id);
      if(it == mEntities.end()) return false;
      entity = it->second;
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::EntityExists(EntityId id) const
   {
      OpenThreads::ScopedReadLock lock(mEntityMutex);
      EntityMap::const_iterator it = mEntities.find(id);
      if(it == mEntities.end()) return false;
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::GetEntityIds(std::vector<EntityId>& toFill)
   {
      OpenThreads::ScopedReadLock lock(mEntityMutex);
      EntityMap::const_iterator it = mEntities.begin();
      for(; it != mEntities.end(); ++it)
      {
         toFill.push_back(it->first);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::KillEntity(EntityId id) 
   {      
      EntitySystemStore::iterator i = mEntitySystemStore.begin();
      for(; i != mEntitySystemStore.end(); ++i)
      {
         if(i->second->HasComponent(id))
         {
            if(!mDeletedCallbacks.empty())
            {
               for(ComponentDeletedCallbacks::iterator j = mDeletedCallbacks.begin(); j != mDeletedCallbacks.end(); ++j)
               {
                  (*j)->ComponentDeleted(i->first, id);
               }
            }
            i->second->DeleteComponent(id);            
         }
      }
      
      OpenThreads::ScopedWriteLock lock(mEntityMutex);
      EntityMap::iterator toKill = mEntities.find(id);
      if(toKill == mEntities.end())
      {
         return false;
      }
      mEntities.erase(toKill);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::KillAllEntities()
   {
      while(!mEntities.empty())
      {
         EntityId id = mEntities.begin()->first;
         RemoveFromScene(id);
         KillEntity(id);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::HasEntitySystem(ComponentType t) const
   {
      EntitySystemStore::const_iterator i = mEntitySystemStore.find(t);
      return (i != mEntitySystemStore.end());
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::AddEntitySystem(EntitySystem& s)
   {
      if(HasEntitySystem(s.GetComponentType()))
      {
         LOG_ERROR("Entity system already added! Type: " + GetStringFromSID(s.GetComponentType()));
         return;
      }
      mEntitySystemStore[s.GetComponentType()] = &s;

      if(s.GetBaseType() != StringId())
      {
         mTypeHierarchy.insert(std::make_pair(s.GetBaseType(), s.GetComponentType()));
      }

      s.OnAddedToEntityManager(*this);
      EntitySystemAddedMessage msg;
      msg.SetComponentType(s.GetComponentType());
      msg.SetComponentTypeString(GetStringFromSID(s.GetComponentType()));
      msg.SetSystemProperties(s.GetAllProperties());
      EmitMessage(msg);      
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveEntitySystem(EntitySystem& s)
   {
      if(!HasEntitySystem(s.GetComponentType()))
      {
         return false;
      }

      s.OnRemoveFromEntityManager(*this);
      EntitySystemRemovedMessage msg;
      msg.SetComponentType(s.GetComponentType());
      msg.SetComponentTypeString(GetStringFromSID(s.GetComponentType()));
      EmitMessage(msg);
      mEntitySystemStore.erase(mEntitySystemStore.find(s.GetComponentType()));

      if(s.GetBaseType() != StringId())
      {
         std::pair<TypeHierarchyMap::iterator, TypeHierarchyMap::iterator> keyRange;
         keyRange = mTypeHierarchy.equal_range(s.GetBaseType());

         TypeHierarchyMap::iterator it = keyRange.first;
         while(it != keyRange.second)
         {
            if(it->second == s.GetComponentType())
            {
               mTypeHierarchy.erase(it);
               break;
            }
            ++it;
         }
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntitySystem* EntityManager::GetEntitySystem(ComponentType t) const
   {
      EntitySystemStore::const_iterator i = mEntitySystemStore.find(t);
      if(i == mEntitySystemStore.end())
         return NULL;
      return i->second.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::GetEntitySystems(std::vector<EntitySystem*>& toFill)
   {
      EntitySystemStore::iterator i = mEntitySystemStore.begin();
      for(; i != mEntitySystemStore.end(); ++i)
      {
         toFill.push_back(i->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::GetEntitySystems(std::vector<const EntitySystem*>& toFill) const
   {
      EntitySystemStore::const_iterator i = mEntitySystemStore.begin();
      for(; i != mEntitySystemStore.end(); ++i)
      {
         toFill.push_back(i->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::GetComponent(EntityId eid, ComponentType t, Component*& component, bool searchDerived)
   {
      EntitySystem* es;
      
      if(GetEntitySystem(t, es) && es->GetComponent(eid, component))
      {
         return true;
      }
      if(searchDerived)
      {
         return GetDerived(eid, t, component);
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::GetDerived(EntityId eid, ComponentType ctype, Component*& comp) const
   {

      std::pair<TypeHierarchyMap::const_iterator, TypeHierarchyMap::const_iterator> keyRange;
      keyRange = mTypeHierarchy.equal_range(ctype);

      TypeHierarchyMap::const_iterator it = keyRange.first;
      while(it != keyRange.second)
      {
         dtEntity::EntitySystem* es;
         bool success = this->GetEntitySystem(it->second, es);
         if(!success)
         {
            LOG_ERROR("Error in type hierarchy structure! Cannot find entity system " << GetStringFromSID(it->second));
            continue;
         }

         if(es->GetComponent(eid, comp))
         {
            return true;
         }

         if(GetDerived(eid, es->GetComponentType(), comp))
         {
            return true;
         }

         ++it;
      }
      return false;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::GetComponents(EntityId eid, std::list<Component*>& toFill)
   {
      EntitySystemStore::iterator i;
      for(i = mEntitySystemStore.begin(); i != mEntitySystemStore.end(); ++i)
      {
         Component* c;
         
         if(i->second->GetComponent(eid, c))
         {
            toFill.push_back(c);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityManager::GetComponents(EntityId eid, std::list<const Component*>& toFill) const
   {
      EntitySystemStore::const_iterator i;
      for(i = mEntitySystemStore.begin(); i != mEntitySystemStore.end(); ++i)
      {
         Component* c;
         
         if(i->second->GetComponent(eid, c))
         {
            toFill.push_back(c);
         }
      }
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::HasComponent(EntityId eid, ComponentType t, bool searchDerived) const
   {
      EntitySystem* es;
      if(GetEntitySystem(t, es) && es->HasComponent(eid))
      {
         return true;
      }
      if(searchDerived)
      {
         dtEntity::Component* c;
         return GetDerived(eid, t, c);
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::CreateComponent(EntityId eid, ComponentType id, Component*& component)
   {
      EntitySystem* es;
      
      if(!GetEntitySystem(id, es))
      {
         MapSystem* mapSystem;
         GetEntitySystem(MapComponent::TYPE, mapSystem);
         if(mapSystem->GetPluginManager().FactoryExists(id))
         {
            mapSystem->GetPluginManager().StartEntitySystem(id);
            if(!GetEntitySystem(id, es))
            {
               LOG_ERROR("Factory error: Factory is registered for type but "
                         "does not create entity system with that type");
               return false;
            }
         }
         else
         {
            LOG_ERROR("Could not add component: no entity system of type " + GetStringFromSID(id));
            return false;
         }
      }
      return es->CreateComponent(eid, component);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::DeleteComponent(EntityId eid, Component& component)
   {
      return DeleteComponent(eid, component.GetType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::DeleteComponent(EntityId eid, ComponentType componentType)
   {
      EntitySystem* es;
      if(!GetEntitySystem(componentType, es))
      {
         return false;
      }

      if(!es->HasComponent(eid))
      {    
         return false;
      }
      if(!mDeletedCallbacks.empty())
      {
         for(ComponentDeletedCallbacks::iterator i = mDeletedCallbacks.begin(); i != mDeletedCallbacks.end(); ++i)
         {
            (*i)->ComponentDeleted(componentType, eid);
         }
      }
      es->DeleteComponent(eid);
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EntityManager::SetMessagePump(MessagePump& p)
   {
      if(mMessagePump != NULL)
      {
         delete mMessagePump;
      }
      mMessagePump = &p;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EntityManager::AddDeletedCallback(ComponentDeletedCallback* cb)
   {
      mDeletedCallbacks.push_back(cb);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveDeletedCallback(ComponentDeletedCallback* cb)
   {
      ComponentDeletedCallbacks::iterator i;
      for(i = mDeletedCallbacks.begin(); i != mDeletedCallbacks.end(); ++i)
      {
         if(*i == cb)
         {
            mDeletedCallbacks.erase(i);
            return true;
         }
      }
      return false;
   }
}

