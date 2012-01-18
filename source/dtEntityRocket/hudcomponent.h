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

#pragma once

#include "export.h"

#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/property.h>
#include <Rocket/Core/Element.h>
#include <v8.h>


namespace dtEntityRocket
{
   
   class DT_ROCKET_EXPORT HUDComponent : public dtEntity::NodeComponent
   {

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ElementId;
      static const dtEntity::StringId OffsetId;
      static const dtEntity::StringId PixelOffsetId;
      static const dtEntity::StringId VisibleId;
      static const dtEntity::StringId AlignToBoundingSphereCenterId;

      HUDComponent();
      virtual ~HUDComponent();

      void OnAddedToEntity(dtEntity::Entity& e);
      void OnRemovedFromEntity(dtEntity::Entity& e);
      void Finished();
      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop);

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      void SetVisible(bool v);
      bool GetVisible() const { return mVisible.Get(); }

      void SetElementById(const std::string& id);
      std::string GetElementId() const;

      Rocket::Core::Element* GetElement() { return mElement; }

      osg::Vec3 GetOffset() const  { return mOffset.Get(); }
      void SetOffset(const osg::Vec3& o) { mOffset.Set(o); }

      void SetPixelOffset(const osg::Vec2& o) { mPixelOffset.Set(o); }
      osg::Vec2 GetPixelOffset() const { return mPixelOffset.Get(); }

      /**
        * if false, set HUD to origin of transform.
        * if true, get bounding sphere of entity and set HUD to center
        */
      void SetAlignToBoundingSphereCenter(bool v) { mAlignToBoundingSphereCenter.Set(v); }
      bool GetAlignToBoundingSphereCenter() const { return mAlignToBoundingSphereCenter.Get(); }

   private:

      dtEntity::Entity* mEntity;
      Rocket::Core::Element* mElement;
      dtEntity::StringProperty mElementProp;
      dtEntity::Vec3Property mOffset;
      dtEntity::Vec2Property mPixelOffset;
      dtEntity::BoolProperty mVisible;
      dtEntity::BoolProperty mAlignToBoundingSphereCenter;
      
   };


   ////////////////////////////////////////////////////////////////////////////////

   class DT_ROCKET_EXPORT HUDSystem
      : public dtEntity::DefaultEntitySystem<HUDComponent>
   {
      typedef dtEntity::DefaultEntitySystem<HUDComponent> BaseClass;

   public:

      static const dtEntity::StringId EnabledId;
     
      HUDSystem(dtEntity::EntityManager& em);
      ~HUDSystem();

      void Tick(const dtEntity::Message& msg);
      void OnVisibilityChanged(const dtEntity::Message& msg);

      void OnRemoveFromEntityManager(dtEntity::EntityManager &em);

      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop);
      void SetEnabled(bool);
      bool GetEnabled() const { return mEnabled.Get(); }

   private:

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mVisibilityChangedFunctor;
      dtEntity::BoolProperty mEnabled;
   };
}
