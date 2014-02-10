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

#include <dtEntity/propertycontainer.h>
#include <dtEntity/property.h>
#include <dtEntity/entityid.h>
#include <dtEntity/stringid.h>
#include <list>

namespace dtEntity
{
   class Component;
   class EntityManager;


   /**
    * Each entity system holds a number of components.
    * Entity systems are themselves responsible for allocating and destroying
    * their components.
    */
   class EntitySystem
      : public PropertyContainer
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
      EntitySystem(EntityManager& em, ComponentType baseType = StringId())
         : mBaseType(baseType)
         , mEntityManager(&em)
      {
      }

      virtual ~EntitySystem() 
      {
      }

      /**
       * @return The type id of the component that this system handles
       */
      virtual ComponentType GetComponentType() const = 0;
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
      virtual GroupProperty GetComponentProperties() const { return GroupProperty(); }

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

      ComponentType mBaseType;
      EntityManager* mEntityManager;
   };
}
