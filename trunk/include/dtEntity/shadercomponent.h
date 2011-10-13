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
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/component.h>
#include <dtEntity/stringid.h>
#include <dtEntity/message.h>
#include <osg/Group>

namespace dtEntity
{ 

   class DT_ENTITY_EXPORT ShaderComponent 
      : public Component
   {
  
   public:

      static const ComponentType TYPE;
      static const StringId ShaderNameId;
      
      ShaderComponent();
     
      virtual ~ShaderComponent();

      void OnAddedToEntity(dtEntity::Entity& entity);

      virtual ComponentType GetType() const { 
         return TYPE; 
      }

      virtual bool IsInstanceOf(ComponentType id) const
      { 
         return ((id == TYPE)); 
      }

      void ApplyShader();

   private:
      StringProperty mShaderName;
      dtEntity::Entity* mEntity;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT ShaderSystem
      : public DefaultEntitySystem<ShaderComponent>
   {
   public:
     
      ShaderSystem(EntityManager& em);
      void OnRemoveFromEntityManager(dtEntity::EntityManager& em);
      void OnEnterWorld(const Message&);

   private:
      MessageFunctor mEnterWorldFunctor;
    };
}
