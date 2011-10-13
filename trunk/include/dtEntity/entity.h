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
#include <dtEntity/entityid.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/component.h>
#include <osg/Referenced>
#include <list>
#include <dtEntity/component.h>

namespace dtEntity
{
   class EntityManager;

   /**
    * The entity class is simply a convenience class to bundle
    * the entity ID and access to the entity manager.
    */
   class DT_ENTITY_EXPORT Entity : public osg::Referenced
   {
   public:
      
      /**
       * Constructor.
       * @param em The entity manager that holds this entity
       * @param id The unique ID of this entity
       */
      Entity(EntityManager& em, EntityId id);

      /**
       * Create a component of given type and assign it to component
       * @param t Type of component to construct
       * @param component Receives newly constructed component
       * @return true if success
       */
      bool CreateComponent(ComponentType t, Component*& component);
      
      // templated version
      template<typename T> 
      bool CreateComponent(T*& component);

      /**
       * Get component from this entity
       * @param component receives the component if found
       * @return true if found
       */
      template<typename T> 
      bool GetComponent(T*& component);

      /**
       * Get component from this entity
       * @param component receives the component if found
       * @return true if found
       */
      bool GetComponent(ComponentType t, Component*& component);

      /**
       * Get a list of all component associated with this entity
       * @param toFill receives the components
       */
      void GetComponents(std::list<Component*>& toFill);
      void GetComponents(std::list<const Component*>& toFill) const;

      /**
       * @return true if a component of this type is assigned to entity
       */
      bool HasComponent(ComponentType t) const;

      /**
       * get unique identifier of this entity
       */
      EntityId GetId() const;
          
      /**
       * @return the entity manager this entity is assigned to
       */
      EntityManager& GetEntityManager() const;
      
   protected:

      ~Entity() {}

   private:

      // internal ID
      EntityId mId;

      // The entity manager that holds this entity
      EntityManager* mEntityManager;
   };


   ////////////////////////////////////////////////////////////////////////////////


   template<typename T> 
   bool Entity::CreateComponent(T*& component)
   {
      Component* c;
      bool success = CreateComponent(T::TYPE, c);
      if(success)
      {
         component = static_cast<T*>(c);
      }
      return success;
   }

   template<typename T> 
   bool Entity::GetComponent(T*& component) 
   {
       return GetEntityManager().GetComponent<T>(this->GetId(), component);
   }
}

