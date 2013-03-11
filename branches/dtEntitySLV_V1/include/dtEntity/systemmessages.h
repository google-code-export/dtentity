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

   /**
    * These messages are sent by dtEntity subsystems like map or application. User code can
    * register itself to these messages and react to them. In most cases, user code should not have
    * to send
    * these messages.
    */

   ////////////////////////////////////////////////////////////////////////////////
   class MessageFactory;

   /**
    * Adds all messages to message factory
   */
   void DT_ENTITY_EXPORT RegisterSystemMessages(MessageFactory&);

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Tick message gets sent each frame by the system.
    * Register for this message if you want to continually update something.
    * Messages in the message queue are posted after the tick message
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
      void SetSimulationTime(double v) { mSimulationTime.Set(v); }

   private:

      FloatProperty mDeltaSimTime;
      FloatProperty mDeltaRealTime;
      FloatProperty mSimTimeScale;
      DoubleProperty mSimulationTime;
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
      static const StringId ContextIdId;

      CameraAddedMessage();

      virtual Message* Clone() const { return CloneContainer<CameraAddedMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      void SetContextId(unsigned int v) { mContextId.Set(v); }
      unsigned int GetContextId() const { return mContextId.Get(); }

   private:

      UIntProperty mAboutEntityId;
      dtEntity::UIntProperty mContextId;
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

   ////////////////////////////////////////////////////////////////////////////////
   /**
     * Is sent when a window is to be closed, triggered by WindowManager::CloseWindow.
     * used internally to asynchronously close window
     */
   class DT_ENTITY_EXPORT InternalCloseWindowMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId NameId;

      InternalCloseWindowMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<InternalCloseWindowMessage>(); }

      void SetName(const std::string& v) { mName.Set(v); }
      std::string GetName() const { return mName.Get(); }

   private:
      dtEntity::StringProperty mName;

   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * EndOfFrameMessage gets sent after TickMessage and after messages in
    * the message queue are emitted.
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
    * PostUpdateMessage gets sent after scene graph update traversal
    */
   class DT_ENTITY_EXPORT PostUpdateMessage
      : public TickMessage
   {
   public:
      static const MessageType TYPE;

      PostUpdateMessage();
      virtual Message* Clone() const { return CloneContainer<PostUpdateMessage>(); }
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
      static const StringId EntityNameId;
      static const StringId UniqueIdId;
      static const StringId MapNameId;
      static const StringId VisibleInEntityListId;

      EntityAddedToSceneMessage();

      virtual Message* Clone() const { return CloneContainer<EntityAddedToSceneMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      std::string GetEntityName() const { return mEntityName.Get(); }
      void SetEntityName(const std::string& v) { mEntityName.Set(v); }

      std::string GetUniqueId() const { return mUniqueId.Get(); }
      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v) { mMapName.Set(v); }

      bool GetVisibleInEntityList() const { return mVisibleInEntityList.Get(); }
      void SetVisibleInEntityList(bool v) { mVisibleInEntityList.Set(v); }

   private:

      UIntProperty mAboutEntityId;
      StringProperty mEntityName;
      StringProperty mUniqueId;
      StringProperty mMapName;
      BoolProperty mVisibleInEntityList;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when an entity was removed from the selection
    */
   class DT_ENTITY_EXPORT EntityDeselectedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      EntityDeselectedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntityDeselectedMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when name or unique id of entity is changed
    */
   class DT_ENTITY_EXPORT EntityNameUpdatedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId EntityNameId;
      static const StringId UniqueIdId;

      EntityNameUpdatedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntityNameUpdatedMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetEntityName(const std::string& v) { mEntityName.Set(v); }
      std::string GetEntityName() const { return mEntityName.Get(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

   private:

      UIntProperty mAboutEntity;
      StringProperty mEntityName;
      StringProperty mUniqueId;
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
      static const StringId EntityNameId;
      static const StringId UniqueIdId;
      static const StringId MapNameId;
      static const StringId VisibleInEntityListId;

      EntityRemovedFromSceneMessage();

      virtual Message* Clone() const { return CloneContainer<EntityRemovedFromSceneMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      std::string GetEntityName() const { return mEntityName.Get(); }
      void SetEntityName(const std::string& v) { mEntityName.Set(v); }

      std::string GetUniqueId() const { return mUniqueId.Get(); }
      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v) { mMapName.Set(v); }

      bool GetVisibleInEntityList() const { return mVisibleInEntityList.Get(); }
      void SetVisibleInEntityList(bool v) { mVisibleInEntityList.Set(v); }

   private:

      UIntProperty mAboutEntityId;
      StringProperty mEntityName;
      StringProperty mUniqueId;
      StringProperty mMapName;
      BoolProperty mVisibleInEntityList;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when an entity was added to the selection
    */
   class DT_ENTITY_EXPORT EntitySelectedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      EntitySelectedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntitySelectedMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when an entity was added to the selection
    */
   class DT_ENTITY_EXPORT EntitySpawnedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId SpawnerNameId;

      EntitySpawnedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntitySpawnedMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetSpawnerName(const std::string& name) { mSpawnerName.Set(name); }
      std::string GetSpawnerName() const { return mSpawnerName.Get(); }

   private:

      UIntProperty mAboutEntity;
      StringProperty mSpawnerName;
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
      static const StringId ComponentTypeStringId;

      EntitySystemAddedMessage();

      virtual Message* Clone() const { return CloneContainer<EntitySystemAddedMessage>(); }

      StringId GetComponentType() const { return mComponentType.Get(); }
      void SetComponentType(StringId v) { mComponentType.Set(v); }

      std::string GetComponentTypeString() const { return mComponentTypeString.Get(); }
      void SetComponentTypeString(const std::string& v) { mComponentTypeString.Set(v); }

   private:

      StringIdProperty mComponentType;
      StringProperty mComponentTypeString;
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
      static const StringId ComponentTypeStringId;

      EntitySystemRemovedMessage();

      virtual Message* Clone() const { return CloneContainer<EntitySystemRemovedMessage>(); }

      StringId GetComponentType() const { return mComponentType.Get(); }
      void SetComponentType(StringId v) { mComponentType.Set(v); }

      std::string GetComponentTypeString() const { return mComponentTypeString.Get(); }
      void SetComponentTypeString(const std::string& v) { mComponentTypeString.Set(v); }

   private:

      StringIdProperty mComponentType;
      StringProperty mComponentTypeString;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when name or unique id of entity is changed
    */
   class DT_ENTITY_EXPORT EntityVelocityNotNullMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId IsNullId;

      EntityVelocityNotNullMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<EntityVelocityNotNullMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetIsNull(bool v) { mIsNull.Set(v); }
      bool GetIsNull() const { return mIsNull.Get(); }

   private:

      UIntProperty mAboutEntity;
      BoolProperty mIsNull;
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
      static const StringId DataPathId;
      static const StringId SaveOrderId;

      MapBeginLoadMessage();

      virtual Message* Clone() const { return CloneContainer<MapBeginLoadMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }
      
      std::string GetDataPath() const { return mDataPath.Get(); }
      void SetDataPath(const std::string& v){ mDataPath.Set(v); }

      unsigned int GetSaveOrder() const { return mSaveOrder.Get(); }
      void SetSaveOrder(unsigned int v){ mSaveOrder.Set(v); }

   private:

      StringProperty mMapPath;
      StringProperty mDataPath;
      UIntProperty mSaveOrder;
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
    * Gets sent when a map was loaded
   */
   class DT_ENTITY_EXPORT MapLoadedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId MapPathId;
      static const StringId DataPathId;
      static const StringId SaveOrderId;

      MapLoadedMessage();

      virtual Message* Clone() const { return CloneContainer<MapLoadedMessage>(); }

      std::string GetMapPath() const { return mMapPath.Get(); }
      void SetMapPath(const std::string& v){ mMapPath.Set(v); }

      std::string GetDataPath() const { return mDataPath.Get(); }
      void SetDataPath(const std::string& v){ mDataPath.Set(v); }

      unsigned int GetSaveOrder() const { return mSaveOrder.Get(); }
      void SetSaveOrder(unsigned int v){ mSaveOrder.Set(v); }

   private:

      StringProperty mMapPath;
      StringProperty mDataPath;
      UIntProperty mSaveOrder;
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
    * Is sent when a watched resource was changed on disk
    */
   class DT_ENTITY_EXPORT ResourceChangedMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId PathId;

      ResourceChangedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<ResourceChangedMessage>(); }

      void SetPath(const std::string& v) { mPath.Set(v); }
      std::string GetPath() const { return mPath.Get(); }

   private:
      dtEntity::StringProperty mPath;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when a watched resource was changed on disk
    */
   class DT_ENTITY_EXPORT ResourceLoadedMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId PathId;

      ResourceLoadedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<ResourceLoadedMessage>(); }

      void SetPath(const std::string& v) { mPath.Set(v); }
      std::string GetPath() const { return mPath.Get(); }

   private:
      dtEntity::StringProperty mPath;
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
    * Gets sent when a spawner was modified
   */
   class DT_ENTITY_EXPORT SpawnerModifiedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId OldCategoryId;
      static const StringId NewCategoryId;
      static const StringId NameId;
      static const StringId MapNameId;

      SpawnerModifiedMessage();

      virtual Message* Clone() const { return CloneContainer<SpawnerModifiedMessage>(); }

      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v){ mName.Set(v); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v){ mMapName.Set(v); }

      std::string GetOldCategory() const { return mOldCategory.Get(); }
      void SetOldCategory(const std::string& v){ mOldCategory.Set(v); }

      std::string GetNewCategory() const { return mNewCategory.Get(); }
      void SetNewCategory(const std::string& v){ mNewCategory.Set(v); }

   private:

      StringProperty mName;
      StringProperty mMapName;
      StringProperty mOldCategory;
      StringProperty mNewCategory;
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
      static const StringId CategoryId;

      SpawnerRemovedMessage();

      virtual Message* Clone() const { return CloneContainer<SpawnerRemovedMessage>(); }

      std::string GetMapName() const { return mMapName.Get(); }
      void SetMapName(const std::string& v){ mMapName.Set(v); }

      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v){ mName.Set(v); }

      std::string GetCategory() const { return mCategory.Get(); }
      void SetCategory(const std::string& v){ mCategory.Set(v); }

   private:

      StringProperty mName;
      StringProperty mMapName;
      StringProperty mCategory;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when entering simulation loop
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
   /**
    * Is sent when entering simulation loop
    */
   class DT_ENTITY_EXPORT StopSystemMessage
      : public Message
   {
   public:

      static const MessageType TYPE;

      StopSystemMessage();

      virtual Message* Clone() const { return CloneContainer<StopSystemMessage>(); }

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
    * Is sent when a window was opened with WindowManager::OpenWindow
    */
   class DT_ENTITY_EXPORT WindowCreatedMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId NameId;
      static const StringId ContextIdId;
      static const StringId CameraEntityIdId;

      WindowCreatedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<WindowCreatedMessage>(); }

      void SetName(const std::string& v) { mName.Set(v); }
      std::string GetName() const { return mName.Get(); }

      void SetContextId(unsigned int v) { mContextId.Set(v); }
      unsigned int GetContextId() const { return mContextId.Get(); }

      void SetCameraEntityId(unsigned int v) { mCameraEntityId.Set(v); }
      unsigned int GetCameraEntityId() const { return mCameraEntityId.Get(); }

   private:
      dtEntity::StringProperty mName;
      dtEntity::UIntProperty mContextId;
      dtEntity::UIntProperty mCameraEntityId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Is sent when a window was closed
    */
   class DT_ENTITY_EXPORT WindowClosedMessage
      : public Message
   {
   public:

      // type identifier of this message class
      static const MessageType TYPE;
      static const StringId NameId;

      WindowClosedMessage();

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<WindowClosedMessage>(); }

      void SetName(const std::string& v) { mName.Set(v); }
      std::string GetName() const { return mName.Get(); }

   private:
      dtEntity::StringProperty mName;
   };
   
}
