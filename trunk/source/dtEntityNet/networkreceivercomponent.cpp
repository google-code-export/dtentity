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

#include <dtEntityNet/networkreceivercomponent.h>

#include <dtEntityNet/networksendercomponent.h>
#include <dtEntityNet/messages.h>
#include <dtEntityNet/enetcomponent.h>

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/protobufmapencoder.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/uniqueid.h>

namespace dtEntityNet
{
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NetworkReceiverComponent::TYPE(dtEntity::SID("NetworkReceiver"));

   ////////////////////////////////////////////////////////////////////////////
   NetworkReceiverComponent::NetworkReceiverComponent()
      : mTransformComponent(NULL)
      , mDynamicsComponent(NULL)
      , mTimeLastReceive(0)
      , mDeadRecAlg(DeadReckoningAlgorithm::DISABLED)
   {

   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NetworkReceiverSystem::TYPE(dtEntity::SID("NetworkReceiver"));

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   NetworkReceiverSystem::NetworkReceiverSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mApplicationSystem(NULL)
      , mMapSystem(NULL)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &NetworkReceiverSystem::Tick);

      em.GetES(mMapSystem);
      em.GetES(mApplicationSystem);

   }

   ////////////////////////////////////////////////////////////////////////////
   NetworkReceiverSystem::~NetworkReceiverSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::OnAddedToEntityManager(dtEntity::EntityManager &em)
   {
      ENetSystem* es;
      bool enetSystemInEntityManager = em.GetES(es);
      assert(enetSystemInEntityManager);
      dtEntity::MessagePump& incoming = es->GetIncomingMessagePump();

      incoming.RegisterForMessages(UpdateTransformMessage::TYPE,
         dtEntity::MessageFunctor(this, &NetworkReceiverSystem::OnUpdateTransform),
                                   dtEntity::FilterOptions::ORDER_LATE, "UpdateTransformMessage::OnUpdateTransform");


      incoming.RegisterForMessages(JoinMessage::TYPE,
         dtEntity::MessageFunctor(this, &NetworkReceiverSystem::OnJoin),
                                   dtEntity::FilterOptions::ORDER_DEFAULT, "UpdateTransformMessage::OnJoin");

      incoming.RegisterForMessages(ResignMessage::TYPE,
         dtEntity::MessageFunctor(this, &NetworkReceiverSystem::OnResign),
                                   dtEntity::FilterOptions::ORDER_DEFAULT, "UpdateTransformMessage::OnResign");

      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "NetworkReceiverSystem::Tick");


   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::OnRemovedFromEntityManager(dtEntity::EntityManager &em)
   {
      GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::Tick(const dtEntity::Message& m)
   {
      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      for(ComponentStore::iterator i = mComponents.begin();i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         NetworkReceiverComponent* comp = i->second;
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
               LOG_ERROR("NetworkReceiver Component expects a Dynamics Component!");
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
               osg::Vec3d smootht = (newpos + comp->mTransformComponent->GetTranslation()) * 0.5f;
               comp->mTransformComponent->SetTranslation(smootht);

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
   void NetworkReceiverSystem::OnJoin(const dtEntity::Message& m)
   {
      const JoinMessage& msg = static_cast<const JoinMessage&>(m);

      std::string entitytype = msg.GetEntityType();
      std::string uniqueid = msg.GetUniqueId();

      dtEntity::Entity* entity;
      GetEntityManager().CreateEntity(entity);
      dtEntity::MapComponent* mapcomp;
      entity->CreateComponent(mapcomp);
      mapcomp->SetUniqueId(uniqueid);

      dtEntity::MapSystem* mapsys;
      GetEntityManager().GetES(mapsys);

      dtEntity::Spawner* spawner;
      if(!mapsys->GetSpawner(entitytype, spawner))
      {
         LOG_ERROR("Cannot instantiate remote entity, spawner not found: " << entitytype);
         return;
      }

      spawner->Spawn(*entity);
      NetworkReceiverComponent* rc;
      entity->CreateComponent(rc);
      rc->mUniqueId = msg.GetUniqueId();

      mapsys->AddToScene(entity->GetId());

   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::OnResign(const dtEntity::Message& m)
   {
      const ResignMessage& msg = static_cast<const ResignMessage&>(m);
      dtEntity::MapSystem* mapsys;
      GetEntityManager().GetES(mapsys);
      dtEntity::Entity* entity;
      if(!mapsys->GetEntityByUniqueId(msg.GetUniqueId(), entity))
      {
         LOG_ERROR("Cannot resign: Entity not found with unique id " << msg.GetUniqueId());
         return;
      }
      mapsys->RemoveFromScene(entity->GetId());
      GetEntityManager().KillEntity(entity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::OnUpdateTransform(const dtEntity::Message& m)
   {
      const UpdateTransformMessage& msg = static_cast<const UpdateTransformMessage&>(m);

      assert(mMapSystem != NULL);
      dtEntity::EntityId id = mMapSystem->GetEntityIdByUniqueId(msg.GetUniqueId());
      if(id == 0)
      {
         LOG_ERROR("Got transform for an entity that has not yet joined!?!");
         return;
      }
      NetworkReceiverComponent* comp = GetComponent(id);
      if(!comp)
      {
         LOG_ERROR("Received transform, entity exists but has no receiver component!");
         return;
      }

      assert(mApplicationSystem != NULL);
      comp->mTimeLastReceive = mApplicationSystem->GetSimulationTime();
      comp->mPosition = msg.GetPosition();
      comp->mOrientation = msg.GetOrientation();
      comp->mVelocity = msg.GetVelocity();
      comp->mAngularVelocity = msg.GetAngularVelocity();
      comp->mDeadRecAlg = msg.GetDeadReckoning();
   }

}
