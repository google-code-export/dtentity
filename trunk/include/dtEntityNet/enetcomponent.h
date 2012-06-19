#pragma once

/* -*-c++-*-
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


#include <dtEntity/entitysystem.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntityNet/export.h>

struct _ENetHost;
struct _ENetPeer;


namespace dtEntityNet
{

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT ENetSystem
      : public dtEntity::EntitySystem
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::EntitySystem BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      ENetSystem(dtEntity::EntityManager& em);
      ~ENetSystem();

      virtual void OnAddedToEntityManager(dtEntity::EntityManager &em);

      dtEntity::ComponentType GetComponentType() const { return TYPE; }

      dtEntity::MessagePump& GetIncomingMessagePump() { return mIncoming; }

      bool InitializeServer(unsigned int port);
      bool Connect(const std::string& address, unsigned int port);

      bool IsConnected() const;

      void Disconnect();

      void SendToClients(const dtEntity::Message&);
      void SendToServer(const dtEntity::Message&);
      void SendToPeer(const dtEntity::Message&, _ENetPeer* peer);

      void Flush();

      void OnUpdateTransform(const dtEntity::Message& msg);
      void OnJoin(const dtEntity::Message& msg);
      void OnResign(const dtEntity::Message& msg);

   private:

      dtEntity::Property* ScriptConnect(const dtEntity::PropertyArgs& args);
      void Tick(const dtEntity::Message& m);

      dtEntity::MessagePump mIncoming;
      dtEntity::MessageFunctor mTickFunctor;
      _ENetHost* mHost;
      _ENetPeer* mPeer;
      typedef std::vector<_ENetPeer*> Clients;
      Clients mConnectedClients;

   };
}
