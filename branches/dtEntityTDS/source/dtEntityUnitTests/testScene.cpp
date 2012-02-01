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

#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <osgDB/FileUtils>

using namespace UnitTest;
using namespace dtEntity;

struct SceneFixture
{
   SceneFixture()
   {
      std::string baseassets = "BaseAssets";      
      const char* env_baseassets = getenv("DTENTITY_BASEASSETS");
      if(env_baseassets != NULL)
      {
         baseassets = env_baseassets;
      }

      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      if(!baseassets.empty()) paths.push_back(baseassets);
      osgDB::setDataFilePathList(paths);

      mEntityManager = new dtEntity::EntityManager();
      mMapSystem = new dtEntity::MapSystem(*mEntityManager);
	  mEntityManager->AddEntitySystem(*mMapSystem);
      mEntityManager->AddEntitySystem(*new dtEntity::ApplicationSystem(*mEntityManager));
   }
   osg::ref_ptr<dtEntity::EntityManager> mEntityManager;
   dtEntity::MapSystem* mMapSystem;
};


TEST_FIXTURE(SceneFixture, SceneCanBeLoaded)
{
   bool success = mMapSystem->LoadScene("TestData/testscene.dtescene");
   CHECK(success);
}

TEST_FIXTURE(SceneFixture, SceneSetsProps)
{
   mMapSystem->LoadScene("TestData/testscene.dtescene");
   dtEntity::ApplicationSystem* appsys;
   mEntityManager->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
   CHECK_CLOSE(appsys->GetTimeScale(), 3, 0.1);
}

TEST_FIXTURE(SceneFixture, SceneLoadsMap)
{
   mMapSystem->LoadScene("TestData/testscene.dtescene");
   CHECK(mMapSystem->IsMapLoaded("TestData/testmap.dtemap"));
}
