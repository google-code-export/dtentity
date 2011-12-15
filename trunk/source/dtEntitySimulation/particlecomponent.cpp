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
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/BoxPlacer>
#include <osgParticle/PointPlacer>

namespace dtEntitySimulation
{

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ParticleComponent::TYPE(dtEntity::SID("Particle"));
   
   const dtEntity::StringId ParticleComponent::EmissiveParticlesId(dtEntity::SID("EmissiveParticles"));
   const dtEntity::StringId ParticleComponent::LifeTimeId(dtEntity::SID("LifeTime"));
   const dtEntity::StringId ParticleComponent::LightingId(dtEntity::SID("Lighting"));
   const dtEntity::StringId ParticleComponent::MassId(dtEntity::SID("Mass"));
   const dtEntity::StringId ParticleComponent::PlacerId(dtEntity::SID("Placer"));
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

   ////////////////////////////////////////////////////////////////////////////
   ParticleComponent::ParticleComponent()
      : BaseClass(new osg::Group())
      , mGeode(new osg::Geode())
      , mParticleSystem(new osgParticle::ParticleSystem())
      , mModularEmitter(new osgParticle::ModularEmitter())
   {      
      Register(EmissiveParticlesId, &mEmissiveParticles);
      Register(LifeTimeId, &mLifeTime);
      Register(LightingId, &mLighting);
      Register(MassId, &mMass);
      Register(PlacerId, &mPlacer);
      Register(SizeRangeId, &mSizeRange);
      Register(TextureFileId, &mTextureFile);
      Register(TextureUnitId, &mTextureUnit);
      
      mTextureFile.Set("Textures/smoke.rgb");
      mLifeTime.Set(4);
      mMass.Set(0.01f);
      mSizeRange.Set(osg::Vec2(0.01f,20.0f));

      osg::Group* grp = static_cast<osg::Group*>(GetNode());
      grp->addChild(mGeode);
      mGeode->addDrawable(mParticleSystem);

      grp->addChild(mModularEmitter);

      osgParticle::ParticleSystemUpdater* updater = new osgParticle::ParticleSystemUpdater();
      grp->addChild(updater);
      updater->addParticleSystem(mParticleSystem);

      mModularEmitter->setParticleSystem(mParticleSystem);
      
      osgParticle::RandomRateCounter* dustRate =
         static_cast<osgParticle::RandomRateCounter*>(mModularEmitter->getCounter());
      dustRate->setRateRange(5, 10);
      
      osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
      lineSegment->addVertex(0,0,-2);
      lineSegment->addVertex(0,-2,-2);
      lineSegment->addVertex(0,-16,0);
      
      mModularEmitter->setPlacer(lineSegment);

      // To customize the shooter, create and initialize a radial shooter
      osgParticle::RadialShooter* smokeShooter = new osgParticle::RadialShooter();
      // Set properties of this shooter
      smokeShooter->setThetaRange(0.0, 3.14159/2); // radians, relative to Z axis.
      smokeShooter->setInitialSpeedRange(50,100); // meters/second
      // Use this shooter for our emitter
      mModularEmitter->setShooter(smokeShooter);

      //Box|Composite|MultiSegment|Point|Sector|Segment

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
   }
    
   ////////////////////////////////////////////////////////////////////////////
   ParticleComponent::~ParticleComponent()
   {
   }
  
   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::Finished()
   {
      mParticleSystem->setDefaultAttributes(mTextureFile.Get(),  
         mEmissiveParticles.Get(), mLighting.Get(), mTextureUnit.Get());

      osgParticle::Particle particle;
      particle.setSizeRange(osgParticle::rangef(mSizeRange.Get()[0], mSizeRange.Get()[1]));
      particle.setLifeTime(mLifeTime.Get());
      particle.setMass(mMass.Get());
      mParticleSystem->setDefaultParticleTemplate(particle);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ParticleComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      using namespace dtEntity;
      if(propname == PlacerId)
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
