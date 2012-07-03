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
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/transformcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class MatrixTransform;
}

namespace dtEntity
{


   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single matrix transform
    */
   class DT_ENTITY_EXPORT MatrixTransformComponent 
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId MatrixId;
      
      MatrixTransformComponent();
      MatrixTransformComponent(osg::MatrixTransform* trans);
     
      virtual ~MatrixTransformComponent();

      virtual ComponentType GetType() const { 
         return TYPE; 
      }


      const osg::MatrixTransform* GetMatrixTransform() const;
      osg::MatrixTransform* GetMatrixTransform();

      virtual osg::Matrix GetMatrix() const;
      virtual void SetMatrix(const osg::Matrix& m);
      
      virtual osg::Vec3d GetTranslation() const
      {
         osg::Matrix m = GetMatrix();
         return m.getTrans();
      }

      virtual void SetTranslation(const osg::Vec3d& t)
      {
         osg::Matrix m = GetMatrix();
         m.setTrans(t);
         SetMatrix(m);
      }

      virtual osg::Quat GetRotation() const
      {
         osg::Matrix m = GetMatrix();
         return m.getRotate();
      }

      virtual void SetRotation(const osg::Quat& q)
      {
         osg::Matrix m = GetMatrix();
         m.setRotate(q);
         SetMatrix(m);
      }

      virtual osg::Vec3d GetScale() const
      {
         osg::Matrix m = GetMatrix();
         return m.getScale();
      }

      virtual void SetScale(const osg::Vec3d& v)
      {
         osg::Matrix m = GetMatrix();
         m = m.scale(v);
         SetMatrix(m);
      }

   private:

      DynamicMatrixProperty mMatrix;
   };

   
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT MatrixTransformSystem
      : public DefaultEntitySystem<MatrixTransformComponent>
   {
   public:
      MatrixTransformSystem(EntityManager& em)
         : DefaultEntitySystem<MatrixTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

}
