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
#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace dtEntityOSG
{

   ///////////////////////////////////////////////////////////////////////////
   /**
    * abstract base class for transform components
    */
   class DTENTITY_OSG_EXPORT TransformComponent
      : public GroupComponent
   {
      typedef GroupComponent BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      TransformComponent(osg::Transform* trans);

      virtual ~TransformComponent();

      virtual dtEntity::ComponentType GetType() const {
         return TYPE;
      }


      virtual dtEntity::Vec3d GetTranslation() const = 0;
      virtual void SetTranslation(const dtEntity::Vec3d&) = 0;

      virtual dtEntity::Vec3d GetScale() const { return dtEntity::Vec3d(1,1,1); }
      virtual void SetScale(const dtEntity::Vec3d&) {}

      virtual dtEntity::Quat GetRotation() const = 0;
      virtual void SetRotation(const dtEntity::Quat&) = 0;

      virtual dtEntity::Matrix GetMatrix() const
      {
         dtEntity::Matrix matrix;
         matrix.setTrans(GetTranslation());
         matrix.setRotate(GetRotation());
         return matrix;
      }

      virtual void SetMatrix(const dtEntity::Matrix& mat)
      {
         SetTranslation(mat.getTrans());
         SetRotation(mat.getRotate());
      }


   private:

   };

   ///////////////////////////////////////////////////////////////////////////
   class DTENTITY_OSG_EXPORT TransformSystem
      : public dtEntity::EntitySystem
   {
   public:
      TransformSystem(dtEntity::EntityManager& em)
         : dtEntity::EntitySystem(em)
      {
      }

      dtEntity::ComponentType GetComponentType() const { return TransformComponent::TYPE; }
   };

}
