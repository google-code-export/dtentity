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

#include <dtEntity/basemessages.h>
#include <dtEntity/component.h>
#include <dtEntity/stringid.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntityENet/netcomponent.h>



int main()
{
/*
   // do standard delta3d demo setup
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" + 
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data/demoMap" + ";" +
      EXAMPLE_DATA_PATH + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/testApp/;");

   dtCore::RefPtr<TestApp> app = new TestApp();

   dtCore::FlyMotionModel* fmm = new dtCore::FlyMotionModel(app->GetKeyboard(), app->GetMouse());
   fmm->SetTarget(app->GetCamera());

   //load some terrain
   dtCore::RefPtr<dtCore::Object> terrain = new dtCore::Object("Terrain");
   terrain->LoadFile("models/terrain_simple.ive");
   terrain->SetScale(osg::Vec3(1, 1, 0));
   app->AddDrawable(terrain.get());
   app->SetNextStatisticsType();
   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.f, -30.f, 15.f);
   osg::Vec3 lookAtXYZ(0.0f, 0.0f, 3.5f);
   osg::Vec3 upVec(0.f, 0.f, 1.f);
   camPos.Set(camXYZ, lookAtXYZ, upVec);
   app->GetCamera()->SetTransform(camPos);

   /////////////////////////////////////////////////////////////////////

   // create new entity manager and register with system
   osg::ref_ptr<dtEntity::EntityManager> em = new dtEntity::EntityManager();

   dtEntity::MapSystem* mSystem;
   em->GetEntitySystem(dtEntity::MapComponent::TYPE, mSystem);
   std::string path = EXAMPLE_DATA_PATH + std::string("/testscene.xml");
   
   dtEntity::MapSystem* mapSystem;
   em->GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);

   mapSystem->GetPluginManager().LoadPluginsInDir("plugins");

   bool success = mapSystem->GetPluginManager().StartEntitySystem(dtEntity::SIDHash("Net"));
   assert(success && "Net system not found, check plugin paths");

   dtEntityENet::NetSystem* netSystem;
   bool found = em->GetEntitySystem(dtEntityENet::NetComponent::TYPE, netSystem);
   assert(found);

   dtEntity::TickMessage msg;
   msg.SetFloat(dtEntity::TickMessage::DeltaSimTimeId, 10);
   msg.SetFloat(dtEntity::TickMessage::DeltaRealTimeId, 20);
   msg.SetFloat(dtEntity::TickMessage::SimTimeScaleId, 30);
   msg.SetDouble(dtEntity::TickMessage::SimulationTimeId, 40);

   netSystem->EnqueueMessage(msg);
   netSystem->SendToNetwork();
   app->Config();
   app->Run();
   delete emb;*/
   return 0;
}
