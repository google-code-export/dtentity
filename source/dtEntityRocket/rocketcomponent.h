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
#include <Rocket/Core/Variant.h>
#include <v8.h>

namespace Rocket
{
   namespace Core
   {
      class Context;
   }
}


namespace dtEntityRocket
{
   
   dtEntity::Property* RocketVariantToProperty(const Rocket::Core::Variant& value);
   v8::Handle<v8::Value> RocketVariantToVal(const Rocket::Core::Variant& value);
   Rocket::Core::Variant ValToRocketVariant(v8::Handle<v8::Value> value);

   Rocket::Core::String ToRocketString(const v8::Handle<v8::Value>&);
   
   class DT_ROCKET_EXPORT RocketComponent : public dtEntity::NodeComponent
   {

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId FullScreenId;
      static const dtEntity::StringId ContextNameId;
      static const dtEntity::StringId DebugId;

      RocketComponent();
      virtual ~RocketComponent();

      void OnAddedToEntity(dtEntity::Entity& e);
      void OnRemovedFromEntity(dtEntity::Entity& e);
      void OnFinishedSettingProperties();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      Rocket::Core::Context* GetRocketContext();

   private:

      dtEntity::Entity* mEntity;
      dtEntity::BoolProperty mFullScreen;
      dtEntity::StringProperty mContextName;
      dtEntity::BoolProperty mDebug;
      
   };


   ////////////////////////////////////////////////////////////////////////////////

   class DT_ROCKET_EXPORT RocketSystem
      : public dtEntity::DefaultEntitySystem<RocketComponent>
   {
      typedef dtEntity::DefaultEntitySystem<RocketComponent> BaseClass;
      
   public:
     
      RocketSystem(dtEntity::EntityManager& em);
      ~RocketSystem();

   private:

   };
}
