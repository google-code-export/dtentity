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

#include <dtEntityQtWidgets/messages.h>

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   
   const dtEntity::MessageType SpawnerSelectedMessage::TYPE(dtEntity::SID("SpawnerSelectedMessage"));
   const dtEntity::StringId SpawnerSelectedMessage::NameId(dtEntity::SID("Name"));

   const dtEntity::MessageType EntitySystemSelectedMessage::TYPE(dtEntity::SID("EntitySystemSelectedMessage"));
   const dtEntity::StringId EntitySystemSelectedMessage::NameId(dtEntity::SID("Name"));

   const dtEntity::MessageType MapSelectedMessage::TYPE(dtEntity::SID("MapSelectedMessage"));
   const dtEntity::StringId MapSelectedMessage::NameId(dtEntity::SID("Name"));

   const dtEntity::MessageType EnableMotionModelMessage::TYPE(dtEntity::SID("EnableMotionModelMessage"));
   const dtEntity::StringId EnableMotionModelMessage::EnableId(dtEntity::SID("Enable"));

   const dtEntity::MessageType EntityDblClickedMessage::TYPE(dtEntity::SID("EntityDblClickedMessage"));
   const dtEntity::StringId EntityDblClickedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));   
   const dtEntity::StringId EntityDblClickedMessage::PositionId(dtEntity::SID("Position"));

   const dtEntity::MessageType ComponentDataChangedMessage::TYPE(dtEntity::SID("ComponentDataChangedMessage"));
   const dtEntity::StringId ComponentDataChangedMessage::ComponentTypeId(dtEntity::SID("ComponentType"));
   const dtEntity::StringId ComponentDataChangedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterMessageTypes(dtEntity::EntityManager& em)
   {
      em.RegisterMessageType<SpawnerSelectedMessage>(SpawnerSelectedMessage::TYPE);
      em.RegisterMessageType<EntitySystemSelectedMessage>(EntitySystemSelectedMessage::TYPE);
      em.RegisterMessageType<MapSelectedMessage>(MapSelectedMessage::TYPE);
      em.RegisterMessageType<EnableMotionModelMessage>(EnableMotionModelMessage::TYPE);
      em.RegisterMessageType<ComponentDataChangedMessage>(ComponentDataChangedMessage::TYPE);
      em.RegisterMessageType<EntityDblClickedMessage>(EntityDblClickedMessage::TYPE);
   }
}
