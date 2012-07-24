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

#include <dtEntity/entitymanager.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/core.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/logmanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/systeminterface.h>
#include <dtEntityNet/messages.h>
#include <dtEntityNet/enetcomponent.h>
#include <dtEntityNet/deadreckoningreceivercomponent.h>
#include <dtEntityNet/deadreckoningsendercomponent.h>
#include <dtEntityNet/enetcomponent.h>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#ifdef _WIN32
#include <windows.h>
#define SLEEPFUN Sleep
#else
#define SLEEPFUN sleep
#endif

#define PORT_NUMBER 6789

void Joined(const dtEntity::Message& m)
{
   const dtEntityNet::JoinMessage& msg = static_cast<const dtEntityNet::JoinMessage&>(m);
   LOG_ALWAYS("JoinedMessage EntityType " << msg.GetEntityType() << " uid " << msg.GetUniqueId());
}

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
   enetsys->Connect("127.0.0.1", PORT_NUMBER);

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

   // skybox screws up OSG initial position, set manually
   viewer.setCameraManipulator(new osgGA::TrackballManipulator());
   viewer.getCameraManipulator()->setHomePosition(osg::Vec3(0, -50, 5), osg::Vec3(), osg::Vec3(0,0,1),false);
   viewer.getCameraManipulator()->home(0);

   dtEntity::SystemInterface* iface = dtEntity::GetSystemInterface();

   while (!viewer.done())
   {
      viewer.advance(DBL_MAX);
      viewer.eventTraversal();
      iface->EmitTickMessagesAndQueuedMessages();
      viewer.updateTraversal();
      viewer.renderingTraversals();
   }

}
