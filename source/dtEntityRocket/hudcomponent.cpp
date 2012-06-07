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

#include "hudcomponent.h"
#include "rocketcomponent.h"
#include <dtEntity/entity.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/transformcomponent.h>
#include <Rocket/Core/Context.h>
#include <Rocket/Core/ElementDocument.h>
#include <iostream>
#include <osg/Camera>
#include <osg/ComputeBoundsVisitor>

namespace dtEntityRocket
{
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId HUDComponent::TYPE(dtEntity::SID("HUD"));
   const dtEntity::StringId HUDComponent::ElementId(dtEntity::SID("Element"));
   const dtEntity::StringId HUDComponent::OffsetId(dtEntity::SID("Offset"));
   const dtEntity::StringId HUDComponent::PixelOffsetId(dtEntity::SID("PixelOffset"));
   const dtEntity::StringId HUDComponent::VisibleId(dtEntity::SID("Visible"));
   const dtEntity::StringId HUDComponent::AlignmentId(dtEntity::SID("Alignment"));
   const dtEntity::StringId HUDComponent::AlignToOriginId(dtEntity::SID("AlignToOrigin"));
   const dtEntity::StringId HUDComponent::AlignToBoundingSphereCenterId(dtEntity::SID("AlignToBoundingSphereCenter"));
   const dtEntity::StringId HUDComponent::AlignToBoundingSphereTopId(dtEntity::SID("AlignToBoundingSphereTop"));
   const dtEntity::StringId HUDComponent::AlignToBoundingSphereBottomId(dtEntity::SID("AlignToBoundingSphereBottom"));
   const dtEntity::StringId HUDComponent::AlignToBoundingBoxTopId(dtEntity::SID("AlignToBoundingBoxTop"));
   const dtEntity::StringId HUDComponent::AlignToBoundingBoxBottomId(dtEntity::SID("AlignToBoundingBoxBottom"));
   const dtEntity::StringId HUDComponent::HideWhenNormalPointsAwayId(dtEntity::SID("HideWhenNormalPointsAway"));

   ////////////////////////////////////////////////////////////////////////////
   HUDComponent::HUDComponent()
      : mEntity(NULL)
      , mElement(NULL)
      , mTransformComponent(NULL)
   {
      Register(ElementId, &mElementProp);
      Register(OffsetId, &mOffset);
      Register(PixelOffsetId, &mPixelOffset);
      Register(VisibleId, &mVisible);
      Register(AlignmentId, &mAlignment);
      Register(HideWhenNormalPointsAwayId, &mHideWhenNormalPointsAway);

      mVisible.Set(true);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   HUDComponent::~HUDComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      BaseClass::OnAddedToEntity(e);
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::Finished()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::SetVisible(bool v)
   {
      if(mElement)
      {
         mElement->SetProperty("visibility", v ? "visible" : "hidden");
      }
      mVisible.Set(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop)
   {
      if(propname == ElementId)
      {
         SetElementById(prop.StringValue());
      }
      else if(propname == AlignmentId || propname == OffsetId)
      {
         CalculateRelPosition();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::CalculateRelPosition()
   {

      mRelPosition = GetOffset();

      if(mTransformComponent == NULL)
      {
         if(mEntity == NULL || !mEntity->GetEntityManager().GetComponent(mEntity->GetId(), mTransformComponent, true))
         {
            return;
         }
      }

      dtEntity::StringId alignment = GetAlignment();

      osg::BoundingSphere bs = mTransformComponent->GetNode()->getBound();

      if(alignment == HUDComponent::AlignToBoundingSphereCenterId)
      {
         mRelPosition = bs.center() - mTransformComponent->GetTranslation() + GetOffset();
      }
      else if(alignment == HUDComponent::AlignToBoundingSphereTopId)
      {
         mRelPosition = bs.center();
         mRelPosition[2] += bs.radius();
         mRelPosition -= mTransformComponent->GetTranslation();
         mRelPosition += GetOffset();
      }
      else if(alignment == HUDComponent::AlignToBoundingSphereBottomId)
      {
         mRelPosition = bs.center();
         mRelPosition[2] -= bs.radius();
         mRelPosition -= mTransformComponent->GetTranslation();
         mRelPosition += GetOffset();
      }
      else if(alignment == HUDComponent::AlignToBoundingBoxTopId)
      {
         osg::ComputeBoundsVisitor v;
         v.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
         mTransformComponent->GetNode()->accept(v);
         osg::BoundingBox bb = v.getBoundingBox();
         mRelPosition = bb.center();
         mRelPosition[2] = bb.zMax();
         mRelPosition += GetOffset();
         mRelPosition -= mTransformComponent->GetTranslation();
      }
      else if(alignment == HUDComponent::AlignToBoundingBoxBottomId)
      {
         osg::ComputeBoundsVisitor v;
         v.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
         mTransformComponent->GetNode()->accept(v);
         osg::BoundingBox bb = v.getBoundingBox();
         mRelPosition = bb.center();
         mRelPosition[2] = bb.zMin();
         mRelPosition += GetOffset();
         mRelPosition -= mTransformComponent->GetTranslation();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::SetElementById(const std::string& id)
   {
      mElementProp.Set(id);
      mElement = NULL;
      RocketSystem* rs;
      bool success = mEntity->GetEntityManager().GetEntitySystem(RocketComponent::TYPE, rs);
      if(!success)
      {
         LOG_ERROR("Cannot set element by id: rocket system not started!");
         return;
      }

      for(RocketSystem::ComponentStore::iterator i = rs->begin(); i != rs->end(); ++i)
      {
         RocketComponent* rocketcomponent = i->second;
         Rocket::Core::Context* context = rocketcomponent->GetRocketContext();
         for(int j = 0; j < context->GetNumDocuments(); ++j)
         {
            Rocket::Core::Element* el = context->GetDocument(j)->GetElementById(id.c_str());
            if(el != NULL)
            {
               mElement = el;
               return;
            }
         }
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   std::string HUDComponent::GetElementId() const
   {
      if(mElement == NULL)
      {
         return "";
      }
      return mElement->GetId().CString();
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId HUDSystem::EnabledId(dtEntity::SID("Enabled"));

   HUDSystem::HUDSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {

      Register(EnabledId, &mEnabled);

      mTickFunctor = dtEntity::MessageFunctor(this, &HUDSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_LATE, "HUDSystem::Tick");

      mVisibilityChangedFunctor = dtEntity::MessageFunctor(this, &HUDSystem::OnVisibilityChanged);
      GetEntityManager().RegisterForMessages(dtEntity::VisibilityChangedMessage::TYPE,
         mVisibilityChangedFunctor, dtEntity::FilterOptions::ORDER_LATE, "HUDSystem::OnVisibilityChanged");

      mMeshChangedFunctor = dtEntity::MessageFunctor(this, &HUDSystem::OnMeshChanged);
      GetEntityManager().RegisterForMessages(dtEntity::MeshChangedMessage::TYPE, mMeshChangedFunctor,
                            dtEntity::FilterOptions::ORDER_DEFAULT, "HUDSystem::OnMeshChanged");

      mEnabled.Set(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   HUDSystem::~HUDSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop)
   {
      if(propname == EnabledId)
      {
         SetEnabled(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::SetEnabled(bool v)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->SetVisible(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::OnRemoveFromEntityManager(dtEntity::EntityManager &em)
   {
      GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::VisibilityChangedMessage::TYPE, mVisibilityChangedFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::MeshChangedMessage::TYPE, mMeshChangedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::OnVisibilityChanged(const dtEntity::Message& m)
   {
      const dtEntity::VisibilityChangedMessage& msg =
            static_cast<const dtEntity::VisibilityChangedMessage&>(m);
      HUDComponent* comp = GetComponent(msg.GetAboutEntityId());
      if(comp)
      {
         comp->SetVisible(msg.GetVisible());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::OnMeshChanged(const dtEntity::Message& m)
   {
     const dtEntity::MeshChangedMessage& msg =
           static_cast<const dtEntity::MeshChangedMessage&>(m);
     dtEntity::EntityId id = msg.GetAboutEntityId();
     HUDComponent* comp = GetComponent(id);
     if(comp)
     {
       comp->CalculateRelPosition();
     }
   }
   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::Tick(const dtEntity::Message& msg)
   {
      if(!mEnabled.Get() || mComponents.empty())
      {
         return;
      }

      RocketSystem* rsys;
      bool success = GetEntityManager().GetEntitySystem(RocketComponent::TYPE, rsys);
      if(!success)
      {
         return;
      }

      dtEntity::ApplicationSystem* appsys;
      if(!GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys))
      {
         return;
      }

      osg::Camera* cam = appsys->GetPrimaryCamera();
      if(cam == NULL)
      {
         return;
      }
      osg::Matrix matrix = cam->getViewMatrix();
      matrix *= cam->getProjectionMatrix();

      int window_w, window_h;

      osg::Viewport* viewport = cam->getViewport();
      if(viewport)
      {
         matrix.postMult(viewport->computeWindowMatrix());
         window_w = viewport->width();
         window_h = viewport->height();

      }
      else
      {
         // ??
         window_w = 1024; window_h = 768;
      }


      for(RocketSystem::ComponentStore::iterator i = rsys->begin(); i != rsys->end(); ++i)
      {
         RocketComponent* rocketcomponent = i->second;
         Rocket::Core::Context* context = rocketcomponent->GetRocketContext();
        
         ComponentStore::iterator j = mComponents.begin();
         for(; j!= mComponents.end(); ++j)
         {

            HUDComponent* comp = j->second;
            Rocket::Core::Element* element = comp->GetElement();
            if(element == NULL || element->GetContext() != context || element->GetNumChildren() == 0)
            {
               continue;
            }

            dtEntity::EntityId id = j->first;
            if(comp->mTransformComponent == NULL)
            {
               if(!GetEntityManager().GetComponent(id, comp->mTransformComponent, true))
               {
                  LOG_ERROR("HUD component depends on a transform component!");
                  continue;
               }
            }

            osg::Vec4 trans = osg::Vec4(comp->mTransformComponent->GetTranslation() + comp->mRelPosition, 1);

            osg::Vec4 camtrans = trans * matrix;
            double w = camtrans[3];
            Rocket::Core::Vector2i dim = context->GetDimensions();
            float x = camtrans[0] / w + comp->GetPixelOffset()[0];
            float y = dim[1] - (camtrans[1] / w  + comp->GetPixelOffset()[1]);
            Rocket::Core::Vector2f sizes = element->GetBox(0).GetSize(Rocket::Core::Box::MARGIN);

            bool hideBecauseNormalPointsAway = false;
            if(comp->GetHideWhenNormalPointsAway())
            {
               osg::Vec3 normal(0, 0, 1);
               normal = comp->mTransformComponent->GetRotation() * normal;

               osg::Vec4 camnormal = osg::Vec4(normal, 0) * matrix;
               if(camnormal[2] > 0)
               {
                  hideBecauseNormalPointsAway = true;
               }
            }
            if(hideBecauseNormalPointsAway || !comp->GetVisible() || w < 0 || x < -sizes.x || x > window_w || y < -sizes.y || y > window_h)
            {
               if(element->GetProperty("visibility")->ToString() != "hidden")
               {
                  element->SetProperty("visibility", "hidden");
               }
            }
            else
            {
               element->SetOffset(Rocket::Core::Vector2f(x, y), context->GetDocument(0), true);
               if(element->GetProperty("visibility")->ToString() != "visible")
               {
                  element->SetProperty("visibility", "visible");
               }
            }
         }
      }
   }
}
