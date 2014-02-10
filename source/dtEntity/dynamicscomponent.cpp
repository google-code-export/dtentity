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
