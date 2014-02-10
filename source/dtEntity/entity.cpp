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

#include <dtEntity/entity.h>

#include <dtEntity/entitymanager.h>

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   Entity::Entity(EntityManager& em, EntityId id)
      : mId(id)
      , mEntityManager(&em)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   EntityId Entity::GetId() const
   {
      return mId;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   EntityManager& Entity::GetEntityManager() const
   {
      return *mEntityManager;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool Entity::CreateComponent(ComponentType t, Component*& component)
   {
      return mEntityManager->CreateComponent(this->GetId(), t, component);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool Entity::GetComponent(ComponentType t, Component*& component, bool derived) const
   {
      return GetEntityManager().GetComponent(this->GetId(), t, component, derived);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void Entity::GetComponents(std::vector<Component*>& toFill)
   {
      GetEntityManager().GetComponents(this->GetId(), toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void Entity::GetComponents(std::vector<const Component*>& toFill) const
   {
      GetEntityManager().GetComponents(this->GetId(), toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool Entity::HasComponent(ComponentType t) const
   {
      return GetEntityManager().HasComponent(this->GetId(), t);
   }
}
