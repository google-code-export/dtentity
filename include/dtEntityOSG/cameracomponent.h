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

#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/transformcomponent.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class Group;
   class Camera;
}

namespace dtEntityOSG
{ 

   class CameraSystem;

   /** 
	* The camera component holds properties for the osg::Camera object
   */
   class DTENTITY_OSG_EXPORT CameraComponent 
      : public dtEntity::TransformComponent
   {
  
      typedef dtEntity::TransformComponent BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ContextIdId;
      static const dtEntity::StringId LayerAttachPointId;

      static const dtEntity::StringId CullingModeId;
      static const dtEntity::StringId CullMaskId;
      static const dtEntity::StringId NoAutoNearFarCullingId;
      static const dtEntity::StringId BoundingVolumeNearFarCullingId;
      static const dtEntity::StringId PrimitiveNearFarCullingId;
      static const dtEntity::StringId FieldOfViewId;
      static const dtEntity::StringId AspectRatioId;
      static const dtEntity::StringId NearClipId;
      static const dtEntity::StringId FarClipId;
      static const dtEntity::StringId LODScaleId;
      static const dtEntity::StringId ClearColorId;
      static const dtEntity::StringId PositionId;
      static const dtEntity::StringId EyeDirectionId;
      static const dtEntity::StringId UpId;

      static const dtEntity::StringId OrthoLeftId;
      static const dtEntity::StringId OrthoRightId;
      static const dtEntity::StringId OrthoTopId;
      static const dtEntity::StringId OrthoBottomId;
      static const dtEntity::StringId OrthoZNearId;
      static const dtEntity::StringId OrthoZFarId;

      static const dtEntity::StringId ProjectionModeId;
      static const dtEntity::StringId ModePerspectiveId;
      static const dtEntity::StringId ModeOrthoId;
      
      CameraComponent();
     
      virtual ~CameraComponent();

      virtual dtEntity::ComponentType GetType() const 
      { 
         return TYPE; 
      }

      virtual void OnAddedToEntity(dtEntity::Entity& entity);
      virtual void OnRemovedFromEntity(dtEntity::Entity& entity);

      osg::Camera* GetCamera() const;

      virtual void Finished();

      void FetchCamera();

      void SetContextId(int id);
      int GetContextId() const { return mContextIdVal; }

      void SetLayerAttachPoint(dtEntity::StringId id) { mLayerAttachPoint.Set(id); }
      dtEntity::StringId GetLayerAttachPoint() const { return mLayerAttachPoint.Get(); }

		void SetProjectionMode(dtEntity::StringId);
		dtEntity::StringId GetProjectionMode() const { return mProjectionMode.Get(); }

		void SetCullingMode(dtEntity::StringId);
		dtEntity::StringId GetCullingMode() const { return mCullingModeVal; }

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
       * Re-calculate osg camera projection matrix from mode, near, far etc
       */
      void UpdateProjectionMatrix();

      /**
       * Set the osg node mask controlling which nodes are
       * visible to the camera. Please see dtEntity/nodemasks.h
       */
      void SetCullMask(unsigned int mask);
      unsigned int GetCullMask() const;

		void SetClearColor(const osg::Vec4&);
      osg::Vec4 GetClearColor() const;

      virtual osg::Vec3d GetTranslation() const { return GetPosition(); }
      virtual void SetTranslation(const osg::Vec3d& v) { SetPosition(v); }

      virtual osg::Quat GetRotation() const 
      { 
         osg::Quat q;
         q.makeRotate(osg::Vec3d(0,1,0), mEyeDirection.Get());
         return q;
      }

      virtual void SetRotation(const osg::Quat& q)
      { 
         mEyeDirection.Set(q * osg::Vec3d(0, 1, 0));
      }

      void SetFieldOfView(double v);
      double GetFieldOfView() const;

      void SetAspectRatio(double v);
      double GetAspectRatio() const;

      void SetNearClip(double v);
      double GetNearClip() const;

      void SetFarClip(double v);
      double GetFarClip() const;

      void SetLODScale(float v);
      float GetLODScale() const;

      void TryAssignContext();
      
   private:


      
      dtEntity::DynamicIntProperty mContextId;
      int mContextIdVal;
      dtEntity::StringIdProperty mLayerAttachPoint;

      dtEntity::DynamicStringIdProperty mCullingMode;
      dtEntity::StringId mCullingModeVal;
      dtEntity::DynamicDoubleProperty mFieldOfView;
      dtEntity::DynamicDoubleProperty mAspectRatio;
      dtEntity::DynamicDoubleProperty mNearClip;
      dtEntity::DynamicDoubleProperty mFarClip;
      dtEntity::DynamicVec4Property mClearColor;
      dtEntity::DynamicFloatProperty mLODScale;
      dtEntity::Vec3dProperty mPosition;
      dtEntity::Vec3dProperty mUp;
      dtEntity::Vec3dProperty mEyeDirection;
      dtEntity::DynamicUIntProperty mCullMask;
      dtEntity::StringIdProperty mProjectionMode;

      dtEntity::DoubleProperty mOrthoLeft;
      dtEntity::DoubleProperty mOrthoRight;
      dtEntity::DoubleProperty mOrthoBottom;
      dtEntity::DoubleProperty mOrthoTop;
      dtEntity::DoubleProperty mOrthoZNear;
      dtEntity::DoubleProperty mOrthoZFar;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DTENTITY_OSG_EXPORT CameraSystem
      : public dtEntity::DefaultEntitySystem<CameraComponent>
   {
   public:

      static const dtEntity::ComponentType TYPE;

      CameraSystem(dtEntity::EntityManager& em);
      ~CameraSystem();

      dtEntity::EntityId GetCameraEntityByContextId(int id);

      void OnWindowCreated(const dtEntity::Message& msg);

   private:
      dtEntity::MessageFunctor mWindowCreatedFunctor;

   };
}
