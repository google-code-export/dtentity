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
   class MapSystem;
   class TransformComponent;
   class DynamicsComponent;
}

namespace dtEntityNet
{
   class UpdateTransformMessage;
   class DeadReckoningReceiverSystem;

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT DeadReckoningReceiverComponent
         : public dtEntity::Component
   {
      friend class DeadReckoningReceiverSystem;

   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId UniqueIdId;

      DeadReckoningReceiverComponent();

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
      dtEntity::StringProperty mUniqueId;
      DeadReckoningAlgorithm::e mDeadRecAlg;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_NET_EXPORT DeadReckoningReceiverSystem
      : public dtEntity::DefaultEntitySystem<DeadReckoningReceiverComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<DeadReckoningReceiverComponent> BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId SpawnFromEntityTypeId;

      DeadReckoningReceiverSystem(dtEntity::EntityManager& em);
      ~DeadReckoningReceiverSystem();

      void OnAddedToEntityManager(dtEntity::EntityManager&);
      void OnRemovedFromEntityManager(dtEntity::EntityManager&);

      void OnUpdateTransform(const dtEntity::Message& msg);
      void OnJoin(const dtEntity::Message& msg);
      void OnResign(const dtEntity::Message& msg);

      /**
        * if set to true (default):
        * on receive JoinMessage: look for a spawner with the name of the entity type.
        * Spawn entity from that, add a dynamics component and set map
        * component unique id to passed unique id
        * If set to false: Do nothing, user has to handle JoinMessage, ResignMessage
        */
      void SetSpawnFromEntityType(bool v) { mSpawnFromEntityType.Set(v); }
      bool GetSpawnFromEntityType() const { return mSpawnFromEntityType.Get(); }

   private:

      dtEntity::Property* ScriptConnect(const dtEntity::PropertyArgs& args);
      void Tick(const dtEntity::Message& m);

      dtEntity::MapSystem* mMapSystem;
      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::BoolProperty mSpawnFromEntityType;

   };
}
