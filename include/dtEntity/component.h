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
#include <dtEntity/propertycontainer.h>
#include <dtEntity/FastDelegate.h>
#include <dtEntity/FastDelegateBind.h>

namespace dtEntity
{
   class Entity;

   /**
    * Components are lightweight property containers.
    * Components can be used as per-entity data storage for entity systems.
    * It is probably best to keep components as slim as possible and
    * keep the logic in the entity systems.
    */
   class Component
      : public PropertyContainer
   {
   public:

      Component() {}
      virtual ~Component() {}

      /**
       * Return a unique identifier for this component
       */
      virtual ComponentType GetType() const = 0;

      /**
       * Gets called when component is instantiated
       */
      virtual void OnAddedToEntity(Entity& entity) {}

      /**
       * Gets called right before component is deleted
       */
      virtual void OnRemovedFromEntity(Entity& entity) {}

   };
}

