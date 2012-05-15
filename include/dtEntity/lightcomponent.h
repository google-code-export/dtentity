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
#include <dtEntity/entity.h>
#include <dtEntity/entityid.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/nodecomponent.h>


namespace osg
{
   class Light;
   class LightSource;
}

namespace dtEntity
{      
  
   class DT_ENTITY_EXPORT LightComponent : public dtEntity::NodeComponent
   {

      typedef dtEntity::NodeComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;

      static const dtEntity::StringId LightNumId;
      static const dtEntity::StringId PositionId;
      static const dtEntity::StringId AmbientId;
      static const dtEntity::StringId DiffuseId;
      static const dtEntity::StringId SpecularId;
      static const dtEntity::StringId SpotCutoffId;
      static const dtEntity::StringId SpotExponentId;
      static const dtEntity::StringId DirectionId;
      static const dtEntity::StringId ConstantAttenuationId;
      static const dtEntity::StringId LinearAttenuationId;
      static const dtEntity::StringId QuadraticAttenuationId;

      
      LightComponent();
      virtual ~LightComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }      

      virtual void Finished();

      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);

      osg::Light* GetLight() { return mLight; }

   private:

      osg::Light* mLight;
      IntProperty mLightNum;
      Vec4Property mAmbient;
      Vec4Property mDiffuse;
      Vec4Property mSpecular;
      Vec4Property mPosition;
      Vec3Property mDirection;
      FloatProperty mConstantAttenuation;
      FloatProperty mLinearAttenuation;
      FloatProperty mQuadraticAttenuation;
      FloatProperty mSpotExponent;
      FloatProperty mSpotCutOff;
   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DT_ENTITY_EXPORT LightSystem
      : public dtEntity::DefaultEntitySystem<LightComponent>
   {
      typedef dtEntity::DefaultEntitySystem<LightComponent> BaseClass;
      
   public:
      static const dtEntity::ComponentType TYPE;

      LightSystem(dtEntity::EntityManager& em);     

   private:

   };
}
