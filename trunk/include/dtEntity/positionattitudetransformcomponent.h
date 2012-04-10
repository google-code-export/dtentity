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
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/transformcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>
#include <osg/Group>

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single PositionAttitude transform
    */
   class DT_ENTITY_EXPORT PositionAttitudeTransformComponent 
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId PositionId;
      static const StringId AttitudeId;
      static const StringId ScaleId;
      
      PositionAttitudeTransformComponent();
     
      virtual ~PositionAttitudeTransformComponent();

      virtual ComponentType GetType() const { 
         return TYPE; 
      }

      const osg::PositionAttitudeTransform* GetPositionAttitudeTransform() const;
      osg::PositionAttitudeTransform* GetPositionAttitudeTransform();

      osg::Vec3d GetPosition() const;
      void SetPosition(const osg::Vec3d& p);

      osg::Quat GetAttitude() const;
      void SetAttitude(const osg::Quat& r);

      osg::Vec3d GetScale() const;
      void SetScale(const osg::Vec3d& q);

      virtual osg::Vec3d GetTranslation() const  { return GetPosition(); }
      virtual void SetTranslation(const osg::Vec3d& t) { SetPosition(t); }

      virtual osg::Quat GetRotation() const { return GetAttitude(); }
      virtual void SetRotation(const osg::Quat& q) { SetAttitude(q); }

      virtual osg::Matrix GetMatrix() const;
      virtual void SetMatrix(const osg::Matrix& mat);

   private:

      DynamicVec3dProperty mPosition;
      DynamicVec3dProperty mScale;
      DynamicQuatProperty mAttitude;
   };

   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT PositionAttitudeTransformSystem
      : public DefaultEntitySystem<PositionAttitudeTransformComponent>
   {
   public:
      PositionAttitudeTransformSystem(EntityManager& em)
         : DefaultEntitySystem<PositionAttitudeTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

}
