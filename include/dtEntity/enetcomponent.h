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


#include <dtEntity/export.h>
#include <dtEntity/component.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/messagepump.h>

struct _ENetHost;
struct _ENetPeer;

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT ENetSystem
      : public EntitySystem
   {
      typedef EntitySystem BaseClass;

   public:

      static const ComponentType TYPE;

      ENetSystem(EntityManager& em);
      ~ENetSystem();

      ComponentType GetComponentType() const { return TYPE; }

      MessagePump& GetIncomingMessagePump() { return mIncoming; }

      bool InitializeServer(unsigned int port);
      bool Connect(const std::string& address, unsigned int port);

      void Disconnect();

      void Broadcast(const Message&);
      void SendToPeer(const Message&);

      void Flush();

   private:

      void Tick(const Message& m);

      MessagePump mIncoming;
      MessageFunctor mTickFunctor;
      _ENetHost* mHost;
      _ENetPeer* mPeer;
   };
}
