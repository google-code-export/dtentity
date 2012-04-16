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

#include <dtEntity/matrixtransformcomponent.h>
#include <osg/MatrixTransform>

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId MatrixTransformComponent::TYPE(SID("MatrixTransform"));
   const StringId MatrixTransformComponent::MatrixId(SID("Matrix"));
   
   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::MatrixTransformComponent()
      : BaseClass(new osg::MatrixTransform())
      , mMatrix(
           DynamicMatrixProperty::SetValueCB(this, &MatrixTransformComponent::SetMatrix),
           DynamicMatrixProperty::GetValueCB(this, &MatrixTransformComponent::GetMatrix)
        )
   {
      Register(MatrixId, &mMatrix);
      GetNode()->setName("MatrixTransformComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::MatrixTransformComponent(osg::MatrixTransform* trans)
      : BaseClass(trans)
      , mMatrix(
           DynamicMatrixProperty::SetValueCB(this, &MatrixTransformComponent::SetMatrix),
           DynamicMatrixProperty::GetValueCB(this, &MatrixTransformComponent::GetMatrix)
        )
   {
      Register(MatrixId, &mMatrix);
      GetNode()->setName("MatrixTransformComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::~MatrixTransformComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::MatrixTransform* MatrixTransformComponent::GetMatrixTransform()
   {
      return static_cast<osg::MatrixTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   const osg::MatrixTransform* MatrixTransformComponent::GetMatrixTransform() const
   {
      return static_cast<osg::MatrixTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Matrix MatrixTransformComponent::GetMatrix() const
   {
      return GetMatrixTransform()->getMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void MatrixTransformComponent::SetMatrix(const osg::Matrix& m)
   {
      GetMatrixTransform()->setMatrix(m);
   }
}
