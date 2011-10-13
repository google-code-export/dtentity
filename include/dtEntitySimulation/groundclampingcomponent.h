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


#include <dtEntitySimulation/export.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntity/osgcomponents.h>
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

   class DT_ENTITY_SIMULATION_EXPORT GroundClampingComponent : public dtEntity::Component
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
      virtual void OnFinishedSettingProperties();
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

      void SetMinDistToCamera(float  p) { mMinDistToCamera.Set(p); }
      float GetMinDistToCamera() const { return mMinDistToCamera.Get(); }

   private:

      dtEntity::StringIdProperty mClampingMode;
      dtEntity::FloatProperty mVerticalOffset;
      dtEntity::FloatProperty mMinDistToCamera;
      dtEntity::TransformComponent* mTransformComponent;
      unsigned int mIntersectIndex;
      dtEntity::Entity* mEntity;
      osg::ref_ptr<osgUtil::LineSegmentIntersector> mIntersector;
      osg::Vec3d mLastClampedPosition;
      
   };


   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_SIMULATION_EXPORT GroundClampingSystem
      : public dtEntity::DefaultEntitySystem<GroundClampingComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<GroundClampingComponent> BaseClass;
      
   public:
     
      static const dtEntity::StringId EnabledId;

      GroundClampingSystem(dtEntity::EntityManager& em);
      ~GroundClampingSystem();

      void OnRemoveFromEntityManager(dtEntity::EntityManager& em);

      void Tick(const dtEntity::Message& msg);
      void OnMapLoaded(const dtEntity::Message& msg);
      void OnMapUnloaded(const dtEntity::Message& msg);

      bool ClampToTerrain(osg::Vec3d& position, int voffset = 10000);

   private:

      dtEntity::Property* ScriptGetTerrainHeight(const dtEntity::PropertyArgs& args);

      dtEntity::TransformComponent* GetTransformComp(dtEntity::EntityId eid, GroundClampingComponent* component);

      void HandleIntersections(GroundClampingComponent* component,
         const osgUtil::LineSegmentIntersector::Intersections& intersections);

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mMapLoadedFunctor;
      dtEntity::MessageFunctor mMapUnloadedFunctor;
      osgUtil::IntersectionVisitor mIntersectionVisitor;
      osg::observer_ptr<osg::Node> mSceneNode;
      dtEntity::EntityId mTerrainId;
      osg::ref_ptr<osgUtil::IntersectorGroup> mIntersectorGroup;
      dtEntity::BoolProperty mEnabled;
      osgSim::LineOfSight mLos;
      dtEntity::MapSystem* mMapSystem;
   };
}
