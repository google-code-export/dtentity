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

#include <dtEntity/cameracomponent.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntity/scriptaccessor.h>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgSim/LineOfSight>
#include <osg/observer_ptr>

namespace dtEntity
{
   class MapSystem;
}

namespace dtEntitySimulation
{

   class GroundClampingComponent : public dtEntity::Component
   {

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ClampingModeId;
      static const dtEntity::StringId ClampingMode_DisabledId;
      static const dtEntity::StringId ClampingMode_KeepAboveTerrainId;
      static const dtEntity::StringId ClampingMode_SetHeightToTerrainHeightId;
      static const dtEntity::StringId ClampingMode_SetHeightAndRotationToTerrainId;
      static const dtEntity::StringId MinDistToCameraId;
      static const dtEntity::StringId VerticalOffsetId;

      GroundClampingComponent();
      virtual ~GroundClampingComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual void OnAddedToEntity(dtEntity::Entity& e);
      virtual void Finished();
      dtEntity::TransformComponent* GetTransformComponent() const { return mTransformComponent; }

      void SetClampingMode(dtEntity::StringId mode) { mClampingMode.Set(mode); }
      dtEntity::StringId GetClampingMode() const { return mClampingMode.Get(); }

      float GetVerticalOffset() const { return mVerticalOffset.Get(); }
      void SetVerticalOffset(float v) { mVerticalOffset.Set(v); }

      // for keeping track of intersections when using osgSim HeightAboveTerrain
      void SetIntersectIndex(unsigned int i) { mIntersectIndex = i; }
      unsigned int GetIntersectIndex() const { return mIntersectIndex; }

      osgUtil::LineSegmentIntersector* GetIntersector() const { return mIntersector.get(); }
      void SetIntersector(osgUtil::LineSegmentIntersector* isect) { mIntersector = isect; }

      void SetLastClampedPosition(const osg::Vec3d& p) { mLastClampedPosition = p; }
      osg::Vec3d GetLastClampedPosition() const { return mLastClampedPosition; }

      void SetLastClampedAttitude(const osg::Quat& p) { mLastClampedAttitude = p; }
      osg::Quat GetLastClampedAttitude() const { return mLastClampedAttitude; }

      void SetLastClampedNormal(const osg::Vec3& p) { mLastClampedNormal = p; }
      osg::Vec3 GetLastClampedNormal() const { return mLastClampedNormal; }

      void SetMinDistToCamera(float p) { mMinDistToCamera.Set(p); }
      float GetMinDistToCamera() const { return mMinDistToCamera.Get(); }

       void SetDirty(bool v) { mDirty = v; }
      bool GetDirty() const { return mDirty; }

   private:

      dtEntity::StringIdProperty mClampingMode;
      dtEntity::FloatProperty mVerticalOffset;
      dtEntity::FloatProperty mMinDistToCamera;
      dtEntity::TransformComponent* mTransformComponent;
      unsigned int mIntersectIndex;
      dtEntity::Entity* mEntity;
      osg::ref_ptr<osgUtil::LineSegmentIntersector> mIntersector;
      osg::Vec3d mLastClampedPosition;
      osg::Vec3d mLastClampedNormal;

      bool mDirty;
      osg::Quat mLastClampedAttitude;
      
   };


   ////////////////////////////////////////////////////////////////////////////////


   class GroundClampingSystem
      : public dtEntity::DefaultEntitySystem<GroundClampingComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<GroundClampingComponent> BaseClass;
      
   public:
     
      static const dtEntity::StringId EnabledId;
      static const dtEntity::StringId IntersectLayerId;
      static const dtEntity::StringId FetchLODsId;

      GroundClampingSystem(dtEntity::EntityManager& em);
      ~GroundClampingSystem();

      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop);

      void OnRemoveFromEntityManager(dtEntity::EntityManager& em);

      void Tick(const dtEntity::Message& msg);
      void CameraAdded(const dtEntity::Message& msg);
      void CameraRemoved(const dtEntity::Message& msg);
      void MapLoaded(const dtEntity::Message& msg);
      
      bool ClampToTerrain(osg::Vec3d& position, int voffset = 10000);

      void SetIntersectLayer(dtEntity::StringId);
      dtEntity::StringId GetIntersectLayer() const { return  mIntersectLayer.Get(); }


      virtual bool StorePropertiesToScene() const { return true; }

   private:

      dtEntity::Property* ScriptGetTerrainHeight(const dtEntity::PropertyArgs& args);

      dtEntity::TransformComponent* GetTransformComp(dtEntity::EntityId eid, GroundClampingComponent* component);
      void DirtyAll();
      void HandleIntersection(GroundClampingComponent* component,
         const osgUtil::LineSegmentIntersector::Intersection& intersection, float dt, double simTime);

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mCameraAddedFunctor;
      dtEntity::MessageFunctor mCameraRemovedFunctor;
	   dtEntity::MessageFunctor mMapLoadedFunctor;
      osgUtil::IntersectionVisitor mIntersectionVisitor;
      osg::observer_ptr<osg::Node> mRootNode;
      osg::ref_ptr<osgUtil::IntersectorGroup> mIntersectorGroup;

      dtEntity::BoolProperty mEnabled;
      dtEntity::StringIdProperty mIntersectLayer;
      dtEntity::BoolProperty mFetchLODs;

      osgSim::LineOfSight mLos;
      dtEntity::CameraComponent* mCamera;

   };
}
