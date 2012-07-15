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
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/groupcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////
   /**
    * abstract base class for transform components
    */
   class DT_ENTITY_EXPORT TransformComponent
      : public GroupComponent
   {
      typedef GroupComponent BaseClass;

   public:

      static const ComponentType TYPE;

      TransformComponent(osg::Transform* trans);

      virtual ~TransformComponent();

      virtual ComponentType GetType() const {
         return TYPE;
      }


      virtual Vec3d GetTranslation() const = 0;
      virtual void SetTranslation(const Vec3d&) = 0;

      virtual Vec3d GetScale() const { return Vec3d(1,1,1); }
      virtual void SetScale(const Vec3d&) {}

      virtual Quat GetRotation() const = 0;
      virtual void SetRotation(const Quat&) = 0;

      virtual Matrix GetMatrix() const
      {
         Matrix matrix;
         matrix.setTrans(GetTranslation());
         matrix.setRotate(GetRotation());
         return matrix;
      }

      virtual void SetMatrix(const Matrix& mat)
      {
         SetTranslation(mat.getTrans());
         SetRotation(mat.getRotate());
      }


   private:

   };

   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT TransformSystem
      : public EntitySystem
   {
   public:
      TransformSystem(EntityManager& em)
         : EntitySystem(em)
      {
      }

      ComponentType GetComponentType() const { return TransformComponent::TYPE; }
   };

}
