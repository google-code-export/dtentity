/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
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
      em.RegisterMessageType<PostUpdateMessage>(PostUpdateMessage::TYPE);
      em.RegisterMessageType<EntityAddedToSceneMessage>(EntityAddedToSceneMessage::TYPE);
      em.RegisterMessageType<EntityDeselectedMessage>(EntityDeselectedMessage::TYPE);
      em.RegisterMessageType<EntityNameUpdatedMessage>(EntityNameUpdatedMessage::TYPE);
      em.RegisterMessageType<EntityRemovedFromSceneMessage>(EntityRemovedFromSceneMessage::TYPE);
      em.RegisterMessageType<EntitySelectedMessage>(EntitySelectedMessage::TYPE);
      em.RegisterMessageType<EntitySpawnedMessage>(EntitySpawnedMessage::TYPE);
      em.RegisterMessageType<EntitySystemAddedMessage>(EntitySystemAddedMessage::TYPE);
      em.RegisterMessageType<EntitySystemRemovedMessage>(EntitySystemRemovedMessage::TYPE);
      em.RegisterMessageType<EntityVelocityNotNullMessage>(EntityVelocityNotNullMessage::TYPE);
      em.RegisterMessageType<MapBeginLoadMessage>(MapBeginLoadMessage::TYPE);
      em.RegisterMessageType<MapBeginUnloadMessage>(MapBeginUnloadMessage::TYPE);
      em.RegisterMessageType<MapLoadedMessage>(MapLoadedMessage::TYPE);
      em.RegisterMessageType<MapUnloadedMessage>(MapUnloadedMessage::TYPE);
      em.RegisterMessageType<MeshChangedMessage>(MeshChangedMessage::TYPE);
      em.RegisterMessageType<ResourceChangedMessage>(ResourceChangedMessage::TYPE);
      em.RegisterMessageType<ResourceLoadedMessage>(ResourceLoadedMessage::TYPE);      
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
      em.RegisterMessageType<WindowClosedMessage>(WindowClosedMessage::TYPE);      
            
   }
   ////////////////////////////////////////////////////////////////////////////////
   const MessageType CameraAddedMessage::TYPE(dtEntity::SID("CameraAddedMessage"));
   const StringId CameraAddedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId CameraAddedMessage::ContextIdId(dtEntity::SID("ContextId"));

   CameraAddedMessage::CameraAddedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(ContextIdId, &mContextId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType CameraRemovedMessage::TYPE(dtEntity::SID("CameraRemovedMessage"));
   const StringId CameraRemovedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   CameraRemovedMessage::CameraRemovedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const MessageType InternalCloseWindowMessage::TYPE(dtEntity::SID("InternalCloseWindowMessage"));
   const StringId InternalCloseWindowMessage::NameId(dtEntity::SID("Name"));

   InternalCloseWindowMessage::InternalCloseWindowMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EndOfFrameMessage::TYPE(dtEntity::SID("EndOfFrameMessage"));

   EndOfFrameMessage::EndOfFrameMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType PostUpdateMessage::TYPE(dtEntity::SID("PostUpdateMessage"));

   PostUpdateMessage::PostUpdateMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityAddedToSceneMessage::TYPE(dtEntity::SID("EntityAddedToSceneMessage"));
   const StringId EntityAddedToSceneMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId EntityAddedToSceneMessage::EntityNameId(dtEntity::SID("EntityName"));
   const StringId EntityAddedToSceneMessage::UniqueIdId(dtEntity::SID("UniqueId"));
   const StringId EntityAddedToSceneMessage::MapNameId(dtEntity::SID("MapName"));
   const StringId EntityAddedToSceneMessage::VisibleInEntityListId(dtEntity::SID("VisibleInEntityList"));

   EntityAddedToSceneMessage::EntityAddedToSceneMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(EntityNameId, &mEntityName);
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(MapNameId, &mMapName);
      this->Register(VisibleInEntityListId, &mVisibleInEntityList);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityDeselectedMessage::TYPE(dtEntity::SID("EntityDeselectedMessage"));
   const StringId EntityDeselectedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   EntityDeselectedMessage::EntityDeselectedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityNameUpdatedMessage::TYPE(dtEntity::SID("EntityNameUpdatedMessage"));
   const StringId EntityNameUpdatedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId EntityNameUpdatedMessage::EntityNameId(dtEntity::SID("EntityName"));
   const StringId EntityNameUpdatedMessage::UniqueIdId(dtEntity::SID("UniqueId"));

   EntityNameUpdatedMessage::EntityNameUpdatedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
      Register(EntityNameId, &mEntityName);
      Register(UniqueIdId, &mUniqueId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityRemovedFromSceneMessage::TYPE(dtEntity::SID("EntityRemovedFromSceneMessage"));
   const StringId EntityRemovedFromSceneMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId EntityRemovedFromSceneMessage::EntityNameId(dtEntity::SID("EntityName"));
   const StringId EntityRemovedFromSceneMessage::UniqueIdId(dtEntity::SID("UniqueId"));
   const StringId EntityRemovedFromSceneMessage::MapNameId(dtEntity::SID("MapName"));
   const StringId EntityRemovedFromSceneMessage::VisibleInEntityListId(dtEntity::SID("VisibleInEntityList"));

   EntityRemovedFromSceneMessage::EntityRemovedFromSceneMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(EntityNameId, &mEntityName);
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(MapNameId, &mMapName);
      this->Register(VisibleInEntityListId, &mVisibleInEntityList);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySelectedMessage::TYPE(dtEntity::SID("EntitySelectedMessage"));
   const StringId EntitySelectedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   EntitySelectedMessage::EntitySelectedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySpawnedMessage::TYPE(dtEntity::SID("EntitySpawnedMessage"));
   const StringId EntitySpawnedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId EntitySpawnedMessage::SpawnerNameId(dtEntity::SID("SpawnerName"));

   EntitySpawnedMessage::EntitySpawnedMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
      Register(SpawnerNameId, &mSpawnerName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemAddedMessage::TYPE(dtEntity::SID("EntitySystemAddedMessage"));
   const StringId EntitySystemAddedMessage::ComponentTypeId(dtEntity::SID("ComponentType"));
   const StringId EntitySystemAddedMessage::ComponentTypeStringId(dtEntity::SID("ComponentTypeString"));

   EntitySystemAddedMessage::EntitySystemAddedMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(ComponentTypeStringId, &mComponentTypeString);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemRemovedMessage::TYPE(dtEntity::SID("EntitySystemRemovedMessage"));
   const StringId EntitySystemRemovedMessage::ComponentTypeId(dtEntity::SID("ComponentType"));
   const StringId EntitySystemRemovedMessage::ComponentTypeStringId(dtEntity::SID("ComponentTypeString"));

   EntitySystemRemovedMessage::EntitySystemRemovedMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(ComponentTypeStringId, &mComponentTypeString);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityVelocityNotNullMessage::TYPE(dtEntity::SID("EntityVelocityNotNullMessage"));
   const StringId EntityVelocityNotNullMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId EntityVelocityNotNullMessage::IsNullId(dtEntity::SID("IsNull"));


   EntityVelocityNotNullMessage::EntityVelocityNotNullMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
      Register(IsNullId, &mIsNull);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType TickMessage::TYPE(dtEntity::SID("TickMessage"));
   const StringId TickMessage::DeltaSimTimeId(dtEntity::SID("DeltaSimTime"));
   const StringId TickMessage::DeltaRealTimeId(dtEntity::SID("DeltaRealTime"));
   const StringId TickMessage::SimTimeScaleId(dtEntity::SID("SimTimeScale"));
   const StringId TickMessage::SimulationTimeId(dtEntity::SID("SimulationTime"));


   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ResourceChangedMessage::TYPE(dtEntity::SID("ResourceChanged"));
   const StringId ResourceChangedMessage::PathId(dtEntity::SID("Path"));
   
   ResourceChangedMessage::ResourceChangedMessage()
      : Message(TYPE)
   {
      this->Register(PathId, &mPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ResourceLoadedMessage::TYPE(dtEntity::SID("ResourceLoadedMessage"));
   const StringId ResourceLoadedMessage::PathId(dtEntity::SID("Path"));
   
   ResourceLoadedMessage::ResourceLoadedMessage()
      : Message(TYPE)
   {
      this->Register(PathId, &mPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerAddedMessage::TYPE(dtEntity::SID("SpawnerAddedMessage"));
   const StringId SpawnerAddedMessage::NameId(dtEntity::SID("Name"));
   const StringId SpawnerAddedMessage::MapNameId(dtEntity::SID("MapName"));
   const StringId SpawnerAddedMessage::ParentNameId(dtEntity::SID("ParentName"));

   SpawnerAddedMessage::SpawnerAddedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(ParentNameId, &mParentName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerModifiedMessage::TYPE(dtEntity::SID("SpawnerModifiedMessage"));
   const StringId SpawnerModifiedMessage::NameId(dtEntity::SID("Name"));
   const StringId SpawnerModifiedMessage::MapNameId(dtEntity::SID("MapName"));
   const StringId SpawnerModifiedMessage::OldCategoryId(dtEntity::SID("OldCategory"));
   const StringId SpawnerModifiedMessage::NewCategoryId(dtEntity::SID("NewCategory"));

   SpawnerModifiedMessage::SpawnerModifiedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(OldCategoryId, &mOldCategory);
      this->Register(NewCategoryId, &mNewCategory);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnerRemovedMessage::TYPE(dtEntity::SID("SpawnerRemovedMessage"));
   const StringId SpawnerRemovedMessage::NameId(dtEntity::SID("Name"));
   const StringId SpawnerRemovedMessage::MapNameId(dtEntity::SID("MapName"));
   const StringId SpawnerRemovedMessage::CategoryId(dtEntity::SID("Category"));

   SpawnerRemovedMessage::SpawnerRemovedMessage()
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
      this->Register(CategoryId, &mCategory);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapBeginLoadMessage::TYPE(dtEntity::SID("MapBeginLoadMessage"));
   const StringId MapBeginLoadMessage::MapPathId(dtEntity::SID("MapPath"));
   const StringId MapBeginLoadMessage::DataPathId(dtEntity::SID("DataPath"));
   const StringId MapBeginLoadMessage::SaveOrderId(dtEntity::SID("SaveOrder"));

   MapBeginLoadMessage::MapBeginLoadMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
      this->Register(DataPathId, &mDataPath);
      this->Register(SaveOrderId, &mSaveOrder);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapBeginUnloadMessage::TYPE(dtEntity::SID("MapBeginUnloadMessage"));
   const StringId MapBeginUnloadMessage::MapPathId(dtEntity::SID("MapPath"));

   MapBeginUnloadMessage::MapBeginUnloadMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapLoadedMessage::TYPE(dtEntity::SID("MapLoadedMessage"));
   const StringId MapLoadedMessage::MapPathId(dtEntity::SID("MapPath"));
   const StringId MapLoadedMessage::DataPathId(dtEntity::SID("DataPath"));
   const StringId MapLoadedMessage::SaveOrderId(dtEntity::SID("SaveOrder"));

   MapLoadedMessage::MapLoadedMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
      this->Register(DataPathId, &mDataPath);
      this->Register(SaveOrderId, &mSaveOrder);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MapUnloadedMessage::TYPE(dtEntity::SID("MapUnloadedMessage"));
   const StringId MapUnloadedMessage::MapPathId(dtEntity::SID("MapPath"));

   MapUnloadedMessage::MapUnloadedMessage()
      : Message(TYPE)
   {
      this->Register(MapPathId, &mMapPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MeshChangedMessage::TYPE(dtEntity::SID("MeshChangedMessage"));
   const StringId MeshChangedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId MeshChangedMessage::FilePathId(dtEntity::SID("FilePathId"));

   MeshChangedMessage::MeshChangedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(FilePathId, &mFilePath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SceneLoadedMessage::TYPE(dtEntity::SID("SceneLoadedMessage"));
   const StringId SceneLoadedMessage::SceneNameId(dtEntity::SID("SceneName"));

    SceneLoadedMessage::SceneLoadedMessage()
      : Message(TYPE)
   {
      this->Register(SceneNameId, &mSceneName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SceneUnloadedMessage::TYPE(dtEntity::SID("SceneUnloadedMessage"));

   SceneUnloadedMessage::SceneUnloadedMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType StartSystemMessage::TYPE(dtEntity::SID("StartSystemMessage"));

   StartSystemMessage::StartSystemMessage()
      : Message(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType StopSystemMessage::TYPE(dtEntity::SID("StopSystemMessage"));

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
   const MessageType TimeChangedMessage::TYPE(dtEntity::SID("TimeChangedMessage"));
   const StringId TimeChangedMessage::SimulationTimeId(dtEntity::SID("SimulationTime"));
   const StringId TimeChangedMessage::TimeScaleId(dtEntity::SID("TimeScale"));
   
   TimeChangedMessage::TimeChangedMessage() 
      : Message(TYPE)
   {
      this->Register(SimulationTimeId, &mSimulationTime);
      this->Register(TimeScaleId, &mTimeScale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType VisibilityChangedMessage::TYPE(dtEntity::SID("VisibilityChangedMessage"));
   const StringId VisibilityChangedMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId VisibilityChangedMessage::VisibleId(dtEntity::SID("Visible"));

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

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const MessageType WindowClosedMessage::TYPE(dtEntity::SID("WindowClosedMessage"));
   const StringId WindowClosedMessage::NameId(dtEntity::SID("Name"));
   
   WindowClosedMessage::WindowClosedMessage() 
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
   }

}
