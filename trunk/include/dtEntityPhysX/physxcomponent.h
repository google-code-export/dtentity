/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#ifndef DTENTITY_PHYSX_COMPONENT
#define DTENTITY_PHYSX_COMPONENT

#include <dtEntityPhysX/export.h>
#include <dtEntity/component.h>
#include <dtEntity/debugdrawmanager.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/property.h>

class NxPhysicsSDK;
class NxScene;
class NxSceneQuery;
class NxActor;

namespace dtEntityPhysX
{
   class ErrorStream;

   class DT_ENTITY_PHYSX_EXPORT PhysXComponent : public dtEntity::Component
   {
      friend class PhysXSystem;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId CollisionTypeId;      
      static const dtEntity::StringId IsKinematicId;
      static const dtEntity::StringId IsStaticId;
      static const dtEntity::StringId InitialVelocityId;
      static const dtEntity::StringId RadiusId;
      static const dtEntity::StringId BoxHalfLengthsId;
      static const dtEntity::StringId CollisionEnabledId;
      static const dtEntity::StringId CollisionTypeCubeId;
      static const dtEntity::StringId CollisionTypeSphereId;
      static const dtEntity::StringId CollisionTypeMeshId;
      static const dtEntity::StringId OffsetId;
      static const dtEntity::StringId AngularDampingId;
      static const dtEntity::StringId DensityId;
      static const dtEntity::StringId SkinWidthId;
      static const dtEntity::StringId CollisionGroupId;
      
      PhysXComponent();      
      virtual ~PhysXComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      virtual void OnAddedToEntity(dtEntity::Entity& entity);
      virtual void OnRemovedFromEntity(dtEntity::Entity& entity);
      

      dtEntity::EntityId GetOwnerId() const { return mOwnerId; }

      dtEntity::StringId GetCollisionType() const { return mCollisionType.Get();}
      void SetCollisionType(dtEntity::StringId c){ mCollisionType.Set(c); }

      float GetRadius() const { return mRadius.Get(); }

      NxActor* GetPhysXActor() const;

      void AddForce(const osg::Vec3& force);

      bool IsKinematic() const { return mIsKinematic.Get(); }
      
      bool IsStatic() const {return mIsStatic.Get(); }
      void SetIsStatic(bool v) { mIsStatic.Set(v); }

      bool CollisionEnabled() const { return mCollisionEnabled.Get(); }
      void SetCollisionEnabled(bool enabled);

      int GetCollisionGroup() const { return mCollisionGroup.Get(); }
      void SetCollisionGroup(int v) { mCollisionGroup.Set(v); }

      osg::Vec3 GetInitialVelocity() const { return mInitialVelocity.Get(); }
      void SetInitialVelocity(const osg::Vec3& v){ mInitialVelocity.Set(v); }

      osg::Vec3 GetBoxHalfLengths() const { return mBoxHalfLengths.Get(); }
      void SetBoxHalfLengths(const osg::Vec3& v){ mBoxHalfLengths.Set(v); }
      
   private:

      dtEntity::EntityId mOwnerId;
      dtEntity::BoolProperty mCollisionEnabled;
      dtEntity::StringIdProperty mCollisionType;
      dtEntity::FloatProperty mRadius;
      dtEntity::FloatProperty mAngularDamping;
      dtEntity::FloatProperty mDensity;
      dtEntity::FloatProperty mSkinWidth;
      dtEntity::BoolProperty mIsKinematic;
      dtEntity::BoolProperty mIsStatic;
      dtEntity::Vec3Property mOffset;
      dtEntity::Vec3Property mInitialVelocity;
      dtEntity::Vec3Property mBoxHalfLengths;
      dtEntity::IntProperty mCollisionGroup;
      NxActor* mPhysXActor;
   };


   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_PHYSX_EXPORT PhysXSystem
      : public dtEntity::DefaultEntitySystem<PhysXComponent>
   {
      typedef dtEntity::DefaultEntitySystem<PhysXComponent> BaseClass;
      
   public:
     
      static const dtEntity::StringId EnabledId;

      PhysXSystem(dtEntity::EntityManager& em);
      ~PhysXSystem();

      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);

      bool GetEnabled() const { return mEnabled.Get(); }
      void SetEnabled(bool);
      void Tick(const dtEntity::Message& msg);
      void OnEnterWorld(const dtEntity::Message& msg);
      void OnLeaveWorld(const dtEntity::Message& msg);
      
      bool RaycastClosestEntity(const osg::Vec3& origin, const osg::Vec3& dir, dtEntity::EntityId& result) const;

      void ClearPhysXActor(dtEntity::EntityId id);
      void SetCubeShape(dtEntity::EntityId id);
      void SetSphereShape(dtEntity::EntityId id);
      void SetMeshShape(dtEntity::EntityId id);

      NxScene* GetPhysXScene() const { return mPhysXScene; }

      dtEntity::DebugDrawManager* GetDebugDrawManager();

   private:

      bool SetupPhysX();
      void TearDownPhysX();
      void DoDebugDrawing(float dt);
      void SetupPhysXActor(NxActor* actor, PhysXComponent* component);

      void ApplySceneGraphTransformsToPhysics();
      void ApplyPhysicsTransformsToSceneGraph();

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mEnterWorldFunctor;
      dtEntity::MessageFunctor mLeaveWorldFunctor;
      osg::ref_ptr<dtEntity::DebugDrawManager> mDebugDrawManager;
      
      float mTimeSinceLastDebugDraw;
      NxPhysicsSDK* mPhysXSDK;
      NxScene* mPhysXScene;
      ErrorStream* mErrorStream;
      dtEntity::BoolProperty mEnabled;

   };
}

#endif // DTENTITY_PHYSX_COMPONENT
