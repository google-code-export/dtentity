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
#include <dtEntity/messagepump.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntityNet/deadreckoning.h>
#include <dtEntityNet/export.h>
#include <osg/Timer>

namespace dtEntity
{
   class ApplicationSystem;
   class MapSystem;
   class TransformComponent;
   class DynamicsComponent;
}

namespace dtEntityNet
{
   class UpdateTransformMessage;
   class NetworkReceiverSystem;

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetworkReceiverComponent
         : public dtEntity::Component
   {
      friend class NetworkReceiverSystem;

   public:
      static const dtEntity::ComponentType TYPE;

      NetworkReceiverComponent();

      dtEntity::ComponentType GetType() const { return TYPE; }

      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      {
         return (id == TYPE);
      }

   private:
      dtEntity::TransformComponent* mTransformComponent;
      dtEntity::DynamicsComponent* mDynamicsComponent;
      double mTimeLastReceive;
      osg::Vec3d mPosition;
      osg::Vec3 mOrientation;
      osg::Vec3 mVelocity;
      osg::Vec3 mAngularVelocity;
      std::string mEntityType;
      std::string mUniqueId;
      DeadReckoningAlgorithm::e mDeadRecAlg;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT NetworkReceiverSystem
      : public dtEntity::DefaultEntitySystem<NetworkReceiverComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<NetworkReceiverComponent> BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      NetworkReceiverSystem(dtEntity::EntityManager& em);
      ~NetworkReceiverSystem();

      void OnAddedToEntityManager(dtEntity::EntityManager&);
      void OnRemovedFromEntityManager(dtEntity::EntityManager&);

      void OnUpdateTransform(const dtEntity::Message& msg);
      void OnJoin(const dtEntity::Message& msg);
      void OnResign(const dtEntity::Message& msg);

   private:

      dtEntity::Property* ScriptConnect(const dtEntity::PropertyArgs& args);
      void Tick(const dtEntity::Message& m);


      dtEntity::ApplicationSystem* mApplicationSystem;
      dtEntity::MapSystem* mMapSystem;
      dtEntity::MessageFunctor mTickFunctor;

   };
}
