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


#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/transformcomponent.h>
#include <dtEntityNet/deadreckoning.h>
#include <dtEntityNet/export.h>

struct _ENetHost;
struct _ENetPeer;

namespace dtEntityNet
{

   class NetworkReceiverSystem;
   class NetworkSenderSystem;
   class UpdateTransformMessage;

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetworkSenderComponent
         : public dtEntity::Component
   {
      friend class NetworkSenderSystem;

   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId DeadReckoningAlgorithmId;

      NetworkSenderComponent();

      dtEntity::ComponentType GetType() const { return TYPE; }

      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      {
         return (id == TYPE);
      }

      void SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::e v);
      DeadReckoningAlgorithm::e GetDeadReckoningAlgorithm() const;

      void SetDeadReckoningAlgorithmString(const std::string& v);
      std::string GetDeadReckoningAlgorithmString() const;

      void SetEntityType(const std::string& v) { mEntityType = v; }
      std::string GetEntityType() const { return mEntityType; }

      void SetUniqueId(const std::string& v) { mUniqueId = v; }
      std::string GetUniqueId() const { return mUniqueId; }

      void FillMessage(UpdateTransformMessage& msg);

   private:
      dtEntity::DynamicStringProperty mDeadReckoningAlgorithm;
      DeadReckoningAlgorithm::e mDeadReck;
      dtEntity::TransformComponent* mTransformComponent;
      dtEntity::DynamicsComponent* mDynamicsComponent;
      double mTimeLastSend;
      osg::Vec3d mLastPosition;
      osg::Vec3 mLastOrientation;
      osg::Vec3 mLastVelocity;
      osg::Vec3 mLastAngularVelocity;
      std::string mEntityType;
      std::string mUniqueId;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetworkSenderSystem
      : public dtEntity::DefaultEntitySystem<NetworkSenderComponent>
   {
      typedef dtEntity::DefaultEntitySystem<NetworkSenderComponent> BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId MinUpdateIntervalId;
      static const dtEntity::StringId MaxUpdateIntervalId;
      static const dtEntity::StringId MaxPositionDeviationId;
      static const dtEntity::StringId MaxOrientationDeviationId;

      NetworkSenderSystem(dtEntity::EntityManager& em);
      ~NetworkSenderSystem();

      dtEntity::ComponentType GetComponentType() const { return TYPE; }  

      float GetMinUpdateInterval() const { return mMinUpdateInterval.Get(); }
      void SetMinUpdateInterval(float v) { mMinUpdateInterval.Set(v); }

      float GetMaxUpdateInterval() const { return mMaxUpdateInterval.Get(); }
      void SetMaxUpdateInterval(float v) { mMaxUpdateInterval.Set(v); }

      float GetMaxPositionDeviation() const { return mMaxPositionDeviation.Get(); }
      void SetMaxPositionDeviation(float v) { mMaxPositionDeviation.Set(v); }

      float GetMaxOrientationDeviation() const { return mMaxOrientationDeviation.Get(); }
      void SetMaxOrientationDeviation(float v) { mMaxOrientationDeviation.Set(v); }

      void SendEntitiesToClient(_ENetPeer*, NetworkReceiverSystem* rcvrsys);

   private:

      void Tick(const dtEntity::Message& m);

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::FloatProperty mMinUpdateInterval;
      dtEntity::FloatProperty mMaxUpdateInterval;
      dtEntity::FloatProperty mMaxPositionDeviation;
      dtEntity::FloatProperty mMaxOrientationDeviation;
      NetworkReceiverSystem* mNetworkReceiverSystem;
   };
}
