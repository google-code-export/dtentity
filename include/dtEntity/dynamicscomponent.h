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
#include <dtEntity/stringid.h>

namespace dtEntity
{

   
   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicsComponent
      : public Component
   {
   public:

      typedef Component BaseClass;

      static const ComponentType TYPE;
      static const StringId VelocityId;
      static const StringId AngularVelocityId;
      static const StringId AccelerationId;

      DynamicsComponent();
      virtual ~DynamicsComponent();

      virtual ComponentType GetType() const { return TYPE; }

      void OnAddedToEntity(Entity &entity) { mEntity = &entity; }

      osg::Vec3 GetVelocity() const { return mVelocityVal; }
      void SetVelocity(const osg::Vec3& v);

      const osg::Quat& GetAngularVelocity() const { return mAngularVelocity.GetAsQuat(); }
      void SetAngularVelocity(const osg::Quat& v) { mAngularVelocity.Set(v); }

      const osg::Vec3& GetAcceleration() const { return mAcceleration.GetAsVec3(); }
      void SetAcceleration(const osg::Vec3& v) { mAcceleration.Set(v); }

   protected:
      Entity* mEntity;
     
      // path to loaded script file
      DynamicVec3Property mVelocity;
      osg::Vec3 mVelocityVal;
      QuatProperty mAngularVelocity;
      Vec3Property mAcceleration;
      bool mIsMoving;

   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT DynamicsSystem
      : public DefaultEntitySystem<DynamicsComponent>
   {
   public:

      static const ComponentType TYPE;

      DynamicsSystem(EntityManager& em);
      ~DynamicsSystem();

   };
}
