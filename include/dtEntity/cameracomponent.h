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

#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/component.h>
#include <dtEntity/stringid.h>
#include <osg/Group>
#include <osg/ref_ptr>
#include <osg/Camera>

namespace dtEntity
{ 

   class CameraSystem;

   /** 
	* The camera component holds properties for the osg::Camera object
   */
   class DT_ENTITY_EXPORT CameraComponent 
      : public TransformComponent
   {
  
   public:

      static const ComponentType TYPE;
      static const StringId IsMainCameraId;
      static const StringId CullingModeId;
      static const StringId NoAutoNearFarCullingId;
      static const StringId BoundingVolumeNearFarCullingId;
      static const StringId PrimitiveNearFarCullingId;
      static const StringId FieldOfViewId;
      static const StringId AspectRatioId;
      static const StringId NearClipId;
      static const StringId FarClipId;
      static const StringId LODScaleId;
      static const StringId ClearColorId;
      static const StringId PositionId;
      static const StringId EyeDirectionId;
      static const StringId UpId;
      
      CameraComponent();
     
      virtual ~CameraComponent();

      virtual ComponentType GetType() const 
      { 
         return TYPE; 
      }

      virtual void OnAddedToEntity(Entity& entity);
      virtual void OnRemovedFromEntity(Entity& entity);
      virtual void OnPropertyChanged(StringId propname, Property& prop);

      void SetCamera(osg::Camera* cam);
      osg::Camera* GetCamera() const { return mCamera.get(); }

      virtual void Finished();

		bool GetIsMainCamera() const
		{
			return mIsMainCamera.Get();
		}

		void SetIsMainCamera(bool v);

	  /** Set up vector of camera. Call UpdateViewMatrix to apply changes. */
      void SetUp(const osg::Vec3d&);
      osg::Vec3d GetUp() const;

	  /** Set eye position of camera. Call UpdateViewMatrix to apply changes. */
      void SetPosition(const osg::Vec3d&);
      osg::Vec3d GetPosition() const;

	  /** Set look direction of camera. Should be a normalized vector!
	   * Call UpdateViewMatrix to apply changes. 
	   */
      void SetEyeDirection(const osg::Vec3d&);
      osg::Vec3d GetEyeDirection() const;

	  /**
	   * Re-calculate osg camera view matrix from up, pos and eye dir vectors
	   */
      void UpdateViewMatrix();

      /**
       * Set the osg node mask controlling which nodes are
       * visible to the camera. Please see dtEntity/nodemasks.h
       */
      void SetCullMask(unsigned int mask);
      unsigned int GetCullMask() const;

      virtual osg::Vec3d GetTranslation() const { return GetPosition(); }
      virtual void SetTranslation(const osg::Vec3d& v) { SetPosition(v); }

      virtual osg::Quat GetRotation() const 
      { 
         //TODO implement
         return osg::Quat();
      }

      virtual void SetRotation(const osg::Quat&) 
      { 
         //TODO implement
      }

   private:

      Entity* mEntity;
      unsigned int mCullMask;
      osg::ref_ptr<osg::Camera> mCamera;
      BoolProperty mIsMainCamera;
      StringIdProperty mCullingMode;
      DoubleProperty mFieldOfView;
      DoubleProperty mAspectRatio;
      DoubleProperty mNearClip;
      DoubleProperty mFarClip;
      FloatProperty mLODScale;
      Vec3dProperty mPosition;
      Vec3dProperty mUp;
      Vec3dProperty mEyeDirection;
      Vec4Property mClearColor;

   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT CameraSystem
      : public DefaultEntitySystem<CameraComponent>
   {
   public:

      CameraSystem(EntityManager& em);

      dtEntity::EntityId GetMainCameraEntity();
      dtEntity::EntityId GetOrCreateMainCameraEntity(const std::string& mapToSaveCamera);

   private:
   };
}
