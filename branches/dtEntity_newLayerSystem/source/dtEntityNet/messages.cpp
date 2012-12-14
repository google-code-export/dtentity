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

#include <dtEntityNet/messages.h>

#include <dtEntity/messagefactory.h>
#include <dtEntity/entity.h>
#include <dtEntity/stringid.h>


namespace dtEntityNet
{
   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void RegisterMessageTypes(dtEntity::MessageFactory& em)
   {
      em.RegisterMessageType<JoinMessage>(JoinMessage::TYPE);
      em.RegisterMessageType<NetConnectedMessage>(NetConnectedMessage::TYPE);
      em.RegisterMessageType<NetDisconnectedMessage>(NetDisconnectedMessage::TYPE);
      em.RegisterMessageType<ResignMessage>(ResignMessage::TYPE);
      em.RegisterMessageType<UpdateTransformMessage>(UpdateTransformMessage::TYPE);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::MessageType JoinMessage::TYPE(dtEntity::SID("JoinMessage"));
   const dtEntity::StringId JoinMessage::EntityTypeId(dtEntity::SID("EntityType"));
   const dtEntity::StringId JoinMessage::UniqueIdId(dtEntity::SID("UniqueId"));

   JoinMessage::JoinMessage()
      : Message(TYPE)
   {
      Register(EntityTypeId, &mEntityType);
      Register(UniqueIdId, &mUniqueId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::MessageType NetConnectedMessage::TYPE(dtEntity::SID("NetConnectedMessage"));

   NetConnectedMessage::NetConnectedMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::MessageType NetDisconnectedMessage::TYPE(dtEntity::SID("NetDisconnectedMessage"));

   NetDisconnectedMessage::NetDisconnectedMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::MessageType ResignMessage::TYPE(dtEntity::SID("ResignMessage"));
   const dtEntity::StringId ResignMessage::UniqueIdId(dtEntity::SID("UniqueId"));

   ResignMessage::ResignMessage()
      : Message(TYPE)
   {
      Register(UniqueIdId, &mUniqueId);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const dtEntity::MessageType UpdateTransformMessage::TYPE(dtEntity::SID("UpdateTransformMessage"));
   const dtEntity::StringId UpdateTransformMessage::DeadReckoningAlgorithmId(dtEntity::SID("DeadReckoningAlgorithm"));
   const dtEntity::StringId UpdateTransformMessage::PositionId(dtEntity::SID("Position"));
   const dtEntity::StringId UpdateTransformMessage::VelocityId(dtEntity::SID("Velocity"));
   const dtEntity::StringId UpdateTransformMessage::OrientationId(dtEntity::SID("Orientation"));
   const dtEntity::StringId UpdateTransformMessage::AngularVelocityId(dtEntity::SID("AngularVelocity"));
   const dtEntity::StringId UpdateTransformMessage::SimTimeId(dtEntity::SID("SimTime"));
   const dtEntity::StringId UpdateTransformMessage::UniqueIdId(dtEntity::SID("UniqueId"));

   UpdateTransformMessage::UpdateTransformMessage()
      : dtEntity::Message(TYPE)
   {
      Register(DeadReckoningAlgorithmId, &mDeadReckoningAlgorithm);
      Register(PositionId, &mPosition);
      Register(VelocityId, &mVelocity);
      Register(OrientationId, &mOrientation);
      Register(AngularVelocityId, &mAngularVelocity);
      Register(SimTimeId, &mSimTime);
      Register(UniqueIdId, &mUniqueId);
   }
}

