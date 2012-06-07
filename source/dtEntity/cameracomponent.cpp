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

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/systemmessages.h>

#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>
#include <sstream>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   const StringId CameraComponent::TYPE(dtEntity::SID("Camera"));

   const StringId CameraComponent::ContextIdId(dtEntity::SID("ContextId"));
   const StringId CameraComponent::LayerAttachPointId(dtEntity::SID("LayerAttachPoint"));

   const StringId CameraComponent::CullingModeId(dtEntity::SID("CullingMode"));
   const StringId CameraComponent::NoAutoNearFarCullingId(dtEntity::SID("NoAutoNearFarCulling"));
   const StringId CameraComponent::BoundingVolumeNearFarCullingId(dtEntity::SID("BoundingVolumeNearFarCulling"));
   const StringId CameraComponent::PrimitiveNearFarCullingId(dtEntity::SID("PrimitiveNearFarCulling"));
   const StringId CameraComponent::FieldOfViewId(dtEntity::SID("FieldOfView"));
   const StringId CameraComponent::AspectRatioId(dtEntity::SID("AspectRatio"));
   const StringId CameraComponent::NearClipId(dtEntity::SID("NearClip"));
   const StringId CameraComponent::FarClipId(dtEntity::SID("FarClip"));
   const StringId CameraComponent::ClearColorId(dtEntity::SID("ClearColor"));
   const StringId CameraComponent::LODScaleId(dtEntity::SID("LODScale"));
   const StringId CameraComponent::PositionId(dtEntity::SID("Position"));
   const StringId CameraComponent::UpId(dtEntity::SID("Up"));
   const StringId CameraComponent::EyeDirectionId(dtEntity::SID("EyeDirection"));
   const StringId CameraComponent::CullMaskId(dtEntity::SID("CullMask"));

   const StringId CameraComponent::ProjectionModeId(dtEntity::SID("ProjectionMode"));
   const StringId CameraComponent::ModePerspectiveId(dtEntity::SID("ModePerspective"));
   const StringId CameraComponent::ModeOrthoId(dtEntity::SID("ModeOrtho"));

   const StringId CameraComponent::OrthoLeftId(dtEntity::SID("OrthoLeft"));
   const StringId CameraComponent::OrthoRightId(dtEntity::SID("OrthoRight"));
   const StringId CameraComponent::OrthoBottomId(dtEntity::SID("OrthoBottom"));
   const StringId CameraComponent::OrthoTopId(dtEntity::SID("OrthoTop"));
   const StringId CameraComponent::OrthoZNearId(dtEntity::SID("OrthoZNear"));
   const StringId CameraComponent::OrthoZFarId(dtEntity::SID("OrthoZFar"));

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::CameraComponent()
      : BaseClass(new osg::Camera())
      , mContextId(
           DynamicIntProperty::SetValueCB(this, &CameraComponent::SetContextId),
           DynamicIntProperty::GetValueCB(this, &CameraComponent::GetContextId)
        )
      , mFieldOfView(
           DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetFieldOfView),
           DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetFieldOfView)
        )
      , mAspectRatio(
           DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetAspectRatio),
           DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetAspectRatio)
        )
      , mNearClip(
           DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetNearClip),
           DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetNearClip)
        )
      , mFarClip (
           DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetFarClip),
           DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetFarClip)
        )
      , mClearColor (
           DynamicVec4Property::SetValueCB(this, &CameraComponent::SetClearColor),
           DynamicVec4Property::GetValueCB(this, &CameraComponent::GetClearColor)
        )
      , mLODScale (
           DynamicFloatProperty::SetValueCB(this, &CameraComponent::SetLODScale),
           DynamicFloatProperty::GetValueCB(this, &CameraComponent::GetLODScale)
        )
      , mCullMask (
           DynamicUIntProperty::SetValueCB(this, &CameraComponent::SetCullMask),
           DynamicUIntProperty::GetValueCB(this, &CameraComponent::GetCullMask)
        )
      , mContextIdVal(-1)
      , mCullingMode (
           DynamicStringIdProperty::SetValueCB(this, &CameraComponent::SetCullingMode),
           DynamicStringIdProperty::GetValueCB(this, &CameraComponent::GetCullingMode)
        )
   {

      Register(ContextIdId, &mContextId);
      Register(LayerAttachPointId, &mLayerAttachPoint);

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

      mLayerAttachPoint.Set(SID("root"));

      GetCamera()->setProjectionMatrixAsPerspective(45, 1, 1, 100000);
      GetCamera()->setCullMask(NodeMasks::VISIBLE);
      mUp.Set(osg::Vec3(0, 0, 1));
      mEyeDirection.Set(osg::Vec3(0, 1, 0));

      mAspectRatio.Set(1);
      mClearColor.Set(osg::Vec4(0.5f, 0.5f, 0.5f, 1));
      mProjectionMode.Set(ModePerspectiveId);

      mOrthoLeft.Set(-1000);
      mOrthoRight.Set(1000);
      mOrthoBottom.Set(-1000);
      mOrthoTop.Set(1000);
      mOrthoZNear.Set(-10000);
      mOrthoZFar.Set(10000);

      SetCullingMode(NoAutoNearFarCullingId);
      GetCamera()->setAllowEventFocus(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::~CameraComponent()
   {
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnAddedToEntity(Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
      TryAssignContext();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnRemovedFromEntity(Entity& entity)
   {
      CameraRemovedMessage msg;
      msg.SetAboutEntityId(entity.GetId());
      mEntity->GetEntityManager().EmitMessage(msg);
      
      BaseClass::OnRemovedFromEntity(entity);      
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetContextId(int id)
   {
      mContextIdVal = id;
      TryAssignContext();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetFieldOfView(double v)
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      GetCamera()->setProjectionMatrixAsPerspective(v, asp, nc, fc);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   double CameraComponent::GetFieldOfView() const
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      return fov;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetAspectRatio(double v)
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      GetCamera()->setProjectionMatrixAsPerspective(fov, v, nc, fc);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   double CameraComponent::GetAspectRatio() const
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      return asp;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetNearClip(double v)
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      GetCamera()->setProjectionMatrixAsPerspective(fov, asp, v, fc);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   double CameraComponent::GetNearClip() const
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      return nc;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetFarClip(double v)
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      GetCamera()->setProjectionMatrixAsPerspective(fov, asp, nc, v);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   double CameraComponent::GetFarClip() const
   {
      double fov, asp, nc, fc;
      GetCamera()->getProjectionMatrixAsPerspective(fov, asp, nc, fc);
      return fc;
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetLODScale(float v)
   {
      GetCamera()->setLODScale(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   float CameraComponent::GetLODScale() const
   {
      return GetCamera()->getLODScale();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::TryAssignContext()
   {
      if(mEntity == NULL || mContextId.Get() == -1) return;

      ApplicationSystem* appsys;
      bool success = mEntity->GetEntityManager().GetEntitySystem(ApplicationSystem::TYPE, appsys);
      assert(success);

      osgViewer::View* view;
            
      if(GetCamera()->getGraphicsContext() &&
         GetCamera()->getGraphicsContext()->getState()->getContextID() == (unsigned int)mContextId.Get())
      {
         view = static_cast<osgViewer::View*>(GetCamera()->getView());
      }
      else
      {
         osg::View* lastView = GetCamera()->getView();
         osgViewer::ViewerBase::Views views;
         appsys->GetViewer()->getViews(views);

         for(osgViewer::ViewerBase::Views::iterator i = views.begin(); i != views.end(); ++i)
         {
            view = *i;

            osg::Camera* oldcam = view->getCamera();
            osg::Camera* newcam = GetCamera();

            if(oldcam->getGraphicsContext() == NULL)
            {
               if(view->getNumSlaves() != 0)
               {
                  oldcam = view->getSlave(0)._camera;
               }
               else
               {
                  LOG_ERROR("Camera without graphics context found!");
                  continue;
               }
            }
            unsigned int cid = oldcam->getGraphicsContext()->getState()->getContextID();
            if(cid == (unsigned int)mContextId.Get())
            {               
               
               appsys->GetViewer()->stopThreading();

               osg::ref_ptr<osg::GraphicsContext> ctx = oldcam->getGraphicsContext();
               for(unsigned int j = 0; j < oldcam->getNumChildren(); ++j)
               {
                  newcam->addChild(oldcam->getChild(j));
               }

               oldcam->setGraphicsContext(NULL);
               newcam->setGraphicsContext(ctx);
               view->setName(ctx->getName());
               view->setCamera(newcam);
               
               // no slaves supported at the moment
               while(view->getNumSlaves() > 0)
               {
                  view->removeSlave(0);
               }

               appsys->GetViewer()->startThreading(); 

               CameraAddedMessage msg;
               msg.SetAboutEntityId(mEntity->GetId());
               msg.SetContextId(mContextId.Get());

               // enqueueing instead of emitting, this way recipients can be sure that
               // camera entity is fully constructed
               mEntity->GetEntityManager().EnqueueMessage(msg);

               const osg::GraphicsContext::Traits& traits = *newcam->getGraphicsContext()->getTraits();
               
               double fovy, aspectRatio, zNear, zFar;
               newcam->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
               double newAspectRatio = double(traits.width) / double(traits.height);
               double aspectRatioChange = newAspectRatio / aspectRatio;
               if (aspectRatioChange != 1.0)
               {
                 newcam->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange, 1.0, 1.0);
               }
               
               newcam->setViewport(new osg::Viewport(0, 0, traits.width, traits.height));

               GLenum buffer = traits.doubleBuffer ? GL_BACK : GL_FRONT;

               newcam->setDrawBuffer(buffer);
               newcam->setReadBuffer(buffer);

               std::ostringstream os;
               os << "cam_"  << mContextId.Get();
               newcam->setName(os.str());

               if(lastView != NULL && lastView != view)
               {
                  osgViewer::CompositeViewer* compview = dynamic_cast<osgViewer::CompositeViewer*>(appsys->GetViewer());
                  if(compview)
                  {
                     compview->removeView(dynamic_cast<osgViewer::View*>(lastView));
                  }
               }
               break;
            }

         }
        
      }
      
      LayerAttachPointSystem* layersys;
      success = mEntity->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, layersys);
      assert(success);

      LayerAttachPointComponent* lcomp;

      if(mLayerAttachPoint.Get() == LayerAttachPointSystem::RootId)
      {
         view->setSceneData(layersys->GetSceneGraphRoot());
      }
      else
      {
         bool found = layersys->GetByName(mLayerAttachPoint.Get(), lcomp);
         if(!found)
         {
            LOG_ERROR("Cannot attach scene to camera view, layer attach point not found: " << GetStringFromSID(mLayerAttachPoint.Get()));
            return;
         }
         view->setSceneData(lcomp->GetNode());
      }

      UpdateProjectionMatrix();

   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetClearColor(const osg::Vec4& v)
   {       
      GetCamera()->setClearColor(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 CameraComponent::GetClearColor() const
   {
      return GetCamera()->getClearColor();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetProjectionMode(StringId v)
   {
      mProjectionMode.Set(v);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetCullingMode(StringId v)
   {
      mCullingModeVal = v;
      if(v == NoAutoNearFarCullingId)
      {
         GetCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
      }
      else if(v == BoundingVolumeNearFarCullingId)
      {
         GetCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
      }
      else if(v == PrimitiveNearFarCullingId)
      {
         GetCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
      }
      else
      {
         LOG_ERROR("Unknown culling mode: " + GetStringFromSID(v));
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Camera* CameraComponent::GetCamera() const
   {
      return static_cast<osg::Camera*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::Finished()
   {
      UpdateViewMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::UpdateViewMatrix()
   {
      osg::Vec3d lookat = mPosition.Get() + mEyeDirection.Get();
      GetCamera()->setViewMatrixAsLookAt(mPosition.Get(), lookat, mUp.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::UpdateProjectionMatrix()
   {
      if(mProjectionMode.Get() == ModePerspectiveId)
      {
         double fov = mFieldOfView.Get();
         double asp = mAspectRatio.Get();
         double nc = mNearClip.Get();
         double fc = mFarClip.Get();
         GetCamera()->setProjectionMatrixAsPerspective(fov, asp, nc, fc);
      }
      else if(mProjectionMode.Get() == ModeOrthoId)
      {
         GetCamera()->setProjectionMatrixAsOrtho(mOrthoLeft.Get(), mOrthoRight.Get(),
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
      GetCamera()->setCullMask(mask);
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned int CameraComponent::GetCullMask() const
   {
      return GetCamera()->getCullMask();
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId CameraSystem::TYPE(dtEntity::SID("Camera"));

   ////////////////////////////////////////////////////////////////////////////
   CameraSystem::CameraSystem(EntityManager& em)
      : DefaultEntitySystem<CameraComponent>(em, dtEntity::TransformComponent::TYPE)
   {
      mWindowCreatedFunctor  = MessageFunctor(this, &CameraSystem::OnWindowCreated);
      em.RegisterForMessages(WindowCreatedMessage::TYPE, mWindowCreatedFunctor, "CameraSystem::OnWindowCreated");
   }

   ////////////////////////////////////////////////////////////////////////////
   CameraSystem::~CameraSystem()
   {
      GetEntityManager().UnregisterForMessages(WindowCreatedMessage::TYPE, mWindowCreatedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraSystem::OnWindowCreated(const Message& m)
   {
      const WindowCreatedMessage& msg = static_cast<const WindowCreatedMessage&>(m);
      EntityId cameid = GetCameraEntityByContextId(msg.GetContextId());
      if(cameid != EntityId())
      {
         GetComponent(cameid)->TryAssignContext();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   EntityId CameraSystem::GetCameraEntityByContextId(unsigned int id)
   {

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         if(i->second->GetContextId() == id)
         {
            return i->first;
         }
      }
      return EntityId();
   }
}
