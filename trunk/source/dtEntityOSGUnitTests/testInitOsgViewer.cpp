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
#include <dtEntity/core.h>
#include <dtEntity/core.h>
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/initosgviewer.h>
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

   CHECK(getenv("DTENTITY_BASEASSETS") != NULL);
   CHECK(getenv("DTENTITY_PROJECTASSETS") != NULL);
   char* args[2];
   args[0] = getenv("DTENTITY_BASEASSETS");
   args[1] = getenv("DTENTITY_PROJECTASSETS");

   bool success = dtEntityOSG::InitOSGViewer(2, args, viewer, em, true, true, true, root);
   CHECK(success);

   dtEntityOSG::OSGSystemInterface* iface = static_cast<dtEntityOSG::OSGSystemInterface*>(dtEntity::GetSystemInterface());
   // check if passed root node is actually used as root node
   CHECK(iface->GetPrimaryView()->getSceneData() == root);


   ComponentPluginManager::DestroyInstance();
}

int main()
{
 int ret = UnitTest::RunAllTests();
 return ret;
}
