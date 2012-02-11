/* -*-c++-*-
* testEntity - testEntity(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Martin Scheffler
*/
#include <osgDB/FileUtils>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/component.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/skyboxcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <iostream>

#define NUMBER_OF_ENTITIES 200

////////////////////////////////////////////////////////////////////////////////


// small helper function to get 2D position of an entity
void GetEntityTranslation(dtEntity::EntityManager& em, dtEntity::EntityId eid, osg::Vec2& translation)
{
   dtEntity::PositionAttitudeTransformComponent* tcomp;
   bool found = em.GetComponent(eid, tcomp);
   assert(found);
   osg::Vec3 t = tcomp->GetPosition();
   translation[0] = t[0];
   translation[1] = t[1];
}


///////////////////////////////////////////////

// this component stores speed and force vectos for an entity.
class MovementComponent : public dtEntity::Component
{

public:
   static const dtEntity::StringId TYPE;
   static const dtEntity::StringId SpeedId;
   static const dtEntity::StringId ForceId;
   
   MovementComponent()
   {
      Register(SpeedId, &mSpeed);
      Register(ForceId, &mForce);
   }

   virtual dtEntity::ComponentType GetType() const { return TYPE; }

   virtual bool IsInstanceOf(dtEntity::ComponentType id) const
   { 
      return ((id == TYPE)); 
   }

   
   void AddForce(const osg::Vec2& f)
   {
      mForce.Set(mForce.Get() + f);
   }

   void SetForce(const osg::Vec2& f)
   {
      mForce.Set(f);
   }

   osg::Vec2 GetForce() const
   {
      return mForce.Get();
   }

private:
   dtEntity::Vec2Property mSpeed;
   dtEntity::Vec2Property mForce;
};

const dtEntity::StringId MovementComponent::TYPE(dtEntity::SID("MovementComponent"));
const dtEntity::StringId MovementComponent::SpeedId(dtEntity::SID("Speed"));
const dtEntity::StringId MovementComponent::ForceId(dtEntity::SID("Force"));


///////////////////////////////////////////////


// iterates over MovementComponents and updates the translation of the entity
// after applying forces
class MovementSystem
   : public dtEntity::DefaultEntitySystem<MovementComponent>
{  

public:
   MovementSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<MovementComponent>(em)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &MovementSystem::Tick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }

   void Tick(const dtEntity::Message& msg)
   {
      assert(msg.GetType() == dtEntity::TickMessage::TYPE);
      float dt = msg.GetFloat(dtEntity::TickMessage::DeltaSimTimeId);
      if(dt > 0.05f) dt = 0.05f; // stabilize
      
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         MovementComponent* mcomp = i->second;
         osg::Vec2 speed = mcomp->GetVec2(MovementComponent::SpeedId);
         osg::Vec2 force = mcomp->GetForce();
         
         // forward euler integration
         speed += force * dt;
         speed *= 0.2; // apply some drag
         mcomp->SetVec2(MovementComponent::SpeedId, speed);
         mcomp->SetForce(osg::Vec2());

         dtEntity::PositionAttitudeTransformComponent* tcomp;
         bool found = GetEntityManager().GetComponent(id, tcomp);
         assert(found);
         osg::Vec3 trans = tcomp->GetPosition();
         trans[0] += speed[0] * dt;
         trans[1] += speed[1] * dt;
         tcomp->SetPosition(trans);
      }      
   }

private:
   dtEntity::MessageFunctor mTickFunctor;
};

  
////////////////////////////////////////////////////////////////////////////////


// Target is the entity to follow
class SteeringComponent : public dtEntity::Component
{
public:
   static const dtEntity::ComponentType TYPE;
   static const dtEntity::StringId TargetId;

   SteeringComponent()
   {
      Register(TargetId, &mTarget);
   }

   virtual dtEntity::ComponentType GetType() const { return TYPE; }

   virtual bool IsInstanceOf(dtEntity::ComponentType id) const
   { 
      return ((id == TYPE)); 
   }

private:
   dtEntity::IntProperty mTarget;
};

const dtEntity::StringId SteeringComponent::TYPE(dtEntity::SID("SteeringComponent")); 
const dtEntity::StringId SteeringComponent::TargetId(dtEntity::SID("Target"));


////////////////////////////////////////////////////////////////////////////////

// apply forces to movement component to steer towards target
class SteeringSystem
   : public dtEntity::DefaultEntitySystem<SteeringComponent>
{
     
public:
   SteeringSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<SteeringComponent>(em)
   {
      
      mTickFunctor = dtEntity::MessageFunctor(this, &SteeringSystem::Tick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }


   void Tick(const dtEntity::Message& msg)
   {
      assert(msg.GetType() == dtEntity::TickMessage::TYPE);      

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         SteeringComponent* scomp = i->second;
         dtEntity::EntityId target = (dtEntity::EntityId)scomp->GetInt(SteeringComponent::TargetId);

         MovementComponent* mc;
         GetEntityManager().GetComponent(id, mc);
         
         osg::Vec2 mytrans;
         GetEntityTranslation(GetEntityManager(), id, mytrans);

         // make the first one go in a circle
         if(target == 0)
         {
            double time = msg.GetDouble(dtEntity::TickMessage::SimulationTimeId);
            osg::Vec2 targetv((float)sin(time / 10.0f), (float)cos(time / 10.0f));
            mc->AddForce((targetv * 400 - mytrans) * 400);
         }
         else
         {
            // make all others follow their predecessor
            osg::Vec2 transtarget;
            GetEntityTranslation(GetEntityManager(), target, transtarget);
            
            osg::Vec2 force = (transtarget - mytrans) * 500;

            float jscale = sin((float)id / 100.0f);

            // add some sideways jitter
            float jitterX = force[1] * jscale;
            float jitterY = -force[0] * jscale;
            float jitterl = 1000 / sqrt(jitterX * jitterX + jitterY * jitterY);
            
            force[0] += jitterX * jitterl;
            force[1] += jitterY * jitterl;
            mc->AddForce(force);
         }
      }      
   }

private:
   dtEntity::MessageFunctor mTickFunctor;
};


int main(int argc, char** argv)
{
   osg::ArgumentParser arguments(&argc,argv);   
   osgViewer::Viewer viewer(arguments);
   dtEntity::EntityManager em;
   
   if(!dtEntity::InitOSGViewer(argc, argv, &viewer, &em))
   {
      LOG_ERROR("Error setting up dtEntity!");
      return 0;
   }

   dtEntity::ApplicationSystem* appsys;
   em.GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
   
   // give entity system access to the scene graph
   em.AddEntitySystem(*new MovementSystem(em));
   em.AddEntitySystem(*new SteeringSystem(em));

   dtEntity::MapSystem* mSystem;
   em.GetEntitySystem(dtEntity::MapComponent::TYPE, mSystem);
   
   std::string path = "Scenes/boids.dtescene";
   bool success = mSystem->LoadScene(path);
   if(!success)
   {
      LOG_ERROR("Could not load scene " + path);
   }

   // make sure these component systems are started
   mSystem->GetPluginManager().StartEntitySystem(dtEntity::SIDHash("StaticMesh"));
   mSystem->GetPluginManager().StartEntitySystem(dtEntity::SIDHash("PositionAttitudeTransform"));
     
   // create spawner for entity
   dtEntity::Spawner* spawner;
   if(!mSystem->GetSpawner("Boids", spawner))
   {
	   LOG_ERROR("Could not find spawner 'Boids' in map!");
      return 0;
   }



   // Now create a large number of moving entities
   // store id of last entity to make each entity follow its predecessor
   dtEntity::EntityId lastEntity = 0;
   for(unsigned int i = 0; i < NUMBER_OF_ENTITIES; ++i)
   {      
      dtEntity::Entity* spawned;
      em.CreateEntity(spawned);
      // instantiate entity
      bool success = spawner->Spawn(*spawned);
      assert(success);
      // set a start position
      dtEntity::PositionAttitudeTransformComponent* trans;
      success = em.GetComponent(spawned->GetId(), trans);
      assert(success);
      trans->SetPosition(osg::Vec3(i * 3, 0.0f, 0.5f));
      
      // make entity steer towards predecessor
      SteeringComponent* steer;
      em.CreateComponent(spawned->GetId(), steer);
      steer->SetInt(SteeringComponent::TargetId, lastEntity);
      
      MovementComponent* move;
      success = em.CreateComponent(spawned->GetId(), move);
      assert(success);
      // add to scene and scene graph
      em.AddToScene(spawned->GetId());

      lastEntity = spawned->GetId();      
   }


   // skybox screws up OSG initial position, set manually
   appsys->GetPrimaryView()->setCameraManipulator(new osgGA::TrackballManipulator());
   appsys->GetPrimaryView()->getCameraManipulator()->setHomePosition(osg::Vec3(0, -50, 5), osg::Vec3(), osg::Vec3(0,0,1),false);
   appsys->GetPrimaryView()->getCameraManipulator()->home(0);

   return viewer.run();

}
