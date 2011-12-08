#pragma once

/* -*-c++-*-
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

#include <dtEntity/export.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>

namespace dtEntity
{

   /**
    * These messages can be sent by user code to trigger a reaction in
    * a dtEntity subsystem.
    */


   ////////////////////////////////////////////////////////////////////////////////
   class MessageFactory;

   /**
    * Adds all messages to message factory
	*/
	void RegisterCommandMessages(MessageFactory&);


   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Send this message to cause the map system to remove an entity from
    * scene and delete it
    */
   class DT_ENTITY_EXPORT DeleteEntityMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId UniqueIdId;

      DeleteEntityMessage();

      virtual Message* Clone() const { return CloneContainer<DeleteEntityMessage>(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

   private:

      StringProperty mUniqueId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Systems can react to this message by enabling their debug drawing.
    */
   class DT_ENTITY_EXPORT EnableDebugDrawingMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId EnableId;

      EnableDebugDrawingMessage();

      virtual Message* Clone() const { return CloneContainer<EnableDebugDrawingMessage>(); }

      bool GetEnable() const { return mEnable.Get(); }
      void SetEnable(bool v){ mEnable.Set(v); }

   private:

      BoolProperty mEnable;
   };

   ///////////////////////////////////////////////////////////////////////////////
   /**
    * Cause an animation to be played
    */
   class DT_ENTITY_EXPORT PlayAnimationMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId AnimationNameId;

      PlayAnimationMessage();

      virtual Message* Clone() const { return CloneContainer<PlayAnimationMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      std::string GetAnimationName() const { return mAnimationName.Get(); }
      void SetAnimationName(const std::string& id) { mAnimationName.Set(id); }

   private:

      UIntProperty mAboutEntityId;
      StringProperty mAnimationName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestEntityDeselectMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      RequestEntityDeselectMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestEntityDeselectMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestEntitySelectMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId UseMultiSelectId;

      RequestEntitySelectMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestEntitySelectMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetUseMultiSelect(bool v) { mUseMultiSelect.Set(v); }
      bool GetUseMultiSelect() const { return mUseMultiSelect.Get(); }

   private:

      UIntProperty mAboutEntity;
      BoolProperty mUseMultiSelect;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestToggleEntitySelectionMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      RequestToggleEntitySelectionMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestToggleEntitySelectionMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Cause all maps and scripts to be unloaded
    */
   class DT_ENTITY_EXPORT ResetSystemMessage
      : public Message
   {
      static const StringId SceneNameId;
   public:

      static const MessageType TYPE;

      ResetSystemMessage();

      virtual Message* Clone() const { return CloneContainer<ResetSystemMessage>(); }

      std::string GetSceneName() const { return mSceneName.Get(); }
      void SetSceneName(const std::string& v){ mSceneName.Set(v); }

   private:

      StringProperty mSceneName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Application system reacts to this message by retrieving a component of type t
   * from entity identified by id and applying the property values of mProperty
   * to the component.
   */
   class DT_ENTITY_EXPORT SetComponentPropertiesMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId EntityUniqueIdId;
      static const StringId ComponentTypeId;
      static const StringId PropertiesId;

      SetComponentPropertiesMessage();

      /**
       * Initialize with single property. Prop is cloned!
       */
      SetComponentPropertiesMessage(const std::string& entityUniqueId,
         ComponentType t, StringId propname, Property& prop);

      virtual Message* Clone() const { return CloneContainer<SetComponentPropertiesMessage>(); }

      std::string GetEntityUniqueId() const { return mEntityUniqueId.Get(); }
      void SetEntityUniqueId(const std::string& id) { mEntityUniqueId.Set(id); }

      void SetComponentType(ComponentType t) { mComponentType.Set(t); }
      ComponentType GetComponentType() const { return mComponentType.Get(); }

	  /**
		* Group of properties that will be applied to entity system
		*/
		void SetProperties(PropertyGroup& v) { mProperties.Set(v); }
		PropertyGroup GetProperties() const { return mProperties.Get(); }

   private:
      StringIdProperty mComponentType;
      GroupProperty mProperties;
      StringProperty mEntityUniqueId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Application system reacts to this message by retrieving the entity system
   * of type t and applying the property values of mProperty to the entity system.
   */
   class DT_ENTITY_EXPORT SetSystemPropertiesMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ComponentTypeId;
      static const StringId PropertiesId;

      SetSystemPropertiesMessage();

      /**
       * Initialize with single property. Prop is cloned!
       */
      SetSystemPropertiesMessage(ComponentType t, StringId propname, Property& prop);

      virtual Message* Clone() const { return CloneContainer<SetSystemPropertiesMessage>(); }

      void SetComponentType(ComponentType t) { mComponentType.Set(t); }
      ComponentType GetComponentType() const { return mComponentType.Get(); }

	  /**
		* Group of properties that will be applied to entity system
		*/
		void SetProperties(PropertyGroup& v) { mProperties.Set(v); }
		PropertyGroup GetProperties() const { return mProperties.Get(); }

   private:
      StringIdProperty mComponentType;
      GroupProperty mProperties;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * User can send this message to cause the map system to spawn an entity from the given spawner.
    * The contained unique id will be assigned to the spawned entity.
    */
   class DT_ENTITY_EXPORT SpawnEntityMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId UniqueIdId;
      static const StringId EntityNameId;
      static const StringId SpawnerNameId;
      static const StringId AddToSceneId;
      SpawnEntityMessage();      
      
      virtual Message* Clone() const { return CloneContainer<SpawnEntityMessage>(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

      std::string GetSpawnerName() const { return mSpawnerName.Get(); }
      void SetSpawnerName(const std::string& v) { mSpawnerName.Set(v); }

      std::string GetEntityName() const { return mEntityName.Get(); }
      void SetEntityName(const std::string& v) { mEntityName.Set(v); }

      bool GetAddToScene() const { return mAddToScene.Get(); }
      void SetAddToScene(bool v) { mAddToScene.Set(v); }

   private:

      StringProperty mUniqueId;
      StringProperty mEntityName;
      StringProperty mSpawnerName;
      BoolProperty mAddToScene;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Send this to cause the tool system to switch to using the named tool
    */
   class DT_ENTITY_EXPORT ToolActivatedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ToolNameId;

      ToolActivatedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<ToolActivatedMessage>(); }

      void SetToolName(const std::string& n) { mToolName.Set(n); }
      std::string GetToolName() const { return mToolName.Get(); }

   private:

      StringProperty mToolName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /** send this when you have added or removed a tool from tool manager
     * mTools property should have data in this format:
     * mtools contains a number of group properties.
     * Each group property has entries
     * StringProperty "ToolName" - name of tool as registered in tool manager
     * StringProperty "IconPath" - path to icon to display for tool
     * StringProperty "Shortcut" - keyboard shortcut for tool, example: "Ctrl+t"
     */
   class DT_ENTITY_EXPORT ToolsUpdatedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ToolsId;
      static const StringId ToolNameId;
      static const StringId IconPathId;
      static const StringId ShortcutId;

      ToolsUpdatedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<ToolsUpdatedMessage>(); }

      void SetTools(const PropertyArray& n) { mTools.Set(n); }
      PropertyArray GetTools() const { return mTools.Get(); }

   private:

      ArrayProperty mTools;
   };
}
