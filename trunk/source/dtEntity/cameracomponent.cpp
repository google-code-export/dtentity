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
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/applicationcomponent.h>
#include <osgViewer/View>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   const StringId CameraComponent::TYPE(SID("Camera"));
   const StringId CameraComponent::IsMainCameraId(SID("IsMainCamera"));
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
   const StringId CameraComponent::CullMaskId(SID("CullMask"));

   const StringId CameraComponent::ProjectionModeId(SID("ProjectionMode"));
   const StringId CameraComponent::ModePerspectiveId(SID("ModePerspective"));
   const StringId CameraComponent::ModeOrthoId(SID("ModeOrtho"));

   const StringId CameraComponent::OrthoLeftId(SID("OrthoLeft"));
   const StringId CameraComponent::OrthoRightId(SID("OrthoRight"));
   const StringId CameraComponent::OrthoBottomId(SID("OrthoBottom"));
   const StringId CameraComponent::OrthoTopId(SID("OrthoTop"));
   const StringId CameraComponent::OrthoZNearId(SID("OrthoZNear"));
   const StringId CameraComponent::OrthoZFarId(SID("OrthoZFar"));

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::CameraComponent()
      : mCamera(NULL)
      , mCullMask(NodeMasks::VISIBLE)
   {

      Register(IsMainCameraId, &mIsMainCamera);
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
      Register(CullMaskId, &mCullMask);
      Register(ProjectionModeId, &mProjectionMode);

      Register(OrthoLeftId, &mOrthoLeft);
      Register(OrthoRightId, &mOrthoRight);
      Register(OrthoBottomId, &mOrthoBottom);
      Register(OrthoTopId, &mOrthoTop);
      Register(OrthoZNearId, &mOrthoZNear);
      Register(OrthoZFarId, &mOrthoZFar);

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
      mCullMask.Set(0xFFFFFFFF);
      mProjectionMode.Set(ModePerspectiveId);

      mOrthoLeft.Set(-1000);
      mOrthoRight.Set(1000);
      mOrthoBottom.Set(-1000);
      mOrthoTop.Set(1000);
      mOrthoZNear.Set(-10000);
      mOrthoZFar.Set(10000);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::~CameraComponent()
   {
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnAddedToEntity(Entity& entity)
   {
      mEntity = &entity;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnRemovedFromEntity(Entity& entity)
   {
      if(mCamera.valid())
      {
         CameraRemovedMessage msg;
         msg.SetAboutEntityId(mEntity->GetId());
         mEntity->GetEntityManager().EmitMessage(msg);
      }
   }


   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnPropertyChanged(StringId propname, Property& prop)
   {

      if(propname == IsMainCameraId)
      {
         SetIsMainCamera(prop.BoolValue());
         return;
      }

      if(!mCamera.valid())
         return;

      if(propname == PositionId || propname == EyeDirectionId || propname == UpId)
      {
         return;
      }

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

      }
      else if(propname == OrthoLeftId || propname == OrthoRightId ||
         propname == OrthoBottomId || propname == OrthoTopId ||
         propname == OrthoZNearId || propname == OrthoZFarId ||
         propname == FieldOfViewId || propname == AspectRatioId ||
         propname == NearClipId || propname == FarClipId ||
         propname == ProjectionModeId

      )
      {
         UpdateProjectionMatrix();
      }
      else if(propname == LODScaleId)
      {
         mCamera->setLODScale(prop.FloatValue());
      }
      else if(propname == ClearColorId)
      {
         SetClearColor(prop.Vec4Value());
      }
      else if(propname == CullMaskId)
      {
         SetCullMask(prop.UIntValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetClearColor(const osg::Vec4& v)
   { 
      mClearColor.Set(v); 
      if(mCamera.valid())
      {
         mCamera->setClearColor(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetIsMainCamera(bool v)
   {
      mIsMainCamera.Set(v);
      if(mCamera == NULL)
      {
         if(v)
         {
            ApplicationSystem* appsys;
            mEntity->GetEntityManager().GetEntitySystem(ApplicationSystem::TYPE, appsys);
            SetCamera(appsys->GetPrimaryCamera());
         }
         else
         {
            SetCamera(new osg::Camera());
         }
         CameraAddedMessage msg;
         msg.SetAboutEntityId(mEntity->GetId());
         mEntity->GetEntityManager().EmitMessage(msg);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetProjectionMode(StringId v)
   {
      mProjectionMode.Set(v);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetCamera(osg::Camera* cam) 
   { 
      mCamera = cam; 
      OnPropertyChanged(CullingModeId, mCullingMode);
      OnPropertyChanged(FieldOfViewId, mFieldOfView);
      OnPropertyChanged(LODScaleId, mLODScale);
      OnPropertyChanged(ClearColorId, mClearColor);
      OnPropertyChanged(CullMaskId, mCullMask);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::Finished()
   {
      if(mCamera.valid())
      {
         UpdateViewMatrix();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::UpdateViewMatrix()
   {
      osg::Vec3d lookat = mPosition.Get() + mEyeDirection.Get();
      mCamera->setViewMatrixAsLookAt(mPosition.Get(), lookat, mUp.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::UpdateProjectionMatrix()
   {
      if(mCamera == NULL)
      {
         return;
      }

      if(mProjectionMode.Get() == ModePerspectiveId)
      {
         mCamera->setProjectionMatrixAsPerspective(mFieldOfView.Get(),  mAspectRatio.Get(), mNearClip.Get(), mFarClip.Get());
      }
      else if(mProjectionMode.Get() == ModeOrthoId)
      {
         mCamera->setProjectionMatrixAsOrtho(mOrthoLeft.Get(), mOrthoRight.Get(),
                                             mOrthoBottom.Get(), mOrthoTop.Get(),
                                             mOrthoZNear.Get(), mOrthoZFar.Get());
      }

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
      mCullMask.Set(mask);
      mCamera->setCullMask(mCullMask.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned int CameraComponent::GetCullMask() const
   {
      return mCullMask.Get();
   }

	////////////////////////////////////////////////////////////////////////////
   osg::Vec4 CameraComponent::GetClearColor() const
	{
		return mClearColor.Get();
	}

   ////////////////////////////////////////////////////////////////////////////
   CameraSystem::CameraSystem(EntityManager& em)
      : DefaultEntitySystem<CameraComponent>(em, dtEntity::TransformComponent::TYPE)
   {
   }


   ////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId CameraSystem::GetMainCameraEntity()
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         if(i->second->GetIsMainCamera())
         {
            return i->first;
         }
      }
      return dtEntity::EntityId();
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId CameraSystem::GetOrCreateMainCameraEntity(const std::string& mapToSaveCamera)
   {
      dtEntity::EntityId id = GetMainCameraEntity();

      if(id != dtEntity::EntityId())
      {
         return id;
      }
      dtEntity::Entity* entity;
      GetEntityManager().CreateEntity(entity);

      CameraComponent* camcomp;
      entity->CreateComponent(camcomp);
      camcomp->SetIsMainCamera(true);
      camcomp->SetClearColor(osg::Vec4(0,0,0,1));
      camcomp->Finished();
      dtEntity::MapComponent* mapcomp;
      entity->CreateComponent(mapcomp);
      mapcomp->SetEntityName("defaultCam");
      mapcomp->SetUniqueId("defaultCam");
      mapcomp->SetMapName(mapToSaveCamera);
      mapcomp->Finished();
      GetEntityManager().AddToScene(entity->GetId());
      return entity->GetId();
   }

}
