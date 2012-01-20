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
   void Entity::GetComponents(std::list<Component*>& toFill)
   {
      GetEntityManager().GetComponents(this->GetId(), toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void Entity::GetComponents(std::list<const Component*>& toFill) const
   {
      GetEntityManager().GetComponents(this->GetId(), toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool Entity::HasComponent(ComponentType t) const
   {
      return GetEntityManager().HasComponent(this->GetId(), t);
   }
}
