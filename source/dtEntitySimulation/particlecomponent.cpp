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

#include <dtEntitySimulation/particlecomponent.h>

#include <dtEntity/nodemasks.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/BoxPlacer>
#include <osgParticle/ConstantRateCounter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/VariableRateCounter>
#include <osgParticle/PointPlacer>
#include <osgParticle/BounceOperator>
#include <osgParticle/ForceOperator>

namespace dtEntitySimulation
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
      , mDebugDrawManager(NULL)
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

      mPlacer.Add(__SELECTED__Id, new StringIdProperty(BoxId));
      mPlacer.Add(BoxId, boxplacer);
      mPlacer.Add(PointId, pointplacer);

      GroupProperty* randomratecounter = new GroupProperty;
      randomratecounter->Add(RateRangeId, new Vec2Property(osg::Vec2(1, 1)));

      GroupProperty* constantratecounter = new GroupProperty;
      constantratecounter->Add(NumberOfParticlesPerSecondToCreateId, new DoubleProperty(0));
      constantratecounter->Add(MinimumNumberOfParticlesToCreateId, new IntProperty(0));

      mCounter.Add(__SELECTED__Id, new StringIdProperty(ConstantRateCounterId));
      mCounter.Add(RandomRateCounterId, randomratecounter);
      mCounter.Add(ConstantRateCounterId, constantratecounter);

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
      dtEntity::LayerAttachPointSystem* layersys;
      entity.GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      dtEntity::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();
      sceneLayer->GetNode()->asGroup()->addChild(mGeode);
      sceneLayer->GetNode()->asGroup()->addChild(mProgram);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::OnRemovedFromEntity(dtEntity::Entity &entity)
   {
      BaseClass::OnRemovedFromEntity(entity);
      dtEntity::LayerAttachPointSystem* layersys;
      entity.GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      mGeode->getParent(0)->removeChild(mGeode);
      mProgram->getParent(0)->removeChild(mProgram);
   }
  
   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::Finished()
   {
      if(mEnabled.Get())
      {
         mModularEmitter->setNodeMask(dtEntity::NodeMasks::VISIBLE);
         mParticleSystem->setFrozen(false);
      }
      else
      {
         mModularEmitter->setNodeMask(0);
         mParticleSystem->setFrozen(true);
         for(unsigned int i = 0; i < mParticleSystem->numParticles(); ++i)
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
   void ParticleComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      using namespace dtEntity;
      if(propname == CounterId)
      {
         PropertyGroup props = prop.GroupValue();
         StringId selected = props[__SELECTED__Id]->StringIdValue();

         if(selected == ConstantRateCounterId)
         {
            PropertyGroup pprops = props[ConstantRateCounterId]->GroupValue();
            osgParticle::ConstantRateCounter* crc = new osgParticle::ConstantRateCounter();
            crc->setMinimumNumberOfParticlesToCreate(pprops[MinimumNumberOfParticlesToCreateId]->IntValue());
            crc->setNumberOfParticlesPerSecondToCreate(pprops[NumberOfParticlesPerSecondToCreateId]->DoubleValue());
            mModularEmitter->setCounter(crc);
         }
         else if(selected == RandomRateCounterId){
            PropertyGroup pprops = props[RandomRateCounterId]->GroupValue();
            osgParticle::RandomRateCounter* rrc = new osgParticle::RandomRateCounter();
            osg::Vec2 range = pprops[RateRangeId]->Vec2Value();
            rrc->setRateRange(range[0], range[1]);
            mModularEmitter->setCounter(rrc);
         }

      }
      else if(propname == DebugOnId)
      {
         if(prop.BoolValue())
         {
            mDebugDrawManager = new dtEntity::DebugDrawManager(mEntity->GetEntityManager());
         }
         else
         {
            mDebugDrawManager = NULL;
         }
      }
      else if(propname == PlacerId)
      {
         PropertyGroup props = prop.GroupValue();
         StringId selected = props[__SELECTED__Id]->StringIdValue();

         if(selected == BoxId)
         {
            osgParticle::BoxPlacer* bp = new osgParticle::BoxPlacer();
            PropertyGroup pprops = props[BoxId]->GroupValue();
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
            PropertyGroup pprops = props[PointId]->GroupValue();
            osg::Vec3 center = pprops[CenterId]->Vec3Value();
            cp->setCenter(center);
            mModularEmitter->setPlacer(cp);
         }

      }
      else if(propname == ShooterThetaRangeId)
      {
         osgParticle::RadialShooter* shooter =
               static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
         osg::Vec2 v = prop.Vec2Value();
         shooter->setThetaRange(v[0], v[1]);
      }
      else if(propname == ShooterPhiRangeId)
      {
         osgParticle::RadialShooter* shooter =
               static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
         osg::Vec2 v = prop.Vec2Value();
         shooter->setPhiRange(v[0], v[1]);
      }
      else if(propname == ShooterInitialSpeedRangeId)
      {
         osgParticle::RadialShooter* shooter =
               static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
         osg::Vec2 v = prop.Vec2Value();
         shooter->setInitialSpeedRange(v[0], v[1]);
      }
      else if(propname == ShooterInitialRotationalSpeedMinId ||
              propname == ShooterInitialRotationalSpeedMaxId)
      {
         osgParticle::RadialShooter* shooter =
               static_cast<osgParticle::RadialShooter*>(mModularEmitter->getShooter());
         shooter->setInitialRotationalSpeedRange(
                  mShooterInitialRotationalSpeedMin.Get(),
                  mShooterInitialRotationalSpeedMax.Get());
      }
      else if(propname == OperatorsId)
      {
         if(mDebugDrawManager)
         {
            mDebugDrawManager->Clear();
         }
         while(mProgram->numOperators() > 0)
         {
            mProgram->removeOperator(0);
         }
         PropertyArray arr = mOperators.Get();
         for(PropertyArray::const_iterator i = arr.begin(); i != arr.end(); ++i)
         {
            GroupProperty* grp = dynamic_cast<GroupProperty*>(*i);
            if(!grp) continue;
            PropertyGroup props = grp->Get();
            StringId selected = props[__SELECTED__Id]->StringIdValue();
            if(selected == BounceOperatorId)
            {
               PropertyGroup bprops = props[BounceOperatorId]->GroupValue();
               osgParticle::BounceOperator* bo = new osgParticle::BounceOperator();;
               bo->setResilience(bprops[ResilienceId]->FloatValue());
               bo->setCutoff(bprops[CutoffId]->FloatValue());
               PropertyArray domains = bprops[DomainsId]->ArrayValue();
               for(PropertyArray::const_iterator i = domains.begin(); i != domains.end(); ++i)
               {
                  PropertyGroup domainprops = (*i)->GroupValue();
                  StringId domainsel = domainprops[__SELECTED__Id]->StringIdValue();
                  PropertyGroup selprops = domainprops[domainsel]->GroupValue();
                  if(domainsel == PlaneDomainId)
                  {
                     osg::Vec3 normal = selprops[NormalId]->Vec3Value();
                     float dist = selprops[DistId]->FloatValue();
                     bo->addPlaneDomain(osg::Plane(normal, dist));
                     if(mDebugDrawManager)
                     {
                        mDebugDrawManager->AddCircle(osg::Vec3(0, 0, -dist), normal,  10, osg::Vec4(0,1,0,1), FLT_MAX);
                     }
                  }
                  else if(domainsel == SphereDomainId)
                  {
                      osg::Vec3 pos = selprops[CenterId]->Vec3Value();
                      float radius = selprops[RadiusId]->FloatValue();
                      bo->addSphereDomain(pos, radius);
                      if(mDebugDrawManager)
                      {
                         mDebugDrawManager->AddSphere(pos, radius, osg::Vec4(0,1,0,1), FLT_MAX);
                      }
                  }

               }
               mProgram->addOperator(bo);

            }
            else if(selected == ForceOperatorId)
            {
               PropertyGroup bprops = props[ForceOperatorId]->GroupValue();
               osgParticle::ForceOperator* fo = new osgParticle::ForceOperator();
               fo->setForce(bprops[ForceId]->Vec3Value());
               mProgram->addOperator(fo);
            }
         }
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////
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
