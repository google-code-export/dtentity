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

#include <dtEntityNet/deadreckoningreceivercomponent.h>

#include <dtEntity/core.h>
#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/protobufmapencoder.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/systemmessages.h>
#include <dtEntityOSG/transformcomponent.h>
#include <dtEntity/uniqueid.h>
#include <dtEntityNet/messages.h>

namespace dtEntityNet
{
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId DeadReckoningReceiverComponent::TYPE(dtEntity::SID("DeadReckoningReceiver"));
   const dtEntity::StringId DeadReckoningReceiverComponent::UniqueIdId(dtEntity::SID("UniqueId"));

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningReceiverComponent::DeadReckoningReceiverComponent()
      : mTransformComponent(NULL)
      , mDynamicsComponent(NULL)
      , mTimeLastReceive(0)
      , mDeadRecAlg(DeadReckoningAlgorithm::DISABLED)
   {
      Register(UniqueIdId, &mUniqueId);
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId DeadReckoningReceiverSystem::TYPE(dtEntity::SID("DeadReckoningReceiver"));
   const dtEntity::StringId DeadReckoningReceiverSystem::SpawnFromEntityTypeId(dtEntity::SID("SpawnFromEntityType"));

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   DeadReckoningReceiverSystem::DeadReckoningReceiverSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mMapSystem(NULL)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &DeadReckoningReceiverSystem::Tick);

      em.GetES(mMapSystem);

      Register(SpawnFromEntityTypeId, &mSpawnFromEntityType);
      mSpawnFromEntityType.Set(true);

   }

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningReceiverSystem::~DeadReckoningReceiverSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::OnAddedToEntityManager(dtEntity::EntityManager &em)
   {
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "DeadReckoningReceiverSystem::Tick");

   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::OnRemovedFromEntityManager(dtEntity::EntityManager &em)
   {
      GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::Tick(const dtEntity::Message& m)
   {
      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      for(ComponentStore::iterator i = mComponents.begin();i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         DeadReckoningReceiverComponent* comp = i->second;

         // if did not yet receive a valid position
         if(comp->mTimeLastReceive == 0)
         {
            continue;
         }

         if(comp->mDeadRecAlg == DeadReckoningAlgorithm::DISABLED)
         {
            continue;
         }
         if(comp->mTransformComponent == NULL)
         {
            bool success = GetEntityManager().GetComponent(id, comp->mTransformComponent, true);
            if(!success)
            {
               LOG_ERROR("NetworSender Component expects a Transform Component!");
               return;
            }
         }

         if(comp->mDynamicsComponent == NULL)
         {
            bool success = GetEntityManager().GetComponent(id, comp->mDynamicsComponent, true);
            if(!success)
            {
               LOG_ERROR("DeadReckoningReceiver Component expects a Dynamics Component!");
               return;
            }
         }

         comp->mDynamicsComponent->SetVelocity(comp->mVelocity);
         comp->mDynamicsComponent->SetAngularVelocity(EulerToQuat(comp->mAngularVelocity));

         switch(comp->mDeadRecAlg)
         {
            case DeadReckoningAlgorithm::STATIC:
            {
               comp->mTransformComponent->SetTranslation(comp->mPosition);
               comp->mTransformComponent->SetRotation(EulerToQuat(comp->mOrientation));
               break;
            }
            case DeadReckoningAlgorithm::FPW:
            {

               osg::Vec3d newpos;
               osg::Vec3 newori;
               CalculateDRFPW(comp->mPosition,
                           comp->mOrientation,
                           comp->mVelocity,
                           comp->mAngularVelocity,
                           msg.GetSimulationTime() - comp->mTimeLastReceive,
                           newpos,
                           newori);

               // apply smoothing
               if(true)
               {
                  newpos = (newpos + comp->mTransformComponent->GetTranslation()) * 0.5f;
               }
               comp->mTransformComponent->SetTranslation(newpos);

               osg::Quat smoothr;
               smoothr.slerp(0.5, EulerToQuat(newori), comp->mTransformComponent->GetRotation());
               comp->mTransformComponent->SetRotation(smoothr);
               break;
            }
            default: assert(false && "Unimplemented dead reckoning algorithm used");
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::OnJoin(const dtEntity::Message& m)
   {

      if(GetSpawnFromEntityType())
      {
         const JoinMessage& msg = static_cast<const JoinMessage&>(m);
         std::string entitytype = msg.GetEntityType();
         std::string uniqueid = msg.GetUniqueId();

         dtEntity::Entity* entity;
         GetEntityManager().CreateEntity(entity);

         dtEntity::Spawner* spawner;
         if(!mMapSystem->GetSpawner(entitytype, spawner))
         {
            LOG_ERROR("Cannot instantiate remote entity, spawner not found: " << entitytype);
            return;
         }

         spawner->Spawn(*entity);

         dtEntity::MapComponent* mapcomp;
         entity->GetComponent(mapcomp);
         mapcomp->SetUniqueId(uniqueid);

         dtEntity::DynamicsComponent* dc;
         entity->CreateComponent(dc);

         DeadReckoningReceiverComponent* rc;
         entity->CreateComponent(rc);
         rc->mUniqueId = msg.GetUniqueId();

      }


   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::OnResign(const dtEntity::Message& m)
   {
      if(GetSpawnFromEntityType())
      {
         const ResignMessage& msg = static_cast<const ResignMessage&>(m);

         dtEntity::Entity* entity;
         if(!mMapSystem->GetEntityByUniqueId(msg.GetUniqueId(), entity))
         {
            LOG_ERROR("Cannot resign: Entity not found with unique id " << msg.GetUniqueId());
            return;
         }
         mMapSystem->RemoveFromScene(entity->GetId());
         GetEntityManager().KillEntity(entity->GetId());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningReceiverSystem::OnUpdateTransform(const dtEntity::Message& m)
   {
      assert(dynamic_cast<const UpdateTransformMessage*>(&m) != NULL);
      const UpdateTransformMessage& msg = static_cast<const UpdateTransformMessage&>(m);

      assert(mMapSystem != NULL);
      dtEntity::EntityId id = mMapSystem->GetEntityIdByUniqueId(msg.GetUniqueId());
      if(id == 0)
      {
         LOG_ERROR("Got transform for an entity that has not yet joined!?!");
         return;
      }
      DeadReckoningReceiverComponent * comp = GetComponent(id);
      if(!comp)
      {
         LOG_ERROR("Received transform, entity exists but has no receiver component!");
         return;
      }

      // first time a transform was received, make visible!
      if(comp->mTimeLastReceive == 0)
      {
         if(comp->mTransformComponent == NULL)
         {
            bool success = GetEntityManager().GetComponent(id, comp->mTransformComponent, true);
            if(!success)
            {
               LOG_ERROR("NetworSender Component expects a Transform Component!");
               return;
            }
         }

         comp->mTransformComponent->SetTranslation(msg.GetPosition());
         comp->mTransformComponent->SetRotation(EulerToQuat(msg.GetOrientation()));
         mMapSystem->AddToScene(id);
      }


      comp->mTimeLastReceive = dtEntity::GetSystemInterface()->GetSimulationTime();
      comp->mPosition = msg.GetPosition();
      comp->mOrientation = msg.GetOrientation();
      comp->mVelocity = msg.GetVelocity();
      comp->mAngularVelocity = msg.GetAngularVelocity();
      comp->mDeadRecAlg = msg.GetDeadReckoning();
   }

}
