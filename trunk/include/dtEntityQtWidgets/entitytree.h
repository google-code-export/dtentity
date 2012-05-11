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

#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QtGui>
#include <dtEntityQtWidgets/export.h>
#include <dtEntity/entityid.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/log.h>

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   namespace EntityTreeType
   {
      enum e
      {
         ROOT,
         MAP,
         SPAWNER,
         ENTITY,
         ENTITYSYSTEM
      };
   }

   ////////////////////////////////////////////////////////////////////////////////
   // used for transfering item data from game to qt thread
   struct EntityEntry
   {
      EntityTreeType::e mItemType;
      dtEntity::EntityId mEntityId;
      QString mName;
      QString mMapName;
      QString mUniqueId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class EntityTreeItem
   {
   public:
      EntityTreeItem(EntityTreeItem* parent, EntityTreeType::e itemtype)
         : mEntityId(0)
         , mParentItem(parent)
         , mItemType(itemtype)
         , mSelectedBySelectionManager(false)
      {
      }

      virtual ~EntityTreeItem() {
         if(!mChildItems.empty())
         {
            LOG_WARNING("Error removing from entity tree!\n");
         }
         qDeleteAll(mChildItems);
      }
      void insertChild(int index, EntityTreeItem* child) { mChildItems.insert(index, child); }
      void appendChild(EntityTreeItem* child) { mChildItems.append(child); }
      bool removeChild(EntityTreeItem* child) { return mChildItems.removeOne(child); }
      EntityTreeItem* child(int row) { return mChildItems.value(row); }
      unsigned int childCount() const { return mChildItems.count(); }
      EntityTreeItem* parent() { return mParentItem; }
      int row() const { return mParentItem ? mParentItem->mChildItems.indexOf(const_cast<EntityTreeItem*>(this)) : 0; }
      EntityTreeType::e GetItemType() const { return mItemType; }

      dtEntity::EntityId mEntityId;
      QString mName;
      QString mMapName;
      unsigned int mSaveOrder; // for maps

      // hack to prevent a loop between entity tree and selection manager
      // when entity tree receives selection msg from selection manager it
      // would otherwise emit a RequestSelection message, causing a loop.
      // TODO: Find out how to differentiate between a user-click selection
      // and an API selection
      bool mSelectedBySelectionManager;
   protected:

      QList<EntityTreeItem*> mChildItems;
      EntityTreeItem* mParentItem;
      EntityTreeType::e mItemType;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityTreeModel
      : public QAbstractItemModel
   {
      Q_OBJECT

   public:

      EntityTreeModel(dtEntity::EntityManager& em, bool hideInvisible = true, bool showEntitySystems = true, bool showSpawners = true);
      ~EntityTreeModel();

      Qt::DropActions supportedDropActions() const;
      Qt::ItemFlags flags(const QModelIndex &index) const;
      QStringList mimeTypes() const;
      QMimeData* mimeData(const QModelIndexList &indexes) const;
      bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

      dtEntity::EntityManager& GetEntityManager() { return *mEntityManager; }

      // for changing selection of items
      QModelIndex GetMapIndex(const QString& mapName);
      QModelIndex GetEntityIndex(dtEntity::EntityId id);
      QModelIndex GetSpawnerIndex(EntityTreeItem* item, const QString& spawnerName);

      void EnqueueMessage(const dtEntity::Message& m);
      void EmitMoveMapToRow(const QString& mapname, unsigned int row);

      void OnEntitySelected(const dtEntity::Message& msg);
      void OnEntityDeselected(const dtEntity::Message& msg);
      void OnEnterWorld(const dtEntity::Message& msg);
      void OnEntityNameUpdated(const dtEntity::Message& msg);
      void OnLeaveWorld(const dtEntity::Message& msg);
      void OnMapBeginAdd(const dtEntity::Message& msg);
      void OnMapRemoved(const dtEntity::Message& msg);
      void OnEntitySystemAdded(const dtEntity::Message& msg);
      void OnEntitySystemRemoved(const dtEntity::Message& msg);
      void OnSpawnerAdded(const dtEntity::Message& msg);
      void OnSpawnerRemoved(const dtEntity::Message& msg);
      void OnSceneLoaded(const dtEntity::Message& m);

      dtEntity::MessagePump& GetMessagePump() { return mMessagePump; }

      dtEntity::MessageFunctor& GetEnqueueFunctor() { return mEnqueueFunctor; }

   public slots:

      void ProcessMessages();
      QModelIndex parent(const QModelIndex &index) const;
      QModelIndex index(int row, int column, const QModelIndex &parent) const;
      int rowCount(const QModelIndex &parent) const;
      int columnCount(const QModelIndex &parent) const;
      QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const;
      bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
      void OnShowErrorMessage(const QString&);

   signals:

      void ExpandTree(const QModelIndex&);
      void SceneLoaded();
      void EntityWasSelected(const QModelIndex&);
      void EntityWasDeselected(const QModelIndex&);
      void MoveEntityToMap(dtEntity::EntityId, const QString& mapname);
      void MoveSpawnerToMap(const QString& name, const QString& oldmapname, const QString& newmapname);
      void MoveMapToRow(const QString& mapname, unsigned int saveorder);

   private:
      void RemoveEntryFromRoot(const QString& name, EntityTreeType::e t);

      EntityTreeItem* mRootItem;
      dtEntity::MessagePump mMessagePump;
      dtEntity::MessageFunctor mEnqueueFunctor;
      dtEntity::EntityManager* mEntityManager;

      EntityTreeItem* mEntitySystemRootItem;

      bool mHideInvisible;
      bool mShowSpawners;
      bool mShowEntitySystems;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityTreeView
      : public QWidget
   {
      Q_OBJECT

   public:

      EntityTreeView(QWidget *parent = 0);

      virtual ~EntityTreeView();
      void SetModel(QAbstractItemModel* model);
      QTreeView* GetTreeView() const { return mTreeView; }

      QMenu& GetEntityContextMenu() { return mEntityContextMenu; }
      QMenu& GetMapContextMenu() { return mMapContextMenu; }
      QMenu& GetSpawnerContextMenu() { return mSpawnerContextMenu; }

   signals:

      void EntitySelected(dtEntity::EntityId);
      void EntityDeselected(dtEntity::EntityId);

      void EntitySystemSelected(const QString&);
      void EntitySystemDeselected(const QString&);

      void MapSelected(const QString&);
      void MapDeselected(const QString&);

      void SpawnerSelected(const QString&);
      void SpawnerDeselected(const QString&);

      void DeleteEntity(dtEntity::EntityId);
      void DeleteSpawner(const QString& name);
      void SpawnSpawner(const QString& spawnername, const QString& entityname);
      void CreateEntityInMap(const QString& mapname);
      void CreateSpawnerInMap(const QString& spawnername, const QString& mapname);
      void CreateSpawnerAsChild(const QString& spawnername, const QString& mapname);
      void CreateNewMap(const QString& datapath, const QString& mappath);
      void AddExistingMap(const QString& name);
      void UnloadMap(const QString& name);
      void SaveMap(const QString& name);
      void SaveMapCopy(const QString& name, const QString& copyname);

   public slots:

      void OnSceneLoaded();
      void EntityWasSelected(const QModelIndex&);
      void EntityWasDeselected(const QModelIndex&);

   protected slots:

      void OnDoubleClick(const QModelIndex&);
      void ShowContextMenu(const QPoint&);
      void OnDeleteEntityAction(bool);
      void OnJumpToEntityAction(bool);
      void OnAddEntityAction(bool);
      void OnAddNewMapAction(bool);
      void OnAddExistingMapAction(bool);
      void OnAddSpawnerAction(bool);
      void OnAddChildSpawnerAction(bool);
      void OnDeleteSpawnerAction(bool);
      void OnSpawnSpawnerAction(bool);
      void OnUnloadMapAction(bool);
      void OnSaveMapAction(bool);
      void OnSaveMapCopyAction(bool);
      void OnAddItemButton();
      void OnRemoveItemButton();
      void OnSelectionChanged(const QItemSelection&, const QItemSelection&);

   private:

      // if a single map item is selected, return true and assign map name to name
      bool IsSingleMapItemSelected(QString& name) const;
      QTreeView* mTreeView;
      QModelIndex mContextMenuSelectedIndex;
      QAction* mDeleteEntityAction;
      QAction* mJumpToEntityAction;
      QAction* mDeleteSpawnerAction;
      QAction* mSpawnSpawnerAction;
      QAction* mAddEntityAction;
      QAction* mAddNewMapAction;
      QAction* mAddExistingMapAction;
      QAction* mAddSpawnerAction;
      QAction* mAddChildSpawnerAction;
      QAction* mUnloadMapAction;
      QAction* mSaveMapCopyAction;
      QAction* mSaveMapAction;
      QToolButton* mAddItemButton;
      QMenu mEntityContextMenu;
      QMenu mMapContextMenu;
      QMenu mSpawnerContextMenu;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityTreeController
      : public QObject
   {
      Q_OBJECT

   public:

      EntityTreeController(dtEntity::EntityManager*);
      virtual ~EntityTreeController();

      void SetupSlots(EntityTreeModel* model, EntityTreeView*);

      void EntitySystemAdded(const dtEntity::Message& msg);

   public slots:

      void Init();

      void OnDeleteEntity(dtEntity::EntityId id);
      void OnDeleteSpawner(const QString& name);
      void OnSpawnSpawner(const QString& spawnername, const QString& entityname);
      void OnCreateEntityInMap(const QString& mapname);
      void OnCreateSpawnerInMap(const QString& spawnername, const QString& mapname);
      void OnCreateSpawnerAsChild(const QString& spawnername, const QString& parentname);
      void OnCreateNewMap(const QString& datapath, const QString& mapname);
      void OnAddExistingMap(const QString& mapname);
      void OnUnloadMap(const QString& mapname);
      void OnSaveMap(const QString& mapname);
      void OnSaveMapCopy(const QString& mapname, const QString& copyname);
      void OnMoveEntityToMap(dtEntity::EntityId, const QString& mapname);
      void OnMoveSpawnerToMap(const QString& spawnername, const QString& oldmapname, const QString& newmapname);
      void OnMoveMapToRow(const QString& mapname, unsigned int saveorder);

   signals:

     void ShowErrorMessage(const QString&);

   private:

      dtEntity::EntityManager* mEntityManager;
      dtEntity::MessageFunctor mEntitySystemCreatedFunctor;
      EntityTreeModel* mModel;
   };
}

