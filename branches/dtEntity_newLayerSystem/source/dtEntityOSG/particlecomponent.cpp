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

#include <dtEntityOSG/particlecomponent.h>

#include <dtEntity/core.h>
#include <dtEntity/debugdrawinterface.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/BoxPlacer>
#include <osgParticle/ConstantRateCounter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/VariableRateCounter>
#include <osgParticle/PointPlacer>
#include <osgParticle/BounceOperator>
#include <osgParticle/ForceOperator>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ParticleComponent::TYPE(dtEntity::SID("Particle"));
   
   const dtEntity::StringId ParticleComponent::AlphaRangeId(dtEntity::SID("AlphaRange"));
   const dtEntity::StringId ParticleComponent::ColorRangeMinId(dtEntity::SID("ColorRangeMin"));
   const dtEntity::StringId ParticleComponent::ColorRangeMaxId(dtEntity::SID("ColorRangeMax"));

   const dtEntity::StringId ParticleComponent::CounterId(dtEntity::SID("Counter"));
   const dtEntity::StringId ParticleComponent::DebugOnId(dtEntity::SID("DebugOn"));
   const dtEntity::StringId ParticleComponent::EmissiveParticlesId(dtEntity::SID("EmissiveParticles"));
   const dtEntity::StringId ParticleComponent::EnabledId(dtEntity::SID("Enabled"));
   const dtEntity::StringId ParticleComponent::LifeTimeId(dtEntity::SID("LifeTime"));
   const dtEntity::StringId ParticleComponent::LightingId(dtEntity::SID("Lighting"));
   const dtEntity::StringId ParticleComponent::MassId(dtEntity::SID("Mass"));
   const dtEntity::StringId ParticleComponent::PlacerId(dtEntity::SID("Placer"));
   const dtEntity::StringId ParticleComponent::OperatorsId(dtEntity::SID("Operators"));
   const dtEntity::StringId ParticleComponent::SizeRangeId(dtEntity::SID("SizeRange"));
   const dtEntity::StringId ParticleComponent::TextureFileId(dtEntity::SID("TextureFile"));
   const dtEntity::StringId ParticleComponent::TextureUnitId(dtEntity::SID("TextureUnit"));
   const dtEntity::StringId ParticleComponent::__SELECTED__Id(dtEntity::SID("__SELECTED__"));

   const dtEntity::StringId ParticleComponent::BoxId(dtEntity::SID("Box"));
   const dtEntity::StringId ParticleComponent::CompositeId(dtEntity::SID("Composite"));
   const dtEntity::StringId ParticleComponent::MultiSegmentId(dtEntity::SID("MultiSegment"));
   const dtEntity::StringId ParticleComponent::PointId(dtEntity::SID("Point"));
   const dtEntity::StringId ParticleComponent::SectorId(dtEntity::SID("Sector"));
   const dtEntity::StringId ParticleComponent::SegmentId(dtEntity::SID("Segment"));
 
   const dtEntity::StringId ParticleComponent::XRangeId(dtEntity::SID("XRange"));
   const dtEntity::StringId ParticleComponent::YRangeId(dtEntity::SID("YRange"));
   const dtEntity::StringId ParticleComponent::ZRangeId(dtEntity::SID("ZRange"));
   const dtEntity::StringId ParticleComponent::CenterId(dtEntity::SID("Center"));

   const dtEntity::StringId ParticleComponent::RandomRateCounterId(dtEntity::SID("RandomRateCounter"));
   const dtEntity::StringId ParticleComponent::ConstantRateCounterId(dtEntity::SID("ConstantRateCounter"));

   const dtEntity::StringId ParticleComponent::RateRangeId(dtEntity::SID("RateRange"));
   const dtEntity::StringId ParticleComponent::MinimumNumberOfParticlesToCreateId(dtEntity::SID("MinimumNumberOfParticlesToCreate"));
   const dtEntity::StringId ParticleComponent::NumberOfParticlesPerSecondToCreateId(dtEntity::SID("NumberOfParticlesPerSecondToCreate"));

   const dtEntity::StringId ParticleComponent::ShooterThetaRangeId(dtEntity::SID("ShooterThetaRange"));
   const dtEntity::StringId ParticleComponent::ShooterPhiRangeId(dtEntity::SID("ShooterPhiRange"));
   const dtEntity::StringId ParticleComponent::ShooterInitialSpeedRangeId(dtEntity::SID("ShooterInitialSpeedRange"));
   const dtEntity::StringId ParticleComponent::ShooterInitialRotationalSpeedMinId(dtEntity::SID("ShooterInitialRotationalSpeedMin"));
   const dtEntity::StringId ParticleComponent::ShooterInitialRotationalSpeedMaxId(dtEntity::SID("ShooterInitialRotationalSpeedMax"));

   const dtEntity::StringId ParticleComponent::ShapeId(dtEntity::SID("Shape"));
   const dtEntity::StringId ParticleComponent::ShapePointId(dtEntity::SID("Point"));
   const dtEntity::StringId ParticleComponent::ShapeQuadId(dtEntity::SID("Quad"));
   const dtEntity::StringId ParticleComponent::ShapeQuadTriangeStripId(dtEntity::SID("QuadTriangeStrip"));
   const dtEntity::StringId ParticleComponent::ShapeHexagonId(dtEntity::SID("Hexagon"));
   const dtEntity::StringId ParticleComponent::ShapeLineId(dtEntity::SID("Line"));

   const dtEntity::StringId ParticleComponent::BounceOperatorId(dtEntity::SID("BounceOperator"));
   const dtEntity::StringId ParticleComponent::ForceOperatorId(dtEntity::SID("ForceOperator"));

   const dtEntity::StringId ParticleComponent::FrictionId(dtEntity::SID("Friction"));
   const dtEntity::StringId ParticleComponent::ResilienceId(dtEntity::SID("Resilience"));
   const dtEntity::StringId ParticleComponent::CutoffId(dtEntity::SID("Cutoff"));
   const dtEntity::StringId ParticleComponent::DomainsId(dtEntity::SID("Domains"));
   const dtEntity::StringId ParticleComponent::PlaneDomainId(dtEntity::SID("PlaneDomain"));
   const dtEntity::StringId ParticleComponent::SphereDomainId(dtEntity::SID("SphereDomain"));

   const dtEntity::StringId ParticleComponent::ForceId(dtEntity::SID("Force"));
   const dtEntity::StringId ParticleComponent::NormalId(dtEntity::SID("Normal"));
   const dtEntity::StringId ParticleComponent::DistId(dtEntity::SID("Dist"));
   const dtEntity::StringId ParticleComponent::RadiusId(dtEntity::SID("Radius"));




   ////////////////////////////////////////////////////////////////////////////
   ParticleComponent::ParticleComponent()
      : BaseClass(new osg::Group())
      , mGeode(new osg::Geode())
      , mParticleSystem(new osgParticle::ParticleSystem())
      , mModularEmitter(new osgParticle::ModularEmitter())
      , mProgram(new osgParticle::ModularProgram())
      , mDebugOn(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &ParticleComponent::SetDebugOn),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &ParticleComponent::GetDebugOn)
        )
      , mPlacer(
           dtEntity::DynamicGroupProperty::SetValueCB(this, &ParticleComponent::SetPlacer),
           dtEntity::DynamicGroupProperty::GetValueCB(this, &ParticleComponent::GetPlacer)
        )
      , mCounter(
           dtEntity::DynamicGroupProperty::SetValueCB(this, &ParticleComponent::SetCounter),
           dtEntity::DynamicGroupProperty::GetValueCB(this, &ParticleComponent::GetCounter)
        )
      , mShooterThetaRange(
           dtEntity::DynamicVec2Property::SetValueCB(this, &ParticleComponent::SetShooterThetaRange),
           dtEntity::DynamicVec2Property::GetValueCB(this, &ParticleComponent::GetShooterThetaRange)
        )
      , mShooterPhiRange(
           dtEntity::DynamicVec2Property::SetValueCB(this, &ParticleComponent::SetShooterPhiRange),
           dtEntity::DynamicVec2Property::GetValueCB(this, &ParticleComponent::GetShooterPhiRange)
        )
      , mShooterInitialSpeedRange(
           dtEntity::DynamicVec2Property::SetValueCB(this, &ParticleComponent::SetShooterInitialSpeedRange),
           dtEntity::DynamicVec2Property::GetValueCB(this, &ParticleComponent::GetShooterInitialSpeedRange)
        )
      , mShooterInitialRotationalSpeedMin(
           dtEntity::DynamicVec3Property::SetValueCB(this, &ParticleComponent::SetShooterInitialRotationalSpeedMin),
           dtEntity::DynamicVec3Property::GetValueCB(this, &ParticleComponent::GetShooterInitialRotationalSpeedMin)
        )
      , mShooterInitialRotationalSpeedMax(
           dtEntity::DynamicVec3Property::SetValueCB(this, &ParticleComponent::SetShooterInitialRotationalSpeedMax),
           dtEntity::DynamicVec3Property::GetValueCB(this, &ParticleComponent::GetShooterInitialRotationalSpeedMax)
        )
      , mOperators(dtEntity::DynamicArrayProperty::SetValueCB(this, &ParticleComponent::SetOperators),
                   dtEntity::DynamicArrayProperty::GetValueCB(this, &ParticleComponent::GetOperators)
        )
   {      
      Register(AlphaRangeId, &mAlphaRange);
      Register(ColorRangeMinId, &mColorRangeMin);
      Register(ColorRangeMaxId, &mColorRangeMax);
      Register(CounterId, &mCounter);
      Register(DebugOnId, &mDebugOn);
      Register(EmissiveParticlesId, &mEmissiveParticles);
      Register(EnabledId, &mEnabled);
      Register(LifeTimeId, &mLifeTime);
      Register(LightingId, &mLighting);
      Register(MassId, &mMass);
      Register(PlacerId, &mPlacer);
      Register(SizeRangeId, &mSizeRange);
      Register(TextureFileId, &mTextureFile);
      Register(TextureUnitId, &mTextureUnit);
      Register(ShapeId, &mShape);

      Register(ShooterThetaRangeId, &mShooterThetaRange);
      Register(ShooterPhiRangeId, &mShooterPhiRange);
      Register(ShooterInitialSpeedRangeId, &mShooterInitialSpeedRange);
      Register(ShooterInitialRotationalSpeedMinId, &mShooterInitialRotationalSpeedMin);
      Register(ShooterInitialRotationalSpeedMaxId, &mShooterInitialRotationalSpeedMax);
      Register(OperatorsId, &mOperators);

      mEnabled.Set(true);
      mTextureFile.Set("Textures/smoke.rgb");
      mLifeTime.Set(4);
      mMass.Set(0.01f);
      mSizeRange.Set(osg::Vec2(0.01f,20.0f));

      osg::Group* grp = static_cast<osg::Group*>(GetNode());
      //grp->addChild(mGeode);

      mGeode->addDrawable(mParticleSystem);

      grp->addChild(mModularEmitter);

      osgParticle::ParticleSystemUpdater* updater = new osgParticle::ParticleSystemUpdater();
      grp->addChild(updater);
      updater->addParticleSystem(mParticleSystem);

      mModularEmitter->setParticleSystem(mParticleSystem);

      osgParticle::RadialShooter* shooter = new osgParticle::RadialShooter();
      shooter->setThetaRange(0, 0);
      shooter->setInitialSpeedRange(0,0);
      mModularEmitter->setShooter(shooter);

      using namespace dtEntity;
      GroupProperty* boxplacer = new GroupProperty;
      boxplacer->Add(XRangeId, new Vec2Property(osg::Vec2(-1, 1)));
      boxplacer->Add(YRangeId, new Vec2Property(osg::Vec2(-1, 1)));
      boxplacer->Add(ZRangeId, new Vec2Property(osg::Vec2(-1, 1)));

      dtEntity::GroupProperty* pointplacer = new GroupProperty;
      pointplacer->Add(CenterId, new Vec3Property());

      mPlacerVal.Add(__SELECTED__Id, new StringIdProperty(BoxId));
      mPlacerVal.Add(BoxId, boxplacer);
      mPlacerVal.Add(PointId, pointplacer);

      GroupProperty* randomratecounter = new GroupProperty;
      randomratecounter->Add(RateRangeId, new Vec2Property(osg::Vec2(1, 1)));

      GroupProperty* constantratecounter = new GroupProperty;
      constantratecounter->Add(NumberOfParticlesPerSecondToCreateId, new DoubleProperty(0));
      constantratecounter->Add(MinimumNumberOfParticlesToCreateId, new IntProperty(0));

      mCounterVal.Add(__SELECTED__Id, new StringIdProperty(ConstantRateCounterId));
      mCounterVal.Add(RandomRateCounterId, randomratecounter);
      mCounterVal.Add(ConstantRateCounterId, constantratecounter);

      mShape.Set(ShapeQuadId);

      mColorRangeMin.Set(osg::Vec4(1,1,1,1));
      mColorRangeMax.Set(osg::Vec4(1,1,1,1));

      mAlphaRange.Set(osg::Vec2(1,1));

      mProgram->setParticleSystem(mParticleSystem);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   ParticleComponent::~ParticleComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::OnAddedToEntity(dtEntity::Entity &entity)
   {
      BaseClass::OnAddedToEntity(entity);
      dtEntityOSG::LayerAttachPointSystem* layersys;
      entity.GetEntityManager().GetEntitySystem(dtEntityOSG::LayerAttachPointComponent::TYPE, layersys);
      dtEntityOSG::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();
      sceneLayer->GetNode()->asGroup()->addChild(mGeode);
      sceneLayer->GetNode()->asGroup()->addChild(mProgram);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::OnRemovedFromEntity(dtEntity::Entity &entity)
   {
      BaseClass::OnRemovedFromEntity(entity);
      dtEntityOSG::LayerAttachPointSystem* layersys;
      entity.GetEntityManager().GetEntitySystem(dtEntityOSG::LayerAttachPointComponent::TYPE, layersys);
      mGeode->getParent(0)->removeChild(mGeode);
      mProgram->getParent(0)->removeChild(mProgram);
   }
  
   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::Finished()
   {
      BaseClass::Finished();
      if(mEnabled.Get())
      {
         mModularEmitter->setNodeMask(dtEntity::NodeMasks::VISIBLE);
         mParticleSystem->setFrozen(false);
      }
      else
      {
         mModularEmitter->setNodeMask(0);
         mParticleSystem->setFrozen(true);
         for(int i = 0; i < mParticleSystem->numParticles(); ++i)
         {
            mParticleSystem->destroyParticle(i);
         }
         osg::NodeVisitor nv;
         mParticleSystem->update(1, nv);
      }
      mParticleSystem->setDefaultAttributes(mTextureFile.Get(),  
         mEmissiveParticles.Get(), mLighting.Get(), mTextureUnit.Get());

      osgParticle::Particle particle;
      particle.setSizeRange(osgParticle::rangef(mSizeRange.Get()[0], mSizeRange.Get()[1]));
      particle.setLifeTime(mLifeTime.Get());
      particle.setMass(mMass.Get());
      if(mShape.Get() == ShapePointId)
         particle.setShape(osgParticle::Particle::POINT);
      else if(mShape.Get() == ShapeQuadId)
         particle.setShape(osgParticle::Particle::QUAD);
      else if(mShape.Get() == ShapeQuadTriangeStripId)
         particle.setShape(osgParticle::Particle::QUAD_TRIANGLESTRIP);
      else if(mShape.Get() == ShapeHexagonId)
         particle.setShape(osgParticle::Particle::HEXAGON);
      else if(mShape.Get() == ShapeLineId)
         particle.setShape(osgParticle::Particle::LINE);

      particle.setAlphaRange(osgParticle::rangef(mAlphaRange.Get()[0], mAlphaRange.Get()[1]));
      particle.setColorRange(osgParticle::rangev4(mColorRangeMin.Get(), mColorRangeMax.Get()));
      mParticleSystem->setDefaultParticleTemplate(particle);

   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetCounter(const dtEntity::PropertyGroup& props)
   {
      mCounterVal.Set(props);
      dtEntity::PropertyGroup::const_iterator i1 = props.find(__SELECTED__Id);
      if(i1 == props.end())
      {
         LOG_ERROR("No __SELECTED_Id in particle counter!");
         return;
      }
      dtEntity::StringId selected = i1->second->StringIdValue();

      if(selected == ConstantRateCounterId)
      {
         dtEntity::PropertyGroup::const_iterator i2 = props.find(ConstantRateCounterId);
         if(i2 == props.end())
         {
            LOG_ERROR("No ConstantRateCounterId in particle counter!");
            return;
         }
         dtEntity::PropertyGroup pprops = i2->second->GroupValue();
         osgParticle::ConstantRateCounter* crc = new osgParticle::ConstantRateCounter();
         crc->setMinimumNumberOfParticlesToCreate(pprops[MinimumNumberOfParticlesToCreateId]->IntValue());
         crc->setNumberOfParticlesPerSecondToCreate(pprops[NumberOfParticlesPerSecondToCreateId]->DoubleValue());
         mModularEmitter->setCounter(crc);
      }
      else if(selected == RandomRateCounterId)
      {
         dtEntity::PropertyGroup::const_iterator i2 = props.find(RandomRateCounterId);
         if(i2 == props.end())
         {
            LOG_ERROR("No RandomRateCounterId in particle counter!");
            return;
         }
         dtEntity::PropertyGroup pprops = i2->second->GroupValue();
         osgParticle::RandomRateCounter* rrc = new osgParticle::RandomRateCounter();
         osg::Vec2 range = pprops[RateRangeId]->Vec2Value();
         rrc->setRateRange(range[0], range[1]);
         mModularEmitter->setCounter(rrc);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::PropertyGroup ParticleComponent::GetCounter() const
   {
      return mPlacerVal.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetPlacer(const dtEntity::PropertyGroup& props)
   {
      mPlacerVal.Set(props);

      dtEntity::PropertyGroup::const_iterator i1 = props.find(__SELECTED__Id);
      if(i1 == props.end())
      {
         LOG_ERROR("No __SELECTED_Id in particle placer!");
         return;
      }
      dtEntity::StringId selected = i1->second->StringIdValue();

      if(selected == BoxId)
      {
         osgParticle::BoxPlacer* bp = new osgParticle::BoxPlacer();
         dtEntity::PropertyGroup::const_iterator i2 = props.find(BoxId);
         if(i2 == props.end())
         {
            LOG_ERROR("No BoxId in particle placer!");
            return;
         }

         dtEntity::PropertyGroup pprops = i2->second->GroupValue();
         osg::Vec2 xr = pprops[XRangeId]->Vec2Value();
         osg::Vec2 yr = pprops[YRangeId]->Vec2Value();
         osg::Vec2 zr = pprops[ZRangeId]->Vec2Value();
         bp->setXRange(xr[0], xr[1]);
         bp->setYRange(yr[0], yr[1]);
         bp->setZRange(zr[0], zr[1]);
         mModularEmitter->setPlacer(bp);
      }
      else if(selected == PointId)
      {
         osgParticle::PointPlacer* cp = new osgParticle::PointPlacer();

         dtEntity::PropertyGroup::const_iterator i2 = props.find(PointId);
         if(i2 == props.end())
         {
            LOG_ERROR("No PointId in particle placer!");
            return;
         }

         dtEntity::PropertyGroup pprops = i2->second->GroupValue();
         osg::Vec3 center = pprops[CenterId]->Vec3Value();
         cp->setCenter(center);
         mModularEmitter->setPlacer(cp);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::PropertyGroup ParticleComponent::GetPlacer() const
   {
      return mCounterVal.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetDebugOn(bool v)
   {      
      mDebugOnVal = v;
      dtEntity::GetDebugDrawInterface()->SetEnabled(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetShooterThetaRange(const osg::Vec2& v)
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
      shooter->setThetaRange(v[0], v[1]);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleComponent::GetShooterThetaRange() const
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());

      return osg::Vec2(shooter->getThetaRange().minimum, shooter->getThetaRange().maximum);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetShooterPhiRange(const osg::Vec2& v)
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
      shooter->setPhiRange(v[0], v[1]);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleComponent::GetShooterPhiRange() const
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());

      return osg::Vec2(shooter->getPhiRange().minimum, shooter->getPhiRange().maximum);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetShooterInitialSpeedRange(const osg::Vec2& v)
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
      shooter->setInitialSpeedRange(v[0], v[1]);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec2 ParticleComponent::GetShooterInitialSpeedRange() const
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());

      return osg::Vec2(shooter->getInitialSpeedRange().minimum, shooter->getInitialSpeedRange().maximum);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetShooterInitialRotationalSpeedMin(const osg::Vec3& v)
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
      shooter->setInitialRotationalSpeedRange(v, GetShooterInitialRotationalSpeedMax());
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 ParticleComponent::GetShooterInitialRotationalSpeedMin() const
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());

      return shooter->getInitialRotationalSpeedRange().minimum;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetShooterInitialRotationalSpeedMax(const osg::Vec3& v)
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
      shooter->setInitialRotationalSpeedRange(GetShooterInitialRotationalSpeedMin(), v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 ParticleComponent::GetShooterInitialRotationalSpeedMax() const
   {
      osgParticle::RadialShooter* shooter =
            static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());

      return shooter->getInitialRotationalSpeedRange().maximum;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::SetOperators(const dtEntity::PropertyArray& arr)
   {
      mOperatorsVal.Set(arr);

      dtEntity::GetDebugDrawInterface()->Clear();

      while(mProgram->numOperators() > 0)
      {
         mProgram->removeOperator(0);
      }

      for(dtEntity::PropertyArray::const_iterator i = arr.begin(); i != arr.end(); ++i)
      {
         GroupProperty* grp = dynamic_cast<GroupProperty*>(*i);
         if(!grp) continue;
         dtEntity::PropertyGroup props = grp->Get();
         dtEntity::StringId selected = props[__SELECTED__Id]->StringIdValue();
         if(selected == BounceOperatorId)
         {
            dtEntity::PropertyGroup bprops = props[BounceOperatorId]->GroupValue();
            osgParticle::BounceOperator* bo = new osgParticle::BounceOperator();;
            bo->setResilience(bprops[ResilienceId]->FloatValue());
            bo->setCutoff(bprops[CutoffId]->FloatValue());
            dtEntity::PropertyArray domains = bprops[DomainsId]->ArrayValue();
            for(dtEntity::PropertyArray::const_iterator i = domains.begin(); i != domains.end(); ++i)
            {
               dtEntity::PropertyGroup domainprops = (*i)->GroupValue();
               dtEntity::StringId domainsel = domainprops[__SELECTED__Id]->StringIdValue();
               dtEntity::PropertyGroup selprops = domainprops[domainsel]->GroupValue();
               if(domainsel == PlaneDomainId)
               {
                  osg::Vec3 normal = selprops[NormalId]->Vec3Value();
                  float dist = selprops[DistId]->FloatValue();
                  bo->addPlaneDomain(osg::Plane(normal, dist));

                  dtEntity::GetDebugDrawInterface()->AddCircle(osg::Vec3(0, 0, -dist), normal,  10, osg::Vec4(0,1,0,1), FLT_MAX);

               }
               else if(domainsel == SphereDomainId)
               {
                   osg::Vec3 pos = selprops[CenterId]->Vec3Value();
                   float radius = selprops[RadiusId]->FloatValue();
                   bo->addSphereDomain(pos, radius);
                   dtEntity::GetDebugDrawInterface()->AddSphere(pos, radius, osg::Vec4(0,1,0,1), FLT_MAX);

               }

            }
            mProgram->addOperator(bo);

         }
         else if(selected == ForceOperatorId)
         {
            dtEntity::PropertyGroup bprops = props[ForceOperatorId]->GroupValue();
            osgParticle::ForceOperator* fo = new osgParticle::ForceOperator();
            fo->setForce(bprops[ForceId]->Vec3Value());
            mProgram->addOperator(fo);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::PropertyArray ParticleComponent::GetOperators() const
   {
      return mOperatorsVal.Get();
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ParticleSystem::TYPE(dtEntity::SID("Particle"));

   ////////////////////////////////////////////////////////////////////////////
   ParticleSystem::ParticleSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   ParticleSystem::~ParticleSystem()
   {
   }
}
