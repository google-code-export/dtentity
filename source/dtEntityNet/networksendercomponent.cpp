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

#include <dtEntityNet/networksendercomponent.h>

#include <dtEntityNet/messages.h>
#include <dtEntityNet/networkreceivercomponent.h>

#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/uniqueid.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/protobufmapencoder.h>
#include <enet/enet.h>

namespace dtEntityNet
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NetworkSenderComponent::TYPE(dtEntity::SID("NetworkSender"));
   const dtEntity::StringId NetworkSenderComponent::DeadReckoningAlgorithmId(dtEntity::SID("DeadReckoningAlgorithm"));

   NetworkSenderComponent::NetworkSenderComponent()
      : mDeadReckoningAlgorithm (
           dtEntity::DynamicStringProperty::SetValueCB(this, &NetworkSenderComponent::SetDeadReckoningAlgorithmString),
           dtEntity::DynamicStringProperty::GetValueCB(this, &NetworkSenderComponent::GetDeadReckoningAlgorithmString)
        )
      , mDeadReck(DeadReckoningAlgorithm::DISABLED)
      , mTransformComponent(NULL)
      , mDynamicsComponent(NULL)
      , mTimeLastSend(-1)
      , mUniqueId(dtEntity::CreateUniqueIdString())
   {
      Register(DeadReckoningAlgorithmId, &mDeadReckoningAlgorithm);
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkSenderComponent::SetDeadReckoningAlgorithmString(const std::string& v)
   {
      mDeadReck = DeadReckoningAlgorithm::fromString(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string NetworkSenderComponent::GetDeadReckoningAlgorithmString() const
   {
      return DeadReckoningAlgorithm::toString(mDeadReck);
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkSenderComponent::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::e v)
   {
      mDeadReck = v;
   }

   ////////////////////////////////////////////////////////////////////////////
   DeadReckoningAlgorithm::e NetworkSenderComponent::GetDeadReckoningAlgorithm() const
   {
      return mDeadReck;
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkSenderComponent::FillMessage(UpdateTransformMessage& msg)
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
   const dtEntity::StringId NetworkSenderSystem::TYPE(dtEntity::SID("NetworkSender"));
   const dtEntity::StringId NetworkSenderSystem::MinUpdateIntervalId(dtEntity::SID("MinUpdateInterval"));
   const dtEntity::StringId NetworkSenderSystem::MaxPositionDeviationId(dtEntity::SID("MaxPositionDeviation"));
   const dtEntity::StringId NetworkSenderSystem::MaxOrientationDeviationId(dtEntity::SID("MaxOrientationDeviation"));

   ////////////////////////////////////////////////////////////////////////////
   NetworkSenderSystem::NetworkSenderSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mNetworkReceiverSystem(NULL)
   {
      Register(MinUpdateIntervalId, &mMinUpdateInterval);
      Register(MaxPositionDeviationId, &mMaxPositionDeviation);
      Register(MaxOrientationDeviationId, &mMaxOrientationDeviation);

      mMinUpdateInterval.Set(10.0f);
      mMaxPositionDeviation.Set(0.01);
      mMaxOrientationDeviation.Set(0.01);

      mTickFunctor = dtEntity::MessageFunctor(this, &NetworkSenderSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "NetworkSenderSystem::Tick");
   }

   ////////////////////////////////////////////////////////////////////////////
   NetworkSenderSystem::~NetworkSenderSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkSenderSystem::Tick(const dtEntity::Message& m)
   {

      if(mComponents.empty())
      {
         return;
      }

      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);

      double simtime = msg.GetSimulationTime();
      double sendTime = simtime - GetMinUpdateInterval();

      osg::Vec3d newpos;
      osg::Vec3 newori;

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         NetworkSenderComponent* comp = i->second;

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
         if(comp->mTimeLastSend < sendTime)
         {
            resend = true;
         }
         else
         {
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

            if(mNetworkReceiverSystem == NULL)
            {
               bool success = GetEntityManager().GetES(mNetworkReceiverSystem);
               assert(success);
            }

            comp->mLastPosition = currentTrans;
            comp->mLastOrientation = currentAtt;
            comp->mLastVelocity = comp->mDynamicsComponent->GetVelocity();
            comp->mLastAngularVelocity = QuatToEuler(comp->mDynamicsComponent->GetAngularVelocity());
            comp->mTimeLastSend = simtime;


            if(mNetworkReceiverSystem->IsConnected())
            {
               UpdateTransformMessage msg;
               comp->FillMessage(msg);
               mNetworkReceiverSystem->SendToClients(msg);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkSenderSystem::SendEntitiesToClient(_ENetPeer* peer, NetworkReceiverSystem* rcvrsys)
   {
      JoinMessage joinmsg;

      for(ComponentStore::const_iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         NetworkSenderComponent* comp = i->second;
         joinmsg.SetUniqueId(comp->GetUniqueId());
         joinmsg.SetEntityType(comp->GetEntityType());
         rcvrsys->SendToPeer(joinmsg, peer);
      }

      UpdateTransformMessage transmsg;
      for(ComponentStore::const_iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->FillMessage(transmsg);
         rcvrsys->SendToPeer(transmsg, peer);
      }
   }
}
