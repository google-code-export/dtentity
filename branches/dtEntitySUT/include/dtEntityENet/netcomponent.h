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


#include <dtEntityENet/export.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/messagepump.h>

// Fwd declaration
struct _ENetPacket;
class QDataStream;

namespace dtEntityENet
{
   
   class DT_ENTITY_ENET_EXPORT NetComponent : public dtEntity::Component
   {
      friend class NetSystem;

   public:
      
      static const dtEntity::ComponentType TYPE;
      
      NetComponent();      
      virtual ~NetComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }
      
   private:
      
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_ENET_EXPORT NetSystem
      : public dtEntity::DefaultEntitySystem<NetComponent>
      , public dtEntity::MessagePump
   {
      typedef dtEntity::DefaultEntitySystem<NetComponent> BaseClass;

   public:
     
      NetSystem(dtEntity::EntityManager& em);
      ~NetSystem();

      void SendMessageToNetwork(const dtEntity::Message& msg);
      void SendToNetwork();
      _ENetPacket* EncodeMessage(const dtEntity::Message& msg);
      dtEntity::Message* DecodeMessage(const _ENetPacket& packet);

   private:
      void EncodeProperty(QDataStream& out, const dtEntity::Property* prop);
      void DecodeProperty(QDataStream& ins, dtEntity::Property* prop);
   };
}
