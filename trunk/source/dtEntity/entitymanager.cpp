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
      : mNextAvailableId(0)
      , mMessagePump(new MessagePump())
   {     
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityManager::~EntityManager() 
   {
      // notify all about shutdown, last chance to deregister
      StopSystemMessage msg;
      EmitMessage(msg);

      // send and delete all outstanding messages
      EmitQueuedMessages(FLT_MAX);
      delete mMessagePump;
      mMessagePump = NULL;

      for(EntitySystemStore::iterator i = mEntitySystemStore.begin();
         i != mEntitySystemStore.end(); ++i)
      {
         i->second->OnRemoveFromEntityManager(*this);
      }
      // delete all entity objects
      while(HasEntities())
      {
         std::pair<EntityId, Entity*> p = *mEntities.begin();
         mEntities.erase(mEntities.begin());
      }

      for(EntitySystemStore::iterator i = mEntitySystemStore.begin();
         i != mEntitySystemStore.end(); ++i)
      {
         delete i->second;
      }
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
   bool EntityManager::HasEntities() const
   {
      OpenThreads::ScopedReadLock lock(mEntityMutex);
      return (!mEntities.empty());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::AddToScene(EntityId eid)
   {
      MapSystem* mapSystem;
      if(GetEntitySystem(MapComponent::TYPE, mapSystem))
      {
         return mapSystem->AddToScene(eid);
      }
      return false;
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveFromScene(EntityId eid)
   {
      MapSystem* mapSystem;
      if(GetEntitySystem(MapComponent::TYPE, mapSystem))
      {
         return mapSystem->RemoveFromScene(eid);
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityId EntityManager::GetNextAvailableID() 
   {
      return ++mNextAvailableId;
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
   bool EntityManager::HasEntitySystem(ComponentType t) const
   {
      EntitySystemStore::const_iterator i = mEntitySystemStore.find(t);
      return (i != mEntitySystemStore.end());
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::AddEntitySystem(EntitySystem& s)
   {
      if(HasEntitySystem(s.GetComponentType()))
      {
         LOG_ERROR("Entity system already added! Type: " + GetStringFromSID(s.GetComponentType()));
         return false;
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
      EmitMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveEntitySystem(EntitySystem& s)
   {
      ComponentType componentType = s.GetComponentType();
      if(!HasEntitySystem(componentType))
      {
         return false;
      }
      ComponentType baseType = s.GetBaseType();
      s.OnRemoveFromEntityManager(*this);
      EntitySystemRemovedMessage msg;
      msg.SetComponentType(componentType);
      msg.SetComponentTypeString(GetStringFromSID(componentType));
      EmitMessage(msg);
      mEntitySystemStore.erase(mEntitySystemStore.find(componentType));

      if(baseType != StringId())
      {
         std::pair<TypeHierarchyMap::iterator, TypeHierarchyMap::iterator> keyRange;
         keyRange = mTypeHierarchy.equal_range(baseType);

         TypeHierarchyMap::iterator it = keyRange.first;
         while(it != keyRange.second)
         {
            if(it->second == componentType)
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
      return i->second;
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
   void EntityManager::GetComponents(EntityId eid, std::vector<Component*>& toFill)
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
   void EntityManager::GetComponents(EntityId eid, std::vector<const Component*>& toFill) const
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
      EntitySystem* es = NULL;
      
      if(!GetEntitySystem(id, es))
      {
         EntitySystemRequestCallbacks::iterator i;
         for(i = mEntitySystemRequestCallbacks.begin(); i != mEntitySystemRequestCallbacks.end(); ++i)
         {
            if((*i)->CreateEntitySystem(this, id))
            {
               bool success = GetEntitySystem(id, es);
               assert(success);
               break;
            }
         }
         if(es == NULL)
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

   ///////////////////////////////////////////////////////////////////////////////
   void EntityManager::AddEntitySystemRequestCallback(EntitySystemRequestCallback* cb)
   {
      mEntitySystemRequestCallbacks.push_back(cb);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EntityManager::RemoveEntitySystemRequestCallback(EntitySystemRequestCallback* cb)
   {
      EntitySystemRequestCallbacks::iterator i;
      for(i = mEntitySystemRequestCallbacks.begin(); i != mEntitySystemRequestCallbacks.end(); ++i)
      {
         if(*i == cb)
         {
            mEntitySystemRequestCallbacks.erase(i);
            return true;
         }
      }
      return false;
   }
}

