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
#include <dtEntity/basemessages.h>
#include <dtEntity/applicationcomponent.h>
#include <Rocket/Core/Context.h>
#include <Rocket/Core/ElementDocument.h>
#include <iostream>

namespace dtEntityRocket
{
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId HUDComponent::TYPE(dtEntity::SID("HUD"));
   const dtEntity::StringId HUDComponent::ElementId(dtEntity::SID("Element"));
   const dtEntity::StringId HUDComponent::OffsetId(dtEntity::SID("Offset"));
   const dtEntity::StringId HUDComponent::PixelOffsetId(dtEntity::SID("PixelOffset"));
   const dtEntity::StringId HUDComponent::VisibleId(dtEntity::SID("Visible"));

   ////////////////////////////////////////////////////////////////////////////
   HUDComponent::HUDComponent()
      : mEntity(NULL)
      , mElement(NULL)
   {
      Register(ElementId, &mElementProp);
      Register(OffsetId, &mOffset);
      Register(PixelOffsetId, &mPixelOffset);
      Register(VisibleId, &mVisible);

      mVisible.Set(true);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   HUDComponent::~HUDComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
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
      mElement->SetProperty("visibility", v ? "visible" : "hidden");
      mVisible.Set(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop)
   {
      if(propname == ElementId)
      {
         SetElementById(prop.StringValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDComponent::SetElementById(const std::string& id)
   {
      mElementProp.Set(id);
      mElement = NULL;
      RocketSystem* rs;
      mEntity->GetEntityManager().GetEntitySystem(RocketComponent::TYPE, rs);

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
         mTickFunctor, dtEntity::FilterOptions::PRIORITY_LOWEST, "HUDSystem::Tick");

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
   }

   ////////////////////////////////////////////////////////////////////////////
   void HUDSystem::Tick(const dtEntity::Message& msg)
   {
      if(!mEnabled.Get() || mComponents.empty())
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


      RocketSystem* rsys;
      GetEntityManager().GetEntitySystem(RocketComponent::TYPE, rsys);
      for(RocketSystem::ComponentStore::iterator i = rsys->begin(); i != rsys->end(); ++i)
      {
         RocketComponent* rocketcomponent = i->second;
         Rocket::Core::Context* context = rocketcomponent->GetRocketContext();
        // osg::Matrix matrix = rocketcomponent->GetWorldToScreenMatrix();
         ComponentStore::iterator j = mComponents.begin();
         for(; j!= mComponents.end(); ++j)
         {

            HUDComponent* comp = j->second;
            Rocket::Core::Element* element = comp->GetElement();
            if(element == NULL || element->GetContext() != context)
            {
               continue;
            }

            dtEntity::EntityId id = j->first;
            dtEntity::TransformComponent* tc;
            if(GetEntityManager().GetComponent(id, tc, true))
            {
               osg::Vec4 trans = osg::Vec4(tc->GetTranslation() + comp->GetOffset(), 1);

               trans = trans * matrix;
               double w = trans[3];
               Rocket::Core::Vector2i dim = context->GetDimensions();
               float x = trans[0] / w + comp->GetPixelOffset()[0];
               float y = dim[1] - (trans[1] / w  + comp->GetPixelOffset()[1]);
               Rocket::Core::Vector2f sizes = element->GetBox(0).GetSize(Rocket::Core::Box::MARGIN);
               if(!comp->GetVisible() || w < 0 || x < -sizes.x || x > window_w || y < -sizes.y || y > window_h)
               {
                  element->SetProperty("visibility", "hidden");
               }
               else
               {
                  element->SetOffset(Rocket::Core::Vector2f(x, y), context->GetDocument(0), false);
                  element->SetProperty("visibility", "visible");
               }
            }
         }
      }
   }
}
