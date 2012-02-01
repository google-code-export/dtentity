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

#include <osg/Referenced>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/entityid.h>
#include <dtEntity/stringid.h>
#include <dtEntity/component.h>
#include <list>

namespace dtEntity
{
   class EntityManager;


   /**
    * Each entity system holds a number of components.
    * Entity systems are themselves responsible for allocating and destroying
    * their components.
    */
   class EntitySystem
      : public osg::Referenced
      , public PropertyContainer
   {
   public:

      /**
       * Constructor.
       * @param id The type id of the component
       * @param em The entity manager that this system belongs to
       * @param baseType If component class is derived from component of another entity system then
       *                 set the parent component type here. EntityManager::GetComponent will return
       *                 components of this entity system when base type is requested.
       */
      EntitySystem(ComponentType id, EntityManager& em, ComponentType baseType = StringId())
         : mComponentType(id)
         , mBaseType(baseType)
         , mEntityManager(&em)
      {
      }

      virtual ~EntitySystem() 
      {
      }

      /**
       * @return The type id of the component that this system handles
       */
      ComponentType GetComponentType() const 
      {
         return mComponentType; 
      }

      /**
       * @return If component class is derived from a base component type, give base
       *         type here
       */
      ComponentType GetBaseType() const
      {
         return mBaseType;
      }
      
      /**
       * @return true if a component for this entity exists in the system
       */
      virtual bool HasComponent(EntityId eid) const { return false; }
      
      /**
       * @param eid Get component for the entity with this id
       * @param component Receives the component if found
       * @return true if found
       */
      virtual bool GetComponent(EntityId eid, Component*& component) { return false; }
      virtual bool GetComponent(EntityId eid, const Component*& component) { return false; }
      
      /**
       * @param eid Create a component for the entity with this id.
       * @param component Receives the component if it was succesfully created
       * @return true if success
       */
      virtual bool CreateComponent(EntityId eid, Component*& component) { return false; }

      /**
       * Delete component. It is best to not call this directly but instead use
       * the method EntityManager::DeleteComponent. Calling this method
       * directly will not execute the component deletion callbacks, which
       * can result in wrappers for that component not being invalidated
       *
       * @param eid Delete component for entity with this id
       * @return true if entity had a component of this type
       */
      virtual bool DeleteComponent(EntityId eid) { return false; }
      
      /**
       * Put ids of all entities registered with this system into toFill
       */
      virtual void GetEntitiesInSystem(std::list<EntityId>& toFill) const { }

      /**
       * Get property names and default property values of component. Used
       * for spawner creation
       */
      virtual DynamicPropertyContainer GetComponentProperties() const { return DynamicPropertyContainer(); }

	  /**
	   * @return entity manager that the entity system was added to
	   */
      EntityManager& GetEntityManager() const
      {
         return *mEntityManager;
      }

      /** overwrite to react to add to entity manager */
      virtual void OnAddedToEntityManager(dtEntity::EntityManager& em) {}
      virtual void OnRemoveFromEntityManager(dtEntity::EntityManager& em) {}

      /**
       * Override this if components of this entity system should not
       * be stored to map
       */
      virtual bool StoreComponentToMap(dtEntity::EntityId) const { return true; }

      /**
       * Override to return false if no spawner should be allowed to spawn this component
       */
      virtual bool AllowComponentCreationBySpawner() const { return true; }

      /**
       * Override this if properties of this entity system should
       * be stored to scene file
       */
      virtual bool StorePropertiesToScene() const { return false; }

   private:

      // no copy ctor
      EntitySystem(EntitySystem&);

      ComponentType mComponentType;
      ComponentType mBaseType;
      EntityManager* mEntityManager;
   };
}
