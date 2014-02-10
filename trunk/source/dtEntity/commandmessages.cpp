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

#include <dtEntity/commandmessages.h>
#include <dtEntity/messagefactory.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterCommandMessages(MessageFactory& em)
   {
      em.RegisterMessageType<DeleteEntityMessage>(DeleteEntityMessage::TYPE);
      em.RegisterMessageType<EnableDebugDrawingMessage>(EnableDebugDrawingMessage::TYPE);
      em.RegisterMessageType<MoveCameraToEntityMessage>(MoveCameraToEntityMessage::TYPE);
      em.RegisterMessageType<MoveCameraToPositionMessage>(MoveCameraToPositionMessage::TYPE);
      em.RegisterMessageType<PlayAnimationMessage>(PlayAnimationMessage::TYPE);
      em.RegisterMessageType<RequestEntityDeselectMessage>(RequestEntityDeselectMessage::TYPE);
      em.RegisterMessageType<RequestEntitySelectMessage>(RequestEntitySelectMessage::TYPE);
      em.RegisterMessageType<RequestToggleEntitySelectionMessage>(RequestToggleEntitySelectionMessage::TYPE);
      em.RegisterMessageType<SetComponentPropertiesMessage>(SetComponentPropertiesMessage::TYPE);
      em.RegisterMessageType<SetSystemPropertiesMessage>(SetSystemPropertiesMessage::TYPE);
      em.RegisterMessageType<SpawnEntityMessage>(SpawnEntityMessage::TYPE);
      em.RegisterMessageType<ToolActivatedMessage>(ToolActivatedMessage::TYPE);
      em.RegisterMessageType<ToolsUpdatedMessage>(ToolsUpdatedMessage::TYPE);
   }


   ////////////////////////////////////////////////////////////////////////////////
   const MessageType DeleteEntityMessage::TYPE(dtEntity::SID("DeleteEntityMessage"));
   const StringId DeleteEntityMessage::UniqueIdId(dtEntity::SID("UniqueId"));

   DeleteEntityMessage::DeleteEntityMessage()
      : Message(TYPE)
   {
      this->Register(UniqueIdId, &mUniqueId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType EnableDebugDrawingMessage::TYPE(dtEntity::SID("EnableDebugDrawingMessage"));
   const StringId EnableDebugDrawingMessage::EnableId(dtEntity::SID("Enable"));
   
   EnableDebugDrawingMessage::EnableDebugDrawingMessage() 
      : Message(TYPE)
   {
      this->Register(EnableId, &mEnable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType MoveCameraToEntityMessage::TYPE(dtEntity::SID("MoveCameraToEntityMessage"));
   const StringId MoveCameraToEntityMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId MoveCameraToEntityMessage::KeepCameraDirectionId(dtEntity::SID("KeepCameraDirection"));
   const StringId MoveCameraToEntityMessage::DistanceId(dtEntity::SID("Distance"));
   const StringId MoveCameraToEntityMessage::ContextIdId(dtEntity::SID("ContextId"));

   MoveCameraToEntityMessage::MoveCameraToEntityMessage()
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
   const MessageType MoveCameraToPositionMessage::TYPE(dtEntity::SID("MoveCameraToPositionMessage"));
   const StringId MoveCameraToPositionMessage::PositionId(dtEntity::SID("Position"));
   const StringId MoveCameraToPositionMessage::LookAtId(dtEntity::SID("LookAt"));
   const StringId MoveCameraToPositionMessage::UpId(dtEntity::SID("Up"));
   const StringId MoveCameraToPositionMessage::ContextIdId(dtEntity::SID("ContextId"));

   MoveCameraToPositionMessage::MoveCameraToPositionMessage()
      : Message(TYPE)
      , mUp(0,0,1)
   {
      this->Register(PositionId, &mPosition);
      this->Register(LookAtId, &mLookAt);
      this->Register(UpId, &mUp);
      this->Register(ContextIdId, &mContextId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType PlayAnimationMessage::TYPE(dtEntity::SID("PlayAnimationMessage"));
   const StringId PlayAnimationMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId PlayAnimationMessage::AnimationNameId(dtEntity::SID("AnimationName"));

   PlayAnimationMessage::PlayAnimationMessage()
      : Message(TYPE)
   {
      this->Register(AboutEntityId, &mAboutEntityId);
      this->Register(AnimationNameId, &mAnimationName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestEntityDeselectMessage::TYPE(dtEntity::SID("RequestEntityDeselectMessage"));
   const StringId RequestEntityDeselectMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   RequestEntityDeselectMessage::RequestEntityDeselectMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestEntitySelectMessage::TYPE(dtEntity::SID("RequestEntitySelectMessage"));
   const StringId RequestEntitySelectMessage::AboutEntityId(dtEntity::SID("AboutEntity"));
   const StringId RequestEntitySelectMessage::UseMultiSelectId(dtEntity::SID("UseMultiSelect"));

   RequestEntitySelectMessage::RequestEntitySelectMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
      Register(UseMultiSelectId, &mUseMultiSelect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType RequestToggleEntitySelectionMessage::TYPE(dtEntity::SID("RequestToggleEntitySelectionMessage"));
   const StringId RequestToggleEntitySelectionMessage::AboutEntityId(dtEntity::SID("AboutEntity"));

   RequestToggleEntitySelectionMessage::RequestToggleEntitySelectionMessage()
      : Message(TYPE)
   {
      Register(AboutEntityId, &mAboutEntity);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SetComponentPropertiesMessage::TYPE(dtEntity::SID("SetComponentPropertiesMessage"));
   const StringId SetComponentPropertiesMessage::ComponentTypeId(dtEntity::SID("ComponentType"));
   const StringId SetComponentPropertiesMessage::EntityUniqueIdId(dtEntity::SID("EntityUniqueId"));
   const StringId SetComponentPropertiesMessage::PropertiesId(dtEntity::SID("Properties"));

   SetComponentPropertiesMessage::SetComponentPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(EntityUniqueIdId, &mEntityUniqueId);
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

   SetComponentPropertiesMessage::SetComponentPropertiesMessage(const std::string& uid, const std::string& componenttype, const std::string& propname, Property& prop)
      : Message(TYPE)
      , mComponentType(componenttype)
      , mEntityUniqueId(uid)
   {
      this->Register(EntityUniqueIdId, &mEntityUniqueId);
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);

      PropertyGroup grp;
      grp[dtEntity::SIDHash(propname)] = &prop;
      mProperties.Set(grp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SetSystemPropertiesMessage::TYPE(dtEntity::SID("SetSystemPropertiesMessage"));
   const StringId SetSystemPropertiesMessage::ComponentTypeId(dtEntity::SID("ComponentType"));
   const StringId SetSystemPropertiesMessage::PropertiesId(dtEntity::SID("Properties"));

   SetSystemPropertiesMessage::SetSystemPropertiesMessage()
      : Message(TYPE)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);
   }

   SetSystemPropertiesMessage::SetSystemPropertiesMessage(const std::string& componenttype, const std::string& propname, Property& prop)
      : Message(TYPE)
      , mComponentType(componenttype)
   {
      this->Register(ComponentTypeId, &mComponentType);
      this->Register(PropertiesId, &mProperties);

      PropertyGroup grp;
      grp[SIDHash(propname)] = &prop;
      mProperties.Set(grp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType SpawnEntityMessage::TYPE(dtEntity::SID("SpawnEntityMessage"));
   const StringId SpawnEntityMessage::UniqueIdId(dtEntity::SID("UniqueId"));
   const StringId SpawnEntityMessage::SpawnerNameId(dtEntity::SID("SpawnerName"));
   const StringId SpawnEntityMessage::EntityNameId(dtEntity::SID("EntityName"));
   const StringId SpawnEntityMessage::AddToSceneId(dtEntity::SID("AddToScene"));

   SpawnEntityMessage::SpawnEntityMessage()
      : Message(TYPE)
   {
      this->Register(UniqueIdId, &mUniqueId);
      this->Register(SpawnerNameId, &mSpawnerName);
      this->Register(EntityNameId, &mEntityName);
      this->Register(AddToSceneId, &mAddToScene);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ToolActivatedMessage::TYPE(dtEntity::SID("ToolActivatedMessage"));
   const StringId ToolActivatedMessage::ToolNameId(dtEntity::SID("ToolName"));

   ToolActivatedMessage::ToolActivatedMessage()
      : Message(TYPE)
   {
      Register(ToolNameId, &mToolName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageType ToolsUpdatedMessage::TYPE(dtEntity::SID("ToolsUpdatedMessage"));
   const StringId ToolsUpdatedMessage::ToolsId(dtEntity::SID("Tools"));
   const StringId ToolsUpdatedMessage::ToolNameId(dtEntity::SID("ToolName"));
   const StringId ToolsUpdatedMessage::IconPathId(dtEntity::SID("IconPath"));
   const StringId ToolsUpdatedMessage::ShortcutId(dtEntity::SID("Shortcut"));

   ToolsUpdatedMessage::ToolsUpdatedMessage()
      : Message(TYPE)
   {
      Register(ToolsId, &mTools);
   }




}
