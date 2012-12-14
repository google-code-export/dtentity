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

#include <dtEntityOSG/shadowcomponent.h>

#include <dtEntity/core.h>
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/lightcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/entity.h>
#include <assert.h>
#include <osgShadow/ShadowedScene>

#include <osg/LightSource>
#include <osg/Light>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgViewer/View>

namespace dtEntityOSG
{
  
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ShadowComponent::TYPE(dtEntity::SID("Shadow"));
   const dtEntity::StringId ShadowComponent::ShadowTechniqueId(dtEntity::SID("ShadowTechnique"));
   const dtEntity::StringId ShadowComponent::MinLightMarginId(dtEntity::SID("MinLightMargin"));
   const dtEntity::StringId ShadowComponent::MaxFarPlaneId(dtEntity::SID("MaxFarPlane"));
   const dtEntity::StringId ShadowComponent::TexSizeId(dtEntity::SID("TexSize"));
   const dtEntity::StringId ShadowComponent::BaseTexUnitId(dtEntity::SID("BaseTexUnit"));
   const dtEntity::StringId ShadowComponent::ShadowTexUnitId(dtEntity::SID("ShadowTexUnit"));
   const dtEntity::StringId ShadowComponent::ShadowTexCoordIndexId(dtEntity::SID("ShadowTexCoordIndex"));
   const dtEntity::StringId ShadowComponent::BaseTexCoordIndexId(dtEntity::SID("BaseTexCoordIndex"));
   const dtEntity::StringId ShadowComponent::MapCountId(dtEntity::SID("MapCount"));
   const dtEntity::StringId ShadowComponent::MapResId(dtEntity::SID("MapRes"));
   const dtEntity::StringId ShadowComponent::DebugColorOnId(dtEntity::SID("DebugColorOn"));
   const dtEntity::StringId ShadowComponent::MinNearSplitId(dtEntity::SID("MinNearSplit"));
   const dtEntity::StringId ShadowComponent::MaxFarDistId(dtEntity::SID("MaxFarDist"));
   const dtEntity::StringId ShadowComponent::MoveVCamFactorId(dtEntity::SID("MoveVCamFactor"));
   const dtEntity::StringId ShadowComponent::PolyOffsetFactorId(dtEntity::SID("PolyOffsetFactor"));
   const dtEntity::StringId ShadowComponent::PolyOffsetUnitId(dtEntity::SID("PolyOffsetUnit"));
   const dtEntity::StringId ShadowComponent::EnabledId(dtEntity::SID("Enabled"));
   const dtEntity::StringId ShadowComponent::ShadowTypeId(dtEntity::SID("ShadowType"));
   const dtEntity::StringId ShadowComponent::LISPId(dtEntity::SID("LISP"));
   const dtEntity::StringId ShadowComponent::PSSMId(dtEntity::SID("PSSM"));
   const dtEntity::StringId ShadowComponent::__SELECTED__Id(dtEntity::SID("__SELECTED__"));



   ////////////////////////////////////////////////////////////////////////////
   ShadowComponent::ShadowComponent()
      : BaseClass(new osgShadow::ShadowedScene())
      , mEntity(NULL)
   {
      using namespace dtEntity;

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
   void ShadowComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      mEntity = &entity;
      BaseClass::OnAddedToEntity(entity);
   }


   ////////////////////////////////////////////////////////////////////////////
   void ShadowComponent::Finished()
   { 
      using namespace dtEntity;

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

      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(GetSystemInterface());
      osg::ref_ptr<osg::Light> light = iface->GetPrimaryView()->getLight();
      
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
   const dtEntity::StringId ShadowSystem::TYPE(dtEntity::SID("Shadow"));
   const dtEntity::StringId ShadowSystem::EnabledId(dtEntity::SID("Enabled"));

   ////////////////////////////////////////////////////////////////////////////
   ShadowSystem::ShadowSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<ShadowComponent>(em)
      , mEnabled(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &ShadowSystem::SetEnabled),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &ShadowSystem::GetEnabled)
          )
      , mEnabledVal(true)
   {
      Register(EnabledId, &mEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////
   ShadowSystem::~ShadowSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShadowSystem::SetEnabled(bool v)
   {
      mEnabledVal = v;
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->SetEnabled(v);
         i->second->Finished();
      }
   }

}
