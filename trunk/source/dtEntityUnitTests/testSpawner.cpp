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

#include <dtEntity/osgcomponents.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/spawner.h>
#include <UnitTest++.h>


using namespace UnitTest;
using namespace dtEntity;


TEST(SpawnComponent)
{
   #if BOOST_MSVC
      _CrtSetDbgFlag(0);
   #endif
   
   Spawner* spawner = new Spawner("mymap", "");

   DynamicPropertyContainer pprops;
   pprops.AddProperty(PositionAttitudeTransformComponent::PositionId, Vec3Property(osg::Vec3(33,66,99)));
   spawner->AddComponent(PositionAttitudeTransformComponent::TYPE, pprops);

   osg::ref_ptr<EntityManager> em = new EntityManager();
   
   Entity* entity;
   em->CreateEntity(entity);

   spawner->Spawn(*entity);

   PositionAttitudeTransformComponent* component;
   CHECK(entity->GetComponent(component));

   Property* prop = component->Get(PositionAttitudeTransformComponent::PositionId);
   CHECK(prop != NULL);

   CHECK_EQUAL(prop->Vec3Value()[0], 33.0f);

}

TEST(SpawnHierarchy)
{
   Spawner* parentSpawner = new Spawner("Bla", "mapname");
   Spawner* childSpawner = new Spawner("Bla2", "mapname",parentSpawner);   

   DynamicPropertyContainer parentprops;
   parentprops.AddProperty(PositionAttitudeTransformComponent::PositionId, Vec3Property(osg::Vec3(33,66,99)));
   parentSpawner->AddComponent(PositionAttitudeTransformComponent::TYPE, parentprops);

   DynamicPropertyContainer childprops;
   childprops.AddProperty(PositionAttitudeTransformComponent::PositionId, Vec3Property(osg::Vec3(333,666,999)));
   childSpawner->AddComponent(PositionAttitudeTransformComponent::TYPE, childprops);

   osg::ref_ptr<EntityManager> em = new EntityManager();

   CHECK(childSpawner->GetParent() == parentSpawner);
   

   Entity* entity;
   em->CreateEntity(entity);

   childSpawner->Spawn(*entity);

   PositionAttitudeTransformComponent* component;
   CHECK(entity->GetComponent(component));

   Property* prop = component->Get(PositionAttitudeTransformComponent::PositionId);
   CHECK(prop != NULL);

   CHECK_EQUAL(prop->Vec3Value()[0], 333.0f);

}
