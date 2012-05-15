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
#include <dtEntity/transformcomponent.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class Group;
   class Camera;
}

namespace dtEntity
{ 

   class CameraSystem;

   /** 
	* The camera component holds properties for the osg::Camera object
   */
   class DT_ENTITY_EXPORT CameraComponent 
      : public TransformComponent
   {
  
      typedef TransformComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId ContextIdId;
      static const StringId LayerAttachPointId;

      static const StringId CullingModeId;
      static const StringId CullMaskId;
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

      static const StringId OrthoLeftId;
      static const StringId OrthoRightId;
      static const StringId OrthoTopId;
      static const StringId OrthoBottomId;
      static const StringId OrthoZNearId;
      static const StringId OrthoZFarId;

      static const StringId ProjectionModeId;
      static const StringId ModePerspectiveId;
      static const StringId ModeOrthoId;
      
      CameraComponent();
     
      virtual ~CameraComponent();

      virtual ComponentType GetType() const 
      { 
         return TYPE; 
      }

      virtual void OnAddedToEntity(Entity& entity);
      virtual void OnRemovedFromEntity(Entity& entity);
      virtual void OnPropertyChanged(StringId propname, Property& prop);

      osg::Camera* GetCamera() const;

      virtual void Finished();

      void FetchCamera();

      void SetContextId(unsigned int id);
      unsigned int GetContextId() const { return mContextId.Get(); }

      void SetLayerAttachPoint(StringId id) { mLayerAttachPoint.Set(id); }
      StringId GetLayerAttachPoint() const { return mLayerAttachPoint.Get(); }

		void SetProjectionMode(StringId);
		StringId GetProjectionMode() const { return mProjectionMode.Get(); }

		void SetCullingMode(StringId);
		StringId GetCullingMode() const { return mCullingMode.Get(); }

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


      
      IntProperty mContextId;
      StringIdProperty mLayerAttachPoint;

      StringIdProperty mCullingMode;
      DynamicDoubleProperty mFieldOfView;
      DynamicDoubleProperty mAspectRatio;
      DynamicDoubleProperty mNearClip;
      DynamicDoubleProperty mFarClip;
      DynamicVec4Property mClearColor;
      DynamicFloatProperty mLODScale;
      Vec3dProperty mPosition;
      Vec3dProperty mUp;
      Vec3dProperty mEyeDirection;
      DynamicUIntProperty mCullMask;
      StringIdProperty mProjectionMode;

      DoubleProperty mOrthoLeft;
      DoubleProperty mOrthoRight;
      DoubleProperty mOrthoBottom;
      DoubleProperty mOrthoTop;
      DoubleProperty mOrthoZNear;
      DoubleProperty mOrthoZFar;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT CameraSystem
      : public DefaultEntitySystem<CameraComponent>
   {
   public:

      static const ComponentType TYPE;

      CameraSystem(EntityManager& em);

      EntityId GetCameraEntityByContextId(unsigned int id);

      void OnWindowCreated(const Message& msg);

   private:
      MessageFunctor mWindowCreatedFunctor;

   };
}
