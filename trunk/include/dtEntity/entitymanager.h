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

#include <osg/ref_ptr>
#include <dtEntity/component.h>
#include <dtEntity/entityid.h>
#include <dtEntity/export.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/objectfactory.h>
#include <map>
#include <list>
#include <assert.h>
#include <osg/Referenced>
#include <OpenThreads/ReadWriteMutex>
#include <dtEntity/component.h>

namespace dtEntity
{

   class Entity;
   class EntitySystem;
   class Message;

   /**
    * can be used to clean up wrappers for components or
    * execute other actions
    */
   class ComponentDeletedCallback
   {
   public:
      virtual void ComponentDeleted(ComponentType t, EntityId id) = 0;
   };

   typedef std::vector<ComponentDeletedCallback*> ComponentDeletedCallbacks;

   /**
    * Entity manager is a container for entity systems. 
    * Each entity system holds entity components of a specific type.
    * The entity manager gives access to the entity systems and can be used to
    * create, retrieve and delete components.
    *
    * Also, Entity manager is a central point to send and register to messages.
    * Messages can be sent (emitted) immediately to all registrants, or they can be 
    * posted to a message queue to be emitted at a later time. 
    */
   class DT_ENTITY_EXPORT EntityManager 
      : public osg::Referenced
   {
   
   public:

      /**
       * Constructor.
       * @param startEntitySystems if true, start default entity
       * systems like OSG components etc
       */
      EntityManager();

      /**
       * Get entity object for entity ID.
       * @param id Unique id of the entity to retrieve
	   * @param entity Will receive pointer to entity object
	   * @return true if entity with this id was found
       * @threadsafe
       */
      bool GetEntity(EntityId id, Entity*& entity);

      /**
       * return true if entity with this ID exists
       */
      bool EntityExists(EntityId id) const;

      /**
       * Fill vector with entity ids of all existing entities
       */
      void GetEntityIds(std::vector<EntityId>& toFill);

      /**
       * delete entity object and tell all entity systems to delete their
       * components for this entity
       * @param id id of the entity to delete
       * @return true if success
       * @threadsafe
       */
      bool KillEntity(EntityId id);

      /**
       * Remove all components from all entity systems and delete
	   * all entity objects
       * @threadsafe
       */
      void KillAllEntities();

      /**
       * Create a new entity with a new unique EntityId
       * All entities are deleted when entity manager is deleted.
       * @param entity Receives pointer to newly created entity
       * @return true if success
       * @threadsafe
       */
      bool CreateEntity(Entity*& entity);

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
       * @param id Check if an entity system of this component type is active
       * @return true if such a system exists
       */
      bool HasEntitySystem(ComponentType id) const;

      /**
       * @param s Add this entity system to the manager
       */
      void AddEntitySystem(EntitySystem& s);

      /**
       * @param s Remove this entity system
       * @return true if entity system was registered, else false
       */
      bool RemoveEntitySystem(EntitySystem& s);

      /**
       * Get entity system of given type.
       * Returns NULL if not found.
       */
      EntitySystem* GetEntitySystem(ComponentType id) const;

      /**
       * @param id Get entity system for this component type
       * @param es receives entity system if found
       * @return true if entity system was found
       */
      template <typename T>
      bool GetEntitySystem(ComponentType id, T*& es) const;

      /**
       * Fill vector with all entity systems registered with
       * the entity manager
       */
      void GetEntitySystems(std::vector<EntitySystem*>& toFill);
      void GetEntitySystems(std::vector<const EntitySystem*>& toFill) const;

      /**
       * @param eid Get component of this entity
       * @param t Get component with this component type
       * @param component Receives component if found
	   * @param searchDerived If true, return component that inherits t if one exists. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if component was found
       */
      bool GetComponent(EntityId eid, ComponentType t, Component*& component, bool searchDerived = false);

      /**
       * @param eid Get component of this entity
       * @param component Receives component of type T if found
       * @param searchDerived If true, return component that inherits t if one exists. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if success
       */
      template <typename T>
      bool GetComponent(EntityId eid, T*& component, bool searchDerived = false);

      /**
       * Get all components of given entity
       * Warning: slow. Iterates over all entity systems.
       * @param eid Get components of this entity
       * @param toFill receives components
       */
      void GetComponents(EntityId eid, std::list<Component*>& toFill);
      void GetComponents(EntityId eid, std::list<const Component*>& toFill) const;

      /**
       * @param eid Check if component exists for this entity
       * @param t Type of component to check for
       * @param searchDerived If true, search for component that inherits t. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if exists
       */
      bool HasComponent(EntityId eid, ComponentType t, bool searchDerived = false) const;

      /**
       * Create a component of given type for entity
       * @param eid Add component to this entity
       * @param id Add component of this type
       * @param component Receives newly created component if success
       * @return true if success
       */
      bool CreateComponent(EntityId eid, ComponentType id, Component*& component);
      
      /**
       * Create a component of given type for entity 
       * @param eid Add component to this entity
       * @param component Receives newly created component of type T if success
       * @return true if success
       */      
      template <typename T>
      bool CreateComponent(EntityId eid, T*& component);
      void DeleteComponent(EntityId eid, Component& component);

	  /**
	   * @return default message pump for inter-system communication
	   */
      MessagePump& GetMessagePump() const 
	  {
	     return *mMessagePump; 
      }
      
	  /**
	   * @param p Set this as default message pump for inter-system communication.
	   * Warning: Old message pump is overwritten, queued messages may get lost.
	   * @TODO remove?
	   */
	  void SetMessagePump(MessagePump& p);

      // See messagepump.h for documentation
      inline void RegisterForMessages(MessageType msgtype, MessageFunctor& ftr, unsigned int options = FilterOptions::DEFAULT, const std::string& funcname = "unnamed")
      {
         if(mMessagePump) mMessagePump->RegisterForMessages(msgtype, ftr, options, funcname);
      }

      // See messagepump.h for documentation
      inline void RegisterForMessages(MessageType msgtype, MessageFunctor& ftr, const std::string& funcname)
      {
         if(mMessagePump) mMessagePump->RegisterForMessages(msgtype, ftr, FilterOptions::DEFAULT, funcname);
      }

      // See messagepump.h for documentation
      inline bool UnregisterForMessages(MessageType msgtype, MessageFunctor& ftr)
      {
         if(!mMessagePump) return false;
         return mMessagePump->UnregisterForMessages(msgtype, ftr);
      }

      // See messagepump.h for documentation
      inline void EmitMessage(const Message& msg)
      {
         if(mMessagePump) mMessagePump->EmitMessage(msg);
      }

      // See messagepump.h for documentation
      inline void EnqueueMessage(const Message& msg, double time = 0)
      {
         if(mMessagePump) mMessagePump->EnqueueMessage(msg, time);
      }

      // See messagepump.h for documentation
      void EmitQueuedMessages(double simtime)
      {
         if(mMessagePump) mMessagePump->EmitQueuedMessages(simtime);
      }

      void AddDeletedCallback(ComponentDeletedCallback* cb);
      bool RemoveDeletedCallback(ComponentDeletedCallback* cb);

   protected:
   
         // Destructor
      ~EntityManager();

   private:

      // Returns next id and increments internal counter
      EntityId GetNextAvailableID();

      /**
       * Look in type hierarchy map if a component derived from type exists
       */
      bool GetDerived(EntityId eid, ComponentType ctype, Component*& comp) const;

      // storage for entity objects.
      typedef std::map<EntityId, osg::ref_ptr<Entity> > EntityMap;
      EntityMap mEntities;

      // controls access to mEntities
      mutable OpenThreads::ReadWriteMutex mEntityMutex;

      // Storage for entity systems
      typedef std::map<ComponentType, osg::ref_ptr<EntitySystem> > EntitySystemStore;
      EntitySystemStore mEntitySystemStore;

      // stores inheritance tree for component types
      typedef std::multimap<ComponentType, ComponentType> TypeHierarchyMap;
      TypeHierarchyMap mTypeHierarchy;

	  // for publish-subscribe of messages
      MessagePump* mMessagePump;

      ComponentDeletedCallbacks mDeletedCallbacks;

   };


   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::CreateComponent(EntityId eid, T*& component)
   {
      assert(eid > 0);
      Component* c;
      if(!this->CreateComponent(eid, T::TYPE, c))
      {
         return false;
      }
      assert(c->GetType() == T::TYPE);
      component = static_cast<T*>(c);
      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::GetComponent(EntityId eid, T*& component, bool searchDerived)
   {
      Component* c;
      if(this->GetComponent(eid, T::TYPE, c, searchDerived))
      {
         assert(dynamic_cast<T*>(c) != NULL);
         component = static_cast<T*>(c);
         return true;
      }
      return false;

   }

   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::GetEntitySystem(ComponentType id, T*& es) const
   {
      EntitySystem* s = GetEntitySystem(id);
      if(s == NULL) return false;
      assert(dynamic_cast<T*>(s) != NULL);
      es = static_cast<T*>(s);
      
      return true;
   }
}
