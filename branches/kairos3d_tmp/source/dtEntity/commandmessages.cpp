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

#include <dtEntity/commandmessages.h>
#include <dtEntity/messagefactory.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterCommandMessages(MessageFactory& em)
   {
      em.RegisterMessageType<DeleteEntityMessage>(DeleteEntityMessage::TYPE);
      em.RegisterMessageType<EnableDebugDrawingMessage>(EnableDebugDrawingMessage::TYPE);
      em.RegisterMessageType<MovementJumpToMessage>(MovementJumpToMessage::TYPE);
      em.RegisterMessageType<PlayAnimationMessage>(PlayAnimationMessage::TYPE);
      em.RegisterMessageType<RequestEntityDeselectMessage>(RequestEntityDeselectMessage::TYPE);
      em.RegisterMessageType<RequestEntitySelectMessage>(RequestEntitySelectMessage::TYPE);
      em.RegisterMessageType<RequestToggleEntitySelectionMessage>(RequestToggleEntitySelectionMessage::TYPE);
      em.RegisterMessageType<ResetSystemMessage>(ResetSystemMessage::TYPE);
      em.RegisterMessageType<SetComponentPropertiesMessage>(SetComponentPropertiesMessage::TYPE);
      em.RegisterMessageType<SetSystemPropertiesMessage>(SetSystemPropertiesMessage::TYPE);
      em.RegisterMessageType<SpawnEntityMessage>(SpawnEntityMessage::TYPE);
      em.RegisterMessageType<ToolActivatedMessage>(ToolActivatedMessage::TYPE);
      em.RegisterMessageType<ToolsUpdatedMessage>(ToolsUpdatedMessage::TYPE);
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
   const MessageType MovementJumpToMessage::TYPE(SID("MovementJumpToMessage"));
   const StringId MovementJumpToMessage::AboutEntityId(SID("AboutEntity"));
   const StringId MovementJumpToMessage::KeepCameraDirectionId(SID("KeepCameraDirection"));
   const StringId MovementJumpToMessage::DistanceId(SID("Distance"));
   const StringId MovementJumpToMessage::ContextIdId(SID("ContextId"));

   MovementJumpToMessage::MovementJumpToMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(DistanceId, &mDistance);
      this->Register(KeepCameraDirectionId, &mKeepCameraDirection);
      this->Register(ContextIdId, &mContextId);
      mDistance.Set(10);
      mKeepCameraDirection.Set(true);
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
   const MessageType ResetSystemMessage::TYPE(SID("ResetSystemMessage"));
   const StringId ResetSystemMessage::SceneNameId(SID("SceneName"));

   ResetSystemMessage::ResetSystemMessage()
      : Message(TYPE)
   {
      Register(SceneNameId, &mSceneName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestEntityDeselectMessage::TYPE(SID("RequestEntityDeselectMessage"));
   const StringId RequestEntityDeselectMessage::AboutEntityId(SID("AboutEntity"));

   RequestEntityDeselectMessage::RequestEntityDeselectMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestEntitySelectMessage::TYPE(SID("RequestEntitySelectMessage"));
   const StringId RequestEntitySelectMessage::AboutEntityId(SID("AboutEntity"));
   const StringId RequestEntitySelectMessage::UseMultiSelectId(SID("UseMultiSelect"));

   RequestEntitySelectMessage::RequestEntitySelectMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
      Register(UseMultiSelectId, &mUseMultiSelect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestToggleEntitySelectionMessage::TYPE(SID("RequestToggleEntitySelectionMessage"));
   const StringId RequestToggleEntitySelectionMessage::AboutEntityId(SID("AboutEntity"));

   RequestToggleEntitySelectionMessage::RequestToggleEntitySelectionMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SetComponentPropertiesMessage::TYPE(SID("SetComponentPropertiesMessage"));
   const StringId SetComponentPropertiesMessage::ComponentTypeId(SID("ComponentType"));
   const StringId SetComponentPropertiesMessage::EntityUniqueIdId(SID("EntityUniqueId"));
   const StringId SetComponentPropertiesMessage::PropertiesId(SID("Properties"));

   SetComponentPropertiesMessage::SetComponentPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(EntityUniqueIdId, &mEntityUniqueId);
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

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
   const MessageType SetSystemPropertiesMessage::TYPE(SID("SetSystemPropertiesMessage"));
   const StringId SetSystemPropertiesMessage::ComponentTypeId(SID("ComponentType"));
   const StringId SetSystemPropertiesMessage::PropertiesId(SID("Properties"));

   SetSystemPropertiesMessage::SetSystemPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

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




}
