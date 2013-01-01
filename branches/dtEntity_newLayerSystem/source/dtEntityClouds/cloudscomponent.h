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

#pragma once

#include <dtEntity/defaultentitysystem.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntity/dynamicproperty.h>
#include <osg/Texture2D>

namespace dtEntityCloud
{
   
   class CloudsComponent 
      : public dtEntityOSG::GroupComponent
   {
      typedef dtEntityOSG::GroupComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId CloudCoverId;  
      static const dtEntity::StringId CloudSharpnessId;  
      static const dtEntity::StringId CloudMutationRateId;  
      static const dtEntity::StringId WindId;      
      static const dtEntity::StringId SunPosId;      
      static const dtEntity::StringId TraceStartId;  
      static const dtEntity::StringId TraceDistId;  

      CloudsComponent();
      virtual ~CloudsComponent();
      
      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual osg::Group* GetAttachmentGroup() const { return mDrawables; }

      void SetSunPos(const osg::Vec3f& w);
      osg::Vec3f GetSunPos() const;

      void SetWind(const osg::Vec2f& w);
      osg::Vec2f GetWind() const;

      void SetCloudCover(float w);
      float GetCloudCover() const;

      void SetCloudSharpness(float w);
      float GetCloudSharpness() const;

      void SetCloudMutationRate(float w);
      float GetCloudMutationRate() const;

      void SetTraceStart(float w);
      float GetTraceStart() const;

      void SetTraceDist(float w);
      float GetTraceDist() const;

   private:

      osg::Camera* CreateCloudDensityCam(osg::Texture2D* tex, unsigned int texsize) const;
      osg::Geometry* CreateCloudPlane() const;

      dtEntity::DynamicVec3Property mSunPos;
      dtEntity::DynamicVec2Property mWind;
      dtEntity::DynamicFloatProperty mCloudCover;
      dtEntity::DynamicFloatProperty mCloudSharpness;
      dtEntity::DynamicFloatProperty mCloudMutationRate;
      dtEntity::DynamicFloatProperty mTraceStart;
      dtEntity::DynamicFloatProperty mTraceDist;

      osg::ref_ptr<osg::Texture2D> mNoise0;
      osg::ref_ptr<osg::Texture2D> mNoise1;
      osg::ref_ptr<osg::Texture2D> mNoise2;
      osg::ref_ptr<osg::Texture2D> mNoise3;
      osg::ref_ptr<osg::Texture2D> mCloudDensity;

      osg::ref_ptr<osg::Uniform> mCloudCoverUniform;
      osg::ref_ptr<osg::Uniform> mCloudMutationRateUniform;
      osg::ref_ptr<osg::Uniform> mCloudSharpnessUniform;     
      osg::ref_ptr<osg::Uniform> mWindUniform;
      osg::ref_ptr<osg::Uniform> mSunPosUniform;
      osg::ref_ptr<osg::Uniform> mTraceStartUniform;
      osg::ref_ptr<osg::Uniform> mTraceDistUniform;

      osg::ref_ptr<osg::Group> mDrawables;
   };


   ////////////////////////////////////////////////////////////////////////////////

   class CloudsSystem
      : public dtEntity::DefaultEntitySystem<CloudsComponent>
   {
      typedef dtEntity::DefaultEntitySystem<CloudsComponent> BaseClass;
   public:

      static const dtEntity::ComponentType TYPE;
      
      CloudsSystem(dtEntity::EntityManager& em);
      ~CloudsSystem();

   private:
      void Tick(const dtEntity::Message& msg);

      dtEntity::MessageFunctor mTickFunctor;
   };
}
