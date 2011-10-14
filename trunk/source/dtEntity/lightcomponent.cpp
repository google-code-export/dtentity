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

#include <dtEntity/lightcomponent.h>


#include <dtEntity/basemessages.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/stringid.h>
#include <osg/Light>
#include <osg/LightSource>
namespace dtEntity
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

      mLight = new osg::Light();
      osg::LightSource* lightSource = static_cast<osg::LightSource*>(GetNode());
      lightSource->setLight(mLight);

      mLight->setLightNum(0);
      mLight->setPosition(osg::Vec4(1.0f,1.0f,1.0f,0.0f)); // directional light from above
      mLight->setAmbient(osg::Vec4(0.3f,0.3f,0.3f,1.0f));
      mLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
      mLight->setSpecular(osg::Vec4(0.2f,0.2f,0.2f,1.0f));

      mLightNum.Set(0);
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
   void LightComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {      
      if(propname == LightNumId)
      {
         mLight->setLightNum(prop.IntValue());
      }
      else if(propname == PositionId)
      {
         mLight->setPosition(prop.Vec4Value());
      }
      else if(propname == AmbientId)
      {
         mLight->setAmbient(prop.Vec4Value());
      }
      else if(propname == DiffuseId)
      {
         mLight->setDiffuse(prop.Vec4Value());
      }
      else if(propname == SpecularId)
      {
         mLight->setSpecular(prop.Vec4Value());
      }
      else if(propname == SpotCutoffId)
      {
         mLight->setSpotCutoff(prop.FloatValue());
      }
      else if(propname == SpotExponentId)
      {
         mLight->setSpotExponent(prop.FloatValue());
      }
      else if(propname == DirectionId)
      {
         mLight->setDirection(prop.Vec3Value());
      }
      else if(propname == ConstantAttenuationId)
      {
         mLight->setConstantAttenuation(prop.FloatValue());
      }
      else if(propname == LinearAttenuationId)
      {
         mLight->setLinearAttenuation(prop.FloatValue());
      }
      else if(propname == QuadraticAttenuationId)
      {
         mLight->setQuadraticAttenuation(prop.FloatValue());
      }
      else
      {
         BaseClass::OnPropertyChanged(propname, prop);
      }
   }

 

   ////////////////////////////////////////////////////////////////////////////
   void LightComponent::OnFinishedSettingProperties()
   {
     
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   //const dtEntity::StringId LightSystem::EnabledId(dtEntity::SID("Enabled"));

   LightSystem::LightSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
   }  

   
}
