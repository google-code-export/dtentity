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

#include <dtEntityQtWidgets/export.h>

#include <dtEntity/entityid.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <osg/ref_ptr>

namespace dtEntity
{
   class MessageFactory;
}

namespace dtEntityQtWidgets
{   

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT SpawnerSelectedMessage
      : public dtEntity::Message
   {
   public:
      
      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId NameId;
     
      SpawnerSelectedMessage()
         : dtEntity::Message(TYPE)
      {
         Register(NameId, &mName);
      }
      
      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<SpawnerSelectedMessage>(); }

      void SetName(const std::string& n) { mName.Set(n); }
      std::string GetName() const { return mName.Get(); }

   private:

      dtEntity::StringProperty mName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntitySystemSelectedMessage
      : public dtEntity::Message
   {
   public:
      
      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId NameId;
     
      EntitySystemSelectedMessage()
         : dtEntity::Message(TYPE)
      {
         Register(NameId, &mName);
      }
      
      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<EntitySystemSelectedMessage>(); }

      void SetName(const std::string& n) { mName.Set(n); }
      std::string GetName() const { return mName.Get(); }

   private:

      dtEntity::StringProperty mName;
   }; 

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT MapSelectedMessage
      : public dtEntity::Message
   {
   public:
      
      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId NameId;
     
      MapSelectedMessage()
         : dtEntity::Message(TYPE)
      {
         Register(NameId, &mName);
      }
      
      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<MapSelectedMessage>(); }

      void SetName(const std::string& n) { mName.Set(n); }
      std::string GetName() const { return mName.Get(); }

   private:

      dtEntity::StringProperty mName;
   }; 

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EnableMotionModelMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId EnableId;

      EnableMotionModelMessage()
         : dtEntity::Message(TYPE)
      {
         Register(EnableId, &mEnable);
      }

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<EnableMotionModelMessage>(); }

      void SetEnable(bool n) { mEnable.Set(n); }
      bool GetEnable() const { return mEnable.Get(); }

   private:

      dtEntity::BoolProperty mEnable;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityDblClickedMessage
      : public dtEntity::Message
   {
   public:
      
      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId AboutEntityId;
      static const dtEntity::StringId PositionId;
     
      EntityDblClickedMessage()
         : dtEntity::Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
         Register(PositionId, &mPosition);
      }
      
      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<EntityDblClickedMessage>(); }

      void SetAboutEntityId(dtEntity::EntityId id) { mAboutEntity.Set(id); }
      dtEntity::EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetPosition(const dtEntity::Vec3f& p) { mPosition.Set(p); }
      dtEntity::Vec3f GetPosition() const { return mPosition.Get(); }

   private:

      dtEntity::UIntProperty mAboutEntity;
      dtEntity::Vec3Property mPosition;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT ComponentDataChangedMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId ComponentTypeId;
      static const dtEntity::StringId AboutEntityId;

      ComponentDataChangedMessage()
         : dtEntity::Message(TYPE)
      {
         Register(ComponentTypeId, &mComponentType);
         Register(AboutEntityId, &mAboutEntity);
      }

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<ComponentDataChangedMessage>(); }

      void SetComponentType(dtEntity::StringId n) { mComponentType.Set(n); }
      dtEntity::StringId GetComponentType() const { return mComponentType.Get(); }

      void SetAboutEntityId(dtEntity::EntityId id) { mAboutEntity.Set(id); }
      dtEntity::EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      dtEntity::StringIdProperty mComponentType;
      dtEntity::UIntProperty mAboutEntity;
   };
   
   ////////////////////////////////////////////////////////////////////////////////
    
   void ENTITYQTWIDGETS_EXPORT RegisterMessageTypes(dtEntity::MessageFactory& mf);

}

