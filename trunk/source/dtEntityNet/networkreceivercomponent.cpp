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

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/protobufmapencoder.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/uniqueid.h>
#include <enet/enet.h>

namespace dtEntityNet
{
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NetworkReceiverComponent::TYPE(dtEntity::SID("NetworkReceiver"));


   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NetworkReceiverSystem::TYPE(dtEntity::SID("NetworkReceiver"));

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   NetworkReceiverSystem::NetworkReceiverSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mHost(NULL)
      , mPeer(NULL)
   {
      if(enet_initialize () != 0)
      {
         LOG_ERROR("An error occurred while initializing ENet");
      }

      mTickFunctor = dtEntity::MessageFunctor(this, &NetworkReceiverSystem::Tick);

      AddScriptedMethod("connect", dtEntity::ScriptMethodFunctor(this, &NetworkReceiverSystem::ScriptConnect));

      mIncoming.RegisterForMessages(UpdateTransformMessage::TYPE,
         dtEntity::MessageFunctor(this, &NetworkReceiverSystem::OnUpdateTransform),
                                   dtEntity::FilterOptions::ORDER_LATE, "UpdateTransformMessage::OnUpdateTransform");

      bool success = em.GetES(mMapSystem);
      assert(success);

      success = em.GetES(mApplicationSystem);
            assert(success);
   }

   ////////////////////////////////////////////////////////////////////////////
   NetworkReceiverSystem::~NetworkReceiverSystem()
   {
      Disconnect();
      enet_deinitialize();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool NetworkReceiverSystem::InitializeServer(unsigned int port)
   {
      if(mHost)
      {
         Disconnect();
      }
      ENetAddress address;

      /* Bind the server to the default localhost.     */
      /* A specific host address can be specified by   */
      /* enet_address_set_host (& address, "x.x.x.x"); */

      address.host = ENET_HOST_ANY;
      address.port = port;

      mHost = enet_host_create(&address /* the address to bind the server host to */,
                                   32      /* allow up to 32 clients and/or outgoing connections */,
                                    2      /* allow up to 2 channels to be used, 0 and 1 */,
                                    0      /* assume any amount of incoming bandwidth */,
                                    0      /* assume any amount of outgoing bandwidth */);
      if(mHost == NULL)
      {
         LOG_ERROR("An error occurred while trying to create an ENet server host");
         return false;
      }

      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_LATE, "NetworkReceiverSystem::Tick");
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool NetworkReceiverSystem::Connect(const std::string& addressstr, unsigned int port)
   {
      if(mHost)
      {
         Disconnect();
      }

      mHost = enet_host_create (NULL /* create a client host */,
                      1 /* only allow 1 outgoing connection */,
                      2 /* allow up 2 channels to be used, 0 and 1 */,
                      0,
                      0);

      if (mHost == NULL)
      {
         LOG_ERROR("An error occurred while trying to create an ENet client host.");
         return false;
      }


      ENetAddress address;
      enet_address_set_host(&address, addressstr.c_str());
      address.port = port;

      /* Initiate the connection, allocating the two channels 0 and 1. */
      mPeer = enet_host_connect(mHost, & address, 2, 0);

      if(mPeer == NULL)
      {
        LOG_ERROR("No available peers for initiating an ENet connection.");
        return false;
      }

      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_LATE, "NetworkReceiverSystem::Tick");
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool NetworkReceiverSystem::IsConnected() const
   {
      return mPeer != NULL || !mConnectedClients.empty();
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::Disconnect()
   {
      if(mHost)
      {
         GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
         enet_host_destroy(mHost);
         mHost = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::Tick(const dtEntity::Message& m)
   {
      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);
      ENetEvent event;
      while(enet_host_service (mHost, &event, 0) > 0)
      {
         switch (event.type)
         {
         case ENET_EVENT_TYPE_CONNECT:
         {
            LOG_ALWAYS("A new client connected from " << event.peer->address.host << ":" << event.peer->address.port);

            std::string uniqueId = dtEntity::CreateUniqueIdString();
            /* Store any relevant client information here. */
            event.peer->data = (void*)uniqueId.c_str();

            mConnectedClients.push_back(event.peer);

            NetworkSenderSystem* sender;
            GetEntityManager().GetES(sender);
            sender->SendEntitiesToClient(event.peer, this);

            break;
         }
         case ENET_EVENT_TYPE_RECEIVE:
         {
            LOG_ALWAYS("A packet of length " << event.packet->dataLength << " containing " <<
                    event.packet->data << " was received from " <<
                    event.peer->data << " on channel " << (int)event.channelID << "\n");

            //std::istringstream is(reinterpret_cast<char*>(event.packet->data), std::ios_base::in | std::ios_base::binary);

            std::stringstream ss;
            ss.rdbuf()->sputn(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);

            dtEntity::Message* msg = dtEntity::ProtoBufMapEncoder::DecodeMessage(ss);
            if(msg == NULL)
            {
               LOG_ERROR("Could not decode message!");
            }
            else
            {
               mIncoming.EmitMessage(*msg);
            }
            delete msg;

            enet_packet_destroy(event.packet);

            break;
         }
         case ENET_EVENT_TYPE_DISCONNECT:
         {
            for(Clients::iterator i = mConnectedClients.begin(); i != mConnectedClients.end(); ++i)
            {
               if(*i == event.peer)
               {
                  mConnectedClients.erase(i);
               }
            }

            LOG_ALWAYS ("" << event.peer->data << " disconected");

            /* Reset the peer's client information. */
            event.peer -> data = NULL;
            break;
         }
         case ENET_EVENT_TYPE_NONE: break;
         }

      }

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
               LOG_ERROR("NetworSender Component expects a Dynamics Component!");
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
            default: assert(false);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::SendToClients(const dtEntity::Message& msg)
   {

      std::stringstream buf(std::ios::binary | std::ios::out);
      bool success = dtEntity::ProtoBufMapEncoder::EncodeMessage(msg, buf);
      if(success)
      {
         const std::string byteArray = buf.str();
         ENetPacket* packet = enet_packet_create (byteArray.c_str(), byteArray.size(),
                                              ENET_PACKET_FLAG_RELIABLE);
         //enet_host_broadcast(mHost, 0, packet);
         for(Clients::iterator i = mConnectedClients.begin(); i != mConnectedClients.end(); ++i)
         {
            enet_peer_send(*i, 0, packet);
         }
      }
      else
      {
         LOG_ERROR("Could not encode message!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::SendToServer(const dtEntity::Message& msg)
   {
      if(mPeer == NULL)
      {
         LOG_ERROR("Cannot send to peer, no connection!");
         return;
      }
      SendToPeer(msg, mPeer);

   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::SendToPeer(const dtEntity::Message& msg, _ENetPeer* peer)
   {

      std::stringstream buf(std::ios::binary | std::ios::out);
      bool success = dtEntity::ProtoBufMapEncoder::EncodeMessage(msg, buf);
      if(success)
      {
         const std::string byteArray = buf.str();
         ENetPacket* packet = enet_packet_create (byteArray.c_str(), byteArray.size(),
                                              ENET_PACKET_FLAG_RELIABLE);
         enet_peer_send(peer, 0, packet);
      }
      else
      {
         LOG_ERROR("Could not encode message!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::Flush()
   {
      if(mHost)
      {
         enet_host_flush(mHost);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetworkReceiverSystem::OnUpdateTransform(const dtEntity::Message& m)
   {
      const UpdateTransformMessage& msg = static_cast<const UpdateTransformMessage&>(m);
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


      comp->mTimeLastReceive = mApplicationSystem->GetSimulationTime();
      comp->mPosition = msg.GetPosition();
      comp->mOrientation = msg.GetOrientation();
      comp->mVelocity = msg.GetVelocity();
      comp->mAngularVelocity = msg.GetAngularVelocity();
      comp->mDeadRecAlg = msg.GetDeadReckoning();
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* NetworkReceiverSystem::ScriptConnect(const dtEntity::PropertyArgs& args)
   {
      return NULL;
   }
}
