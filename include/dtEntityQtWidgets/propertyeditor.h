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

#include <dtEntityQtWidgets/export.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/property.h>
#include <dtEntity/propertycontainer.h>

#include <QtCore/QAbstractItemModel>
#include <QtGui/QtGui>
#include <set>

namespace dtEntityQtWidgets
{

   class DelegateFactory;
   class Translator;
   class PropertySubDelegate;
   class PropertyEditorDelegate;
   class RootDelegateFactory;

   ////////////////////////////////////////////////////////////////////////////////
   class TreeItem
   {
   public:
      TreeItem(TreeItem *parent, DelegateFactory* childfactory);
      virtual ~TreeItem();

      void appendChild(TreeItem *child);
      void removeChild(TreeItem *child);

      TreeItem* child(int row);
      int childCount() const;
      int row() const;
      TreeItem* parent();

      DelegateFactory* GetChildDelegateFactory() const
      {
         return mChildDelegateFactory;
      }
      
   protected:
      QList<TreeItem*> mChildItems;
      TreeItem* mParentItem;
      DelegateFactory* mChildDelegateFactory;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class RootTreeItem : public TreeItem
   {
   public:
      RootTreeItem(DelegateFactory* f)
         : TreeItem(0, f)
      {
      }
      ~RootTreeItem()
      {
      }
   };

   ////////////////////////////////////////////////////////////////////////////////

   class ComponentTreeItem : public TreeItem
   {
   public:
      ComponentTreeItem(TreeItem* parent, DelegateFactory* childfactory , QString ptype)
      : TreeItem(parent, childfactory)
      , mType(ptype)
      {
      }

      ~ComponentTreeItem() {}

      QString mType;
   private:

   };

   ////////////////////////////////////////////////////////////////////////////////
   class PropertyTreeItem : public TreeItem
   {
   public:
      PropertyTreeItem(TreeItem *parent, DelegateFactory* childfactory, const QString& name, dtEntity::Property* prop, PropertySubDelegate* dlgt);
      ~PropertyTreeItem();

      bool mChanged;
      QString mName;
      dtEntity::Property* mProperty;
      PropertySubDelegate* mDelegate;
   };




   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT PropertyEditorModel
      : public QAbstractItemModel
   {
      Q_OBJECT

   public:
      
   
      PropertyEditorModel();
      virtual ~PropertyEditorModel();

      QModelIndex parent(const QModelIndex &index) const;
      QModelIndex index(int row, int column, const QModelIndex &parent) const;
      int rowCount(const QModelIndex &parent) const;
      int columnCount(const QModelIndex &parent) const;
      QVariant data(const QModelIndex &index, int role) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const;
      Qt::ItemFlags flags(const QModelIndex &index) const;
      bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

      void Reset();
      void ResetChangedFlag(PropertyTreeItem* item);
      TreeItem* GetRootItem() const { return mRootItem; }

      ComponentTreeItem* AddComponent(const QString& name);
      void RemoveComponent(const QString& name);
      void AddProperties(TreeItem* parent, const dtEntity::PropertyContainer& map);
      void RemoveProperties(TreeItem* parent);

      // get a list of all available component types
      std::set<dtEntity::ComponentType> GetComponentTypes() const { return mComponentTypes; }

      // get a list of all component types adde to currently edited object
      QList<dtEntity::ComponentType> GetComponentTypesInCurrent() const;

      ComponentTreeItem* FindComponent(dtEntity::ComponentType) const;

      QString GetSelectedSpawner() const { return mSelectedSpawner; }
      QString GetSelectedEntitySystem() const { return mSelectedEntitySystem; }
      dtEntity::EntityId GetSelectedEntity() const { return mSelectedEntity; }

      QString TranslateComponentType(const QString& componenttype) const;
      QString TranslateProperty(const QString& componenttype, const QString& propertyname) const;
      QString TranslateDescription(const QString& componenttype, const QString& propertyname) const;

   public slots:

      void AppendArrayEntry(const QModelIndex& index);
                  
      void EmitChanges();
      void OnEntitySelected(dtEntity::EntityId id);
      void OnEntitySystemSelected(const QString& name);
      void OnSpawnerSelected(const QString& name, bool addToStore, const QString& category, const QString& iconPath);
      void OnMapSelected(const QString& name);
      void ComponentRetrieved(dtEntity::ComponentType, const dtEntity::DynamicPropertyContainer&);
      void ComponentDeleted(dtEntity::ComponentType);
      
      void EntitySystemAdded(dtEntity::ComponentType ctype);
      void EntitySystemRemoved(dtEntity::ComponentType ctype);
      void AddComponentToCurrent(const QString& ctype);
      void OnRemoveComponentFromCurrent(const QString& ctype);
      void OnComponentDataChanged(dtEntity::EntityId, dtEntity::StringId componenttype);

      // causes model to request an update of all component property values
      void RequestRefresh();


   signals:

      void ExpandFullTree();
      void ExpandTree(const QModelIndex& index);  

      void EditSpawner(const QString& name, bool addToStore, const QString& category, const QString& iconPath);
      void EditEntity(const QString& name);
      void EditEntitySystem(const QString& name);
      void EditNone();

      void ApplyPropertiesToEntity(dtEntity::EntityId, const QString& componentType, const dtEntity::DynamicPropertyContainer&);
      void ApplyPropertiesToSpawner(const QString& name, const QString& componentType, const dtEntity::DynamicPropertyContainer&, bool updateEntities);
      void ApplyPropertiesToEntitySystem(const QString& name, const dtEntity::DynamicPropertyContainer&);
      
      void AddComponentToSpawner(const QString& spawner, const QString& ctype);
      void AddComponentToEntity(dtEntity::EntityId id, const QString& ctype);

      void RemoveComponentFromSpawner(const QString& spawner, const QString& ctype);
      void RemoveComponentFromEntitySystem(const QString& es, const QString& ctype);
      void RemoveComponentFromEntity(dtEntity::EntityId id, const QString& ctype);

      void RequestUpdateComponent(dtEntity::EntityId id, dtEntity::StringId componentType);
      void RequestUpdateAll(dtEntity::EntityId id);
      void RequestUpdateEntitySystem(const QString& name);


   private:

      // Find type of component that this property is part of
      QString GetComponentFromProperty(TreeItem* item) const;

      TreeItem* mRootItem;
      dtEntity::EntityId mSelectedEntity;
      QString mSelectedSpawner;
      QString mSelectedEntitySystem;
      RootDelegateFactory* mRootDelegateFactory;
      std::set<dtEntity::ComponentType> mComponentTypes; // for populating the add component list

      QMap<QString, Translator*> mTranslators;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT PropertyEditorView 
      : public QWidget
   {
      Q_OBJECT

   public:
      
      PropertyEditorView(QWidget* parent = NULL);

      virtual ~PropertyEditorView();
      
      QTreeView* GetComponentTree() const { return mComponentTree; }      
      QPushButton* GetApplyButton() const { return mApplyButton; }
      QPushButton* GetRefreshButton() const { return mRefreshButton; }
           
      virtual bool eventFilter (QObject* watched, QEvent* event);

      void SetModel(QAbstractItemModel* model) { mComponentTree->setModel(model); }

   public slots:

      void ExpandTree() { GetComponentTree()->expandToDepth(1); }
      void OnEditSpawner(const QString& name, bool addToStore, const QString& category, const QString& iconPath);
      void OnEditEntity(const QString& name);
      void OnEditEntitySystem(const QString& name);
      void OnEditNone();

      void EnableAddComponent(bool enable);
      void ShowAddComponentDialog();

   protected:

      virtual void closeEvent(QCloseEvent* evt)
      {
         emit(Closed());
      }

   protected slots:

      void ShowContextMenu(const QPoint& p);
      void ActionAppendArrayEntry(bool);
      void OnClick(const QModelIndex&);
      void RemoveSelectedComponent();

      // apply changes in item-specific property section
      void ApplyAdditionalProperties();
      void OnChooseSpawnerIcon();

   signals:

      void Closed();    
      void AppendArrayEntry(const QModelIndex& index);
      void AddComponentToCurrent(const QString& ctype);
      void ComponentSelected(dtEntity::ComponentType);
      void RemoveComponentFromCurrent(const QString& ctype);
      void SpawnerAdditionalPropertiesChanged(const QString& name, bool addToStore, const QString& category, const QString& iconPath);

   private:
      
      QAction* mAppendArrayEntry;
      QModelIndex mContextMenuSelectedIndex;

      QTreeView* mComponentTree;
      QPushButton* mApplyButton;
      QPushButton* mRefreshButton;
      QToolButton* mAddComponentButton;
      QToolButton* mRemoveComponentButton;
      QStackedWidget* mItemOptions;
      QLabel* mEditedItemName;
      QString mSelectedComponent;
      QLineEdit* mSpawnerCategoryName;
      QCheckBox* mSpawnerAddToStore;
      QLabel* mSpawnerIconPath;
   }; 

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT PropertyEditorController 
      : public QObject
   {
      Q_OBJECT

   public:
      
      PropertyEditorController(dtEntity::EntityManager*);
      virtual ~PropertyEditorController();

      void OnEntitySelected(const dtEntity::Message& msg);
      void OnMapSelected(const dtEntity::Message& msg);
      void OnEntitySystemSelected(const dtEntity::Message& msg);
      void OnSpawnerSelected(const dtEntity::Message& msg);

      void OnEntitySystemAdded(const dtEntity::Message& msg);
      void OnEntitySystemRemoved(const dtEntity::Message& msg);
      void OnComponentDataChanged(const dtEntity::Message& m);

      void SetupSlots(PropertyEditorModel* model, PropertyEditorView* view);

   signals:

      void EntitySelected(dtEntity::EntityId);
      void SpawnerSelected(const QString& name, bool addToStore, const QString& category, const QString& iconpath);
      void MapSelected(const QString& name);
      void EntitySystemSelected(const QString& name);
      
      void ComponentRetrieved(dtEntity::ComponentType, const dtEntity::DynamicPropertyContainer&);
      void ComponentDeleted(dtEntity::ComponentType);

      void AddDelegatesForComponent(dtEntity::ComponentType ctype);
      void EntitySystemAdded(dtEntity::ComponentType);
      void EntitySystemRemoved(dtEntity::ComponentType);
      void EnableAddComponent(bool enable);
      void ComponentDataChanged(dtEntity::EntityId, dtEntity::StringId componenttype);

   public slots:
   
      void Init();
      void SetupDelegates();
      void ApplyPropertiesToEntity(dtEntity::EntityId, const QString& componentType,
                                   const dtEntity::DynamicPropertyContainer&);
      void ApplyPropertiesToSpawner(const QString& name, const QString& componentType,
                                    const dtEntity::DynamicPropertyContainer&, bool updateEntities);
      void ApplyPropertiesToEntitySystem(const QString&, const dtEntity::DynamicPropertyContainer&);
      
      void OnAddComponentToSpawner(const QString& spawner, const QString& ctype);
      void OnAddComponentToEntity(dtEntity::EntityId id, const QString& ctype);

      void OnRemoveComponentFromSpawner(const QString& spawner, const QString& ctype);
      void OnRemoveComponentFromEntitySystem(const QString& es, const QString& ctype);
      void OnRemoveComponentFromEntity(dtEntity::EntityId id, const QString& ctype);

      void OnRequestUpdateComponent(dtEntity::EntityId id, dtEntity::StringId componentType);
      void OnRequestUpdateAll(dtEntity::EntityId id);
      void OnRequestUpdateEntitySystem(const QString& name);

      void OnSpawnerAdditionalPropertiesChanged(const QString& name, bool addToStore,
                                                const QString& category, const QString& iconPath);
   private:

      void UpdatePropertyInEntities(const std::string& spawnerName, dtEntity::ComponentType ctype,
                                    dtEntity::StringId name, const dtEntity::Property& oldprop,
                                    const dtEntity::Property& newprop);

      dtEntity::EntityManager* mEntityManager;
      dtEntity::MessageFunctor mEntitySelectedFunctor;
      dtEntity::MessageFunctor mSpawnerSelectedFunctor;
      dtEntity::MessageFunctor mEntitySystemSelectedFunctor;
      dtEntity::MessageFunctor mMapSelectedFunctor;
      dtEntity::MessageFunctor mEntitySystemAddedFunctor;
      dtEntity::MessageFunctor mEntitySystemRemovedFunctor;
      dtEntity::MessageFunctor mComponentDataChangedFunctor;
      
   };
}
