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
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/transformcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class PositionAttitudeTransform;
}

namespace dtEntityOSG
{

   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single PositionAttitude transform
    */
   class DTENTITY_OSG_EXPORT PositionAttitudeTransformComponent
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId PositionId;
      static const dtEntity::StringId AttitudeId;
      static const dtEntity::StringId ScaleId;
      
      PositionAttitudeTransformComponent();
     
      virtual ~PositionAttitudeTransformComponent();

      virtual dtEntity::ComponentType GetType() const {
         return TYPE; 
      }

      const osg::PositionAttitudeTransform* GetPositionAttitudeTransform() const;
      osg::PositionAttitudeTransform* GetPositionAttitudeTransform();

      dtEntity::Vec3d GetPosition() const;
      void SetPosition(const dtEntity::Vec3d& p);

      dtEntity::Quat GetAttitude() const;
      void SetAttitude(const dtEntity::Quat& r);

      dtEntity::Vec3d GetScale() const;
      void SetScale(const dtEntity::Vec3d& q);

      virtual dtEntity::Vec3d GetTranslation() const  { return GetPosition(); }
      virtual void SetTranslation(const dtEntity::Vec3d& t) { SetPosition(t); }

      virtual dtEntity::Quat GetRotation() const { return GetAttitude(); }
      virtual void SetRotation(const dtEntity::Quat& q) { SetAttitude(q); }

      virtual dtEntity::Matrix GetMatrix() const;
      virtual void SetMatrix(const dtEntity::Matrix& mat);

   private:

      dtEntity::DynamicVec3dProperty mPosition;
      dtEntity::DynamicVec3dProperty mScale;
      dtEntity::DynamicQuatProperty mAttitude;
   };

   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT PositionAttitudeTransformSystem
      : public dtEntity::DefaultEntitySystem<PositionAttitudeTransformComponent>
   {
   public:
      PositionAttitudeTransformSystem(dtEntity::EntityManager& em)
         : dtEntity::DefaultEntitySystem<PositionAttitudeTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

}
