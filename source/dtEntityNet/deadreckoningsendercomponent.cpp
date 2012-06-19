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

#include <dtEntityNet/deadreckoningsendercomponent.h>

#include <dtEntityNet/messages.h>
#include <dtEntityNet/enetcomponent.h>

#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/uniqueid.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/protobufmapencoder.h>

namespace dtEntityNet
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId DeadReckoningSenderComponent::TYPE(dtEntity::SID("DeadReckoningSender"));
   const dtEntity::StringId DeadReckoningSenderComponent::DeadReckoningAlgorithmId(dtEntity::SID("DeadReckoningAlgorithm"));

   DeadReckoningSenderComponent::DeadReckoningSenderComponent()
      : mDeadReckoningAlgorithm (
           dtEntity::DynamicStringProperty::SetValueCB(this, &DeadReckoningSenderComponent::SetDeadReckoningAlgorithmString),
           dtEntity::DynamicStringProperty::GetValueCB(this, &DeadReckoningSenderComponent::GetDeadReckoningAlgorithmString)
        )
      , mDeadReck(DeadReckoningAlgorithm::DISABLED)
      , mTransformComponent(NULL)
      , mDynamicsComponent(NULL)
      , mTimeLastSend(-1)
      , mUniqueId(dtEntity::CreateUniqueIdString())
      , mIsInScene(false)
   {
      Register(DeadReckoningAlgorithmId, &mDeadReckoningAlgorithm);
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderComponent::SetDeadReckoningAlgorithmString(const std::string& v)
   {
      mDeadReck = DeadReckoningAlgorithm::fromString(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string DeadReckoningSenderComponent::GetDeadReckoningAlgorithmString() const
   {
      return DeadReckoningAlgorithm::toString(mDeadReck);
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderComponent::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::e v)
   {
      mDeadReck = v;
   }

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningAlgorithm::e DeadReckoningSenderComponent::GetDeadReckoningAlgorithm() const
   {
      return mDeadReck;
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderComponent::FillMessage(UpdateTransformMessage& msg)
   {
      msg.SetUniqueId(mUniqueId);
      msg.SetPosition(mLastPosition);
      msg.SetOrientation(mLastOrientation);
      msg.SetVelocity(mLastVelocity);
      msg.SetAngularVelocity(mLastAngularVelocity);
      msg.SetDeadReckoning(GetDeadReckoningAlgorithm());
      msg.SetSimTime(mTimeLastSend);
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId DeadReckoningSenderSystem::TYPE(dtEntity::SID("DeadReckoningSender"));
   const dtEntity::StringId DeadReckoningSenderSystem::MaxUpdateIntervalId(dtEntity::SID("MaxUpdateInterval"));
   const dtEntity::StringId DeadReckoningSenderSystem::MinUpdateIntervalId(dtEntity::SID("MinUpdateInterval"));
   const dtEntity::StringId DeadReckoningSenderSystem::MaxPositionDeviationId(dtEntity::SID("MaxPositionDeviation"));
   const dtEntity::StringId DeadReckoningSenderSystem::MaxOrientationDeviationId(dtEntity::SID("MaxOrientationDeviation"));

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningSenderSystem::DeadReckoningSenderSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
      mMinUpdateInterval.Set(0.1f);
      mMaxUpdateInterval.Set(10.0f);
      mMaxPositionDeviation.Set(0.01f);
      mMaxOrientationDeviation.Set(0.01f);

      Register(MaxUpdateIntervalId, &mMaxUpdateInterval);
      Register(MinUpdateIntervalId, &mMinUpdateInterval);
      Register(MaxPositionDeviationId, &mMaxPositionDeviation);
      Register(MaxOrientationDeviationId, &mMaxOrientationDeviation);

      mTickFunctor = dtEntity::MessageFunctor(this, &DeadReckoningSenderSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "DeadReckoningSenderSystem::Tick");

      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &DeadReckoningSenderSystem::OnAddedToScene);
      em.RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "DeadReckoningSenderSystem::OnAddedToScene");

      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &DeadReckoningSenderSystem::OnRemovedFromScene);
      em.RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "DeadReckoningSenderSystem::OnRemovedFromScene");

   }

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningSenderSystem::~DeadReckoningSenderSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderSystem::Tick(const dtEntity::Message& m)
   {

      if(mComponents.empty())
      {
         return;
      }

      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      double simtime = msg.GetSimulationTime();

      osg::Vec3d newpos;
      osg::Vec3 newori;

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         DeadReckoningSenderComponent* comp = i->second;

         if(comp->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::DISABLED ||
            comp->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC)
         {
            continue;
         }

         if(comp->mTransformComponent == NULL)
         {
            bool success = GetEntityManager().GetComponent(id, comp->mTransformComponent, true);
            if(!success)
            {
               LOG_ERROR("NetworSender Component expects a Transform Component!");
               continue;
            }
         }

         const osg::Vec3d currentTrans = comp->mTransformComponent->GetTranslation();
         const osg::Vec3 currentAtt = QuatToEuler(comp->mTransformComponent->GetRotation());

         bool resend = false;


         if(simtime < comp->mTimeLastSend + GetMinUpdateInterval())
         {
            // don't resend if not at least MinUpdateInterval seconds have passed since last send
            resend = false;
         }
         else if(simtime > comp->mTimeLastSend + GetMaxUpdateInterval())
         {
            // always resend when no position was sent for MaxUpdateInterval seconds
            resend = true;
         }
         else
         {
            // resend if dead reckoned position significantly deviates from actual position

            switch(comp->GetDeadReckoningAlgorithm())
            {
            case DeadReckoningAlgorithm::FPW:
               CalculateDRFPW(comp->mLastPosition,
                              comp->mLastOrientation,
                              comp->mLastVelocity,
                              comp->mLastAngularVelocity,
                              simtime - comp->mTimeLastSend,
                              newpos,
                              newori);
               break;
            default: assert(false);
            }


            float dist = (newpos - currentTrans).length();
            osg::Vec3 angdiff = (newori - currentAtt);
            if(dist > GetMaxPositionDeviation() ||
                  angdiff[0] > GetMaxOrientationDeviation() ||
                  angdiff[1] > GetMaxOrientationDeviation() ||
                  angdiff[2] > GetMaxOrientationDeviation()
                  )
            {
               resend = true;
            }
         }

         if(resend)
         {
            if(comp->mDynamicsComponent == NULL)
            {
               bool success = GetEntityManager().GetComponent(id, comp->mDynamicsComponent, true);
               if(!success)
               {
                  LOG_ERROR("NetworSender Component expects a Dynamic Component!");
                  continue;
               }
            }

            comp->mLastPosition = currentTrans;
            comp->mLastOrientation = currentAtt;
            comp->mLastVelocity = comp->mDynamicsComponent->GetVelocity();
            comp->mLastAngularVelocity = QuatToEuler(comp->mDynamicsComponent->GetAngularVelocity());
            comp->mTimeLastSend = simtime;

            ENetSystem* es;
            GetEntityManager().GetES(es);
            if(es->IsConnected())
            {
               UpdateTransformMessage msg;
               comp->FillMessage(msg);
               es->SendToClients(msg);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderSystem::OnAddedToScene(const dtEntity::Message& m)
   {
      const dtEntity::EntityAddedToSceneMessage&  msg =
            static_cast<const dtEntity::EntityAddedToSceneMessage&>(m);


      DeadReckoningSenderComponent* comp;
      if(GetEntityManager().GetComponent(msg.GetAboutEntityId(), comp))
      {
         comp->mIsInScene = true;

         ENetSystem* enetsys;
         if(!GetEntityManager().GetES(enetsys))
         {
            return;
         }
         JoinMessage msg;
         msg.SetUniqueId(comp->GetUniqueId());
         msg.SetEntityType(comp->GetEntityType());
         enetsys->SendToClients(msg);

         UpdateTransformMessage transmsg;
         comp->FillMessage(transmsg);
         enetsys->SendToClients(transmsg);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderSystem::OnRemovedFromScene(const dtEntity::Message& m)
   {
      const dtEntity::EntityRemovedFromSceneMessage&  msg =
            static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(m);

      DeadReckoningSenderComponent* comp;
      if(GetEntityManager().GetComponent(msg.GetAboutEntityId(), comp))
      {
         comp->mIsInScene = false;

         ENetSystem* enetsys;
         if(!GetEntityManager().GetES(enetsys))
         {
            return;
         }
         ResignMessage msg;
         msg.SetUniqueId(comp->GetUniqueId());
         enetsys->SendToClients(msg);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void DeadReckoningSenderSystem::SendEntitiesToClient(dtEntity::MessagePump& pump)
   {
      JoinMessage joinmsg;

      for(ComponentStore::const_iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         DeadReckoningSenderComponent* comp = i->second;
         if(comp->mIsInScene)
         {
            joinmsg.SetUniqueId(comp->GetUniqueId());
            joinmsg.SetEntityType(comp->GetEntityType());
            pump.EmitMessage(joinmsg);
         }
      }

      UpdateTransformMessage transmsg;
      for(ComponentStore::const_iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         DeadReckoningSenderComponent* comp = i->second;
         if(comp->mIsInScene)
         {
            comp->FillMessage(transmsg);
         }
         pump.EmitMessage(transmsg);
      }
   }
}
