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

#include <dtEntity/entitymanager.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/mapcomponent.h>
#include <UnitTest++.h>

using namespace UnitTest;
using namespace dtEntity;

namespace EMTest
{



   //------------------------------------------------------------------
   TEST(CreateEntity)
   {
      EntityManager* em = new EntityManager();
      Entity* entity;
      em->CreateEntity(entity);
      CHECK_EQUAL((unsigned int)1, entity->GetId());
      std::vector<EntityId> ids;
      em->GetEntityIds(ids);
      CHECK_EQUAL((unsigned int)1, ids.size());
      CHECK_EQUAL((unsigned int)1, ids.front());
      delete em;
   }

   //------------------------------------------------------------------
   TEST(GetEntity)
   {
      EntityManager* em = new EntityManager();
      Entity* entity1, *entity2, *entity3, *testentity;
      em->CreateEntity(entity1);
      em->CreateEntity(entity2);
      em->CreateEntity(entity3);

      em->GetEntity(entity2->GetId(), testentity);

      CHECK_EQUAL(testentity, entity2);
      delete em;

   }

   //------------------------------------------------------------------
   TEST(KillEntity)
   {
      EntityManager* em = new EntityManager();
      Entity* entity;

      em->CreateEntity(entity);
      EntityId id = entity->GetId();

      CHECK_EQUAL(true, em->EntityExists(id));
      em->KillEntity(entity->GetId());

      CHECK_EQUAL(false, em->EntityExists(id));

      std::vector<EntityId> ids;
      em->GetEntityIds(ids);
      CHECK(ids.empty());

      delete em;

   }

   /*//------------------------------------------------------------------
   TEST(AddEntitySystem)
   {
      // test does not work. Check on windows
      ComponentType ctype = 123456;
      
      TMockObject<EntitySystem> mock;
      mock.Method(&EntitySystem::GetComponentType).Will(ctype);
      mock.Method(&EntitySystem::OnAddedToEntityManager);
      mock.Method(&EntitySystem::OnRemoveFromEntityManager);

      EntitySystem* es = (EntitySystem*)mock;
      unsigned int test = es->GetComponentType();
      CHECK_EQUAL(ctype, test);

      EntityManager* em = new EntityManager();

      CHECK_EQUAL(0, mock.Method(&EntitySystem::OnAddedToEntityManager).Count());
      em->AddEntitySystem(*es);
      CHECK_EQUAL(1u, mock.Method(&EntitySystem::OnAddedToEntityManager).Count());

      CHECK(em->HasEntitySystem(ctype));
 
      EntitySystem* es2 = em->GetEntitySystem(ctype);
      CHECK_EQUAL(es2, es);

      EntitySystem* es3;
      bool success = em->GetEntitySystem(ctype, es3);
      CHECK(success);
      CHECK_EQUAL(es3, es);

      em->RemoveEntitySystem(*es);

      CHECK(!em->HasEntitySystem(ctype));

      
      CHECK_EQUAL(1u, mock.Method(&EntitySystem::OnRemoveFromEntityManager).Count());

       delete em;
   }
*/
   //------------------------------------------------------------------
   TEST(GetEntitySystem)
   {
      EntityManager* em = new EntityManager();
      
      MapSystem* mapsys = new MapSystem(*em);
      em->AddEntitySystem(*mapsys);      
     
      MapSystem* mapsys2;
      CHECK(em->GetES(mapsys2));
      CHECK_EQUAL(mapsys, mapsys2);

      delete em;
   }

}
