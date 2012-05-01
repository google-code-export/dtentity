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

#include <dtEntity/positionattitudetransformcomponent.h>
#include <osg/PositionAttitudeTransform>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId PositionAttitudeTransformComponent::TYPE(dtEntity::SID("PositionAttitudeTransform"));
   const StringId PositionAttitudeTransformComponent::PositionId(dtEntity::SID("Position"));
   const StringId PositionAttitudeTransformComponent::AttitudeId(dtEntity::SID("Attitude"));
   const StringId PositionAttitudeTransformComponent::ScaleId(dtEntity::SID("Scale"));
   
   ////////////////////////////////////////////////////////////////////////////
   PositionAttitudeTransformComponent::PositionAttitudeTransformComponent()
      : BaseClass(new osg::PositionAttitudeTransform())
      , mPosition(
           DynamicVec3dProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetPosition),
           DynamicVec3dProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetPosition)
        )
      , mScale(
           DynamicVec3dProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetScale),
           DynamicVec3dProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetScale)
        )
      , mAttitude(
           DynamicQuatProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetAttitude),
           DynamicQuatProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetAttitude)
        )
   {
      Register(PositionId, &mPosition);
      Register(AttitudeId, &mAttitude);
      Register(ScaleId, &mScale);
      mScale.Set(osg::Vec3(1, 1, 1));
      GetNode()->setName("PositionAttitudeTransformComponent");

   }

   ////////////////////////////////////////////////////////////////////////////
   PositionAttitudeTransformComponent::~PositionAttitudeTransformComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Matrix PositionAttitudeTransformComponent::GetMatrix() const
   {
      osg::Matrix t;
      t.makeTranslate(GetTranslation());
      t.setRotate(GetRotation());
      osg::Matrix s;
      s.makeScale(GetScale());
      return s * t;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetMatrix(const osg::Matrix& mat)
   {
      osg::Vec3d trans, scale;
      osg::Quat rot, so;
      mat.decompose(trans, rot, scale, so);
      SetRotation(rot);
      SetTranslation(trans);
      SetScale(scale);

   }

   ////////////////////////////////////////////////////////////////////////////
   osg::PositionAttitudeTransform* PositionAttitudeTransformComponent::GetPositionAttitudeTransform()
   {
      return static_cast<osg::PositionAttitudeTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   const osg::PositionAttitudeTransform* PositionAttitudeTransformComponent::GetPositionAttitudeTransform() const
   {
      return static_cast<osg::PositionAttitudeTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d PositionAttitudeTransformComponent::GetPosition() const
   {
      return GetPositionAttitudeTransform()->getPosition();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetPosition(const osg::Vec3d& p)
   {
      GetPositionAttitudeTransform()->setPosition(p);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Quat PositionAttitudeTransformComponent::GetAttitude() const
   {
      return GetPositionAttitudeTransform()->getAttitude();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetAttitude(const osg::Quat& p)
   {
      GetPositionAttitudeTransform()->setAttitude(p);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d PositionAttitudeTransformComponent::GetScale() const
   {
      return GetPositionAttitudeTransform()->getScale();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetScale(const osg::Vec3d& s)
   {
      GetPositionAttitudeTransform()->setScale(s);
   }

}
