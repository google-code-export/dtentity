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

#include <dtEntityENet/netcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/stringid.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/messagefactory.h>
#include <enet/enet.h>
#include <sstream>
#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>

namespace dtEntityENet
{

   const dtEntity::StringId NetComponent::TYPE(dtEntity::SID("Net"));   
   
   ////////////////////////////////////////////////////////////////////////////
   NetComponent::NetComponent()
   {
   }
    
   ////////////////////////////////////////////////////////////////////////////
   NetComponent::~NetComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   NetSystem::NetSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {      
   }

   ////////////////////////////////////////////////////////////////////////////
   NetSystem::~NetSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetSystem::EncodeProperty(QDataStream& out, const dtEntity::Property* prop)
   {
      /*
      ARRAY,
     
      COMPONENT,      
      GROUP,
      MATRIX,
      QUAT,
      STRING,
      STRINGID,
      VEC2,
      VEC3,
      VEC4
      */
      using namespace dtEntity;
      switch(prop->GetType())
      {
      case DataType::FLOAT:  { out << static_cast<const FloatProperty*        >(prop)->Get(); break; }
      case DataType::DOUBLE: { out << static_cast<const DoubleProperty*       >(prop)->Get(); break; }
      case DataType::BOOL:   { out << static_cast<const BoolProperty*         >(prop)->Get(); break; }
      case DataType::INT:    { out << (qint32)static_cast<const IntProperty*  >(prop)->Get(); break; }
      case DataType::UINT:   { out << (quint32)static_cast<const UIntProperty*>(prop)->Get(); break; }       
      default:{}
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetSystem::DecodeProperty(QDataStream& ins, dtEntity::Property* prop)
   {
      /*
      ARRAY,
      COMPONENT,      
      GROUP,
      MATRIX,
      QUAT,
      STRING,
      STRINGID,
      VEC2,
      VEC3,
      VEC4
      */
      using namespace dtEntity;
      switch(prop->GetType())
      {
      case DataType::FLOAT:  { float v;  ins >> v;  static_cast<FloatProperty* >(prop)->Set(v); break;}
      case DataType::DOUBLE: { double v; ins >> v;  static_cast<DoubleProperty*>(prop)->Set(v); break;}
      case DataType::BOOL:   { bool v; ins >> v;    static_cast<BoolProperty*  >(prop)->Set(v); break;}
      case DataType::INT:    { qint32 v; ins >> v;  static_cast<IntProperty*   >(prop)->Set(v); break;}
      case DataType::UINT:   { quint32 v; ins >> v; static_cast<IntProperty*   >(prop)->Set(v); break;}
      default:{}
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   _ENetPacket* NetSystem::EncodeMessage(const dtEntity::Message& msg)
   {
      QByteArray arr;
      QDataStream out(&arr, QIODevice::WriteOnly);
      quint32 messageType =  (quint32)msg.GetType();
      out << messageType;
      
      dtEntity::PropertyContainer::ConstPropertyMap props;
      msg.GetProperties(props);
      dtEntity::PropertyContainer::ConstPropertyMap::iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         EncodeProperty(out, i->second);
      }

      ENetPacket* packet = enet_packet_create(arr.constData(), arr.size(), ENET_PACKET_FLAG_RELIABLE);
      return packet;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Message* NetSystem::DecodeMessage(const _ENetPacket& packet)
   {
      QByteArray arr((const char*)packet.data, packet.dataLength);
      QDataStream ins(&arr, QIODevice::ReadOnly);
      quint32 mType;
      ins >> mType;
      dtEntity::MessageType messageType = mType;
      dtEntity::Message* message;
	  
	  dtEntity::MapSystem* mapsys;
	  GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapsys);
	  
      bool success = mapsys->GetMessageFactory().CreateMessage((dtEntity::MessageType)messageType, message);
      if(!success)
      {
         LOG_ERROR("Error decoding network packet: Unknown message type encountered");
         return NULL;
      }
      
      dtEntity::PropertyContainer::PropertyMap props;
      message->GetProperties(props);
      dtEntity::PropertyContainer::PropertyMap::iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         DecodeProperty(ins, i->second);
      }
      
      return message;
   }

   ////////////////////////////////////////////////////////////////////////////
   void NetSystem::SendMessageToNetwork(const dtEntity::Message& msg)
   {
      ENetPacket* packet = EncodeMessage(msg);
      DecodeMessage(*packet);
      /* Extend the packet so and append the string "foo", so it now */
      /* contains "packetfoo\0"                                      */
      enet_packet_resize(packet, strlen ("packetfoo") + 1);
      //strcpy (&packet-> data [strlen ("packet")], "foo");

      /* Send the packet to the peer over channel id 0. */
      /* One could also broadcast the packet by         */
      /* enet_host_broadcast (host, 0, packet);         */
     // enet_peer_send (peer, 0, packet);
     // enet_host_flush (host);

   }

   ////////////////////////////////////////////////////////////////////////////
   void NetSystem::SendToNetwork()
   {
      while(!mMessageQueue.Empty())
      {
         const dtEntity::Message* entry = mMessageQueue.Pop();
         SendMessageToNetwork(*entry);
         delete entry;
      }
   }
}
