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
#include <dtEntity/entityid.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/component.h>
#include <vector>

namespace dtEntity
{
  
   /**
    * The entity class is simply a convenience class to bundle
    * the entity ID and access to the entity manager.
    */
   class DT_ENTITY_EXPORT Entity
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
       * @param searchDerived Return components of derived typs if found
       * @return true if found
       */
      template<typename T> 
      bool GetComponent(T*& component, bool searchDerived = false) const;

      /**
       * Get component from this entity
       * @param component receives the component if found
       * @param searchDerived Return components of derived typs if found
       * @return true if found
       */
      bool GetComponent(ComponentType t, Component*& component, bool searchDerived = false) const;

      /**
       * Get a list of all component associated with this entity
       * @param toFill receives the components
       */
      void GetComponents(std::vector<Component*>& toFill);
      void GetComponents(std::vector<const Component*>& toFill) const;

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
   bool Entity::GetComponent(T*& component, bool derived) const
   {
       return GetEntityManager().GetComponent<T>(this->GetId(), component, derived);
   }
}

