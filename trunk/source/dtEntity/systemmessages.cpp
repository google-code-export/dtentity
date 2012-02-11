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

#include <dtEntity/systemmessages.h>
#include <dtEntity/messagefactory.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterSystemMessages(MessageFactory& em)
   {
      em.RegisterMessageType<CameraAddedMessage>(CameraAddedMessage::TYPE);
      em.RegisterMessageType<CameraRemovedMessage>(CameraRemovedMessage::TYPE);
      em.RegisterMessageType<EndOfFrameMessage>(EndOfFrameMessage::TYPE);
      em.RegisterMessageType<EntityAddedToSceneMessage>(EntityAddedToSceneMessage::TYPE);
      em.RegisterMessageType<EntityDeselectedMessage>(EntityDeselectedMessage::TYPE);
      em.RegisterMessageType<EntityRemovedFromSceneMessage>(EntityRemovedFromSceneMessage::TYPE);
      em.RegisterMessageType<EntitySelectedMessage>(EntitySelectedMessage::TYPE);
      em.RegisterMessageType<EntitySystemAddedMessage>(EntitySystemAddedMessage::TYPE);
      em.RegisterMessageType<EntitySystemRemovedMessage>(EntitySystemRemovedMessage::TYPE);
      em.RegisterMessageType<MapBeginLoadMessage>(MapBeginLoadMessage::TYPE);
      em.RegisterMessageType<MapBeginUnloadMessage>(MapBeginUnloadMessage::TYPE);
      em.RegisterMessageType<MapLoadedMessage>(MapLoadedMessage::TYPE);
      em.RegisterMessageType<MapUnloadedMessage>(MapUnloadedMessage::TYPE);
      em.RegisterMessageType<MeshChangedMessage>(MeshChangedMessage::TYPE);
      em.RegisterMessageType<PostFrameMessage>(PostFrameMessage::TYPE);
      em.RegisterMessageType<SceneLoadedMessage>(SceneLoadedMessage::TYPE);
      em.RegisterMessageType<SceneUnloadedMessage>(SceneUnloadedMessage::TYPE);
      em.RegisterMessageType<SpawnerAddedMessage>(SpawnerAddedMessage::TYPE);
      em.RegisterMessageType<SpawnerModifiedMessage>(SpawnerModifiedMessage::TYPE);
      em.RegisterMessageType<SpawnerRemovedMessage>(SpawnerRemovedMessage::TYPE);
      em.RegisterMessageType<StartSystemMessage>(StartSystemMessage::TYPE);
      em.RegisterMessageType<StopSystemMessage>(StopSystemMessage::TYPE);
      em.RegisterMessageType<TickMessage>(TickMessage::TYPE);
      em.RegisterMessageType<TimeChangedMessage>(TimeChangedMessage::TYPE);
      em.RegisterMessageType<VisibilityChangedMessage>(VisibilityChangedMessage::TYPE);
      em.RegisterMessageType<WindowCreatedMessage>(WindowCreatedMessage::TYPE);

   }
   ////////////////////////////////////////////////////////////////////////////////
   const MessageType CameraAddedMessage::TYPE(SID("CameraAddedMessage"));
   const StringId CameraAddedMessage::AboutEntityId(SID("AboutEntity"));
   const StringId CameraAddedMessage::ContextIdId(SID("ContextId"));

   CameraAddedMessage::CameraAddedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(ContextIdId, &mContextId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType CameraRemovedMessage::TYPE(SID("CameraRemovedMessage"));
   const StringId CameraRemovedMessage::AboutEntityId(SID("AboutEntity"));

   CameraRemovedMessage::CameraRemovedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const MessageType CloseWindowMessage::TYPE(dtEntity::SID("CloseWindowMessage"));
   const StringId CloseWindowMessage::NameId(dtEntity::SID("Name"));

   CloseWindowMessage::CloseWindowMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EndOfFrameMessage::TYPE(SID("EndOfFrameMessage"));

   EndOfFrameMessage::EndOfFrameMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityAddedToSceneMessage::TYPE(SID("EntityAddedToSceneMessage"));
   const StringId EntityAddedToSceneMessage::AboutEntityId(SID("AboutEntity"));
   const StringId EntityAddedToSceneMessage::EntityNameId(SID("EntityName"));
   const StringId EntityAddedToSceneMessage::UniqueIdId(SID("UniqueId"));
   const StringId EntityAddedToSceneMessage::MapNameId(SID("MapName"));

   EntityAddedToSceneMessage::EntityAddedToSceneMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(EntityNameId, &mEntityName);
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(MapNameId, &mMapName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityDeselectedMessage::TYPE(SID("EntityDeselectedMessage"));
   const StringId EntityDeselectedMessage::AboutEntityId(SID("AboutEntity"));

   EntityDeselectedMessage::EntityDeselectedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityRemovedFromSceneMessage::TYPE(SID("EntityRemovedFromSceneMessage"));
   const StringId EntityRemovedFromSceneMessage::AboutEntityId(SID("AboutEntity"));
   const StringId EntityRemovedFromSceneMessage::EntityNameId(SID("EntityName"));
   const StringId EntityRemovedFromSceneMessage::UniqueIdId(SID("UniqueId"));
   const StringId EntityRemovedFromSceneMessage::MapNameId(SID("MapName"));

   EntityRemovedFromSceneMessage::EntityRemovedFromSceneMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(EntityNameId, &mEntityName);
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(MapNameId, &mMapName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySelectedMessage::TYPE(SID("EntitySelectedMessage"));
   const StringId EntitySelectedMessage::AboutEntityId(SID("AboutEntity"));

   EntitySelectedMessage::EntitySelectedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemAddedMessage::TYPE(SID("EntitySystemAddedMessage"));
   const StringId EntitySystemAddedMessage::ComponentTypeId(SID("ComponentType"));
   const StringId EntitySystemAddedMessage::ComponentTypeStringId(SID("ComponentTypeString"));

   EntitySystemAddedMessage::EntitySystemAddedMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(ComponentTypeStringId, &mComponentTypeString);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemRemovedMessage::TYPE(SID("EntitySystemRemovedMessage"));
   const StringId EntitySystemRemovedMessage::ComponentTypeId(SID("ComponentType"));
   const StringId EntitySystemRemovedMessage::ComponentTypeStringId(SID("ComponentTypeString"));

   EntitySystemRemovedMessage::EntitySystemRemovedMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(ComponentTypeStringId, &mComponentTypeString);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType TickMessage::TYPE(SID("TickMessage"));
   const StringId TickMessage::DeltaSimTimeId(SID("DeltaSimTime"));
   const StringId TickMessage::DeltaRealTimeId(SID("DeltaRealTime"));
   const StringId TickMessage::SimTimeScaleId(SID("SimTimeScale"));
   const StringId TickMessage::SimulationTimeId(SID("SimulationTime"));

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType PostFrameMessage::TYPE(SID("PostFrameMessage"));

   PostFrameMessage::PostFrameMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerAddedMessage::TYPE(SID("SpawnerAddedMessage"));
   const StringId SpawnerAddedMessage::NameId(SID("Name"));
   const StringId SpawnerAddedMessage::MapNameId(SID("MapName"));
   const StringId SpawnerAddedMessage::ParentNameId(SID("ParentName"));

   SpawnerAddedMessage::SpawnerAddedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(ParentNameId, &mParentName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerModifiedMessage::TYPE(SID("SpawnerModifiedMessage"));
   const StringId SpawnerModifiedMessage::NameId(SID("Name"));
   const StringId SpawnerModifiedMessage::MapNameId(SID("MapName"));
   const StringId SpawnerModifiedMessage::OldCategoryId(SID("OldCategory"));
   const StringId SpawnerModifiedMessage::NewCategoryId(SID("NewCategory"));

   SpawnerModifiedMessage::SpawnerModifiedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(OldCategoryId, &mOldCategory);
      this->Register(NewCategoryId, &mNewCategory);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerRemovedMessage::TYPE(SID("SpawnerRemovedMessage"));
   const StringId SpawnerRemovedMessage::NameId(SID("Name"));
   const StringId SpawnerRemovedMessage::MapNameId(SID("MapName"));
   const StringId SpawnerRemovedMessage::CategoryId(SID("Category"));

   SpawnerRemovedMessage::SpawnerRemovedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(CategoryId, &mCategory);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapBeginLoadMessage::TYPE(SID("MapBeginLoadMessage"));
   const StringId MapBeginLoadMessage::MapPathId(SID("MapPath"));

   MapBeginLoadMessage::MapBeginLoadMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapBeginUnloadMessage::TYPE(SID("MapBeginUnloadMessage"));
   const StringId MapBeginUnloadMessage::MapPathId(SID("MapPath"));

   MapBeginUnloadMessage::MapBeginUnloadMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapLoadedMessage::TYPE(SID("MapLoadedMessage"));
   const StringId MapLoadedMessage::MapPathId(SID("MapPath"));

   MapLoadedMessage::MapLoadedMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapUnloadedMessage::TYPE(SID("MapUnloadedMessage"));
   const StringId MapUnloadedMessage::MapPathId(SID("MapPath"));

   MapUnloadedMessage::MapUnloadedMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MeshChangedMessage::TYPE(SID("MeshChangedMessage"));
   const StringId MeshChangedMessage::AboutEntityId(SID("AboutEntity"));
   const StringId MeshChangedMessage::FilePathId(SID("FilePathId"));

   MeshChangedMessage::MeshChangedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(FilePathId, &mFilePath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SceneLoadedMessage::TYPE(SID("SceneLoadedMessage"));
   const StringId SceneLoadedMessage::SceneNameId(SID("SceneName"));

    SceneLoadedMessage::SceneLoadedMessage()
      : Message(TYPE)
   {
      this->Register(SceneNameId, &mSceneName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SceneUnloadedMessage::TYPE(SID("SceneUnloadedMessage"));

   SceneUnloadedMessage::SceneUnloadedMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType StartSystemMessage::TYPE(SID("StartSystemMessage"));

   StartSystemMessage::StartSystemMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType StopSystemMessage::TYPE(SID("StopSystemMessage"));

   StopSystemMessage::StopSystemMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   TickMessage::TickMessage() 
      : Message(TYPE)
   {
      this->Register(DeltaSimTimeId, &mDeltaSimTime);
      this->Register(DeltaRealTimeId, &mDeltaRealTime);
      this->Register(SimTimeScaleId, &mSimTimeScale);
      this->Register(SimulationTimeId, &mSimulationTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   TickMessage::TickMessage(MessageType t) 
      : Message(t)
   {
      this->Register(DeltaSimTimeId, &mDeltaSimTime);
      this->Register(DeltaRealTimeId, &mDeltaRealTime);
      this->Register(SimTimeScaleId, &mSimTimeScale);
      this->Register(SimulationTimeId, &mSimulationTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType TimeChangedMessage::TYPE(SID("TimeChangedMessage"));
   const StringId TimeChangedMessage::SimulationTimeId(SID("SimulationTime"));
   const StringId TimeChangedMessage::TimeScaleId(SID("TimeScale"));
   
   TimeChangedMessage::TimeChangedMessage() 
      : Message(TYPE)
   {
      this->Register(SimulationTimeId, &mSimulationTime);
      this->Register(TimeScaleId, &mTimeScale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType VisibilityChangedMessage::TYPE(SID("VisibilityChangedMessage"));
   const StringId VisibilityChangedMessage::AboutEntityId(SID("AboutEntity"));
   const StringId VisibilityChangedMessage::VisibleId(SID("Visible"));

   VisibilityChangedMessage::VisibilityChangedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(VisibleId, &mVisible);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const MessageType WindowCreatedMessage::TYPE(dtEntity::SID("WindowCreatedMessage"));
   const StringId WindowCreatedMessage::NameId(dtEntity::SID("Name"));
   const StringId WindowCreatedMessage::ContextIdId(dtEntity::SID("ContextId"));
   const StringId WindowCreatedMessage::CameraEntityIdId(dtEntity::SID("CameraEntityId"));
   
   WindowCreatedMessage::WindowCreatedMessage() 
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(ContextIdId, &mContextId);
      this->Register(CameraEntityIdId, &mCameraEntityId);
   }

}
