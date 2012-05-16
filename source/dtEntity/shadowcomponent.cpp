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

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/lightcomponent.h>
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
   const StringId ShadowComponent::TYPE(dtEntity::SID("Shadow"));   
   const StringId ShadowComponent::ShadowTechniqueId(dtEntity::SID("ShadowTechnique"));    
   const StringId ShadowComponent::MinLightMarginId(dtEntity::SID("MinLightMargin"));    
   const StringId ShadowComponent::MaxFarPlaneId(dtEntity::SID("MaxFarPlane"));    
   const StringId ShadowComponent::TexSizeId(dtEntity::SID("TexSize"));    
   const StringId ShadowComponent::BaseTexUnitId(dtEntity::SID("BaseTexUnit"));    
   const StringId ShadowComponent::ShadowTexUnitId(dtEntity::SID("ShadowTexUnit"));
   const StringId ShadowComponent::ShadowTexCoordIndexId(dtEntity::SID("ShadowTexCoordIndex"));   
   const StringId ShadowComponent::BaseTexCoordIndexId(dtEntity::SID("BaseTexCoordIndex"));   
   const StringId ShadowComponent::MapCountId(dtEntity::SID("MapCount"));
   const StringId ShadowComponent::MapResId(dtEntity::SID("MapRes"));
   const StringId ShadowComponent::DebugColorOnId(dtEntity::SID("DebugColorOn"));
   const StringId ShadowComponent::MinNearSplitId(dtEntity::SID("MinNearSplit"));
   const StringId ShadowComponent::MaxFarDistId(dtEntity::SID("MaxFarDist"));
   const StringId ShadowComponent::MoveVCamFactorId(dtEntity::SID("MoveVCamFactor"));
   const StringId ShadowComponent::PolyOffsetFactorId(dtEntity::SID("PolyOffsetFactor"));
   const StringId ShadowComponent::PolyOffsetUnitId(dtEntity::SID("PolyOffsetUnit"));
   const StringId ShadowComponent::EnabledId(dtEntity::SID("Enabled"));
   const StringId ShadowComponent::ShadowTypeId(dtEntity::SID("ShadowType"));
   const StringId ShadowComponent::LISPId(dtEntity::SID("LISP"));
   const StringId ShadowComponent::PSSMId(dtEntity::SID("PSSM"));
   const StringId ShadowComponent::__SELECTED__Id(dtEntity::SID("__SELECTED__"));



   ////////////////////////////////////////////////////////////////////////////
   ShadowComponent::ShadowComponent()
      : BaseClass(new osgShadow::ShadowedScene())
      , mEntity(NULL)
   {
      Register(EnabledId, &mEnabled);
      Register(ShadowTypeId, &mShadowType);

      mEnabled.Set(true);

      GroupProperty* pssm = new GroupProperty;
      pssm->Add(MapCountId, new UIntProperty(3));
      pssm->Add(MapResId, new UIntProperty(1024));
      pssm->Add(DebugColorOnId, new BoolProperty(false));
      pssm->Add(MinNearSplitId, new UIntProperty(3));
      pssm->Add(MaxFarDistId, new UIntProperty(200));
      pssm->Add(MoveVCamFactorId, new FloatProperty(0.1));
      pssm->Add(PolyOffsetFactorId, new FloatProperty(0.1));
      pssm->Add(PolyOffsetUnitId, new FloatProperty(0.1));

      GroupProperty* lispsm = new GroupProperty;
      lispsm->Add(TexSizeId, new UIntProperty(1024));
      lispsm->Add(BaseTexUnitId, new UIntProperty(0));
      lispsm->Add(ShadowTexUnitId, new UIntProperty(0));
      lispsm->Add(MinLightMarginId, new FloatProperty(10));
      lispsm->Add(MaxFarPlaneId, new FloatProperty(0));
      lispsm->Add(ShadowTexCoordIndexId, new UIntProperty(1));
      lispsm->Add(BaseTexCoordIndexId, new UIntProperty(0));

      mShadowType.Add(__SELECTED__Id, new StringIdProperty(PSSMId));
      mShadowType.Add(PSSMId, pssm);
      mShadowType.Add(LISPId, lispsm);
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
   void ShadowComponent::Finished()
   { 
      BaseClass::Finished();
      assert(mEntity != NULL);

      osgShadow::MinimalShadowMap* msm = NULL;
      osgShadow::ParallelSplitShadowMap* pssm = NULL;

      PropertyGroup props = mShadowType.GroupValue();
      StringId selected = props[__SELECTED__Id]->StringIdValue();

      if(mEnabled.Get() == false)
      {
         mTechnique = NULL;
      }
      else if(selected == LISPId)
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapDB();
         mTechnique = msm;
      }/*
      else if(mShadowTechnique.Get() == "LISPSM_ViewBounds")
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapVB();
         mTechnique = msm;
      }
      else if(mShadowTechnique.Get() == "LISPSM_CullBounds")
      {
         msm = new osgShadow::LightSpacePerspectiveShadowMapCB();
         mTechnique = msm;
      }*/
      else if(selected == PSSMId)
      {
         PropertyGroup pssmgrp = props[PSSMId]->GroupValue();
         pssm = new osgShadow::ParallelSplitShadowMap(0, pssmgrp[MapCountId]->UIntValue());
         pssm->setTextureResolution(pssmgrp[MapResId]->UIntValue());

         if(pssmgrp[MinNearSplitId]->UIntValue() != 0)
         {
            pssm->setMinNearDistanceForSplits(pssmgrp[MinNearSplitId]->UIntValue());
         }
         if(pssmgrp[MaxFarDistId]->UIntValue() != 0)
         {
            pssm->setMaxFarDistance(pssmgrp[MaxFarDistId]->UIntValue());
            pssm->setMoveVCamBehindRCamFactor(pssmgrp[MoveVCamFactorId]->FloatValue());
         }
         if(pssmgrp[DebugColorOnId]->BoolValue())
         {
            pssm->setDebugColorOn();
         }
         pssm->setPolygonOffset(osg::Vec2(
                                   pssmgrp[PolyOffsetFactorId]->FloatValue(),
                                   pssmgrp[PolyOffsetUnitId]->FloatValue()));
         mTechnique = pssm;
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
         PropertyGroup lispgrp = props[LISPId]->GroupValue();
         msm->setTextureSize(osg::Vec2s(lispgrp[TexSizeId]->UIntValue(), lispgrp[TexSizeId]->UIntValue()) );
         msm->setMinLightMargin(lispgrp[MinLightMarginId]->FloatValue());
         msm->setMaxFarPlane(lispgrp[MaxFarPlaneId]->FloatValue());
         msm->setBaseTextureCoordIndex(lispgrp[BaseTexCoordIndexId]->UIntValue());
         msm->setShadowTextureCoordIndex(lispgrp[ShadowTexCoordIndexId]->UIntValue());
         msm->setShadowTextureUnit(lispgrp[ShadowTexUnitId]->UIntValue());
         msm->setBaseTextureUnit(lispgrp[BaseTexUnitId]->UIntValue());
         msm->setLight(light);

         // setting these for PSSM breaks shadows, so only do it here ...
         shadowedScene->setReceivesShadowTraversalMask(NodeMasks::RECEIVES_SHADOWS);
         shadowedScene->setCastsShadowTraversalMask(NodeMasks::CASTS_SHADOWS);
      }

      if(pssm)
      {
         LightSystem* ls;
         if(mEntity->GetEntityManager().GetEntitySystem(LightComponent::TYPE, ls) && ls->GetNumComponents() != 0)
         {
            for(LightSystem::ComponentStore::iterator i = ls->begin(); i != ls->end(); ++i)
            {
               LightComponent* lcomp = i->second;
               osg::ref_ptr<osg::Group> lightsource = lcomp->GetNode()->asGroup();
               lightsource->getParent(0)->removeChild(lightsource);
               GetGroup()->addChild(lightsource);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   const StringId ShadowSystem::TYPE(dtEntity::SID("Shadow"));
   const StringId ShadowSystem::EnabledId(dtEntity::SID("Enabled"));

   ////////////////////////////////////////////////////////////////////////////
   ShadowSystem::ShadowSystem(EntityManager& em)
      : DefaultEntitySystem<ShadowComponent>(em)
   {
      Register(EnabledId, &mEnabled);

      mEnabled.Set(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   ShadowSystem::~ShadowSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShadowSystem::OnPropertyChanged(StringId propname, Property &prop)
   {
      if(propname == EnabledId)
      {
         SetEnabled(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShadowSystem::SetEnabled(bool v)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->SetEnabled(v);
         i->second->Finished();
      }
   }

}
