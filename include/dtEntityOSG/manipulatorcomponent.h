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

#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/nodecomponent.h>
#include <osgManipulator/Dragger>

namespace dtEntityOSG
{

   class ManipulatorComponent
         : public dtEntityOSG::NodeComponent
   {

      typedef dtEntityOSG::NodeComponent BaseClass;


   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId LayerId;
      static const dtEntity::StringId DraggerTypeId;
      static const dtEntity::StringId OffsetFromStartId;
      static const dtEntity::StringId KeepSizeConstantId;
      static const dtEntity::StringId UseLocalCoordsId;
      static const dtEntity::StringId PivotAtBottomId;

      static const dtEntity::StringId TabPlaneDraggerId;
      static const dtEntity::StringId TabPlaneTrackballDraggerId;
      static const dtEntity::StringId TabBoxTrackballDraggerId;
      static const dtEntity::StringId TrackballDraggerId;
      static const dtEntity::StringId Translate1DDraggerId;
      static const dtEntity::StringId Translate2DDraggerId;
      static const dtEntity::StringId TranslateAxisDraggerId;
      static const dtEntity::StringId TabBoxDraggerId;
      static const dtEntity::StringId TerrainTranslateDraggerId;
      static const dtEntity::StringId ScaleDraggerId;


      ManipulatorComponent();
      virtual ~ManipulatorComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual void OnAddedToEntity(dtEntity::Entity& e);
      virtual void OnRemovedFromEntity(dtEntity::Entity& e);
      virtual void Finished();

      osgManipulator::Dragger* GetDragger() const;

      /**
       * Attach the dragger to the layer with this name. Default value is DefaultLayerId
       */
      dtEntity::StringId GetLayer() const;
      void SetLayer(dtEntity::StringId);

      dtEntity::StringId GetDraggerType() const { return mDraggerTypeVal; }
      void SetDraggerType(dtEntity::StringId);

      void SetOffsetFromStart(const osg::Vec3d& v);
      osg::Vec3d GetOffsetFromStart() const { return mOffsetFromStartVal; }

      void SetUseLocalCoords(bool v);
      bool GetUseLocalCoords() const { return mUseLocalCoordsVal; }

      void SetKeepSizeConstant(bool v);
      bool GetKeepSizeConstant() const { return mKeepSizeConstantVal; }

      void SetPivotAtBottom(bool v);
      bool GetPivotAtBottom() const { return mPivotAtBottomVal; }

   private:

      void RemoveFromParent();
      void AddToLayer();

      osg::ref_ptr<osgManipulator::DraggerCallback> mDraggerCallback;
      dtEntity::DynamicStringIdProperty mLayerProperty;
      dtEntity::StringId mLayerVal;
      dtEntity::StringId mAttachPoint;
      dtEntity::DynamicStringIdProperty mDraggerType;
      dtEntity::StringId mDraggerTypeVal;
      dtEntity::DynamicVec3dProperty mOffsetFromStart;
      osg::Vec3 mOffsetFromStartVal;
      dtEntity::DynamicBoolProperty mKeepSizeConstant;
      bool mKeepSizeConstantVal;
      dtEntity::DynamicBoolProperty mUseLocalCoords;
      bool mUseLocalCoordsVal;
      dtEntity::DynamicBoolProperty mPivotAtBottom;
      bool mPivotAtBottomVal;
      osg::ref_ptr<osg::Group> mDraggerContainer;
      
   };


   ////////////////////////////////////////////////////////////////////////////////

   class ManipulatorSystem
      : public dtEntity::DefaultEntitySystem<ManipulatorComponent>
   {
      typedef dtEntity::DefaultEntitySystem<ManipulatorComponent> BaseClass;
      
   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId UseLocalCoordsId;
      static const dtEntity::StringId UseGroundClampingId;

      ManipulatorSystem(dtEntity::EntityManager& em);
      ~ManipulatorSystem();

      // don't store manipulators to map
      virtual bool StoreComponentsToMap() const { return false; }

      // don't copy manipulators with entities, also don't store to spawners
      virtual bool AllowComponentCreationBySpawner() const { return false; }

      void SetUseLocalCoords(bool v);
      bool GetUseLocalCoords() const { return mUseLocalCoordsVal; }

      void SetUseGroundClamping(bool v) { mUseGroundClamping.Set(v); }
      bool GetUseGroundClamping() const { return mUseGroundClamping.Get(); }

   private:

      dtEntity::DynamicBoolProperty mUseLocalCoords;
      bool mUseLocalCoordsVal;
      dtEntity::BoolProperty mUseGroundClamping;
   };
}
