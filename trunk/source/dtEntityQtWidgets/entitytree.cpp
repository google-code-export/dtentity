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

#include <dtEntityQtWidgets/entitytree.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntityQtWidgets/listdialog.h>
#include <dtEntityQtWidgets/messages.h>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include "ui_entitytree.h"
#include <sstream>

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeItem* GetInternal(const QModelIndex& idx)
   {
      return static_cast<EntityTreeItem*>(idx.internalPointer());
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeModel::EntityTreeModel()
      : mRootItem(new EntityTreeItem(NULL, EntityTreeType::ROOT))
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeModel::~EntityTreeModel()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex EntityTreeModel::GetMapIndex(const QString& mapName)
   {
      for(int i = 0; i < mRootItem->childCount(); ++i)
      {
         EntityTreeItem* item = mRootItem->child(i);
         if(item->GetItemType() == EntityTreeType::MAP && item->mName == mapName)
         {
            return createIndex(item->row(), 0, item);
         }
      }
      return QModelIndex();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex EntityTreeModel::GetEntityIndex(dtEntity::EntityId id)
   {
      for(int i = 0; i < mRootItem->childCount(); ++i)
      {
         EntityTreeItem* mapitem = mRootItem->child(i);
         if(mapitem->GetItemType() == EntityTreeType::MAP)
         {
            for(int j = 0; j < mapitem->childCount(); ++j)
            {
               EntityTreeItem* entityitem = mapitem->child(j);
               if(entityitem->GetItemType() == EntityTreeType::ENTITY && entityitem->mEntityId == id)
               {
                  return createIndex(entityitem->row(), 0, entityitem);
               }
            }
         }
      }
      return QModelIndex();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex EntityTreeModel::GetSpawnerIndex(EntityTreeItem* item, const QString& spawnerName)
   {
      if(spawnerName == "")
      {
         return QModelIndex();
      }

      for(int i = 0; i < item->childCount(); ++i)
      {
         EntityTreeItem* child = item->child(i);
         if(child->GetItemType() != EntityTreeType::MAP &&
            child->GetItemType() != EntityTreeType::SPAWNER
         )
         {
            continue;
         }
         if(child->GetItemType() == EntityTreeType::SPAWNER &&
               child->mName == spawnerName)
         {
            return createIndex(child->row(), 0, child);
         }
         QModelIndex j = GetSpawnerIndex(child, spawnerName);
         if(j.isValid())
         {
            return j;
         }
      }
      return QModelIndex();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnEntityAdded(const EntityEntry& data)
   {

      QModelIndex parent;
      if(data.mMapName == "")
      {
         parent = createIndex(mRootItem->row(), 0, mRootItem);
      }
      else
      {
         parent = GetMapIndex(data.mMapName);
      }

      if(!parent.isValid())
      {
         OnMapBeginAdd(data.mMapName);
         parent = GetMapIndex(data.mMapName);
      }

      int count = GetInternal(parent)->childCount();
      beginInsertRows(parent, count, count);
      EntityTreeItem* nitem = new EntityTreeItem(GetInternal(parent), EntityTreeType::ENTITY);
      nitem->mName = data.mName;
      nitem->mEntityId = data.mEntityId;
      nitem->mMapName = data.mMapName;
      
      GetInternal(parent)->appendChild(nitem);
      endInsertRows();
      //emit ExpandTree(parent);
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnEntityRemoved(const EntityEntry& data)
   {

      QModelIndex parent;
      if(data.mMapName == "")
      {
         parent = QModelIndex();
      }
      else
      {
         parent = GetMapIndex(data.mMapName);
      }

   
      EntityTreeItem* entityitem = NULL;
      if(parent.isValid())
      {

         int i = 0;
         bool found = false;
         EntityTreeItem* parentItem = GetInternal(parent);
         for(; i < parentItem->childCount(); ++i)
         {
            entityitem = parentItem->child(i);
            if(entityitem->GetItemType() == EntityTreeType::ENTITY && entityitem->mEntityId == data.mEntityId)
            {
               found = true;
               break;
            }
         }
         if(!found)
         {
            std::ostringstream os;
            os << "Cannot remove entity from tree: No entry found with id " << data.mEntityId;
            os << " in map " << data.mMapName.toStdString();

            LOG_WARNING(os.str());
            return;
         }
         beginRemoveRows(parent, i, i);
         bool success = parentItem->removeChild(entityitem);
         assert(success);
         delete entityitem;
         endRemoveRows();

      }
      else
      {
         LOG_WARNING("cannot get valid tree entry for map " + data.mMapName.toStdString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnMapBeginAdd(const QString& name)
   {
      unsigned int size = mRootItem->childCount();
      beginInsertRows(QModelIndex(), size, size);
      
      EntityTreeItem* item = new EntityTreeItem(mRootItem, EntityTreeType::MAP);
      item->mName = name;
      item->mMapName = name;
      mRootItem->appendChild(item);
      endInsertRows();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnMapRemoved(const QString& name)
   {    
      RemoveEntryFromRoot(name, EntityTreeType::MAP);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnEntitySystemAdded(const QString& name)
   {
      unsigned int size = mRootItem->childCount();
      beginInsertRows(QModelIndex(), size, size);
      
      EntityTreeItem* item = new EntityTreeItem(mRootItem, EntityTreeType::ENTITYSYSTEM);
      item->mName = name;
      mRootItem->appendChild(item);
      endInsertRows();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::RemoveEntryFromRoot(const QString& name, EntityTreeType::e t)
   {
      int i = 0;
      bool found = false;
      for(; i < mRootItem->childCount(); ++i)
      {
         EntityTreeItem* item = mRootItem->child(i);
         if(item->GetItemType() == t && item->mName == name)
         {
            found = true;
            break;
         }
      }
      if(found)
      {
         EntityTreeItem* item = mRootItem->child(i);
         assert(item->childCount() == 0);

         beginRemoveRows(QModelIndex(), i, i);
         bool success = mRootItem->removeChild(item);

         assert(success);
         delete item;
         endRemoveRows();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnEntitySystemRemoved(const QString& name)
   {
      RemoveEntryFromRoot(name, EntityTreeType::ENTITYSYSTEM);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnSpawnerAdded(const QString& name, const QString& parentname, const QString& mapname)
   {
      QModelIndex parent = GetSpawnerIndex(mRootItem, parentname);
      if(!parent.isValid())
      {
         parent = GetMapIndex(mapname);
      }
      if(!parent.isValid())
      {
         OnMapBeginAdd(mapname);
         parent = GetMapIndex(mapname);
      }

      EntityTreeItem* parentItem = GetInternal(parent);


      EntityTreeItem* nitem = new EntityTreeItem(parentItem, EntityTreeType::SPAWNER);
      nitem->mName = name;
      nitem->mMapName = mapname;

      int count = parentItem->childCount();
      beginInsertRows(parent, count, count);
      parentItem->appendChild(nitem);
      endInsertRows();
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnSpawnerRemoved(const QString& name, const QString& mapname)
   {

      QModelIndex idx = GetSpawnerIndex(mRootItem, name);
      if(!idx.isValid())
      {
         LOG_WARNING("Error removing spawner, spawner not found! Name: " + name.toStdString());
         return;
      }
      EntityTreeItem* item = GetInternal(idx);
      assert(item->childCount() == 0);
      EntityTreeItem* parent = item->parent();
      QModelIndex parentIndex = createIndex(parent->row(), 0, parent);

      if(parentIndex.isValid())
      {

         int row = item->row();
         assert(parent->child(row) == item);
         EntityTreeItem* grandparent = parent->parent();
         EntityTreeItem* grandparent2 = NULL;
         if(grandparent != NULL) grandparent2 = grandparent->parent();
         beginRemoveRows(parentIndex, row, row);
         bool success = parent->removeChild(item);
         assert(success);
         delete item;
         endRemoveRows();

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeModel::OnShowErrorMessage(const QString& txt)
   {
      QMessageBox msgBox;
      msgBox.setText(txt);
      msgBox.exec();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex EntityTreeModel::index(int row, int column, const QModelIndex &parent) const
   {

      if (!hasIndex(row, column, parent))
               return QModelIndex();

      EntityTreeItem* parentItem;

      if(!parent.isValid())
         parentItem = mRootItem;
      else
         parentItem = GetInternal(parent);

      EntityTreeItem* childItem = parentItem->child(row);
      if (childItem)
         return createIndex(row, column, childItem);
      else
         return QModelIndex();

   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex EntityTreeModel::parent(const QModelIndex &index) const
   {
      if (!index.isValid())
         return QModelIndex();

      EntityTreeItem* childItem = GetInternal(index);

      EntityTreeItem* parentItem = childItem->parent();

      if (parentItem == mRootItem)
         return QModelIndex();
      return createIndex(parentItem->row(), 0, parentItem);

   }

   ////////////////////////////////////////////////////////////////////////////////
   int EntityTreeModel::rowCount(const QModelIndex &parent) const
   {
      if (parent.column() > 0)
          return 0;

      EntityTreeItem* parentItem;

      if (!parent.isValid())
         parentItem = mRootItem;
      else
         parentItem = GetInternal(parent);

      return parentItem->childCount();
   }


   ////////////////////////////////////////////////////////////////////////////////
   int EntityTreeModel::columnCount(const QModelIndex &parent) const
   {
      return 1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant EntityTreeModel::data(const QModelIndex &index, int role) const
   {
      if (!index.isValid())
      {
         return QVariant();
      }

      switch(role)
      {         
         case Qt::DecorationRole:
         {
            if(index.column() == 0)
            {
               EntityTreeItem* item = GetInternal(index);
               switch(item->GetItemType())
               {
                  case EntityTreeType::ENTITY: return QIcon(":icons/entitytree_entity.png");
                  case EntityTreeType::ENTITYSYSTEM: return QIcon(":icons/entitytree_entitysystem.png");
                  case EntityTreeType::MAP: return QIcon(":icons/entitytree_map.png");
                  case EntityTreeType::SPAWNER: return QIcon(":icons/entitytree_spawner.png");
                  default: return QVariant();
               }
            }            
         }
         case Qt::DisplayRole:
         case Qt::EditRole:
         {

            EntityTreeItem* item = GetInternal(index);
         
            if(index.column() == 0)
            {
               return item->mName;
            }         
         }
         default: 
            return QVariant();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant EntityTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      if (role != Qt::DisplayRole)
         return QVariant();

      if (orientation == Qt::Horizontal)
      {
         switch(section)
         {
            case 0: return "Name";
            case 1: return "Type";
            default: return "No Name for column";
         }
      }
      return QVariant();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeView::EntityTreeView(QWidget* parent)
      : QWidget(parent)
   {
      Ui_EntityTree ui;
      ui.setupUi(this);
      mTreeView = ui.mTreeView;
      mTreeView->expandToDepth(1);
      mAddItemButton = ui.mAddItemButton;

      connect(ui.mAddItemButton, SIGNAL(clicked()), this, SLOT(OnAddItemButton()));
      connect(ui.mRemoveItemButton, SIGNAL(clicked()), this, SLOT(OnRemoveItemButton()));

      mTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(mTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

      mAddEntityAction = new QAction(tr("Add Entity"), this);
      connect(mAddEntityAction, SIGNAL(triggered(bool)), this, SLOT(OnAddEntityAction(bool)));

      mAddNewMapAction = new QAction(tr("Add New Map"), this);
      connect(mAddNewMapAction, SIGNAL(triggered(bool)), this, SLOT(OnAddNewMapAction(bool)));

      mAddExistingMapAction = new QAction(tr("Add Existing Map"), this);
      connect(mAddExistingMapAction, SIGNAL(triggered(bool)), this, SLOT(OnAddExistingMapAction(bool)));

      mAddSpawnerAction = new QAction(tr("Add Spawner"), this);
      connect(mAddSpawnerAction, SIGNAL(triggered(bool)), this, SLOT(OnAddSpawnerAction(bool)));

      mAddChildSpawnerAction = new QAction(tr("Add Child Spawner"), this);
      connect(mAddChildSpawnerAction, SIGNAL(triggered(bool)), this, SLOT(OnAddChildSpawnerAction(bool)));

      mDeleteEntityAction = new QAction(tr("Delete Entity"), this);
      connect(mDeleteEntityAction, SIGNAL(triggered(bool)), this, SLOT(OnDeleteEntityAction(bool)));

      mDeleteSpawnerAction = new QAction(tr("Delete Spawner"), this);
      connect(mDeleteSpawnerAction, SIGNAL(triggered(bool)), this, SLOT(OnDeleteSpawnerAction(bool)));

      mSpawnSpawnerAction = new QAction(tr("Spawn"), this);
      connect(mSpawnSpawnerAction, SIGNAL(triggered(bool)), this, SLOT(OnSpawnSpawnerAction(bool)));
      
      mUnloadMapAction = new QAction(tr("Unload map"), this);
      connect(mUnloadMapAction, SIGNAL(triggered(bool)), this, SLOT(OnUnloadMapAction(bool)));

      mSaveMapAction = new QAction(tr("Save map"), this);
      connect(mSaveMapAction, SIGNAL(triggered(bool)), this, SLOT(OnSaveMapAction(bool)));

      mSaveMapCopyAction = new QAction(tr("Save copy of map"), this);
      connect(mSaveMapCopyAction, SIGNAL(triggered(bool)), this, SLOT(OnSaveMapCopyAction(bool)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeView::~EntityTreeView()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::SetModel(QAbstractItemModel* model) 
   { 
      mTreeView->setModel(model);
      
      // selection model is created now, so can only now create connection
      connect(mTreeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
         this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::ShowContextMenu(const QPoint& p)
   {
      QModelIndex index = mTreeView->indexAt(p);
      if(index.isValid()) 
      {
         mContextMenuSelectedIndex = index;
         EntityTreeItem* item = GetInternal(index);
         
         switch(item->GetItemType() )
         {
            case EntityTreeType::ENTITY:
            {
               QMenu menu(this);
               menu.addAction(mDeleteEntityAction);
               menu.exec(mTreeView->mapToGlobal(p));
               break;
            }
            case EntityTreeType::SPAWNER:
            {
               QMenu menu(this);
               menu.addAction(mAddChildSpawnerAction);
               menu.addAction(mDeleteSpawnerAction);
               menu.addAction(mSpawnSpawnerAction);
               menu.exec(mTreeView->mapToGlobal(p));
               break;
            }
            case EntityTreeType::MAP:
            {
               QMenu menu(this);
               menu.addAction(mAddEntityAction);
               menu.addAction(mAddSpawnerAction);
               menu.addAction(mUnloadMapAction);
               menu.addAction(mSaveMapAction);
               menu.addAction(mSaveMapCopyAction);
               menu.exec(mTreeView->mapToGlobal(p));
            }
            default: break;
         }      
      }   
   }


   ////////////////////////////////////////////////////////////////////////////////
   bool EntityTreeView::IsSingleMapItemSelected(QString& mapname) const
   {
      QModelIndex sel = mTreeView->selectionModel()->currentIndex();
      if(sel.isValid())
      {
         mapname = GetInternal(sel)->mMapName;
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddItemButton()
   {      
      QString mapname;
      QMenu menu(this);
      menu.addAction(mAddNewMapAction);
      menu.addAction(mAddExistingMapAction);
      if(IsSingleMapItemSelected(mapname))
      {
         menu.addAction(mAddEntityAction);
         menu.addAction(mAddSpawnerAction);
      }      
      menu.exec(mAddItemButton->mapToGlobal(QPoint(0, mAddItemButton->height())));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnRemoveItemButton()
   {
      QModelIndexList sel = mTreeView->selectionModel()->selectedIndexes();
      for(QModelIndexList::iterator i = sel.begin(); i != sel.end(); ++i)
      {
         EntityTreeItem* item = GetInternal(*i);
         switch(item->GetItemType())
         {
            case EntityTreeType::ENTITY: emit(DeleteEntity(item->mEntityId)); break;
            case EntityTreeType::SPAWNER: emit(DeleteSpawner(item->mName)); break;
            default: break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
   {
      QItemSelection::const_iterator i;
      for(i = selected.begin(); i != selected.end(); ++i)
      {
         QModelIndexList indexes = i->indexes();
         for(QModelIndexList::iterator j = indexes.begin(); j != indexes.end(); ++j)
         {
            if(j->column() != 0) continue;
            EntityTreeItem* item = GetInternal(*j);
            switch(item->GetItemType())
            {
            case EntityTreeType::ENTITY: emit EntitySelected(item->mEntityId); break;
            case EntityTreeType::ENTITYSYSTEM: emit EntitySystemSelected(item->mName); break;
            case EntityTreeType::SPAWNER: emit SpawnerSelected(item->mName); break;
            case EntityTreeType::MAP: emit MapSelected(item->mName); break;
            default: break;
            }
         }
      }

      for(i = deselected.begin(); i != deselected.end(); ++i)
      {
         QModelIndexList indexes = i->indexes();
         for(QModelIndexList::iterator j = indexes.begin(); j != indexes.end(); ++j)
         {
            if(j->column() != 0) continue;
            EntityTreeItem* item = GetInternal(*j);
            switch(item->GetItemType())
            {
            case EntityTreeType::ENTITY: emit EntityDeselected(item->mEntityId); break;
            case EntityTreeType::ENTITYSYSTEM: emit EntitySystemDeselected(item->mName); break;
            case EntityTreeType::SPAWNER: emit SpawnerDeselected(item->mName); break;
            case EntityTreeType::MAP: emit MapDeselected(item->mName); break;
            default: break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddEntityAction(bool)
   {
      QString mapname;
      bool sel = IsSingleMapItemSelected(mapname);
      if(!sel) return;
      emit CreateEntityInMap(mapname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddNewMapAction(bool)
   {
      bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new Map"),
                                          tr("Enter name for new Map:"), QLineEdit::Normal,
                                          "MyMap", &ok);
      if (ok && !text.isEmpty())
      {
         QString path = QString("maps/%1.dtemap").arg(text);
         emit CreateNewMap(path);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddExistingMapAction(bool)
   {
      EntityTreeModel* model = static_cast<EntityTreeModel*>(mTreeView->model());

      QSettings settings;
      QStringList paths = settings.value("DataPaths", "ProjectAssets").toStringList();
      QStringList entries;

      for(QStringList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         QDir dir(*i + "/maps");
         if(dir.exists())
         {
            entries += dir.entryList(QStringList("*.dtemap"), QDir::Files);
         }
      }

      if(entries.empty())
      {
         model->OnShowErrorMessage("No existing maps found");
         return;
      }

      QStringList notInScene;
      for(QStringList::iterator i = entries.begin(); i != entries.end(); ++i)
      {
         if(!model->GetMapIndex("maps/" + *i).isValid())
         {
            notInScene.push_back(*i);
         }
      }

      if(notInScene.empty())
      {
         model->OnShowErrorMessage("All existing maps are already included in the scene");
         return;
      }
      notInScene.sort();
      dtEntityQtWidgets::ListDialog* dialog = new dtEntityQtWidgets::ListDialog(notInScene);

      if(dialog->exec() == QDialog::Accepted)
      {
         QStringList sel = dialog->GetSelectedItems();
         if(sel.size() != 0)
         {
            QString seltxt = sel.front();
            emit AddExistingMap("maps/" + seltxt);
         }
      }

      dialog->close();
      dialog->deleteLater();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddSpawnerAction(bool)
   {
      QString mapname;
      bool sel = IsSingleMapItemSelected(mapname);
      if(!sel) return;
      bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new Spawner"),
                                          tr("Enter name for new Spawner:"), QLineEdit::Normal,
                                          "MySpawner", &ok);
      if (ok && !text.isEmpty())
      {
         emit CreateSpawnerInMap(text, mapname);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnAddChildSpawnerAction(bool)
   {

      QModelIndexList sel = mTreeView->selectionModel()->selectedIndexes();

      EntityTreeItem* item = GetInternal(sel.front());
      if(item->GetItemType() != EntityTreeType::SPAWNER)
      {
         return;
      }

      QString parentname = item->mName;

      bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new Spawner"),
                                          tr("Enter name for new Spawner:"), QLineEdit::Normal,
                                          "MySpawner", &ok);
      if (ok && !text.isEmpty())
      {
         emit CreateSpawnerAsChild(text, parentname);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnDeleteEntityAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
         emit(DeleteEntity(item->mEntityId));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnDeleteSpawnerAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
         emit(DeleteSpawner(item->mName));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnSpawnSpawnerAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         bool ok;
         QString text = QInputDialog::getText(this, tr("Enter name for new entity"),
                                             tr("Enter name for new entity:"), QLineEdit::Normal,
                                             "MyEntity", &ok);

         if(ok)
         {
            EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
            emit(SpawnSpawner(item->mName, text));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnUnloadMapAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
         emit(UnloadMap(item->mName));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnSaveMapAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
         emit(SaveMap(item->mName));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnSaveMapCopyAction(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         EntityTreeItem* item = GetInternal(mContextMenuSelectedIndex);
         
         QString fileName = QFileDialog::getSaveFileName(this, tr("Save Map"),
                            QString(),
                            tr("Maps (*.dtemap)"));

         if (fileName != "")
         {
            emit(SaveMapCopy(item->mName, fileName));
         }      
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeView::OnSceneLoaded()
   {
      mTreeView->expandToDepth(1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeController::EntityTreeController(dtEntity::EntityManager* entityManager)
      : mEntityManager(entityManager)
   {      
      qRegisterMetaType<EntityEntry>("EntityEntry");
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityTreeController::~EntityTreeController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);   
      mEntityManager->UnregisterForMessages(dtEntity::MapBeginLoadMessage::TYPE, mMapBeginAddFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::MapUnloadedMessage::TYPE, mMapRemovedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::EntitySystemAddedMessage::TYPE, mEntitySystemAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::EntitySystemRemovedMessage::TYPE, mEntitySystemRemovedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SceneLoadedMessage::TYPE, mSceneLoadedFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::SetupSlots(EntityTreeModel* model, EntityTreeView* view)
   {
      connect(this, SIGNAL(EntityAdd(const EntityEntry&)), model, SLOT(OnEntityAdded(const EntityEntry&)));
      connect(this, SIGNAL(EntityRemove(const EntityEntry&)), model, SLOT(OnEntityRemoved(const EntityEntry&)));
      connect(this, SIGNAL(MapBeginAdd(const QString&)), model, SLOT(OnMapBeginAdd(const QString&)));
      connect(this, SIGNAL(MapRemove(const QString&)), model, SLOT(OnMapRemoved(const QString&)));
      connect(this, SIGNAL(EntitySystemAdd(const QString&)), model, SLOT(OnEntitySystemAdded(const QString&)));
      connect(this, SIGNAL(EntitySystemRemove(const QString&)), model, SLOT(OnEntitySystemRemoved(const QString&)));
      connect(this, SIGNAL(SpawnerAdd(const QString&, const QString&, const QString&)), model, SLOT(OnSpawnerAdded(const QString&, const QString&, const QString&)));
      connect(this, SIGNAL(SpawnerRemove(const QString&, const QString&)), model, SLOT(OnSpawnerRemoved(const QString&, const QString&)));
      connect(this, SIGNAL(ShowErrorMessage(const QString&)), model, SLOT(OnShowErrorMessage(const QString&)));


      connect(view, SIGNAL(EntitySelected(dtEntity::EntityId)), this, SLOT(OnEntitySelected(dtEntity::EntityId)));
      connect(view, SIGNAL(EntityDeselected(dtEntity::EntityId)), this, SLOT(OnEntityDeselected(dtEntity::EntityId)));
      connect(view, SIGNAL(EntitySystemSelected(const QString&)), this, SLOT(OnEntitySystemSelected(const QString&)));
      connect(view, SIGNAL(MapSelected(const QString&)), this, SLOT(OnMapSelected(const QString&)));
      connect(view, SIGNAL(SpawnerSelected(const QString&)), this, SLOT(OnSpawnerSelected(const QString&)));

      connect(view, SIGNAL(DeleteEntity(dtEntity::EntityId)), this, SLOT(OnDeleteEntity(dtEntity::EntityId)));
      connect(view, SIGNAL(DeleteSpawner(const QString&)), this, SLOT(OnDeleteSpawner(const QString&)));
      connect(view, SIGNAL(SpawnSpawner(const QString&, const QString&)), this, SLOT(OnSpawnSpawner(const QString&, const QString&)));
      connect(view, SIGNAL(CreateEntityInMap(const QString&)), this, SLOT(OnCreateEntityInMap(const QString&)));
      connect(view, SIGNAL(CreateSpawnerInMap(const QString&, const QString&)),
              this, SLOT(OnCreateSpawnerInMap(const QString&, const QString&)));
      connect(view, SIGNAL(CreateSpawnerAsChild(const QString&, const QString&)),
              this, SLOT(OnCreateSpawnerAsChild(const QString&, const QString&)));
      connect(view, SIGNAL(CreateNewMap(const QString&)), this, SLOT(OnCreateNewMap(const QString&)));
      connect(view, SIGNAL(AddExistingMap(const QString&)), this, SLOT(OnAddExistingMap(const QString&)));
      connect(view, SIGNAL(UnloadMap(const QString&)), this, SLOT(OnUnloadMap(const QString&)));
      connect(view, SIGNAL(SaveMap(const QString&)), this, SLOT(OnSaveMap(const QString&)));
      connect(view, SIGNAL(SaveMapCopy(const QString&, const QString&)), this, SLOT(OnSaveMapCopy(const QString&, const QString&)));
      connect(model, SIGNAL(ExpandTree(const QModelIndex&)), view->GetTreeView(), SLOT(expand(const QModelIndex&)));

      connect(this, SIGNAL(SceneLoaded()), view, SLOT(OnSceneLoaded()));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::Init()
   {
      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnEnterWorld);
      mEntityManager->RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "EntityTreeController::OnEnterWorld");
         
      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnLeaveWorld);
      mEntityManager->RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "EntityTreeController::OnLeaveWorld");

      mMapBeginAddFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnMapBeginAdd);
      mEntityManager->RegisterForMessages(dtEntity::MapBeginLoadMessage::TYPE, mMapBeginAddFunctor, "EntityTreeController::OnMapBeginAdd");

      mMapRemovedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnMapRemoved);
      mEntityManager->RegisterForMessages(dtEntity::MapUnloadedMessage::TYPE, mMapRemovedFunctor, "EntityTreeController::OnMapRemoved");

      mEntitySystemAddedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnEntitySystemAdded);
      mEntityManager->RegisterForMessages(dtEntity::EntitySystemAddedMessage::TYPE, mEntitySystemAddedFunctor, "EntityTreeController::OnEntitySystemAdded");

      mEntitySystemRemovedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnEntitySystemRemoved);
      mEntityManager->RegisterForMessages(dtEntity::EntitySystemRemovedMessage::TYPE, mEntitySystemRemovedFunctor, "EntityTreeController::OnEntitySystemRemoved");

      mSpawnerAddedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnSpawnerAdded);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor, "EntityTreeController::OnSpawnerAdded");

      mSpawnerRemovedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnSpawnerRemoved);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor, "EntityTreeController::OnSpawnerRemoved");

      mSceneLoadedFunctor = dtEntity::MessageFunctor(this, &EntityTreeController::OnSceneLoaded);
      mEntityManager->RegisterForMessages(dtEntity::SceneLoadedMessage::TYPE, mSceneLoadedFunctor,
         dtEntity::FilterOptions::ORDER_LATE, "EntityTreeController::OnSceneLoaded");

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      // add existing maps to model
      {      
         std::vector<std::string> mapnames = mtsystem->GetLoadedMaps();
         std::vector<std::string>::iterator i;
         for(i = mapnames.begin(); i != mapnames.end(); ++i)
         {            
            dtEntity::MapLoadedMessage m;
            m.SetMapPath(*i);
            OnMapBeginAdd(m);
         }
      }

      // add existing entities to model
      {
         std::vector<dtEntity::EntityId> eids;
         mEntityManager->GetEntityIds(eids);

         std::vector<dtEntity::EntityId>::iterator i;
         for(i = eids.begin(); i != eids.end(); ++i)
         {
            dtEntity::EntityAddedToSceneMessage m;
            m.SetAboutEntityId(*i);
            OnEnterWorld(m);
         }
      }    

      // add existing entity systems to model
      {
         std::vector<const dtEntity::EntitySystem*> esystems;
         mEntityManager->GetEntitySystems(esystems);
         std::vector<const dtEntity::EntitySystem*>::iterator i;
         for(i = esystems.begin(); i != esystems.end(); ++i)
         {
            dtEntity::EntitySystemAddedMessage m;
            m.SetComponentType((*i)->GetComponentType());
            OnEntitySystemAdded(m);
         }
      }

      // add existing spawners to model
      {
         std::map<std::string, dtEntity::Spawner*> spawners;
         mtsystem->GetAllSpawners(spawners);
         std::map<std::string, dtEntity::Spawner*>::const_iterator i;
         for(i = spawners.begin(); i != spawners.end(); ++i)
         {
            dtEntity::Spawner* spawner = i->second;
            dtEntity::SpawnerAddedMessage msg;
            msg.SetName(spawner->GetName());
            msg.SetMapName(spawner->GetMapName());
            if(spawner->GetParent())
            {
               msg.SetParentName(spawner->GetParent()->GetName());
            }
            OnSpawnerAdded(msg);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEnterWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityAddedToSceneMessage& msg = 
         static_cast<const dtEntity::EntityAddedToSceneMessage&>(m);

      dtEntity::EntityId eid = msg.GetAboutEntityId();

      EntityEntry entry;
      entry.mItemType = EntityTreeType::ENTITY;
      entry.mEntityId = eid;

      dtEntity::MapComponent* mcomp;
      if(mEntityManager->GetComponent(entry.mEntityId, mcomp))
      {
         entry.mName = mcomp->GetEntityName().c_str();
         entry.mMapName = mcomp->GetMapName().c_str();
         entry.mUniqueId = mcomp->GetUniqueId().c_str();
      }
      
      emit EntityAdd(entry);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnLeaveWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityRemovedFromSceneMessage& msg = 
         static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(m);
      dtEntity::EntityId eid = msg.GetAboutEntityId();
      EntityEntry entry;
      entry.mItemType = EntityTreeType::ENTITY;
      entry.mEntityId = eid;

      dtEntity::MapComponent* mcomp;
      if(mEntityManager->GetComponent(entry.mEntityId, mcomp))
      {
         entry.mName = mcomp->GetEntityName().c_str();
         entry.mMapName = mcomp->GetMapName().c_str();
         entry.mUniqueId = mcomp->GetUniqueId().c_str();
      }   
      emit(EntityRemove(entry));  
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnMapBeginAdd(const dtEntity::Message& m)
   {
      const dtEntity::MapBeginLoadMessage& msg = 
         static_cast<const dtEntity::MapBeginLoadMessage&>(m);
      emit(MapBeginAdd(msg.GetMapPath().c_str()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnMapRemoved(const dtEntity::Message& m)
   {
      const dtEntity::MapUnloadedMessage& msg = 
         static_cast<const dtEntity::MapUnloadedMessage&>(m);
      emit(MapRemove(msg.GetMapPath().c_str()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEntitySystemAdded(const dtEntity::Message& m)
   {
      const dtEntity::EntitySystemAddedMessage& msg = 
         static_cast<const dtEntity::EntitySystemAddedMessage&>(m);
      dtEntity::EntitySystem* es = mEntityManager->GetEntitySystem(msg.GetComponentType());
      if(es && es->GetAllProperties().size() != 0)
      {
         emit EntitySystemAdd(dtEntity::GetStringFromSID(msg.GetComponentType()).c_str()); 
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEntitySystemRemoved(const dtEntity::Message& m)
   {
      const dtEntity::EntitySystemRemovedMessage& msg = 
         static_cast<const dtEntity::EntitySystemRemovedMessage&>(m);
      emit EntitySystemRemove(dtEntity::GetStringFromSID(msg.GetComponentType()).c_str()); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSpawnerAdded(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerAddedMessage& msg = 
         static_cast<const dtEntity::SpawnerAddedMessage&>(m);
      emit SpawnerAdd(msg.GetName().c_str(), msg.GetParentName().c_str(), msg.GetMapName().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSpawnerRemoved(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerRemovedMessage& msg = 
         static_cast<const dtEntity::SpawnerRemovedMessage&>(m);
      emit SpawnerRemove(msg.GetName().c_str(), msg.GetMapName().c_str()); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSceneLoaded(const dtEntity::Message& m)
   {
      emit SceneLoaded();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnDeleteEntity(dtEntity::EntityId id)
   {
      mEntityManager->RemoveFromScene(id);
      mEntityManager->KillEntity(id);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnDeleteSpawner(const QString& name)
   {
      dtEntity::MapSystem* mtsystem;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      bool success = mtsystem->DeleteSpawner(name.toStdString());
      if(!success)
      {
         LOG_WARNING("Could not delete spawner!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSpawnSpawner(const QString& spawnername, const QString& entityname)
   {
      dtEntity::MapSystem* mtsystem;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      dtEntity::Entity* entity;
      mEntityManager->CreateEntity(entity);
      bool success = mtsystem->Spawn(spawnername.toStdString(), *entity);

      if(!success)
      {
         LOG_WARNING("Could not spawn spawner " + spawnername.toStdString());
         return;
      }

      dtEntity::MapComponent* comp;
      if(!entity->GetComponent(comp))
      {
	     entity->CreateComponent(comp);
	  }
      comp->SetEntityName(entityname.toStdString());
      comp->SetUniqueId(entityname.toStdString());
  
      mEntityManager->AddToScene(entity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnCreateEntityInMap(const QString& mapname)
   {
      dtEntity::Entity* entity;
      bool success = mEntityManager->CreateEntity(entity);
      if(!success)
      {
         LOG_WARNING("Could not create entity");
         return;
      }
      dtEntity::MapComponent* mapcomp;
      success = entity->CreateComponent(mapcomp);
      if(!success)
      {
         LOG_WARNING("Could not create map component for new entity");
         return;
      }
      mapcomp->SetMapName(mapname.toStdString());
      mapcomp->SetEntityName("No Name");
      mEntityManager->AddToScene(entity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnCreateSpawnerInMap(const QString& spawnername, const QString& mapname)
   {
      dtEntity::Spawner* spawner = new dtEntity::Spawner(spawnername.toStdString(), mapname.toStdString());
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);
      mtsystem->AddSpawner(*spawner);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnCreateSpawnerAsChild(const QString& spawnername, const QString& parentname)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);
      dtEntity::Spawner* parent;
      bool found = mtsystem->GetSpawner(parentname.toStdString(), parent);
      if(!found)
      {
         LOG_WARNING("Cannot create spawner: Parent not found!");
         return;
      }
      dtEntity::Spawner* spawner = new dtEntity::Spawner(spawnername.toStdString(), parent->GetMapName(), parent);
      mtsystem->AddSpawner(*spawner);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEntitySelected(dtEntity::EntityId id)
   {
      dtEntity::RequestEntitySelectMessage msg;
      msg.SetAboutEntityId(id);
      msg.SetUseMultiSelect(true);
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEntityDeselected(dtEntity::EntityId id)
   {
      dtEntity::RequestEntityDeselectMessage msg;
      msg.SetAboutEntityId(id);
      mEntityManager->EmitMessage(msg);
   }
      
   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnEntitySystemSelected(const QString& name)
   {
      EntitySystemSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSpawnerSelected(const QString& name)
   {
      SpawnerSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnMapSelected(const QString& name)
   {
      MapSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnCreateNewMap(const QString& path)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);
      success = mtsystem->AddEmptyMap(path.toStdString());
      if(!success)
      {
         emit ShowErrorMessage("Could not create new map, unknown reason!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnAddExistingMap(const QString& path)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      success = mtsystem->LoadMap(path.toStdString());
      if(!success)
      {
         emit ShowErrorMessage("Could not load existing map, unknown reason!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnUnloadMap(const QString& path)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      success = mtsystem->UnloadMap(path.toStdString());
      if(!success)
      {
         emit ShowErrorMessage("Could not unload map, unknown reason!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSaveMap(const QString& path)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      success = mtsystem->SaveMap(path.toStdString());
      if(!success)
      {
         emit ShowErrorMessage("Could not save map, unknown reason!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityTreeController::OnSaveMapCopy(const QString& path, const QString& copypath)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);
      
      success = mtsystem->SaveMapAs(path.toStdString(), copypath.toStdString());
      if(!success)
      {
         emit ShowErrorMessage("Could not save copy of map, unknown reason!");
      }
   }

}
