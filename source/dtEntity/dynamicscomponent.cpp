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

#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/systemmessages.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////
   const StringId DynamicsComponent::TYPE(dtEntity::SID("Dynamics"));
   const StringId DynamicsComponent::VelocityId(dtEntity::SID("Velocity"));
   const StringId DynamicsComponent::AngularVelocityId(dtEntity::SID("AngularVelocity"));
   const StringId DynamicsComponent::AccelerationId(dtEntity::SID("Acceleration"));

   ////////////////////////////////////////////////////////////////////////////
   DynamicsComponent::DynamicsComponent()
      : mEntity(NULL)
      , mVelocity(
         dtEntity::DynamicVec3Property::SetValueCB(this, &DynamicsComponent::SetVelocity),
         dtEntity::DynamicVec3Property::GetValueCB(this, &DynamicsComponent::GetVelocity)
      )
      , mAngularVelocity(Quat(0,0,0,1))
      , mIsMoving(false)
   {
      Register(VelocityId, &mVelocity);
      Register(AngularVelocityId, &mAngularVelocity);
      Register(AccelerationId, &mAcceleration);
   }

   ////////////////////////////////////////////////////////////////////////////
   DynamicsComponent::~DynamicsComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void DynamicsComponent::SetVelocity(const Vec3f& v)
   {
      mVelocityVal = v;
      float delta = 0.0001f;
      bool isMoving = fabs(v[0]) > delta || fabs(v[1]) > delta || fabs(v[2]) > delta;

      if(isMoving != mIsMoving)
      {
         mIsMoving = isMoving;
         if(mEntity)
         {
            EntityVelocityNotNullMessage msg;
            msg.SetAboutEntityId(mEntity->GetId());
            msg.SetIsNull(!isMoving);
            mEntity->GetEntityManager().EmitMessage(msg);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId DynamicsSystem::TYPE(dtEntity::SID("Dynamics"));

   ////////////////////////////////////////////////////////////////////////////
   DynamicsSystem::DynamicsSystem(EntityManager& em)
      : DefaultEntitySystem<DynamicsComponent>(em)
   {

   }

   ////////////////////////////////////////////////////////////////////////////
   DynamicsSystem::~DynamicsSystem()
   {
   }
}
