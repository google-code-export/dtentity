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
#include <dtEntity/core.h>
#include <dtEntity/component.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/positionattitudetransformcomponent.h>
#include <dtEntity/skyboxcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/systemmessages.h>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <dtEntity/dynamicscomponent.h>
#include <dtEntityNet/enetcomponent.h>
#include <dtEntityNet/deadreckoningsendercomponent.h>
#include <dtEntityNet/deadreckoningreceivercomponent.h>
#include <dtEntityNet/messages.h>

#define NUMBER_OF_ENTITIES 5
#define PORT_NUMBER 6789

////////////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
   osg::ArgumentParser arguments(&argc,argv);
   osgViewer::Viewer viewer(arguments);
   dtEntity::EntityManager em;

   dtEntityNet::RegisterMessageTypes(dtEntity::MessageFactory::GetInstance());
   osg::Group* root = new osg::Group();

   if(!dtEntity::InitOSGViewer(argc, argv, viewer, em, true, true, true, root))
   {
      LOG_ERROR("Error setting up dtEntity!");
      return 0;
   }

   dtEntity::MapSystem* mSystem;
   em.GetEntitySystem(dtEntity::MapComponent::TYPE, mSystem);

   em.AddEntitySystem(*new dtEntityNet::DeadReckoningSenderSystem(em));
   em.AddEntitySystem(*new dtEntityNet::DeadReckoningReceiverSystem(em));
   em.AddEntitySystem(*new dtEntityNet::ENetSystem(em));


   dtEntityNet::ENetSystem* enetsys;
   em.GetES(enetsys);
   enetsys->InitializeServer(PORT_NUMBER);

   std::string path = "Scenes/boids.dtescene";
   bool success = mSystem->LoadScene(path);
   if(!success)
   {
      LOG_ERROR("Could not load scene " + path);
   }

   // make sure these component systems are started
   dtEntity::ComponentPluginManager& pm = dtEntity::ComponentPluginManager::GetInstance();
   pm.StartEntitySystem(em, dtEntity::SIDHash("StaticMesh"));
   pm.StartEntitySystem(em, dtEntity::SIDHash("PositionAttitudeTransform"));

   // create spawner for entity
   dtEntity::Spawner* spawner;
   if(!mSystem->GetSpawner("Boids", spawner))
   {
      LOG_ERROR("Could not find spawner 'Boids' in map!");
      return 0;
   }

   std::vector<unsigned int> entityids;
   // Now create a large number of moving entities
   // store id of last entity to make each entity follow its predecessor
   for(unsigned int i = 0; i < NUMBER_OF_ENTITIES; ++i)
   {
      dtEntity::Entity* spawned;
      em.CreateEntity(spawned);
      entityids.push_back(spawned->GetId());

      // instantiate entity
      bool success = spawner->Spawn(*spawned);
      assert(success);
      // set a start position
      dtEntity::PositionAttitudeTransformComponent* trans;
      success = em.GetComponent(spawned->GetId(), trans);
      assert(success);
      trans->SetPosition(osg::Vec3(i * 3, 0.0f, 0.5f));

      dtEntity::DynamicsComponent* dynamics;
      em.GetComponent(spawned->GetId(), dynamics);
      dynamics->SetVelocity(osg::Vec3((float)sin(i / 10.0f), (float)cos(i / 10.0f), 0) * 5);
      dynamics->SetAngularVelocity(osg::Quat(0.3 * i, osg::Vec3(0,0,1)));

      dtEntityNet::DeadReckoningSenderComponent* sender;
      success = em.CreateComponent(spawned->GetId(), sender);
      sender->SetDeadReckoningAlgorithm(dtEntityNet::DeadReckoningAlgorithm::FPW);
      sender->SetEntityType("Boids");
      assert(success);

      // add to scene and scene graph
      em.AddToScene(spawned->GetId());
   }


   // skybox screws up OSG initial position, set manually
   viewer.setCameraManipulator(new osgGA::TrackballManipulator());
   viewer.getCameraManipulator()->setHomePosition(osg::Vec3(0, -50, 5), osg::Vec3(), osg::Vec3(0,0,1),false);
   viewer.getCameraManipulator()->home(0);

   dtEntity::SystemInterface* iface = dtEntity::GetSystemInterface();

   double lastTime = dtEntity::GetSystemInterface()->GetSimulationTime();
   while (!viewer.done())
   {
      double simtime = dtEntity::GetSystemInterface()->GetSimulationTime();
      double dt = simtime - lastTime;
      lastTime = simtime;
      viewer.advance(DBL_MAX);
      viewer.eventTraversal();
      iface->EmitTickMessagesAndQueuedMessages();
      dtEntity::PositionAttitudeTransformComponent* pos;
      dtEntity::DynamicsComponent* dyn;

      for(unsigned int i = 0; i < entityids.size(); ++i)
      {
         unsigned int id = entityids[i];
         em.GetComponent(id, pos);
         em.GetComponent(id, dyn);
         osg::Vec3d cpos = pos->GetPosition();
         osg::Quat catt = pos->GetRotation();
         cpos += dyn->GetVelocity() * dt;

         double angle, x, y, z;
         osg::Quat drot = dyn->GetAngularVelocity();
         drot.getRotate(angle, x, y, z);
         drot.makeRotate(angle * dt, x, y, z);

         catt *= drot;
         dyn->SetVelocity(drot * dyn->GetVelocity());

         pos->SetPosition(cpos);
         pos->SetRotation(catt);

      }
      viewer.updateTraversal();
      viewer.renderingTraversals();
   }

}
