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

#include <UnitTest++.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/windowmanager.h>
#include <dtEntity/initosgviewer.h>
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geode>


using namespace UnitTest;
using namespace dtEntity;



TEST(InitOsgViewer)
{
   osgViewer::Viewer viewer;
   dtEntity::EntityManager em;
   
   osg::Group* root = new osg::Group();
   osg::Sphere* sphere = new osg::Sphere(osg::Vec3(), 5);
   osg::ShapeDrawable* drawable = new osg::ShapeDrawable(sphere);
   osg::Geode* geode = new osg::Geode();
   geode->addDrawable(drawable);
   root->addChild(geode);

   bool success = dtEntity::InitOSGViewer(0, NULL, viewer, em, true, true, true, root);
   CHECK(success);

   CHECK(em.HasEntitySystem(dtEntity::SID("Application")));
   ApplicationSystem* appsys;
   CHECK(em.GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys));

   // check if passed root node is actually used as root node
   CHECK(appsys->GetPrimaryView()->getSceneData() == root);

   // check if input handler was installed to camera
   CHECK(appsys->GetPrimaryCamera()->getEventCallback() == &appsys->GetWindowManager()->GetInputHandler());
}
