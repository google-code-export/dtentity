#pragma once

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

#include <dtEntity/component.h>
#include <dtEntity/entitysystem.h>
#include <dtEntityWrappers/export.h>
#include <v8.h>

namespace dtEntity
{
   class EntitySystem;
}

namespace dtEntityWrappers
{
  
   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_WRAPPERS_EXPORT ComponentJS
      : public dtEntity::Component
   {
   public:
      ComponentJS(dtEntity::ComponentType componentType, v8::Handle<v8::Object> obj);
      ~ComponentJS();

      virtual dtEntity::ComponentType GetType() const 
      { 
         return mComponentType; 
      }

      virtual bool IsInstanceOf(dtEntity::ComponentType t) const
      { 
         return (t == mComponentType); 
      }

      virtual void OnAddedToEntity(dtEntity::Entity& entity);
      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      virtual void Finished();

   private:
      dtEntity::ComponentType mComponentType;
      v8::Persistent<v8::Object> mComponent;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_WRAPPERS_EXPORT EntitySystemJS
      : public dtEntity::EntitySystem
   {
   public:

      typedef dtEntity::EntitySystem BaseClass;

      EntitySystemJS(dtEntity::ComponentType id, dtEntity::EntityManager& em, v8::Handle<v8::Object> obj);
     
      ~EntitySystemJS();

      dtEntity::ComponentType GetComponentType() const { return mComponentType; }
      
      virtual void OnRemovedFromEntityManager(dtEntity::EntityManager& em);

      virtual bool HasComponent(dtEntity::EntityId eid) const;
      virtual bool GetComponent(dtEntity::EntityId eid, dtEntity::Component*& component);
      virtual bool GetComponent(dtEntity::EntityId eid, const dtEntity::Component*& component) const;
      virtual bool CreateComponent(dtEntity::EntityId eid, dtEntity::Component*& component);
      virtual bool DeleteComponent(dtEntity::EntityId eid);
      virtual void GetEntitiesInSystem(std::list<dtEntity::EntityId>& toFill) const;
      virtual void OnPropertyChanged(dtEntity::StringId propnamesid, dtEntity::Property& prop);
      virtual void Finished();
      virtual dtEntity::GroupProperty GetProperties() const;

      virtual bool StoreComponentToMap(dtEntity::EntityId) const;
      virtual bool AllowComponentCreationBySpawner() const;
      virtual bool StorePropertiesToScene() const;

   private:

      dtEntity::ComponentType mComponentType;
      v8::Persistent<v8::Object> mSystem;
      v8::Persistent<v8::Function> mHasCompFun;
      v8::Persistent<v8::Function> mGetCompFun;
      v8::Persistent<v8::Function> mCreateCompFun;
      v8::Persistent<v8::Function> mDelCompFun;
      v8::Persistent<v8::Function> mGetESFun;
      v8::Persistent<v8::String> mStringGetComponent;
      v8::Persistent<v8::String> mStringFinished;
      v8::Persistent<v8::String> mStringOnPropertyChanged;
      v8::Persistent<v8::String> mStringStoreComponentToMap;
      v8::Persistent<v8::String> mStringAllowComponentCreationBySpawner;
      v8::Persistent<v8::String> mStringStorePropertiesToScene;
   };
}
