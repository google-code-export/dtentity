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

#include <dtEntity/basemessages.h>
#include <dtEntity/entitymanager.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterMessages(EntityManager& em)
   {
      em.RegisterMessageType<CameraAddedMessage>(CameraAddedMessage::TYPE);
      em.RegisterMessageType<CameraRemovedMessage>(CameraRemovedMessage::TYPE);
      em.RegisterMessageType<DeleteEntityMessage>(DeleteEntityMessage::TYPE);
      em.RegisterMessageType<EnableDebugDrawingMessage>(EnableDebugDrawingMessage::TYPE);
      em.RegisterMessageType<EndOfFrameMessage>(EndOfFrameMessage::TYPE);
      em.RegisterMessageType<EntityAddedToSceneMessage>(EntityAddedToSceneMessage::TYPE);
      em.RegisterMessageType<EntityDeselectedMessage>(EntityDeselectedMessage::TYPE);
      em.RegisterMessageType<EntityRemovedFromSceneMessage>(EntityRemovedFromSceneMessage::TYPE);
      em.RegisterMessageType<EntitySelectedMessage>(EntitySelectedMessage::TYPE);
      em.RegisterMessageType<EntitySystemAddedMessage>(EntitySystemAddedMessage::TYPE);
      em.RegisterMessageType<EntitySystemRemovedMessage>(EntitySystemRemovedMessage::TYPE);
      em.RegisterMessageType<FrameSynchMessage>(FrameSynchMessage::TYPE);
      em.RegisterMessageType<MapBeginLoadMessage>(MapBeginLoadMessage::TYPE);
      em.RegisterMessageType<MapBeginUnloadMessage>(MapBeginUnloadMessage::TYPE);      
      em.RegisterMessageType<MapLoadedMessage>(MapLoadedMessage::TYPE);
      em.RegisterMessageType<MapUnloadedMessage>(MapUnloadedMessage::TYPE);
      em.RegisterMessageType<MeshChangedMessage>(MeshChangedMessage::TYPE);
      em.RegisterMessageType<PlayAnimationMessage>(PlayAnimationMessage::TYPE);
      em.RegisterMessageType<PostEventTraversalMessage>(PostEventTraversalMessage::TYPE);
      em.RegisterMessageType<PostFrameMessage>(PostFrameMessage::TYPE);
      em.RegisterMessageType<RequestEntityDeselectMessage>(RequestEntityDeselectMessage::TYPE);
      em.RegisterMessageType<RequestEntitySelectMessage>(RequestEntitySelectMessage::TYPE);
      em.RegisterMessageType<RequestToggleEntitySelectionMessage>(RequestToggleEntitySelectionMessage::TYPE);
      em.RegisterMessageType<ResetSystemMessage>(ResetSystemMessage::TYPE);
      em.RegisterMessageType<SceneLoadedMessage>(SceneLoadedMessage::TYPE);
      em.RegisterMessageType<SceneUnloadedMessage>(SceneUnloadedMessage::TYPE);
      em.RegisterMessageType<SetComponentPropertiesMessage>(SetComponentPropertiesMessage::TYPE);
      em.RegisterMessageType<SetSystemPropertiesMessage>(SetSystemPropertiesMessage::TYPE);
      em.RegisterMessageType<SpawnEntityMessage>(SpawnEntityMessage::TYPE);
      em.RegisterMessageType<SpawnerAddedMessage>(SpawnerAddedMessage::TYPE);      
      em.RegisterMessageType<SpawnerModifiedMessage>(SpawnerModifiedMessage::TYPE);
      em.RegisterMessageType<SpawnerRemovedMessage>(SpawnerRemovedMessage::TYPE);            
      em.RegisterMessageType<StartSystemMessage>(StartSystemMessage::TYPE); 
      em.RegisterMessageType<TickMessage>(TickMessage::TYPE);
      em.RegisterMessageType<TimeChangedMessage>(TimeChangedMessage::TYPE);
      em.RegisterMessageType<ToolActivatedMessage>(ToolActivatedMessage::TYPE);
      em.RegisterMessageType<ToolsUpdatedMessage>(ToolsUpdatedMessage::TYPE);
      em.RegisterMessageType<VisibilityChangedMessage>(VisibilityChangedMessage::TYPE);
      em.RegisterMessageType<WindowCreatedMessage>(WindowCreatedMessage::TYPE);      
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType TickMessage::TYPE(SID("TickMessage"));
   const StringId TickMessage::DeltaSimTimeId(SID("DeltaSimTime"));
   const StringId TickMessage::DeltaRealTimeId(SID("DeltaRealTime"));
   const StringId TickMessage::SimTimeScaleId(SID("SimTimeScale"));
   const StringId TickMessage::SimulationTimeId(SID("SimulationTime"));

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
   const MessageType PostEventTraversalMessage::TYPE(SID("PostEventTraversalMessage"));

   PostEventTraversalMessage::PostEventTraversalMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EndOfFrameMessage::TYPE(SID("EndOfFrameMessage"));

   EndOfFrameMessage::EndOfFrameMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType FrameSynchMessage::TYPE(SID("FrameSynchMessage"));

   FrameSynchMessage::FrameSynchMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType PostFrameMessage::TYPE(SID("PostFrameMessage"));

   PostFrameMessage::PostFrameMessage()
      : TickMessage(TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityAddedToSceneMessage::TYPE(SID("EntityAddedToSceneMessage"));
   const StringId EntityAddedToSceneMessage::AboutEntityId(SID("AboutEntity"));

   EntityAddedToSceneMessage::EntityAddedToSceneMessage() 
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntityRemovedFromSceneMessage::TYPE(SID("EntityRemovedFromSceneMessage"));
   const StringId EntityRemovedFromSceneMessage::AboutEntityId(SID("AboutEntity"));

   EntityRemovedFromSceneMessage::EntityRemovedFromSceneMessage() 
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemAddedMessage::TYPE(SID("EntitySystemAddedMessage"));
   const StringId EntitySystemAddedMessage::ComponentTypeId(SID("ComponentType"));

   EntitySystemAddedMessage::EntitySystemAddedMessage() 
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySystemRemovedMessage::TYPE(SID("EntitySystemRemovedMessage"));
   const StringId EntitySystemRemovedMessage::ComponentTypeId(SID("ComponentType"));

   EntitySystemRemovedMessage::EntitySystemRemovedMessage() 
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
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

   SpawnerRemovedMessage::SpawnerRemovedMessage() 
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
      this->Register(MapNameId, &mMapName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnEntityMessage::TYPE(SID("SpawnEntityMessage"));
   const StringId SpawnEntityMessage::UniqueIdId(SID("UniqueId"));
   const StringId SpawnEntityMessage::SpawnerNameId(SID("SpawnerName"));
   const StringId SpawnEntityMessage::EntityNameId(SID("EntityName"));
   const StringId SpawnEntityMessage::AddToSceneId(SID("AddToScene"));

   SpawnEntityMessage::SpawnEntityMessage() 
      : Message(TYPE)
   {
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(SpawnerNameId, &mSpawnerName);
      this->Register(EntityNameId, &mEntityName);
      this->Register(AddToSceneId, &mAddToScene);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType DeleteEntityMessage::TYPE(SID("DeleteEntityMessage"));
   const StringId DeleteEntityMessage::UniqueIdId(SID("UniqueId"));

   DeleteEntityMessage::DeleteEntityMessage()
      : Message(TYPE)
   {
      this->Register(UniqueIdId, &mUniqueId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EnableDebugDrawingMessage::TYPE(SID("EnableDebugDrawingMessage"));
   const StringId EnableDebugDrawingMessage::EnableId(SID("Enable"));
   
   EnableDebugDrawingMessage::EnableDebugDrawingMessage() 
      : Message(TYPE)
   {
      this->Register(EnableId, &mEnable);
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
   const MessageType MapBeginLoadMessage::TYPE(SID("MapBeginLoadMessage"));
   const StringId MapBeginLoadMessage::MapPathId(SID("MapPath"));
   
   MapBeginLoadMessage::MapBeginLoadMessage() 
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
   const MessageType MapBeginUnloadMessage::TYPE(SID("MapBeginUnloadMessage"));
   const StringId MapBeginUnloadMessage::MapPathId(SID("MapPath"));
   
   MapBeginUnloadMessage::MapBeginUnloadMessage() 
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
   const MessageType CameraAddedMessage::TYPE(SID("CameraAddedMessage"));
   const StringId CameraAddedMessage::AboutEntityId(SID("AboutEntity"));

   CameraAddedMessage::CameraAddedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType CameraRemovedMessage::TYPE(SID("CameraRemovedMessage"));
   const StringId CameraRemovedMessage::AboutEntityId(SID("AboutEntity"));

   CameraRemovedMessage::CameraRemovedMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType PlayAnimationMessage::TYPE(SID("PlayAnimationMessage"));
   const StringId PlayAnimationMessage::AboutEntityId(SID("AboutEntity"));
   const StringId PlayAnimationMessage::AnimationNameId(SID("AnimationName"));

   PlayAnimationMessage::PlayAnimationMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(AnimationNameId, &mAnimationName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SetSystemPropertiesMessage::TYPE(SID("SetSystemPropertiesMessage"));
   const StringId SetSystemPropertiesMessage::ComponentTypeId(SID("ComponentType"));
   const StringId SetSystemPropertiesMessage::PropertiesId(SID("Properties"));

   ////////////////////////////////////////////////////////////////////////////////
   SetSystemPropertiesMessage::SetSystemPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

   ////////////////////////////////////////////////////////////////////////////////
   SetSystemPropertiesMessage::SetSystemPropertiesMessage(ComponentType t, StringId propname, Property& prop)
      : Message(TYPE)
      , mComponentType(t)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);

      PropertyGroup grp;
      grp[propname] = &prop;
      mProperties.Set(grp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SetComponentPropertiesMessage::TYPE(SID("SetComponentPropertiesMessage"));
   const StringId SetComponentPropertiesMessage::ComponentTypeId(SID("ComponentType"));
   const StringId SetComponentPropertiesMessage::EntityUniqueIdId(SID("EntityUniqueId"));
   const StringId SetComponentPropertiesMessage::PropertiesId(SID("Properties"));

   ////////////////////////////////////////////////////////////////////////////////
   SetComponentPropertiesMessage::SetComponentPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(EntityUniqueIdId, &mEntityUniqueId);
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

   ////////////////////////////////////////////////////////////////////////////////
   SetComponentPropertiesMessage::SetComponentPropertiesMessage(const std::string& uid, ComponentType t, StringId propname, Property& prop)
      : Message(TYPE)
      , mComponentType(t)
      , mEntityUniqueId(uid)
   {
      this->Register(EntityUniqueIdId, &mEntityUniqueId);
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);

      PropertyGroup grp;
      grp[propname] = &prop;
      mProperties.Set(grp);
   }


   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ResetSystemMessage::TYPE(SID("ResetSystemMessage"));
   const StringId ResetSystemMessage::SceneNameId(SID("SceneName"));

   ResetSystemMessage::ResetSystemMessage()
      : Message(TYPE)
   {
      Register(SceneNameId, &mSceneName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType StartSystemMessage::TYPE(SID("StartSystemMessage"));

   StartSystemMessage::StartSystemMessage()
      : Message(TYPE)
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ToolActivatedMessage::TYPE(SID("ToolActivatedMessage"));
   const StringId ToolActivatedMessage::ToolNameId(SID("ToolName"));

   ToolActivatedMessage::ToolActivatedMessage()
      : Message(TYPE)
   {
      Register(ToolNameId, &mToolName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ToolsUpdatedMessage::TYPE(SID("ToolsUpdatedMessage"));
   const StringId ToolsUpdatedMessage::ToolsId(SID("Tools"));
   const StringId ToolsUpdatedMessage::ToolNameId(SID("ToolName"));
   const StringId ToolsUpdatedMessage::IconPathId(SID("IconPath"));
   const StringId ToolsUpdatedMessage::ShortcutId(SID("Shortcut"));

   ToolsUpdatedMessage::ToolsUpdatedMessage()
      : Message(TYPE)
   {
      Register(ToolsId, &mTools);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const MessageType WindowCreatedMessage::TYPE(dtEntity::SID("WindowCreatedMessage"));
   const StringId WindowCreatedMessage::NameId(dtEntity::SID("Name"));
   
   WindowCreatedMessage::WindowCreatedMessage() 
      : Message(TYPE)
   {
      this->Register(NameId, &mName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EntitySelectedMessage::TYPE(SID("EntitySelectedMessage"));
   const StringId EntitySelectedMessage::AboutEntityId(SID("AboutEntity"));

   const MessageType EntityDeselectedMessage::TYPE(SID("EntityDeselectedMessage"));
   const StringId EntityDeselectedMessage::AboutEntityId(SID("AboutEntity"));

   const MessageType RequestEntitySelectMessage::TYPE(SID("RequestEntitySelectMessage"));
   const StringId RequestEntitySelectMessage::AboutEntityId(SID("AboutEntity"));
   const StringId RequestEntitySelectMessage::UseMultiSelectId(SID("UseMultiSelect"));

   const MessageType RequestEntityDeselectMessage::TYPE(SID("RequestEntityDeselectMessage"));
   const StringId RequestEntityDeselectMessage::AboutEntityId(SID("AboutEntity"));

   const MessageType RequestToggleEntitySelectionMessage::TYPE(SID("RequestToggleEntitySelectionMessage"));
   const StringId RequestToggleEntitySelectionMessage::AboutEntityId(SID("AboutEntity"));


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

}
