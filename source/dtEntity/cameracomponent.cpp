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

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   const StringId CameraComponent::TYPE(SID("Camera"));
   const StringId CameraComponent::CullingModeId(SID("CullingMode"));
   const StringId CameraComponent::NoAutoNearFarCullingId(SID("NoAutoNearFarCulling"));
   const StringId CameraComponent::BoundingVolumeNearFarCullingId(SID("BoundingVolumeNearFarCulling"));
   const StringId CameraComponent::PrimitiveNearFarCullingId(SID("PrimitiveNearFarCulling"));
   const StringId CameraComponent::FieldOfViewId(SID("FieldOfView"));
   const StringId CameraComponent::AspectRatioId(SID("AspectRatio"));
   const StringId CameraComponent::NearClipId(SID("NearClip"));
   const StringId CameraComponent::FarClipId(SID("FarClip"));
   const StringId CameraComponent::ClearColorId(SID("ClearColor"));
   const StringId CameraComponent::LODScaleId(SID("LODScale"));
   const StringId CameraComponent::PositionId(SID("Position"));
   const StringId CameraComponent::UpId(SID("Up"));
   const StringId CameraComponent::EyeDirectionId(SID("EyeDirection"));


   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::CameraComponent()
      :  dtEntity::TransformComponent()
      , mCullMask(NodeMasks::VISIBLE)
   {
      Register(CullingModeId, &mCullingMode);
      Register(FieldOfViewId, &mFieldOfView);
      Register(AspectRatioId, &mAspectRatio);
      Register(NearClipId, &mNearClip);
      Register(FarClipId, &mFarClip);
      Register(LODScaleId, &mLODScale);
      Register(PositionId, &mPosition);
      Register(UpId, &mUp);
      Register(EyeDirectionId, &mEyeDirection);
      Register(ClearColorId, &mClearColor);

      mFieldOfView.Set(45);
      mAspectRatio.Set(1.3f);
      mNearClip.Set(1);
      mFarClip.Set(100000);
      mCullingMode.Set(PrimitiveNearFarCullingId);
      mUp.Set(osg::Vec3(0, 0, 1));
      mEyeDirection.Set(osg::Vec3(0, 1, 0));
      mLODScale.Set(1);
      mAspectRatio.Set(1);
      mClearColor.Set(osg::Vec4(0.5f, 0.5f, 0.5f, 1));
   }

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::~CameraComponent()
   {
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnAddedToEntity(Entity& entity)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(!mCamera.valid())
         return;

      if(propname == CullingModeId)
      {
         if(prop.StringIdValue() == NoAutoNearFarCullingId)
         {
            mCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
         }
         else if(prop.StringIdValue() == BoundingVolumeNearFarCullingId)
         {
            mCamera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
         }
         else if(prop.StringIdValue() == PrimitiveNearFarCullingId)
         {
            mCamera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
         }
         else
         {
            LOG_ERROR("Unknown culling mode: " + GetStringFromSID(prop.StringIdValue()));
         }
      }
      else if(propname == FieldOfViewId || propname == AspectRatioId ||
         propname == NearClipId || propname == FarClipId)
      {
         mCamera->setProjectionMatrixAsPerspective(mFieldOfView.Get(),  mAspectRatio.Get(), mNearClip.Get(), mFarClip.Get());
      }
      else if(propname == LODScaleId)
      {
         mCamera->setLODScale(prop.FloatValue());
      }
      else if(propname == ClearColorId)
      {
         mCamera->setClearColor(prop.Vec4Value());
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetCamera(osg::Camera* cam) 
   { 
      mCamera = cam; 
      OnPropertyChanged(CullingModeId, mCullingMode);
      OnPropertyChanged(FieldOfViewId, mFieldOfView);
      OnPropertyChanged(LODScaleId, mLODScale);
      OnPropertyChanged(ClearColorId, mClearColor);
      OnPropertyChanged(NearClipId, mNearClip);
      OnPropertyChanged(FarClipId, mFarClip);
      mCamera->setCullMask(mCullMask);
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnFinishedSettingProperties()
   {
      UpdateViewMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::UpdateViewMatrix()
   {
      osg::Vec3d lookat = mPosition.Get() + mEyeDirection.Get();
      mCamera->setViewMatrixAsLookAt(mPosition.Get(), lookat, mUp.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetUp(const osg::Vec3d& v) 
   {
      mUp.Set(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d CameraComponent::GetUp() const
   {
      return mUp.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetPosition(const osg::Vec3d& v)
   {
      mPosition.Set(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d CameraComponent::GetPosition() const
   {
      return mPosition.Get();
   }

    ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetEyeDirection(const osg::Vec3d& v)
   {
      mEyeDirection.Set(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d CameraComponent::GetEyeDirection() const
   {
      return mEyeDirection.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetCullMask(unsigned int mask)
   {
      mCullMask = mask;
      mCamera->setCullMask(mCullMask);
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned int CameraComponent::GetCullMask() const
   {
      return mCullMask;
   }

   ////////////////////////////////////////////////////////////////////////////

   CameraSystem::CameraSystem(EntityManager& em)
      : DefaultEntitySystem<CameraComponent>(em, dtEntity::TransformComponent::TYPE)
   {
   }

}
