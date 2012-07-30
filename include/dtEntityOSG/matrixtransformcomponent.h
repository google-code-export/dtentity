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

#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/transformcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class MatrixTransform;
}

namespace dtEntityOSG
{


   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single matrix transform
    */
   class DTENTITY_OSG_EXPORT MatrixTransformComponent
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId MatrixId;
      
      MatrixTransformComponent();
      MatrixTransformComponent(osg::MatrixTransform* trans);
     
      virtual ~MatrixTransformComponent();

      virtual dtEntity::ComponentType GetType() const {
         return TYPE; 
      }


      const osg::MatrixTransform* GetMatrixTransform() const;
      osg::MatrixTransform* GetMatrixTransform();

      virtual dtEntity::Matrix GetMatrix() const;
      virtual void SetMatrix(const dtEntity::Matrix& m);
      
      virtual dtEntity::Vec3d GetTranslation() const
      {
         dtEntity::Matrix m = GetMatrix();
         return m.getTrans();
      }

      virtual void SetTranslation(const dtEntity::Vec3d& t)
      {
         dtEntity::Matrix m = GetMatrix();
         m.setTrans(t);
         SetMatrix(m);
      }

      virtual dtEntity::Quat GetRotation() const
      {
         dtEntity::Matrix m = GetMatrix();
         return m.getRotate();
      }

      virtual void SetRotation(const dtEntity::Quat& q)
      {
         dtEntity::Matrix m = GetMatrix();
         m.setRotate(q);
         SetMatrix(m);
      }

      virtual dtEntity::Vec3d GetScale() const
      {
         dtEntity::Matrix m = GetMatrix();
         return m.getScale();
      }

      virtual void SetScale(const dtEntity::Vec3d& v)
      {
         dtEntity::Matrix m = GetMatrix();
         m = m.scale(v);
         SetMatrix(m);
      }

   private:

      dtEntity::DynamicMatrixProperty mMatrix;
   };

   
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT MatrixTransformSystem
      : public dtEntity::DefaultEntitySystem<MatrixTransformComponent>
   {
   public:
      MatrixTransformSystem(dtEntity::EntityManager& em)
         : dtEntity::DefaultEntitySystem<MatrixTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

}
