#pragma once

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

#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntityNet/export.h>
#include <dtEntityNet/deadreckoning.h>

namespace dtEntity
{
   class MessageFactory;
}

namespace dtEntityNet
{

   void DTENTITY_NET_EXPORT RegisterMessageTypes(dtEntity::MessageFactory&);

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT JoinMessage
      : public dtEntity::Message
   {
   public:

      // type identifier of this message class
      static const dtEntity::MessageType TYPE;

      // string identifiers for parameter names
      static const dtEntity::StringId EntityTypeId;
      static const dtEntity::StringId UniqueIdId;

      JoinMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<JoinMessage>(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

      void SetEntityType(const std::string& v) { mEntityType.Set(v); }
      std::string GetEntityType() const { return mEntityType.Get(); }

   private:

      dtEntity::StringProperty mEntityType;
      dtEntity::StringProperty mUniqueId;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetConnectedMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;

      NetConnectedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<NetConnectedMessage>(); }

   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetDisconnectedMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;

      NetDisconnectedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<NetDisconnectedMessage>(); }

   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT ResignMessage
      : public dtEntity::Message
   {
   public:

      // type identifier of this message class
      static const dtEntity::MessageType TYPE;

      static const dtEntity::StringId UniqueIdId;

      ResignMessage();

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }


      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<ResignMessage>(); }

   private:

      dtEntity::StringProperty mUniqueId;

   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * send this message to cause a script to be loaded
    */
   class DTENTITY_NET_EXPORT UpdateTransformMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId DeadReckoningAlgorithmId;
      static const dtEntity::StringId PositionId;
      static const dtEntity::StringId VelocityId;
      static const dtEntity::StringId OrientationId;
      static const dtEntity::StringId AngularVelocityId;
      static const dtEntity::StringId SimTimeId;
      static const dtEntity::StringId UniqueIdId;

      UpdateTransformMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<UpdateTransformMessage>(); }

      void SetDeadReckoning(DeadReckoningAlgorithm::e v)
      {
         mDeadReckoningAlgorithm.Set(v);
      }

      DeadReckoningAlgorithm::e GetDeadReckoning() const
      {
         return static_cast<DeadReckoningAlgorithm::e>(mDeadReckoningAlgorithm.Get());
      }

      void SetPosition(const osg::Vec3d& v) { mPosition.Set(v); }
      const osg::Vec3d& GetPosition() const { return mPosition.GetAsVec3d(); }

      void SetVelocity(const osg::Vec3f& v) { mVelocity.Set(v); }
      const osg::Vec3f& GetVelocity() const { return mVelocity.GetAsVec3(); }

      void SetOrientation(const osg::Vec3f& v) { mOrientation.Set(v); }
      const osg::Vec3f& GetOrientation() const { return mOrientation.GetAsVec3(); }

      void SetAngularVelocity(const osg::Vec3f& v) { mAngularVelocity.Set(v); }
      const osg::Vec3f& GetAngularVelocity() const { return mAngularVelocity.GetAsVec3(); }

      void SetSimTime(double v) { mSimTime.Set(v); }
      double GetSimTime() const { return mSimTime.Get(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

   private:

      dtEntity::UIntProperty mDeadReckoningAlgorithm;
      dtEntity::Vec3dProperty mPosition;
      dtEntity::Vec3Property mVelocity;
      dtEntity::Vec3Property mOrientation;
      dtEntity::Vec3Property mAngularVelocity;
      dtEntity::DoubleProperty mSimTime;
      dtEntity::StringProperty mUniqueId;
   };
}
