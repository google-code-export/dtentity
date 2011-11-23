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
#include <osg/Timer>

namespace dtEntity
{
   class EntityManager;

   /**
    * Adds all messages to message factory
	*/
   void RegisterMessages(EntityManager& em);

   /**
    * Tick message gets sent each frame by the system.
    * Register for this message if you want to continually update something.
    */
   class DT_ENTITY_EXPORT TickMessage
      : public Message
   {
   public:
      
      // type identifier of this message class
      static const MessageType TYPE;

      // string identifiers for parameter names
      static const StringId DeltaSimTimeId;
      static const StringId DeltaRealTimeId;
      static const StringId SimTimeScaleId;
      static const StringId SimulationTimeId;

      // Constructor
      TickMessage(); 
      TickMessage(MessageType t);
      
      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<TickMessage>(); }

      // getters (you could do GetFloat(TickMessage::DeltaSimTimeId) instead)
      float GetDeltaSimTime() const { return mDeltaSimTime.Get(); }
      float GetDeltaRealTime() const { return mDeltaRealTime.Get(); }
      float GetSimTimeScale() const { return mSimTimeScale.Get(); }
      double GetSimulationTime() const { return mSimulationTime.Get(); }

      void SetDeltaSimTime(float v) { mDeltaSimTime.Set(v); }
      void SetDeltaRealTime(float v) { mDeltaRealTime.Set(v); }
      void SetSimTimeScale(float v) { mSimTimeScale.Set(v); }
      void SetSimulationTime(float v) { mSimulationTime.Set(v); }

   private:

      FloatProperty mDeltaSimTime;
      FloatProperty mDeltaRealTime;
      FloatProperty mSimTimeScale;
      DoubleProperty mSimulationTime;
   };


   ////////////////////////////////////////////////////////////////////////////////


   /**
    * PostEventTraversalMessage gets sent before TickMessage.
    */
   class DT_ENTITY_EXPORT PostEventTraversalMessage 
      : public TickMessage
   {
   public:
      static const MessageType TYPE;

      PostEventTraversalMessage();
      virtual Message* Clone() const { return CloneContainer<PostEventTraversalMessage>(); }
   };


   ////////////////////////////////////////////////////////////////////////////////

   /**
    * EndOfFrameMessage gets sent after PostEventTraversalMessage.
    */
   class DT_ENTITY_EXPORT EndOfFrameMessage 
      : public TickMessage
   {
   public:
      static const MessageType TYPE;

      EndOfFrameMessage();
      virtual Message* Clone() const { return CloneContainer<EndOfFrameMessage>(); }
   };


   ////////////////////////////////////////////////////////////////////////////////


   /**
    * FrameSynchMessage gets sent after EndOfFrameMessage.
    */
   class DT_ENTITY_EXPORT FrameSynchMessage 
      : public TickMessage
   {
   public:
      static const MessageType TYPE;

      FrameSynchMessage();
      virtual Message* Clone() const { return CloneContainer<FrameSynchMessage>(); }
   };


   ////////////////////////////////////////////////////////////////////////////////

   /**
    * PostFrameMessage gets sent after FrameSynchMessage.
    */
   class DT_ENTITY_EXPORT PostFrameMessage 
      : public TickMessage
   {
   public:
      static const MessageType TYPE;

      PostFrameMessage();
      virtual Message* Clone() const { return CloneContainer<PostFrameMessage>(); }
   };


   ////////////////////////////////////////////////////////////////////////////////

   /**
    * This message gets sent when the method EntityManager::AddToScene method is
    * called. The layer system reacts to this message by attaching the entity to the scene
    * graph.
    */
   class DT_ENTITY_EXPORT EntityAddedToSceneMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId AboutEntityId;
      EntityAddedToSceneMessage();      
      
      virtual Message* Clone() const { return CloneContainer<EntityAddedToSceneMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

   private:

      UIntProperty mAboutEntityId;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * This message gets sent when the method EntityManager::RemoveFromScene method is
    * called. The layer system reacts to this message by removing the entity from the scene
    * graph.
    */
   class DT_ENTITY_EXPORT EntityRemovedFromSceneMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId AboutEntityId;
      EntityRemovedFromSceneMessage();      
      
      virtual Message* Clone() const { return CloneContainer<EntityRemovedFromSceneMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

   private:

      UIntProperty mAboutEntityId;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Gets sent when an entity system is added to the entity manager.
    */
   class DT_ENTITY_EXPORT EntitySystemAddedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId ComponentTypeId;
      EntitySystemAddedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<EntitySystemAddedMessage>(); }

      StringId GetComponentType() const { return mComponentType.Get(); }
      void SetComponentType(StringId v) { mComponentType.Set(v); }

   private:

      StringIdProperty mComponentType;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Gets sent when an entity system is removed from the entity manager.
    */
   class DT_ENTITY_EXPORT EntitySystemRemovedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId ComponentTypeId;
      EntitySystemRemovedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<EntitySystemRemovedMessage>(); }

      StringId GetComponentType() const { return mComponentType.Get(); }

   private:

      StringIdProperty mComponentType;
   };


   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Gets sent when a spawner was added to the map system
	*/
   class DT_ENTITY_EXPORT SpawnerAddedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapNameId;
      static const StringId NameId;
      static const StringId ParentNameId;
      
      SpawnerAddedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<SpawnerAddedMessage>(); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v){ mMapName.Set(v); }

      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v){ mName.Set(v); }

      std::string GetParentName() const { return mParentName.Get(); }
      void SetParentName(const std::string& v){ mParentName.Set(v); }

   private:

      StringProperty mName;
      StringProperty mParentName;
      StringProperty mMapName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a spawner was removed from the map system
	*/
   class DT_ENTITY_EXPORT SpawnerRemovedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapNameId;
      static const StringId NameId;
      
      SpawnerRemovedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<SpawnerRemovedMessage>(); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v){ mMapName.Set(v); }

      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v){ mName.Set(v); }

   private:

      StringProperty mName;
      StringProperty mMapName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a spawner was modified
   */
   class DT_ENTITY_EXPORT SpawnerModifiedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId MapNameId;
      static const StringId NameId;

      SpawnerModifiedMessage();

      virtual Message* Clone() const { return CloneContainer<SpawnerModifiedMessage>(); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v){ mMapName.Set(v); }

      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v){ mName.Set(v); }

   private:

      StringProperty mName;
      StringProperty mMapName;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Send this message to cause the map system to spawn an entity from the given spawner.
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

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Is sent when the EntityManager::ChangeTimeSettings method is called.
    */
   class DT_ENTITY_EXPORT TimeChangedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId SimulationTimeId;
      static const StringId SimulationClockTimeId;
      static const StringId TimeScaleId;
      
      TimeChangedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<TimeChangedMessage>(); }
      
      float GetTimeScale() const { return mTimeScale.Get(); }
      void SetTimeScale(float v) { mTimeScale.Set(v); }

      osg::Timer_t GetSimulationClockTime() const { return mSimulationClockTime; }
      void SetSimulationClockTime(osg::Timer_t v) { mSimulationClockTime = v; }

      double GetSimulationTime() const { return mSimulationTime.Get(); }
      void SetSimulationTime(double v) { mSimulationTime.Set(v); }

   private:

      DoubleProperty mSimulationTime;
      osg::Timer_t mSimulationClockTime;
      FloatProperty mTimeScale;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Gets sent before a map is loaded
	*/
   class DT_ENTITY_EXPORT MapBeginLoadMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapPathId;
      
      MapBeginLoadMessage();      
      
      virtual Message* Clone() const { return CloneContainer<MapBeginLoadMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }

   private:

      StringProperty mMapPath;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Gets sent when a map was loaded
	*/
   class DT_ENTITY_EXPORT MapLoadedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapPathId;
      
      MapLoadedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<MapLoadedMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }

   private:

      StringProperty mMapPath;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a map is about to be unloaded
	*/
   class DT_ENTITY_EXPORT MapBeginUnloadMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapPathId;
      
      MapBeginUnloadMessage();      
      
      virtual Message* Clone() const { return CloneContainer<MapBeginUnloadMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }

   private:

      StringProperty mMapPath;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a map was unloaded
	*/
   class DT_ENTITY_EXPORT MapUnloadedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId MapPathId;
      
      MapUnloadedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<MapUnloadedMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }

   private:

      StringProperty mMapPath;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a scene was loaded
	*/
   class DT_ENTITY_EXPORT SceneLoadedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId SceneNameId;
      
      SceneLoadedMessage();      
      
      virtual Message* Clone() const { return CloneContainer<SceneLoadedMessage>(); }

      std::string GetSceneName() const { return mSceneName.Get(); }
      void SetSceneName(const std::string& v){ mSceneName.Set(v); }

   private:

      StringProperty mSceneName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Gets sent when a scene was unloaded
	*/
   class DT_ENTITY_EXPORT SceneUnloadedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;      
      SceneUnloadedMessage();            
      virtual Message* Clone() const { return CloneContainer<SceneUnloadedMessage>(); }

   };

   ////////////////////////////////////////////////////////////////////////////////

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

   /**
    * Emitted when mesh of an entity is changed.
    */
   class DT_ENTITY_EXPORT MeshChangedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId FilePathId;

      MeshChangedMessage();

      virtual Message* Clone() const { return CloneContainer<MeshChangedMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      std::string GetFilePath() const { return mFilePath.Get(); }
      void SetFilePath(const std::string& id) { mFilePath.Set(id); }

   private:

      UIntProperty mAboutEntityId;
      StringProperty mFilePath;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Emitted when entity is shown/hidden
    */
   class DT_ENTITY_EXPORT VisibilityChangedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId VisibleId;

      VisibilityChangedMessage();

      virtual Message* Clone() const { return CloneContainer<VisibilityChangedMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      bool GetVisible() const { return mVisible.Get(); }
      void SetVisible(bool v) { mVisible.Set(v); }

   private:

      UIntProperty mAboutEntityId;
      BoolProperty mVisible;
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
    * Send this when entering OSG event loop
    */
   class DT_ENTITY_EXPORT StartSystemMessage
      : public Message
   {
   public:

      static const MessageType TYPE;

      StartSystemMessage();

      virtual Message* Clone() const { return CloneContainer<StartSystemMessage>(); }

   };

   ////////////////////////////////////////////////////////////////////////////////
   /* send this when you have added or removed a tool from tool manager
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

   ////////////////////////////////////////////////////////////////////////////////
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
   class DT_ENTITY_EXPORT EntitySelectedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId AboutEntityId;
     
      EntitySelectedMessage()
         : Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
      }
      
      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntitySelectedMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT EntityDeselectedMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId AboutEntityId;
     
      EntityDeselectedMessage()
         : Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
      }
      
      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntityDeselectedMessage>(); }

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
     
      RequestEntitySelectMessage()
         : Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
         Register(UseMultiSelectId, &mUseMultiSelect);
      }
      
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
   class DT_ENTITY_EXPORT RequestEntityDeselectMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId AboutEntityId;
     
      RequestEntityDeselectMessage()
         : Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
      }
      
      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestEntityDeselectMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestToggleEntitySelectionMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      RequestToggleEntitySelectionMessage()
         : Message(TYPE)
      {
         Register(AboutEntityId, &mAboutEntity);
      }

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestToggleEntitySelectionMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

    ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT WindowCreatedMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId NameId;

      WindowCreatedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<WindowCreatedMessage>(); }

      void SetName(const std::string& v) { mName.Set(v); }
      std::string GetName() const { return mName.Get(); }

   private:
      dtEntity::StringProperty mName;
      dtEntity::StringProperty mViewName;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * This message gets sent when a camera component is created.
    */
   class DT_ENTITY_EXPORT CameraAddedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      CameraAddedMessage();

      virtual Message* Clone() const { return CloneContainer<CameraAddedMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

   private:

      UIntProperty mAboutEntityId;
   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * This message gets sent when a camera component is deleted.
    */
   class DT_ENTITY_EXPORT CameraRemovedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      CameraRemovedMessage();

      virtual Message* Clone() const { return CloneContainer<CameraRemovedMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

   private:

      UIntProperty mAboutEntityId;
   };

}
