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

#include <dtEntityOSG/cameracomponent.h>

#include <dtEntity/core.h>
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityOSG/osginputinterface.h>
#include <dtEntity/systemmessages.h>

#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>
#include <sstream>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId CameraComponent::TYPE(dtEntity::SID("Camera"));

   const dtEntity::StringId CameraComponent::ContextIdId(dtEntity::SID("ContextId"));
   const dtEntity::StringId CameraComponent::LayerAttachPointId(dtEntity::SID("LayerAttachPoint"));

   const dtEntity::StringId CameraComponent::CullingModeId(dtEntity::SID("CullingMode"));
   const dtEntity::StringId CameraComponent::NoAutoNearFarCullingId(dtEntity::SID("NoAutoNearFarCulling"));
   const dtEntity::StringId CameraComponent::BoundingVolumeNearFarCullingId(dtEntity::SID("BoundingVolumeNearFarCulling"));
   const dtEntity::StringId CameraComponent::PrimitiveNearFarCullingId(dtEntity::SID("PrimitiveNearFarCulling"));
   const dtEntity::StringId CameraComponent::FieldOfViewId(dtEntity::SID("FieldOfView"));
   const dtEntity::StringId CameraComponent::AspectRatioId(dtEntity::SID("AspectRatio"));
   const dtEntity::StringId CameraComponent::NearClipId(dtEntity::SID("NearClip"));
   const dtEntity::StringId CameraComponent::FarClipId(dtEntity::SID("FarClip"));
   const dtEntity::StringId CameraComponent::ClearColorId(dtEntity::SID("ClearColor"));
   const dtEntity::StringId CameraComponent::LODScaleId(dtEntity::SID("LODScale"));
   const dtEntity::StringId CameraComponent::PositionId(dtEntity::SID("Position"));
   const dtEntity::StringId CameraComponent::UpId(dtEntity::SID("Up"));
   const dtEntity::StringId CameraComponent::EyeDirectionId(dtEntity::SID("EyeDirection"));
   const dtEntity::StringId CameraComponent::CullMaskId(dtEntity::SID("CullMask"));

   const dtEntity::StringId CameraComponent::ProjectionModeId(dtEntity::SID("ProjectionMode"));
   const dtEntity::StringId CameraComponent::ModePerspectiveId(dtEntity::SID("ModePerspective"));
   const dtEntity::StringId CameraComponent::ModeOrthoId(dtEntity::SID("ModeOrtho"));

   const dtEntity::StringId CameraComponent::OrthoLeftId(dtEntity::SID("OrthoLeft"));
   const dtEntity::StringId CameraComponent::OrthoRightId(dtEntity::SID("OrthoRight"));
   const dtEntity::StringId CameraComponent::OrthoBottomId(dtEntity::SID("OrthoBottom"));
   const dtEntity::StringId CameraComponent::OrthoTopId(dtEntity::SID("OrthoTop"));
   const dtEntity::StringId CameraComponent::OrthoZNearId(dtEntity::SID("OrthoZNear"));
   const dtEntity::StringId CameraComponent::OrthoZFarId(dtEntity::SID("OrthoZFar"));

   ////////////////////////////////////////////////////////////////////////////
   CameraComponent::CameraComponent()
      : BaseClass(new osg::Camera())
      , mContextId(
           dtEntity::DynamicIntProperty::SetValueCB(this, &CameraComponent::SetContextId),
           dtEntity::DynamicIntProperty::GetValueCB(this, &CameraComponent::GetContextId)
        )
      , mContextIdVal(-1)
      , mCullingMode (
           dtEntity::DynamicStringIdProperty::SetValueCB(this, &CameraComponent::SetCullingMode),
           dtEntity::DynamicStringIdProperty::GetValueCB(this, &CameraComponent::GetCullingMode)
        )
      , mFieldOfView(
           dtEntity::DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetFieldOfView),
           dtEntity::DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetFieldOfView)
        )
      , mAspectRatio(
           dtEntity::DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetAspectRatio),
           dtEntity::DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetAspectRatio)
        )
      , mNearClip(
           dtEntity::DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetNearClip),
           dtEntity::DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetNearClip)
        )
      , mFarClip (
           dtEntity::DynamicDoubleProperty::SetValueCB(this, &CameraComponent::SetFarClip),
           dtEntity::DynamicDoubleProperty::GetValueCB(this, &CameraComponent::GetFarClip)
        )      
      , mClearColor (
           dtEntity::DynamicVec4Property::SetValueCB(this, &CameraComponent::SetClearColor),
           dtEntity::DynamicVec4Property::GetValueCB(this, &CameraComponent::GetClearColor)
        )
      , mLODScale (
           dtEntity::DynamicFloatProperty::SetValueCB(this, &CameraComponent::SetLODScale),
           dtEntity::DynamicFloatProperty::GetValueCB(this, &CameraComponent::GetLODScale)
        )
      , mCullMask (
           dtEntity::DynamicUIntProperty::SetValueCB(this, &CameraComponent::SetCullMask),
           dtEntity::DynamicUIntProperty::GetValueCB(this, &CameraComponent::GetCullMask)
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

      mLayerAttachPoint.Set(dtEntity::SID("root"));

      GetCamera()->setProjectionMatrixAsPerspective(45, 1, 1, 100000);
      GetCamera()->setCullMask(dtEntity::NodeMasks::VISIBLE);
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
   void CameraComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
      TryAssignContext();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::OnRemovedFromEntity(dtEntity::Entity& entity)
   {
      dtEntity::CameraRemovedMessage msg;
      msg.SetAboutEntityId(entity.GetId());
      GetNodeEntity()->GetEntityManager().EmitMessage(msg);
      
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
      if(GetNodeEntity() == NULL || mContextId.Get() == -1) return;

      osgViewer::ViewerBase* viewer = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface())->GetViewer();

      osgViewer::View* view = NULL;
            
      if(GetCamera()->getGraphicsContext() &&
         GetCamera()->getGraphicsContext()->getState()->getContextID() == static_cast<unsigned int>(mContextId.Get()))
      {
         view = static_cast<osgViewer::View*>(GetCamera()->getView());
      }
      else
      {
         osg::View* lastView = GetCamera()->getView();
         osgViewer::ViewerBase::Views views;
         viewer->getViews(views);

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
            if(cid == static_cast<unsigned int>(mContextId.Get()))
            {               
               
               viewer->stopThreading();

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

               viewer->startThreading();

               dtEntity::CameraAddedMessage msg;
               msg.SetAboutEntityId(GetNodeEntity()->GetId());
               msg.SetContextId(mContextId.Get());

               // enqueueing instead of emitting, this way recipients can be sure that
               // camera entity is fully constructed
               GetNodeEntity()->GetEntityManager().EnqueueMessage(msg);

               // add input interface as event listener to view
               OSGInputInterface* wface = static_cast<OSGInputInterface*>(dtEntity::GetInputInterface());
               osgViewer::View::EventHandlers& eh = view->getEventHandlers();
               if(std::find(eh.begin(), eh.end(), wface->GetEventHandler()) ==  eh.end())
               {
                  eh.push_back(wface->GetEventHandler());
               }

               LayerAttachPointSystem* lsys;
               GetNodeEntity()->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, lsys);
               if(GetLayerAttachPoint() != LayerAttachPointSystem::RootId)
               {
                  LayerAttachPointComponent* lc;
                  if(lsys->GetByName(GetLayerAttachPoint(), lc))
                  {
                     static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface())->InstallUpdateCallback(lc->GetNode());
                  }
                  else
                  {
                     LOG_ERROR("Cannot install update callback for layer attach point " << dtEntity::GetStringFromSID(GetLayerAttachPoint()));
                  }
               }

               const osg::GraphicsContext::Traits& traits = *newcam->getGraphicsContext()->getTraits();
               
               double fovy, aspectRatio, zNear, zFar;
               newcam->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
               double newAspectRatio = static_cast<double>(traits.width) / static_cast<double>(traits.height);
               if (newAspectRatio != aspectRatio)
               {
                  double aspectRatioChange = newAspectRatio / aspectRatio;
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
                  osgViewer::CompositeViewer* compview = dynamic_cast<osgViewer::CompositeViewer*>(viewer);
                  if(compview)
                  {
                     compview->removeView(dynamic_cast<osgViewer::View*>(lastView));
                  }
               }
               break;
            }

         }
        
      }

      if(view == NULL)
      {
         return;
      }
      
      LayerAttachPointSystem* layersys;
      bool success = GetNodeEntity()->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, layersys);
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
            LOG_ERROR("Cannot attach scene to camera view, layer attach point not found: " << dtEntity::GetStringFromSID(mLayerAttachPoint.Get()));
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
   void CameraComponent::SetProjectionMode(dtEntity::StringId v)
   {
      mProjectionMode.Set(v);
      UpdateProjectionMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraComponent::SetCullingMode(dtEntity::StringId v)
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
         LOG_ERROR("Unknown culling mode: " + dtEntity::GetStringFromSID(v));
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
      BaseClass::Finished();
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
   const dtEntity::StringId CameraSystem::TYPE(dtEntity::SID("Camera"));

   ////////////////////////////////////////////////////////////////////////////
   CameraSystem::CameraSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<CameraComponent>(em, TransformComponent::TYPE)
   {
      mWindowCreatedFunctor  = dtEntity::MessageFunctor(this, &CameraSystem::OnWindowCreated);
      em.RegisterForMessages(dtEntity::WindowCreatedMessage::TYPE, mWindowCreatedFunctor, "CameraSystem::OnWindowCreated");
   }

   ////////////////////////////////////////////////////////////////////////////
   CameraSystem::~CameraSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::WindowCreatedMessage::TYPE, mWindowCreatedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void CameraSystem::OnWindowCreated(const dtEntity::Message& m)
   {
      const dtEntity::WindowCreatedMessage& msg = static_cast<const dtEntity::WindowCreatedMessage&>(m);
      dtEntity::EntityId cameid = GetCameraEntityByContextId(msg.GetContextId());
      if(cameid != dtEntity::EntityId())
      {
         GetComponent(cameid)->TryAssignContext();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId CameraSystem::GetCameraEntityByContextId(int id)
   {

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         if(i->second->GetContextId() == id)
         {
            return i->first;
         }
      }
      return dtEntity::EntityId();
   }
}
