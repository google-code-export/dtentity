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

