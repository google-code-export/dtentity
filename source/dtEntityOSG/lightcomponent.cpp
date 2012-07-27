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

#include <dtEntityOSG/lightcomponent.h>

#include <dtEntity/nodemasks.h>
#include <dtEntity/stringid.h>
#include <osg/Light>
#include <osg/LightSource>

namespace dtEntityOSG
{
   const dtEntity::StringId LightComponent::TYPE(dtEntity::SID("Light"));
   const dtEntity::StringId LightComponent::LightNumId(dtEntity::SID("LightNum"));
   const dtEntity::StringId LightComponent::PositionId(dtEntity::SID("Position"));
   const dtEntity::StringId LightComponent::AmbientId(dtEntity::SID("Ambient"));
   const dtEntity::StringId LightComponent::DiffuseId(dtEntity::SID("Diffuse"));
   const dtEntity::StringId LightComponent::SpecularId(dtEntity::SID("Specular"));
   const dtEntity::StringId LightComponent::SpotCutoffId(dtEntity::SID("SpotCutoff"));
   const dtEntity::StringId LightComponent::SpotExponentId(dtEntity::SID("SpotExponent"));
   const dtEntity::StringId LightComponent::DirectionId(dtEntity::SID("Direction"));
   const dtEntity::StringId LightComponent::ConstantAttenuationId(dtEntity::SID("ConstantAttenuation"));
   const dtEntity::StringId LightComponent::LinearAttenuationId(dtEntity::SID("LinearAttenuation"));
   const dtEntity::StringId LightComponent::QuadraticAttenuationId(dtEntity::SID("QuadraticAttenuation"));

   ////////////////////////////////////////////////////////////////////////////
   LightComponent::LightComponent()
      : dtEntity::NodeComponent(new osg::LightSource())
   {      
      using namespace dtEntity;

      mLight = new osg::Light();
      osg::LightSource* lightSource = static_cast<osg::LightSource*>(GetNode());
      lightSource->setLight(mLight);

      mLightNum = DynamicIntProperty(DynamicIntProperty::SetValueCB(mLight, &osg::Light::setLightNum),
           DynamicIntProperty::GetValueCB(mLight, &osg::Light::getLightNum));
      
      mPosition = DynamicVec4Property(DynamicVec4Property::SetValueCB(mLight, &osg::Light::setPosition),
           DynamicVec4Property::GetValueCB(this, &LightComponent::GetLightPosition));

      mAmbient = DynamicVec4Property(DynamicVec4Property::SetValueCB(mLight, &osg::Light::setAmbient),
           DynamicVec4Property::GetValueCB(this, &LightComponent::GetAmbient));

      mDiffuse = DynamicVec4Property(DynamicVec4Property::SetValueCB(mLight, &osg::Light::setDiffuse),
           DynamicVec4Property::GetValueCB(this, &LightComponent::GetDiffuse));

      mSpecular = DynamicVec4Property(DynamicVec4Property::SetValueCB(mLight, &osg::Light::setSpecular),
           DynamicVec4Property::GetValueCB(this, &LightComponent::GetSpecular));

      mSpotCutOff = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(mLight, &osg::Light::setSpotCutoff),
         DynamicFloatProperty::GetValueCB(mLight, &osg::Light::getSpotCutoff));

      mSpotExponent = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(mLight, &osg::Light::setSpotExponent),
         DynamicFloatProperty::GetValueCB(mLight, &osg::Light::getSpotExponent));

      mDirection = DynamicVec3Property(DynamicVec3Property::SetValueCB(mLight, &osg::Light::setDirection),
           DynamicVec3Property::GetValueCB(this, &LightComponent::GetDirection));

      mConstantAttenuation = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(mLight, &osg::Light::setConstantAttenuation),
         DynamicFloatProperty::GetValueCB(mLight, &osg::Light::getConstantAttenuation));

      mLinearAttenuation = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(mLight, &osg::Light::setLinearAttenuation),
         DynamicFloatProperty::GetValueCB(mLight, &osg::Light::getLinearAttenuation));

      mQuadraticAttenuation = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(mLight, &osg::Light::setQuadraticAttenuation),
         DynamicFloatProperty::GetValueCB(mLight, &osg::Light::getQuadraticAttenuation));

      Register(LightNumId, &mLightNum);
      Register(PositionId, &mPosition);
      Register(AmbientId, &mAmbient);
      Register(DiffuseId, &mDiffuse);
      Register(SpecularId, &mSpecular);
      Register(SpotCutoffId, &mSpotCutOff);
      Register(SpotExponentId, &mSpotExponent);
      Register(DirectionId, &mDirection);
      Register(ConstantAttenuationId, &mConstantAttenuation);
      Register(LinearAttenuationId, &mLinearAttenuation);
      Register(QuadraticAttenuationId, &mQuadraticAttenuation);

      mLight->setLightNum(0);
      mLight->setPosition(osg::Vec4(1.0f,1.0f,1.0f,0.0f)); // directional light from above
      mLight->setAmbient(osg::Vec4(0.3f,0.3f,0.3f,1.0f));
      mLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
      mLight->setSpecular(osg::Vec4(0.2f,0.2f,0.2f,1.0f));

      mAmbient.Set(mLight->getAmbient());
      mDiffuse.Set(mLight->getDiffuse());
      mSpecular.Set(mLight->getSpecular());
      mSpotCutOff.Set(mLight->getSpotCutoff());
      mSpotExponent.Set(mLight->getSpotExponent());
      mDirection.Set(mLight->getDirection());
      mConstantAttenuation.Set(mLight->getConstantAttenuation());
      mLinearAttenuation.Set(mLight->getLinearAttenuation());
      mQuadraticAttenuation.Set(mLight->getQuadraticAttenuation());
   }
    
   ////////////////////////////////////////////////////////////////////////////
   LightComponent::~LightComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 LightComponent::GetLightPosition() const 
   { 
      return mLight->getPosition(); 
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 LightComponent::GetAmbient() const 
   { 
      return mLight->getAmbient(); 
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 LightComponent::GetDiffuse() const 
   { 
      return mLight->getDiffuse(); 
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 LightComponent::GetSpecular() const 
   { 
      return mLight->getSpecular(); 
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 LightComponent::GetDirection() const 
   { 
      return mLight->getDirection(); 
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId LightSystem::TYPE(dtEntity::SID("Light"));

   ////////////////////////////////////////////////////////////////////////////
   LightSystem::LightSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
   }  

   
}
