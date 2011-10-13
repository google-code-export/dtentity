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

#include <dtEntity/shadowcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/entity.h>
#include <assert.h>
#include <osgShadow/ShadowedScene>

#include <osg/LightSource>
#include <osg/Light>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgViewer/View>

namespace dtEntity
{
  
   ////////////////////////////////////////////////////////////////////////////////
   const StringId ShadowComponent::TYPE(SID("Shadow"));   
   const StringId ShadowComponent::ShadowTechniqueId(SID("ShadowTechnique"));    
   const StringId ShadowComponent::MinLightMarginId(SID("MinLightMargin"));    
   const StringId ShadowComponent::MaxFarPlaneId(SID("MaxFarPlane"));    
   const StringId ShadowComponent::TexSizeId(SID("TexSize"));    
   const StringId ShadowComponent::BaseTexUnitId(SID("BaseTexUnit"));    
   const StringId ShadowComponent::ShadowTexUnitId(SID("ShadowTexUnit"));
   const StringId ShadowComponent::ShadowTexCoordIndexId(SID("ShadowTexCoordIndex"));   
   const StringId ShadowComponent::BaseTexCoordIndexId(SID("BaseTexCoordIndex"));   
   const StringId ShadowComponent::PSSMMapCountId(SID("PSSMMapCount"));   
   const StringId ShadowComponent::PSSMMapResId(SID("PSSMMapRes"));   
   const StringId ShadowComponent::PSSMMapDebugColorOnId(SID("PSSMMapDebugColorOn"));   
   const StringId ShadowComponent::PSSMMinNearSplitId(SID("PSSMMinNearSplit"));   
   const StringId ShadowComponent::PSSMMaxFarDistId(SID("PSSMMaxFarDist"));   
   const StringId ShadowComponent::PSSMMoveVCamFactorId(SID("PSSMMoveVCamFactor"));   
   const StringId ShadowComponent::PSSMPolyOffsetFactorId(SID("PSSMPolyOffsetFactor"));   
   const StringId ShadowComponent::PSSMPolyOffsetUnitId(SID("PSSMPolyOffsetUnit"));   
  

   ////////////////////////////////////////////////////////////////////////////
   ShadowComponent::ShadowComponent()
      : BaseClass(new osgShadow::ShadowedScene())
      , mEntity(NULL)
   {
      Register(ShadowTechniqueId, &mShadowTechnique);
      Register(MinLightMarginId, &mMinLightMargin);
      Register(MaxFarPlaneId, &mMaxFarPlane);
      Register(TexSizeId, &mTexSize);
      Register(BaseTexUnitId, &mBaseTexUnit);
      Register(ShadowTexUnitId, &mShadowTexUnit);
      Register(ShadowTexCoordIndexId, &mShadowTexCoordIndex);
      Register(BaseTexCoordIndexId, &mBaseTexCoordIndex);
      
      Register(PSSMMapCountId, &mPSSMMapCount);
      Register(PSSMMapResId, &mPSSMMapRes);
      Register(PSSMMapDebugColorOnId, &mPSSMMapDebugColorOn);
      Register(PSSMMinNearSplitId, &mPSSMMinNearSplit);
      Register(PSSMMaxFarDistId, &mPSSMMaxFarDist);
      Register(PSSMMoveVCamFactorId, &mPSSMMoveVCamFactor);
      Register(PSSMPolyOffsetFactorId, &mPSSMPolyOffsetFactor);
      Register(PSSMPolyOffsetUnitId, &mPSSMPolyOffsetUnit);
      
      mShadowTechnique.Set("LISPSM");
      mMinLightMargin.Set(10.0f);
      mMaxFarPlane.Set(0);
      mTexSize.Set(1024);
      mBaseTexUnit.Set(0);
      mShadowTexUnit.Set(1);
      mShadowTexCoordIndex.Set(1);
      mBaseTexCoordIndex.Set(0);

      mPSSMMapCount.Set(3);
      mPSSMMapRes.Set(1024);
      mPSSMMapDebugColorOn.Set(false);
      mPSSMMinNearSplit.Set(1);
      mPSSMMaxFarDist.Set(100);
      mPSSMMoveVCamFactor.Set(0);
      mPSSMPolyOffsetFactor.Set(0);
      mPSSMPolyOffsetUnit.Set(0);

   }
  
   ////////////////////////////////////////////////////////////////////////////
   ShadowComponent::~ShadowComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShadowComponent::OnAddedToEntity(Entity& entity)
   {
      mEntity = &entity;
      BaseClass::OnAddedToEntity(entity);
   }


   ////////////////////////////////////////////////////////////////////////////
   void ShadowComponent::OnFinishedSettingProperties()
   { 
      BaseClass::OnFinishedSettingProperties();
      assert(mEntity != NULL);

      osgShadow::MinimalShadowMap* msm = NULL;
      osgShadow::ParallelSplitShadowMap* pssm = NULL;

      if(mShadowTechnique.Get() == "LISPSM")
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapDB();
         mTechnique = msm;
      }
      else if(mShadowTechnique.Get() == "LISPSM_ViewBounds")
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapVB();
         mTechnique = msm;
      }
      else if(mShadowTechnique.Get() == "LISPSM_CullBounds")
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapCB();
         mTechnique = msm;
      }
      else if(mShadowTechnique.Get() == "PSSM")
      {
         pssm = new osgShadow::ParallelSplitShadowMap(0, mPSSMMapCount.Get());
         pssm->setTextureResolution(mPSSMMapRes.Get());

         if(mPSSMMinNearSplit.Get() != 0)
         {
            pssm->setMinNearDistanceForSplits(mPSSMMinNearSplit.Get());
         }
         if(mPSSMMaxFarDist.Get() != 0)
         {
            pssm->setMaxFarDistance(mPSSMMaxFarDist.Get());
            pssm->setMoveVCamBehindRCamFactor(mPSSMMoveVCamFactor.Get());
         }
         if(mPSSMMapDebugColorOn.Get())
         {
            pssm->setDebugColorOn();
         }
         pssm->setPolygonOffset(osg::Vec2(mPSSMPolyOffsetFactor.Get(),mPSSMPolyOffsetUnit.Get())); 
         mTechnique = pssm;

         /*osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
         ls->getLight()->setPosition(osg::Vec4(1000, 1000, 1000,1));
         osg::Vec3 lightdir(0.5f, 0.5f, -0.5f);
         lightdir.normalize();
         ls->getLight()->setDirection(lightdir);
         //ls->getLight()->setSpotCutoff(2500.0f);
         GetGroup()->addChild(ls);*/

      }
      else
      {
         return;
      }

      osgShadow::ShadowedScene* shadowedScene = static_cast<osgShadow::ShadowedScene*>(GetNode());
      shadowedScene->setShadowTechnique(mTechnique);

      ApplicationSystem* appsys;
      mEntity->GetEntityManager().GetEntitySystem(ApplicationSystem::TYPE, appsys);
      
      osg::ref_ptr<osg::Light> light = appsys->GetPrimaryView()->getLight();
      
      if(msm)
      {
         msm->setTextureSize( osg::Vec2s(mTexSize.Get(), mTexSize.Get()) );
         msm->setMinLightMargin(mMinLightMargin.Get());
         msm->setMaxFarPlane(mMaxFarPlane.Get());
         msm->setBaseTextureCoordIndex(mBaseTexCoordIndex.Get());
         msm->setShadowTextureCoordIndex(mShadowTexCoordIndex.Get());
         msm->setShadowTextureUnit(mShadowTexUnit.Get());
         msm->setBaseTextureUnit(mBaseTexUnit.Get());
         msm->setLight(light);

         // setting these for PSSM breaks shadows, so only do it here ...
         shadowedScene->setReceivesShadowTraversalMask(NodeMasks::RECEIVES_SHADOWS);
         shadowedScene->setCastsShadowTraversalMask(NodeMasks::CASTS_SHADOWS);
      }

      if(pssm)
      {
         LOG_ALWAYS("TODO: Re-Implement!");
         /*lightsource->getParent(0)->removeChild(lightsource);
         GetGroup()->addChild(lightsource);*/
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ShadowSystem::ShadowSystem(EntityManager& em)
      : DefaultEntitySystem<ShadowComponent>(em)
   {
   }
}
