/* -*-c++-*-
* testPhysX - testPhysX(.h & .cpp) - Using 'The MIT License'
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
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/component.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityPhysX/pagedterraincullvisitor.h>
#include <dtEntityPhysX/physxcomponent.h>
#include <dtEntityPhysX/physxpagedterraincomponent.h>
#include <osgViewer/Renderer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/FileUtils>
#include <iostream>


using namespace dtEntity;

///////////////////////////////////////////////



int main(int argc, char** argv)
{

    
   /////////////////////////////////////////////////////////////////////

   osg::ArgumentParser arguments(&argc,argv);   
   osgViewer::Viewer viewer(arguments);
   dtEntity::EntityManager* em = new dtEntity::EntityManager();
   
   if(!dtEntity::InitOSGViewer(argc, argv, &viewer, em))
   {
      LOG_ERROR("Error setting up dtEntity!");
      return 0;
   }
   
   dtEntity::ApplicationSystem* appsys;
   em->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
   
   dtEntity::MapSystem* mSystem;
   em->GetEntitySystem(dtEntity::MapComponent::TYPE, mSystem);

   mSystem->GetPluginManager().LoadPluginsInDir("plugins");

   bool success = mSystem->GetPluginManager().StartEntitySystem(dtEntity::SID("PhysX"));
   assert(success && "PhysX system not found, check plugin paths");
   
   dtEntityPhysX::PhysXSystem* physXSystem;
   success = em->GetEntitySystem(dtEntity::SID("PhysX"), physXSystem);
   assert(success);

   success = mSystem->GetPluginManager().StartEntitySystem(dtEntityPhysX::PhysXPagedTerrainComponent::TYPE);
   assert(success && "PhysX paged terrain system not found, check plugin paths");

   dtEntityPhysX::PhysXPagedTerrainSystem* psys;
   em->GetEntitySystem(dtEntityPhysX::PhysXPagedTerrainComponent::TYPE, psys);
   psys->SetFloat(dtEntityPhysX::PhysXPagedTerrainSystem::CookingRadiusId, 30000);
   psys->SetFloat(dtEntityPhysX::PhysXPagedTerrainSystem::CullRadiusId, 75000);
  
   
   std::string path = "/Scenes/testphysxterrain.dtescene";
   success = mSystem->LoadScene(path);
   if(!success)
   {
      LOG_ERROR("Could not load scene " + path);
   }

   //dtEntity::EntityId eid = mSystem->GetEntityByUniqueId("terrain");
   //psys->SetTerrain(eid);

   dtEntity::EnableDebugDrawingMessage msg;
   msg.SetBool(dtEntity::EnableDebugDrawingMessage::EnableId, true);
   em->EmitMessage(msg);
     
   appsys->GetPrimaryView()->setCameraManipulator(new osgGA::TrackballManipulator());
   appsys->GetPrimaryView()->getCameraManipulator()->setHomePosition(osg::Vec3(10000, 10000, 1770), osg::Vec3(11000, 11000, 1770), osg::Vec3(0,0,1),false);
   appsys->GetPrimaryView()->getCameraManipulator()->home(0);

   
   dtEntity::WindowManager* wm = appsys->GetWindowManager();
   dtEntity::InputHandler& ih = wm->GetInputHandler();
   dtEntity::DebugDrawManager* debugDrawManager = new dtEntity::DebugDrawManager(*em);
   debugDrawManager->SetEnabled(true);

   dtEntityPhysX::PhysXSystem* physxSystem;
   em->GetEntitySystem(dtEntityPhysX::PhysXComponent::TYPE, physxSystem);
   physxSystem->GetDebugDrawManager()->SetEnabled(true);

   while (!viewer.done())
   {
      viewer.frame();
      if(ih.GetMouseButtonDown(0))
      {
         osg::Vec3f eye;
         osg::Vec3f center;
         osg::Vec3f up;
         appsys->GetPrimaryCamera()->getViewMatrixAsLookAt(eye, center, up);
         
         osg::Vec3 dir = wm->GetPickRay("defaultView", ih.GetAxis(dtEntity::SID("MouseX")), ih.GetAxis(dtEntity::SID("MouseY")));
         dir.normalize();
         dtEntity::Entity* cube;
         em->CreateEntity(cube);
         mSystem->Spawn("PhysicsCube", *cube);
         dtEntity::PositionAttitudeTransformComponent* mtcomp;
         bool success = cube->GetComponent(mtcomp);
         assert(success);
         mtcomp->SetPosition(eye + dir * 100);

         dtEntityPhysX::PhysXComponent* pcomp;
         cube->GetComponent(pcomp);

         
         em->AddToScene(cube->GetId());

         debugDrawManager->AddLine(eye + dir * 5, eye + dir * 20, osg::Vec4(1,1,0,1), 1, 10);
         
      }

      if(ih.GetMouseButtonDown(1))
      {
         osg::Vec3f eye;
         osg::Vec3f center;
         osg::Vec3f up;
         appsys->GetPrimaryCamera()->getViewMatrixAsLookAt(eye, center, up);
         
         osg::Vec3 dir = wm->GetPickRay("defaultView", ih.GetAxis(dtEntity::SID("MouseX")), ih.GetAxis(dtEntity::SID("MouseY")));
         
         debugDrawManager->AddLine(eye, eye + dir * 10, osg::Vec4(1,1,0,1), 1, 10);
         
         dtEntity::EntityId result;
         bool success = physXSystem->RaycastClosestEntity(eye, dir, result);
         if(success)
         {
            std::ostringstream os;
            os << "Picked: " << result<<"\n";
            LOG_ALWAYS(os.str());
         }
         else
         {
            LOG_ALWAYS("Nothing picked");
         }
      }
    
      if(ih.GetKeyDown("Space"))
      {
         physxSystem->SetEnabled(!physxSystem->GetEnabled());
      }

   }
   return 0;
}
