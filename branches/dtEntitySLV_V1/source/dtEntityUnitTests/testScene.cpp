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
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/init.h>
#include <dtEntity/entitymanager.h>
#include <osgDB/FileUtils>
#include <dtEntityOSG/initosgviewer.h>

using namespace UnitTest;
using namespace dtEntity;

struct SceneFixture
{
   SceneFixture()
   {
      SetupDataPaths(0, NULL, true);
      SetSystemInterface(new dtEntityOSG::OSGSystemInterface(mEntityManager.GetMessagePump(), 0, NULL));
      AddDefaultEntitySystemsAndFactories(0, NULL, mEntityManager); 
      mEntityManager.GetEntitySystem(MapComponent::TYPE, mMapSystem);
   }

   ~SceneFixture()
   {
      ComponentPluginManager::DestroyInstance();
   }

   dtEntity::EntityManager mEntityManager;
   dtEntity::MapSystem* mMapSystem;
};


TEST_FIXTURE(SceneFixture, SceneCanBeLoaded)
{
   bool success = mMapSystem->LoadScene("TestData/testscene.dtescene");
   CHECK(success);
}

TEST_FIXTURE(SceneFixture, SceneLoadsMap)
{
   mMapSystem->LoadScene("TestData/testscene.dtescene");
   CHECK(mMapSystem->IsMapLoaded("TestData/testmap.dtemap"));
}
