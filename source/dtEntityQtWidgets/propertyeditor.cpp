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

#include <dtEntityQtWidgets/propertyeditor.h>

#include <dtEntity/basemessages.h>
#include <dtEntityQtWidgets/assetselector.h>
#include <dtEntityQtWidgets/delegatefactory.h>
#include <dtEntityQtWidgets/listdialog.h>
#include <dtEntityQtWidgets/messages.h>
#include <dtEntityQtWidgets/propertydelegate.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/spawner.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "ui_propertyeditor.h"
#include <set>

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   TreeItem::TreeItem(TreeItem *parent, DelegateFactory* childfactory)
      : mParentItem(parent)
      , mChildDelegateFactory(childfactory)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   TreeItem::~TreeItem()
   {
      qDeleteAll(mChildItems);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TreeItem::appendChild(TreeItem *item)
   {
      mChildItems.append(item);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TreeItem::removeChild(TreeItem* child)
   {
      bool success = mChildItems.removeOne(child);
      if(!success)
      {
         LOG_WARNING("Could not remove component, not found!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   TreeItem* TreeItem::child(int row)
   {
      return mChildItems.value(row);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int TreeItem::childCount() const
   {
      return mChildItems.count();
   }

   ////////////////////////////////////////////////////////////////////////////////
   TreeItem* TreeItem::parent()
   {
      return mParentItem;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int TreeItem::row() const
   {
      if (mParentItem)
      {
         return mParentItem->mChildItems.indexOf(const_cast<TreeItem*>(this));
      }
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyTreeItem::PropertyTreeItem(TreeItem *parent, DelegateFactory* childfactory, const QString& name, dtEntity::Property* prop, PropertySubDelegate* dlgt)
      : TreeItem(parent, childfactory)
      , mChanged(false)
      , mName(name)
      , mProperty(prop)
      , mDelegate(dlgt)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyTreeItem::~PropertyTreeItem() 
   {
      delete mProperty;
      delete mDelegate;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorModel::PropertyEditorModel()
   : mRootItem(new RootTreeItem(new DelegateFactory()))
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorModel::~PropertyEditorModel()
   {
      delete mRootItem;

      for(QMap<QString, Translator*>::iterator i = mTranslators.begin(); i != mTranslators.end(); ++i)
      {
         delete *i;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int PropertyEditorModel::rowCount(const QModelIndex &parent) const
   {
     TreeItem *parentItem;

     if (!parent.isValid())
         parentItem = mRootItem;
     else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());

     return parentItem->childCount();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int PropertyEditorModel::columnCount(const QModelIndex &parent) const
   {
      return 3;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant PropertyEditorModel::data(const QModelIndex &index, int role) const
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
               TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
               ComponentTreeItem* citem = dynamic_cast<ComponentTreeItem*>(item);
               if(citem)
               {
                  return QIcon(":icons/component.png");
               }
               PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
               if(pitem)
               {
                  return pitem->mDelegate->GetIcon();
               }
            }
            return QVariant();
         }

         case Qt::ForegroundRole:
         {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            ComponentTreeItem* citem = dynamic_cast<ComponentTreeItem*>(item);
            if(citem)
            {
               return QColor(0, 0, 0.5);
            }
            return QColor(0, 0.5, 0);
            
         }
         case Qt::BackgroundRole:
         {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
          
            PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
            if(pitem && pitem->mChanged)
            {
               return QColor(255,128,0);
            }
            return QVariant();
         }

         case Qt::DisplayRole:
         case Qt::EditRole:
         {

            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            ComponentTreeItem* citem = dynamic_cast<ComponentTreeItem*>(item);            
            if(citem)
            {
               if(index.column() == 0)
               {
                  return TranslateComponentType(citem->mType);
               }         
            }

            PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
            if(pitem)
            {
               if(index.column() == 0)
               {
                  QString componentType = GetComponentFromProperty(pitem);
                  return TranslateProperty(componentType, pitem->mName);
               }
               else if(index.column() == 1)
               {
                  return pitem->mDelegate->GetEditableValue(*pitem->mProperty);
               }
               else if(index.column() == 2)
               {
                  QModelIndex parent = index.parent();
                  TreeItem* item = static_cast<TreeItem*>(parent.internalPointer());
                  if(dynamic_cast<ArrayDelegateFactory*>(item->GetChildDelegateFactory()) != NULL)
                  {
                     return "-";
                  }
                  return "";
               }
            }
            break;
         }
         case Qt::ToolTipRole:
            
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
            if(pitem)
            {
               if(index.column() == 0)
               {
                  QString componentType = GetComponentFromProperty(pitem);
                  return TranslateDescription(componentType, pitem->mName);
               }
              
            }
         break;            
      }
      return QVariant();
   }


   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex PropertyEditorModel::index(int row, int column, const QModelIndex &parent) const
   {
      if (!hasIndex(row, column, parent))
         return QModelIndex();

      TreeItem* parentItem;

      if(!parent.isValid())
         parentItem = mRootItem;
      else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());

      TreeItem *childItem = parentItem->child(row);
      if (childItem)
         return createIndex(row, column, childItem);
      else
         return QModelIndex();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QModelIndex PropertyEditorModel::parent(const QModelIndex &index) const
   {
      if (!index.isValid())
         return QModelIndex();

      TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
      TreeItem* parentItem = childItem->parent();

      if (parentItem == mRootItem)
         return QModelIndex();

      return createIndex(parentItem->row(), 0, parentItem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant PropertyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      if (role != Qt::DisplayRole)
         return QVariant();

      if (orientation == Qt::Horizontal)
      {
         switch(section)
         {
         case 0 : return tr("Name");
         case 1 : return tr("Value");
         default: return "";
         }
      }
      return QVariant();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Qt::ItemFlags PropertyEditorModel::flags(const QModelIndex &index) const
   {
      if (!index.isValid()) return 0;
      if(index.column() != 1)
      {
         return Qt::ItemIsEnabled;
      }

      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      if(pitem != NULL)
      {
         if(pitem->mDelegate == NULL)
         {
            return Qt::ItemIsEnabled;
         }
         return pitem->mDelegate->GetEditFlags();
      }
      else
      {
         return Qt::ItemIsEnabled;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
   {
      if (!index.isValid()  || index.column() != 1) return false;

      // if nothing changed
      if(value == data(index, role))
      {
         return true;
      }
      
      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      
      if(!pitem)
      {
         return false;
      }

      pitem->mDelegate->SetValueByString(*pitem->mProperty, value.toString());

      // walk item tree upwards marking properties as changed
      QModelIndex idx = index;      
      while(pitem != NULL)
      {  
         pitem->mChanged = true;
         emit(dataChanged(idx, idx));
         
         // re-load all properties into array or group. They are held redundantly :(
         if(pitem->mProperty->GetType() == dtEntity::DataType::ARRAY)
         {
            dtEntity::PropertyArray arr;
            for(int i = 0; i < pitem->childCount(); ++i)
            {
               PropertyTreeItem* child = static_cast<PropertyTreeItem*>(pitem->child(i));
               arr.push_back(child->mProperty);
            }
            static_cast<dtEntity::ArrayProperty*>(pitem->mProperty)->Set(arr);
         }
         else if(pitem->mProperty->GetType() == dtEntity::DataType::GROUP)
         {
            dtEntity::PropertyGroup grp;

            // keep __SELECTED__ value for switch properties
            dtEntity::StringIdProperty enabled;
            dtEntity::StringId selstr = dtEntity::SID("__SELECTED__");
            dtEntity::PropertyGroup pg = pitem->mProperty->GroupValue();

            if(pg.find(selstr) != pg.end())
            {
               enabled.Set(pg[selstr]->StringIdValue());
               grp[selstr] = &enabled;
            }

            for(int i = 0; i < pitem->childCount(); ++i)
            {
               PropertyTreeItem* child = static_cast<PropertyTreeItem*>(pitem->child(i));
               if(child->mName != "__SELECTED__")
               {
                  grp[dtEntity::SID(child->mName.toStdString())] = child->mProperty;
               }
            }

            static_cast<dtEntity::GroupProperty*>(pitem->mProperty)->Set(grp);
         }

         pitem = dynamic_cast<PropertyTreeItem*>(pitem->parent());
         if(pitem == NULL)
         {
            break;
         }
         idx = parent(idx);
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::Reset()
   {
      beginResetModel();
      DelegateFactory* f = mRootItem->GetChildDelegateFactory();
      delete mRootItem;
      mRootItem = new RootTreeItem(f);
      
      endResetModel();
      emit ResetView();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::ResetChangedFlag(PropertyTreeItem* item)
   {
      item->mChanged = false;
      emit(dataChanged(createIndex(item->row(), 0, item), createIndex(item->row(), 1, item)));
      for(int i = 0; i < item->childCount(); ++i)
      {
         ResetChangedFlag(static_cast<PropertyTreeItem*>(item->child(i)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentTreeItem* PropertyEditorModel::AddComponent(const QString& name)
   {
      unsigned int size = mRootItem->childCount();
      beginInsertRows(QModelIndex(), size, size);  
      
      ComponentTreeItem* componentItem = new ComponentTreeItem(mRootItem, mRootItem->GetChildDelegateFactory()->GetFactoryForChildren(name), name);
      mRootItem->appendChild(componentItem);
      endInsertRows();
      return componentItem;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::RemoveComponent(const QString& ctype)
   {
      TreeItem* root = GetRootItem();
      for(int i = 0; i < root->childCount(); ++i)
      {
         ComponentTreeItem* component = static_cast<ComponentTreeItem*>(root->child(i));
         if(component->mType == ctype)
         {
            beginRemoveRows(QModelIndex(), i, i);  
            component->parent()->removeChild(component);
            delete component;
            endRemoveRows();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QList<dtEntity::ComponentType> PropertyEditorModel::GetComponentTypesInCurrent() const
   {
      QList<dtEntity::ComponentType> ret;
      std::set<dtEntity::ComponentType>::const_iterator i;
      for(i = mComponentTypes.begin(); i != mComponentTypes.end(); ++i)
      {
         if(FindComponent(*i))
         {
            ret.push_back(*i);
         }
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ComponentTreeItem* PropertyEditorModel::FindComponent(dtEntity::ComponentType ctype) const
   {
      QString ctypename = dtEntity::GetStringFromSID(ctype).c_str();
      TreeItem* root = GetRootItem();
      for(int i = 0; i < root->childCount(); ++i)
      {
         ComponentTreeItem* component = static_cast<ComponentTreeItem*>(root->child(i));
         if(component->mType == ctypename)
         {
            return component;
         }
      }
      return  NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditorModel::TranslateComponentType(const QString& componenttype) const
   {
      QMap<QString, Translator*>::const_iterator found = mTranslators.find(componenttype);
      if(found == mTranslators.end())
      {
         return componenttype;
      }
      return (*found)->mComponentName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditorModel::TranslateProperty(const QString& componenttype, const QString& propertyname) const
   {
      QMap<QString, Translator*>::const_iterator found = mTranslators.find(componenttype);
      if(found == mTranslators.end())
      {
         return propertyname;
      }
      Translator* t = *found;
      QMap<QString,QString>::const_iterator propiter = t->mPropertyNames.find(propertyname);
      if(propiter == t->mPropertyNames.end())
      {
         return propertyname;
      }
      return *propiter;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditorModel::TranslateDescription(const QString& componenttype, const QString& propertyname) const
   {
      QMap<QString, Translator*>::const_iterator found = mTranslators.find(componenttype);
      if(found == mTranslators.end())
      {
         return "";
      }
      Translator* t = *found;
      QMap<QString,QString>::const_iterator propiter = t->mDescriptions.find(propertyname);
      if(propiter == t->mDescriptions.end())
      {
         return "";
      }
      return *propiter;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::SwitchChanged(const QModelIndex& index)
   {
      emit(SwitchWasChanged(index));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::AddProperties(TreeItem* parent, const dtEntity::PropertyContainer& props)
   {
      dtEntity::PropertyContainer::ConstPropertyMap map;
      props.GetProperties(map);

      // map is sorted by string ID, have to sort by string value, otherwise
      // order is chaotic.
      std::map<std::string, const dtEntity::Property*> sortedMap;
      dtEntity::PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = map.begin(); i != map.end(); ++i)
      {
         sortedMap[dtEntity::GetStringFromSID(i->first)] = i->second;
      }

      // delegatefactory is a factory for creating specialized editors for the properties
      DelegateFactory* delegateFactory = parent->GetChildDelegateFactory();
            
      // Signal to views attached to this model that rows are about to be inserted
      int numitems = parent->childCount();
      beginInsertRows(createIndex(parent->row(), 0, parent), numitems, numitems + map.size());  
      
      // loop through properties and insert them into model
      std::map<std::string, const dtEntity::Property*>::const_iterator j;
      for(j = sortedMap.begin(); j != sortedMap.end(); ++j)
      {
         QString propname = j->first.c_str();

         const dtEntity::Property* prop = j->second;

         PropertySubDelegate* dlgt = delegateFactory->Create(parent, propname, prop);
         
         PropertyTreeItem* pitem = new PropertyTreeItem(parent, delegateFactory->GetFactoryForChildren(propname), propname, prop->Clone(), dlgt);
         parent->appendChild(pitem);

         if(prop->GetType() == dtEntity::DataType::GROUP)
         {
            // add group properties
            dtEntity::PropertyGroup grp = prop->GroupValue();
            dtEntity::DynamicPropertyContainer cmap;
            for(dtEntity::PropertyGroup::iterator j = grp.begin(); j != grp.end(); ++j)
            {
               cmap.AddProperty(j->first, *j->second);
            }
            AddProperties(pitem, cmap);
         }
         else if(prop->GetType() == dtEntity::DataType::ARRAY)
         {
            // add array entries
            dtEntity::PropertyArray grp = prop->ArrayValue();
            dtEntity::DynamicPropertyContainer cmap;
            int index = 0;
            for(dtEntity::PropertyArray::iterator j = grp.begin(); j != grp.end(); ++j)
            {
               std::ostringstream idx;
               idx << std::setfill('0') << std::setw(6) << index++;
               cmap.AddProperty(dtEntity::StringId(dtEntity::SID(idx.str())), **j);
            }
            AddProperties(pitem, cmap);
         }
      }
      endInsertRows();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::RemoveProperties(TreeItem* parent)
   {
      beginRemoveRows(createIndex(parent->row(), 0, parent), 0, parent->childCount());
      while(parent->childCount() > 0)
      {
         TreeItem* child = parent->child(0);
         parent->removeChild(child);
         delete child;
      }
      endRemoveRows();
   }


   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::AppendArrayEntry(const QModelIndex& index)
   {
      TreeItem* parent = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pparent = dynamic_cast<PropertyTreeItem*>(parent);
      assert(pparent);
      assert(pparent->mProperty->GetType() == dtEntity::DataType::ARRAY);
         
      ArrayPropertyDelegate* dlgt = dynamic_cast<ArrayPropertyDelegate*>(pparent->mDelegate);
      assert(dlgt);
      dtEntity::ArrayProperty* arrprop = dynamic_cast<dtEntity::ArrayProperty*>(pparent->mProperty);

      dtEntity::ArrayProperty* clone = dynamic_cast<dtEntity::ArrayProperty*>(arrprop->Clone());
      dtEntity::PropertyArray arr = clone->Get();      
      
      dtEntity::Property* proto = dlgt->GetDataPrototype();
      if(proto == NULL)
      {
         LOG_WARNING("Cannot append array entry: No prototype defined in delegate config!");
      }
      else
      {
         arr.push_back(proto);
         arrprop->Set(arr);
         dtEntity::DynamicPropertyContainer pc;
         dtEntity::StringId name = dtEntity::SID(QString("%1").arg(arr.size() - 1).toStdString());
         pc.AddProperty(name, *proto);
         AddProperties(parent, pc);
      }
      delete clone;

      emit ExpandTree(index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::RemoveArrayEntry(const QModelIndex& index)
   {
      TreeItem* parent = static_cast<TreeItem*>(index.parent().internalPointer());
      PropertyTreeItem* pparent = dynamic_cast<PropertyTreeItem*>(parent);
      assert(pparent);
      assert(pparent->mProperty->GetType() == dtEntity::DataType::ARRAY);
      ArrayPropertyDelegate* dlgt = dynamic_cast<ArrayPropertyDelegate*>(pparent->mDelegate);
      assert(dlgt);
      dtEntity::ArrayProperty* arrprop = dynamic_cast<dtEntity::ArrayProperty*>(pparent->mProperty);
      assert(arrprop);
      int row = index.row();
      dtEntity::PropertyArray pa = arrprop->Get();
      assert(pa.size() > (unsigned int)row);
      //pa.erase(pa.begin() + row);
      dtEntity::ArrayProperty* np = new dtEntity::ArrayProperty();
      int newcount = 0;
      dtEntity::DynamicPropertyContainer pc;
      for(unsigned int i = 0; i < pa.size(); ++i)
      {
         if(i != (unsigned int)row)
         {
            std::ostringstream os;
            os << newcount;
            ++newcount;

            pc.AddProperty(dtEntity::SID(os.str()), *pa[i]);
            np->Add(pa[i]->Clone());
         }
      }
      delete pparent->mProperty;
      pparent->mProperty = np;
      pparent->mChanged = true;
      RemoveProperties(parent);
      AddProperties(parent, pc);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::EntitySystemAdded(dtEntity::ComponentType ctype)
   {
      if(mComponentTypes.find(ctype) != mComponentTypes.end())
      {
         return;
      }
      QString componentName = dtEntity::GetStringFromSID(ctype).c_str();
      QString fileName = QString("Delegates/%1.xml").arg(componentName);
      DelegateFactory* factory = NULL;
      Translator* translator = NULL;
      bool found = CreateDelegateFactory(fileName, factory, translator);
      if(found)
      {
         
         GetRootItem()->GetChildDelegateFactory()->SetFactoryForChildren(componentName, factory);
         if(factory->AllowCreateFromGUI())
         {
            mComponentTypes.insert(ctype);
         }

         if(translator != NULL)
         {
            assert(mTranslators.find(componentName) == mTranslators.end());
            mTranslators[componentName] = translator;
         }
      }
      else
      {
         mComponentTypes.insert(ctype);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::EntitySystemRemoved(dtEntity::ComponentType t)
   {
      mComponentTypes.erase(t);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::OnEntitySelected(dtEntity::EntityId id)
   {
      mSelectedEntity = id;
      mSelectedSpawner = "";
      mSelectedEntitySystem = "";
      Reset();

      emit EditEntity(QString("%1").arg(id));
   }

   ///////////////////////////////////////////////////////////////////////////////
  void PropertyEditorModel::OnMapSelected(const QString& name)
  {
     Reset();
     emit EditNone();
  }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::OnEntitySystemSelected(const QString& name)
   {
      mSelectedEntity = 0;
      mSelectedSpawner = "";
      mSelectedEntitySystem = name;
      Reset();
      emit EditEntitySystem(name);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::OnSpawnerSelected(const QString& name, bool addToStore, const QString& category, const QString& iconPath)
   {
      mSelectedEntity = 0;
      mSelectedSpawner = name;
      mSelectedEntitySystem = "";
      Reset();
      emit EditSpawner(name, addToStore, category, iconPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::OnComponentDataChanged(dtEntity::EntityId id, dtEntity::StringId componenttype)
   {
      if(mSelectedEntity == id)
      {
         emit RequestUpdateComponent(id, componenttype);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::RequestRefresh()
   {
      if(mSelectedEntity != 0)
      {
         emit RequestUpdateAll(mSelectedEntity);
      }
      else if(mSelectedEntitySystem != "")
      {
         emit RequestUpdateEntitySystem(mSelectedEntitySystem);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::EmitChanges()
   {
      TreeItem* root = GetRootItem();
      for(int i = 0; i < root->childCount(); ++i)
      {
         dtEntity::DynamicPropertyContainer props;
         bool changes = false;
         ComponentTreeItem* component = static_cast<ComponentTreeItem*>(root->child(i));
         for(int j = 0; j < component->childCount(); ++j)
         {
            PropertyTreeItem* prop = static_cast<PropertyTreeItem*>(component->child(j));
            if(prop->mChanged)
            {
               ResetChangedFlag(prop);
               changes = true;
               props.AddProperty(dtEntity::SID(prop->mName.toStdString()), *prop->mProperty);
            }
         }
         if(changes)
         {
            if(mSelectedSpawner != "")
            {
               emit(ApplyPropertiesToSpawner(mSelectedSpawner, component->mType,  props, true));
            }
            else if(mSelectedEntitySystem != "")
            {
               emit(ApplyPropertiesToEntitySystem(mSelectedEntitySystem, props));
            }
            else
            {
               emit(ApplyPropertiesToEntity(mSelectedEntity, component->mType,  props));
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::AddComponentToCurrent(const QString& ctype)
   {
      if(mSelectedSpawner != "")
      {
         emit(AddComponentToSpawner(mSelectedSpawner, ctype));
      }
      else if(mSelectedEntity != 0)
      {
         emit(AddComponentToEntity(mSelectedEntity, ctype));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::OnRemoveComponentFromCurrent(const QString& ctype)
   {
      if(mSelectedSpawner != "")
      {
         emit(RemoveComponentFromSpawner(mSelectedSpawner, ctype));
      }
      else if(mSelectedEntitySystem != "")
      {
         emit(RemoveComponentFromEntitySystem(mSelectedEntitySystem, ctype));
      }
      else
      {
         emit(RemoveComponentFromEntity(mSelectedEntity, ctype));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::ComponentRetrieved(dtEntity::ComponentType ctype, const dtEntity::DynamicPropertyContainer& props)
   {
      QString componentName = dtEntity::GetStringFromSID(ctype).c_str();

      ComponentTreeItem* componentItem = FindComponent(ctype);
      if(componentItem == NULL)
      {
         componentItem = AddComponent(componentName);
      }
      else
      {
         RemoveProperties(componentItem);
      }
      
      if(!props.Empty())
      {
         AddProperties(componentItem, props);
      }
      emit ExpandFullTree();
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::ComponentDeleted(dtEntity::ComponentType t)
   {
      RemoveComponent(dtEntity::GetStringFromSID(t).c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditorModel::GetComponentFromProperty(TreeItem* item) const
   {
      ComponentTreeItem* ci = dynamic_cast<ComponentTreeItem*>(item);
      if(ci)
      {
         return ci->mType;
      }

      TreeItem* parent = item->parent();
      if(parent == NULL)
         return "Root";
      return GetComponentFromProperty(parent);
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorView::PropertyEditorView(QWidget* parent)
      : QWidget(parent)
   {
      Ui_PropertyEditor ui;
      ui.setupUi(this);

      mComponentTree = ui.mComponentTree;
      mApplyButton = ui.mApplyButton;
      mRefreshButton = ui.mRefreshButton;
      mEditedItemName = ui.mEditedItemName;
      mAddComponentButton = ui.mAddComponentButton;
      mRemoveComponentButton = ui.mRemoveComponentButton;
      mItemOptions = ui.mItemOptions;

      mSpawnerCategoryName = ui.mSpawnerCategoryName;
      mSpawnerAddToStore = ui.mSpawnerAddToStore;
      mSpawnerIconPath = ui.mSpawnerIconPath;

      connect(ui.mSpawnerChooseIconButton, SIGNAL(pressed()), this, SLOT(OnChooseSpawnerIcon()));
      connect(mApplyButton, SIGNAL(pressed()), this, SLOT(ApplyAdditionalProperties()));
      
      //this->setWidget(new MyWidget());
      mComponentTree->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(mComponentTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
      connect(mComponentTree, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnClick(const QModelIndex&)));

      connect(mAddComponentButton, SIGNAL(clicked()), this, SLOT(ShowAddComponentDialog()));
      connect(mRemoveComponentButton, SIGNAL(clicked()), this, SLOT(RemoveSelectedComponent()));

      mAppendArrayEntry = new QAction("Append Entry", this);
      connect(mAppendArrayEntry, SIGNAL(triggered(bool)), this, SLOT(ActionAppendArrayEntry(bool)));
      mComponentTree->setSortingEnabled(true);
      mComponentTree->sortByColumn(1, Qt::DescendingOrder);
      mComponentTree->viewport()->installEventFilter(this);

      PropertyEditorDelegate* propertyDelegate = new PropertyEditorDelegate(this);
      GetComponentTree()->setItemDelegate(propertyDelegate);


   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorView::~PropertyEditorView()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyEditorView::eventFilter(QObject* watched, QEvent* event)
   {
      
      if (watched != mComponentTree->viewport())
         return QWidget::eventFilter(watched, event);

      if(event->type() == QEvent::MouseButtonPress)
      {
         QMouseEvent* me = static_cast<QMouseEvent*>(event);
         QModelIndex index = mComponentTree->indexAt(me->pos());
         
         if(index.isValid())
         {
            
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
            if(pitem)
            {
               QPoint p = me->pos();
               QRect rect = mComponentTree->visualRect(index);
               pitem->mDelegate->MouseButtonPressed(index, p.x() - rect.left(), p.y() - rect.top());
            }
            
         }
      }
      return QWidget::eventFilter(watched, event);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnEditSpawner(const QString& name, bool addToStore, const QString& category, const QString& iconPath)
   {
      mEditedItemName->setText(QString("Spawner: %1").arg(name));
      mItemOptions->setCurrentIndex(1);
      mSpawnerCategoryName->setText(category);
      mSpawnerAddToStore->setChecked(addToStore);
      mSpawnerIconPath->setText(iconPath);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnEditEntity(const QString& name)
   {
      mEditedItemName->setText(QString("Entity: %1").arg(name));
      mItemOptions->setCurrentIndex(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnEditEntitySystem(const QString& name)
   {
      mEditedItemName->setText(QString("Entity System: %1").arg(name));
      mItemOptions->setCurrentIndex(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnEditNone()
   {
      mEditedItemName->setText("");
      mItemOptions->setCurrentIndex(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::EnableAddComponent(bool enable)
   {
      mAddComponentButton->setEnabled(enable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::ShowContextMenu(const QPoint& p)
   {     

      QModelIndex index = mComponentTree->indexAt(p);
      if(index.isValid()) 
      {
         mContextMenuSelectedIndex = index;
         TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
         PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
         if(pitem)
         {
            if(pitem->mProperty->GetType() == dtEntity::DataType::ARRAY)
            {
               QMenu menu(this);
               menu.addAction(mAppendArrayEntry);
               menu.exec(mComponentTree->mapToGlobal(p));
            }
         }
      }   
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnClick(const QModelIndex& idx)
   {
      PropertyEditorModel* modl = static_cast<PropertyEditorModel*>(GetComponentTree()->model());
      QModelIndex i = idx;
      TreeItem* item;
      while(true)
      {
         item = static_cast<TreeItem*>(i.internalPointer());         
         if(item->parent() == modl->GetRootItem())
         {
            break;
         }
         i = i.parent();
         
      }

      ComponentTreeItem* citem = dynamic_cast<ComponentTreeItem*>(item);
      if(citem)
      {
         mSelectedComponent = citem->mType;
         mRemoveComponentButton->setEnabled(true);
      }
      else
      {
         mSelectedComponent = "";
         mRemoveComponentButton->setEnabled(false);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::SetColumnWidths()
   {
      mComponentTree->setColumnWidth(0, 200);
      mComponentTree->setColumnWidth(1, 200);
      mComponentTree->setColumnWidth(2, 20);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::SwitchWasChanged(const QModelIndex& parent)
   {
      // hide children of switches that are not currently selected
      TreeItem* item = static_cast<TreeItem*>(parent.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      if(pitem)
      {
         if(dynamic_cast<SwitchPropertyDelegate*>(pitem->mDelegate) != NULL)
         {
            dtEntity::PropertyGroup grp = pitem->mProperty->GroupValue();

            dtEntity::PropertyGroup::iterator sel = grp.find(dtEntity::SID("__SELECTED__"));
            if(sel != grp.end())
            {
               dtEntity::StringId selindex = sel->second->StringIdValue();
               std::string selstr = dtEntity::GetStringFromSID((selindex));
               for(int i = 0; i < parent.model()->rowCount(parent); ++i)
               {
                  QModelIndex idx = parent.model()->index(i, 0, parent);
                  if(idx.isValid())
                  {
                     PropertyTreeItem* citem = static_cast<PropertyTreeItem*>(idx.internalPointer());
                     bool hide = (dtEntity::SID(citem->mName.toStdString()) != selindex);
                     mComponentTree->setRowHidden(i, parent, hide);
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::RowsInserted(const QModelIndex& parent,int start, int end)
   {
      SwitchWasChanged(parent);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::ShowAddComponentDialog()
   {
      PropertyEditorModel* modl = static_cast<PropertyEditorModel*>(GetComponentTree()->model());
      std::set<dtEntity::ComponentType> ctypes = modl->GetComponentTypes();
      
      QStringList entries;

      std::set<dtEntity::ComponentType>::const_iterator i;
      for(i = ctypes.begin(); i != ctypes.end(); ++i)
      {
         if(modl->FindComponent(*i) == NULL)
         {
            entries.push_back(dtEntity::GetStringFromSID(*i).c_str());
         }
      }
      entries.sort();
      
      ListDialog* dialog = new ListDialog(entries, true);
      dialog->setWindowTitle(tr("Add Components"));
      
      if(dialog->exec() == QDialog::Accepted)
      {  
         QStringList sel = dialog->GetSelectedItems();
         for(QStringList::iterator i = sel.begin(); i != sel.end(); ++i)
         {
            emit(AddComponentToCurrent(*i));
         }
      }

      dialog->close();
      dialog->deleteLater();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::RemoveSelectedComponent()
   {
      if(mSelectedComponent != "")
      {
         emit(RemoveComponentFromCurrent(mSelectedComponent));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::ApplyAdditionalProperties()
   {
      if(mItemOptions->currentIndex() == 1)
      {
         PropertyEditorModel* modl = static_cast<PropertyEditorModel*>(GetComponentTree()->model());
         QString spawnerName = modl->GetSelectedSpawner();
         if(spawnerName != "")
         {
            emit SpawnerAdditionalPropertiesChanged(spawnerName, mSpawnerAddToStore->isChecked(),
                                         mSpawnerCategoryName->text(), mSpawnerIconPath->text());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::OnChooseSpawnerIcon()
   {
      AssetSelector sel("*.ico *.png *.jpg *.gif *.tga", this);
      if(sel.exec() == QDialog::Accepted)
      {
         mSpawnerIconPath->setText(sel.GetSelected());
      }
      sel.close();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorView::ActionAppendArrayEntry(bool)
   {
      if(mContextMenuSelectedIndex.isValid())
      {
         emit(AppendArrayEntry(mContextMenuSelectedIndex));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorController::PropertyEditorController(dtEntity::EntityManager* entityManager)
   : mEntityManager(entityManager)
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorController::~PropertyEditorController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::EntitySelectedMessage::TYPE, mEntitySelectedFunctor);
      mEntityManager->UnregisterForMessages(SpawnerSelectedMessage::TYPE, mSpawnerSelectedFunctor);
      mEntityManager->UnregisterForMessages(EntitySystemSelectedMessage::TYPE, mEntitySystemSelectedFunctor);
      mEntityManager->UnregisterForMessages(MapSelectedMessage::TYPE, mMapSelectedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::EntitySystemAddedMessage::TYPE, mEntitySystemAddedFunctor);      
      mEntityManager->UnregisterForMessages(dtEntity::EntitySystemRemovedMessage::TYPE, mEntitySystemRemovedFunctor);
      mEntityManager->UnregisterForMessages(dtEntityQtWidgets::ComponentDataChangedMessage::TYPE, mComponentDataChangedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::SetupSlots(PropertyEditorModel* model, PropertyEditorView* view)
   {

      connect(this, SIGNAL(EntitySelected(dtEntity::EntityId)), model, SLOT(OnEntitySelected(dtEntity::EntityId)));
      connect(this, SIGNAL(SpawnerSelected(const QString&, bool, const QString&, const QString&)),
              model, SLOT(OnSpawnerSelected(const QString&, bool, const QString&, const QString&)));
      connect(this, SIGNAL(MapSelected(const QString&)), model, SLOT(OnMapSelected(const QString&)));
      connect(this, SIGNAL(EntitySystemSelected(const QString&)), model, SLOT(OnEntitySystemSelected(const QString&)));
      connect(
         this, 
         SIGNAL(ComponentRetrieved(dtEntity::ComponentType, const dtEntity::DynamicPropertyContainer&)),
         model, 
         SLOT(ComponentRetrieved(dtEntity::ComponentType, const dtEntity::DynamicPropertyContainer&))
      );

      connect(
         this, 
         SIGNAL(ComponentDeleted(dtEntity::ComponentType)),
         model, 
         SLOT(ComponentDeleted(dtEntity::ComponentType))
      );

      connect(this, SIGNAL(EnableAddComponent(bool)), view, SLOT(EnableAddComponent(bool)));
      connect(this, SIGNAL(ComponentDataChanged(dtEntity::EntityId, dtEntity::StringId)),
              model, SLOT(OnComponentDataChanged(dtEntity::EntityId, dtEntity::StringId)));


      connect(this, SIGNAL(EntitySystemAdded(dtEntity::ComponentType)),
         model, SLOT(EntitySystemAdded(dtEntity::ComponentType)));

      connect(this, SIGNAL(EntitySystemRemoved(dtEntity::ComponentType)),
         model, SLOT(EntitySystemRemoved(dtEntity::ComponentType)));


      /////////////////////////////////////////////

      connect(model, SIGNAL(ExpandTree(const QModelIndex&)), view->GetComponentTree(), SLOT(expand(const QModelIndex&)));
      connect(model, SIGNAL(ExpandFullTree()), view, SLOT(ExpandTree()));
      connect(model, SIGNAL(ResetView()), view, SLOT(SetColumnWidths()));

      connect(model, SIGNAL(EditSpawner(const QString&, bool, const QString&, const QString&)),
              view, SLOT(OnEditSpawner(const QString&, bool, const QString&, const QString&)));
      connect(model, SIGNAL(EditEntity(const QString&)), view, SLOT(OnEditEntity(const QString&)));
      connect(model, SIGNAL(EditEntitySystem(const QString&)), view, SLOT(OnEditEntitySystem(const QString&)));
      connect(model, SIGNAL(EditNone()), view, SLOT(OnEditNone()));

      connect(model, SIGNAL(ApplyPropertiesToEntity(dtEntity::EntityId, const QString&, const dtEntity::DynamicPropertyContainer&)),
      this, SLOT(ApplyPropertiesToEntity(dtEntity::EntityId, const QString&, const dtEntity::DynamicPropertyContainer&)));

      connect(model, SIGNAL(ApplyPropertiesToSpawner(const QString&, const QString&, const dtEntity::DynamicPropertyContainer&, bool)),
      this, SLOT(ApplyPropertiesToSpawner(const QString&, const QString&, const dtEntity::DynamicPropertyContainer&, bool)));

      connect(model, SIGNAL(ApplyPropertiesToEntitySystem(const QString&, const dtEntity::DynamicPropertyContainer&)),
      this, SLOT(ApplyPropertiesToEntitySystem(const QString&, const dtEntity::DynamicPropertyContainer&)));
      

      connect(model, SIGNAL(AddComponentToSpawner(const QString&, const QString&)), 
         this, SLOT(OnAddComponentToSpawner(const QString&, const QString&)));
      connect(model, SIGNAL(AddComponentToEntity(dtEntity::EntityId, const QString&)),
         this, SLOT(OnAddComponentToEntity(dtEntity::EntityId, const QString&)));

      connect(model, SIGNAL(RemoveComponentFromSpawner(const QString&, const QString&)), 
         this, SLOT(OnRemoveComponentFromSpawner(const QString&, const QString&)));
      connect(model, SIGNAL(RemoveComponentFromEntitySystem(const QString&, const QString&)),
         this, SLOT(OnRemoveComponentFromEntitySystem(const QString&, const QString&)));
      connect(model, SIGNAL(RemoveComponentFromEntity(dtEntity::EntityId, const QString&)),
         this, SLOT(OnRemoveComponentFromEntity(dtEntity::EntityId, const QString&)));

      connect(model, SIGNAL(RequestUpdateComponent(dtEntity::EntityId, dtEntity::StringId)),
               this, SLOT(OnRequestUpdateComponent(dtEntity::EntityId, dtEntity::StringId)));

      connect(model, SIGNAL(RequestUpdateEntitySystem(const QString&)),
               this, SLOT(OnRequestUpdateEntitySystem(const QString&)));

      connect(model, SIGNAL(RequestUpdateAll(dtEntity::EntityId)), this, SLOT(OnRequestUpdateAll(dtEntity::EntityId)));

      /////////////////////////////////////////////

      connect(view, SIGNAL(AddComponentToCurrent(const QString&)), model, SLOT(AddComponentToCurrent(const QString&)));
      connect(view, SIGNAL(RemoveComponentFromCurrent(const QString&)), model, SLOT(OnRemoveComponentFromCurrent(const QString&)));

      connect(view->GetApplyButton(), SIGNAL(clicked()), model, SLOT(EmitChanges()));
      connect(view->GetRefreshButton(), SIGNAL(clicked()), model, SLOT(RequestRefresh()));
      connect(view, SIGNAL(AppendArrayEntry(const QModelIndex&)), model, SLOT(AppendArrayEntry(const QModelIndex&)));
      connect(view, SIGNAL(SpawnerAdditionalPropertiesChanged(const QString&, bool, const QString&, const QString&)),
              this, SLOT(OnSpawnerAdditionalPropertiesChanged(const QString&, bool, const QString&, const QString&)));
      connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), view, SLOT(RowsInserted(QModelIndex,int,int)));
      connect(model, SIGNAL(SwitchWasChanged(QModelIndex)), view, SLOT(SwitchWasChanged(QModelIndex)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::Init()
   {
      mEntitySelectedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnEntitySelected);
      mEntityManager->RegisterForMessages(dtEntity::EntitySelectedMessage::TYPE, mEntitySelectedFunctor);

      mSpawnerSelectedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnSpawnerSelected);
      mEntityManager->RegisterForMessages(SpawnerSelectedMessage::TYPE, mSpawnerSelectedFunctor);

      mEntitySystemSelectedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnEntitySystemSelected);
      mEntityManager->RegisterForMessages(EntitySystemSelectedMessage::TYPE, mEntitySystemSelectedFunctor);

      mMapSelectedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnMapSelected);
      mEntityManager->RegisterForMessages(MapSelectedMessage::TYPE, mMapSelectedFunctor);

      mEntitySystemAddedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnEntitySystemAdded);
      mEntityManager->RegisterForMessages(dtEntity::EntitySystemAddedMessage::TYPE, mEntitySystemAddedFunctor);

      mEntitySystemRemovedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnEntitySystemRemoved);
      mEntityManager->RegisterForMessages(dtEntity::EntitySystemRemovedMessage::TYPE, mEntitySystemRemovedFunctor);

      mComponentDataChangedFunctor = dtEntity::MessageFunctor(this, &PropertyEditorController::OnComponentDataChanged);
      mEntityManager->RegisterForMessages(dtEntityQtWidgets::ComponentDataChangedMessage::TYPE, mComponentDataChangedFunctor);

      SetupDelegates();

   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::SetupDelegates()
   {
      std::vector<const dtEntity::EntitySystem*> toFill;
      mEntityManager->GetEntitySystems(toFill);
      std::vector<const dtEntity::EntitySystem*>::const_iterator i;
      for(i = toFill.begin(); i != toFill.end(); ++i)
      {
         const dtEntity::EntitySystem* es = *i;
         dtEntity::ComponentType ctype = es->GetComponentType();       
         emit(EntitySystemAdded(ctype));         
      }

      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);
      dtEntity::ComponentPluginManager::PluginFactoryMap& factories = ms->GetPluginManager().GetFactories();
      dtEntity::ComponentPluginManager::PluginFactoryMap::const_iterator j;
      for(j = factories.begin(); j != factories.end(); ++j)
      {
         dtEntity::ComponentType t = j->first;
         emit(EntitySystemAdded(t));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnEntitySystemAdded(const dtEntity::Message& msg)
   {
      dtEntity::ComponentType t = (dtEntity::ComponentType)msg.GetStringId(dtEntity::EntitySystemAddedMessage::ComponentTypeId);  
      emit(EntitySystemAdded(t)); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnEntitySystemRemoved(const dtEntity::Message& msg)
   {
      dtEntity::ComponentType t = (dtEntity::ComponentType)msg.GetStringId(dtEntity::EntitySystemRemovedMessage::ComponentTypeId);
      emit(EntitySystemRemoved(t));       
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnEntitySelected(const dtEntity::Message& msg)
   {
      dtEntity::EntityId id = (dtEntity::EntityId) msg.GetUInt(dtEntity::SID("AboutEntity"));

      emit(EntitySelected(id));
      dtEntity::Entity* entity; 
      bool found = mEntityManager->GetEntity(id, entity);
      if(!found)
      {
         LOG_WARNING("Error: Entity no longer exists!");
         return;
      }

      typedef std::list<const dtEntity::Component*> CompList;
      CompList components;
      entity->GetComponents(components);
      for(CompList::const_iterator i = components.begin(); i != components.end(); ++i)
      {
         const dtEntity::Component* component = *i;
         dtEntity::PropertyContainer::ConstPropertyMap pmap;
         component->GetProperties(pmap);  
         dtEntity::DynamicPropertyContainer props;
         props.SetProperties(pmap);
         emit(ComponentRetrieved(component->GetType(), props));
      }

      emit EnableAddComponent(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnEntitySystemSelected(const dtEntity::Message& msg)
   {
      std::string name = msg.GetString(dtEntity::SID("Name"));
      emit(EntitySystemSelected(name.c_str()));

      dtEntity::StringId sid = dtEntity::SID(name);
      dtEntity::EntitySystem* es = mEntityManager->GetEntitySystem(sid);
      if(es == NULL)
      {
         return;
      }

      dtEntity::PropertyContainer::ConstPropertyMap pmap;
      es->GetProperties(pmap);  
      dtEntity::DynamicPropertyContainer props;
      props.SetProperties(pmap);
      emit(ComponentRetrieved(es->GetComponentType(), props));
      emit EnableAddComponent(false);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnMapSelected(const dtEntity::Message& msg)
   {
      std::string mapName = msg.GetString(dtEntity::SID("Name"));
      emit(MapSelected(mapName.c_str()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnSpawnerSelected(const dtEntity::Message& msg)
   {
      std::string spawnerName = msg.GetString(dtEntity::SID("Name"));

      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

      dtEntity::Spawner* spawner;
      bool found = ms->GetSpawner(spawnerName, spawner);
      if(!found)
      {
         LOG_WARNING("Selected spawner not found!");
         return;
      }

      emit SpawnerSelected(spawner->GetName().c_str(), spawner->GetAddToSpawnerStore(),
                           spawner->GetGUICategory().c_str(), spawner->GetIconPath().c_str());

      dtEntity::Spawner::ComponentProperties cprops;
      spawner->GetAllComponentPropertiesRecursive(cprops);

      dtEntity::Spawner::ComponentProperties::const_iterator i;
      for(i = cprops.begin(); i != cprops.end(); ++i)
      {
         dtEntity::StringId ctype = i->first;
         const dtEntity::DynamicPropertyContainer props = i->second;
         emit(ComponentRetrieved(ctype, props));
      }

      emit EnableAddComponent(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnComponentDataChanged(const dtEntity::Message& m)
   {
      const dtEntityQtWidgets::ComponentDataChangedMessage& msg =
         static_cast<const dtEntityQtWidgets::ComponentDataChangedMessage&>(m);
      emit ComponentDataChanged(msg.GetAboutEntityId(), msg.GetComponentType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::ApplyPropertiesToEntity(dtEntity::EntityId id, const QString& componentType, const dtEntity::DynamicPropertyContainer& props)
   {
      dtEntity::StringId ctype = dtEntity::SID(componentType.toStdString());
      dtEntity::EntitySystem* es;
      bool success = mEntityManager->GetEntitySystem(ctype, es);
      if(!success)
      {
         LOG_WARNING("Error applying entity changes: No component system of this type: " + componentType.toStdString());
         return;
      }
      dtEntity::Component* comp;
      success = es->GetComponent(id, comp);
      if(!success)
      {
         LOG_WARNING("Error applying entity changes: Entity has no component of this type: " + componentType.toStdString());
         return;
      }
      dtEntity::PropertyContainer::ConstPropertyMap map;
      props.GetProperties(map);

      dtEntity::PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = map.begin(); i != map.end(); ++i)
      {
         dtEntity::StringId sid = i->first;
         const dtEntity::Property* prop = i->second;
         dtEntity::Property* propToSet = comp->Get(sid);
         if(propToSet)
         {
            propToSet->SetFrom(*prop);
            comp->OnPropertyChanged(sid, *propToSet);
         }
         else
         {
            std::ostringstream os;
            os << "Error setting property: Component " << componentType.toStdString();
            os << " has no property named " << dtEntity::GetStringFromSID(sid);
            LOG_WARNING(os.str());
         }
      }

      // callback that components can use to rebuild their data after
      // a number of data changes
      comp->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::ApplyPropertiesToEntitySystem(const QString& componentType, const dtEntity::DynamicPropertyContainer& props)
   {

      dtEntity::StringId ctype = dtEntity::SID(componentType.toStdString());
      dtEntity::EntitySystem* es;
      bool success = mEntityManager->GetEntitySystem(ctype, es);
      if(!success)
      {
         LOG_WARNING("Error applying entity system changes: No system of this type: " + componentType.toStdString());
         return;
      }
      dtEntity::PropertyContainer::ConstPropertyMap map;
      props.GetProperties(map);

      dtEntity::PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = map.begin(); i != map.end(); ++i)
      {
         dtEntity::StringId sid = i->first;
         const dtEntity::Property* prop = i->second;
         dtEntity::Property* propToSet = es->Get(sid);
         if(propToSet)
         {
            propToSet->SetFrom(*prop);
            es->OnPropertyChanged(sid, *propToSet);
         }
         else
         {
            std::ostringstream os;
            os << "Error setting property: Entity system " << componentType.toStdString();
            os << " has no property named " << dtEntity::GetStringFromSID(sid);
            LOG_WARNING(os.str());
         }
      }

      // callback that components can use to rebuild their data after
      // a number of data changes
      es->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::UpdatePropertyInEntities(const std::string& spawnerName, dtEntity::ComponentType ctype, dtEntity::StringId propname, const dtEntity::Property& oldprop, const dtEntity::Property& newprop)
   {
      dtEntity::EntitySystem* componentSystem = mEntityManager->GetEntitySystem(ctype);
      if(componentSystem == NULL)
      {
         LOG_DEBUG("Entity system for component not found!");
         return;
      }

      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);
      
      std::list<dtEntity::EntityId> ids;
      ms->GetEntitiesInSystem(ids);

      std::list<dtEntity::EntityId>::const_iterator i;
      for(i = ids.begin(); i != ids.end(); ++i)
      {
         dtEntity::MapComponent* mc;
         mEntityManager->GetComponent(*i, mc);
         
         if(mc->GetString(dtEntity::MapComponent::SpawnerNameId) != spawnerName)
         {
            continue;
         }
         dtEntity::Component* targetComp;
         if(!mEntityManager->HasComponent(*i, ctype))
         {
            componentSystem->CreateComponent(*i, targetComp);
         }
         else
         {
            componentSystem->GetComponent(*i, targetComp);
         }

         dtEntity::Property* targetProp = targetComp->Get(propname);
         assert(targetProp != NULL);
         if(targetProp->operator==(oldprop))
         {
            targetProp->SetFrom(newprop);
            targetComp->OnPropertyChanged(propname, *targetProp);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::ApplyPropertiesToSpawner(const QString& spawnerName, const QString& componentType, const dtEntity::DynamicPropertyContainer& props, bool updateEntities)
   {
      // get spawner
      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);
      dtEntity::Spawner* spawner;
      bool found = ms->GetSpawner(spawnerName.toStdString(), spawner);
      if(!found)
      {
         LOG_WARNING("Selected spawner not found!");
         return;
      }

      // get current component values from spawner ///////////////////////////////
      dtEntity::StringId ctype = dtEntity::SID(componentType.toStdString());
      dtEntity::DynamicPropertyContainer currentSpawnerComponentProps = spawner->GetComponentValues(ctype);
      
      // Loop through incoming properties and check if they are already set in spawner.
      // If not, create a component default property
      dtEntity::EntitySystem* es = mEntityManager->GetEntitySystem(ctype);
      if(es)
      {
         dtEntity::DynamicPropertyContainer defaultprops = es->GetComponentProperties();

         dtEntity::PropertyContainer::ConstPropertyMap incoming;
         props.GetProperties(incoming);

         dtEntity::PropertyContainer::ConstPropertyMap::const_iterator i;
         for(i = incoming.begin(); i != incoming.end(); ++i)
         {
            if(!currentSpawnerComponentProps.Has(i->first))
            {
               currentSpawnerComponentProps.AddProperty(i->first, *defaultprops.Get(i->first));
            }
         }
      }


      // Apply received properties to spawner
      dtEntity::PropertyContainer::ConstPropertyMap map;
      props.GetProperties(map);
      
      dtEntity::PropertyContainer::ConstPropertyMap::const_iterator j;
      for(j = map.begin(); j != map.end(); ++j)
      {
         dtEntity::StringId sid = j->first;
         const dtEntity::Property* newprop = j->second;
         dtEntity::Property* oldprop = currentSpawnerComponentProps.Get(sid);
         if(oldprop == NULL)
         {
            LOG_WARNING("Cannot set property: Not found!");
            continue;
         }
         if(updateEntities)
         {
            UpdatePropertyInEntities(spawnerName.toStdString(), ctype, sid, *oldprop, *newprop);
         }
         oldprop->SetFrom(*newprop);
      }

      spawner->SetComponentValues(ctype, currentSpawnerComponentProps);
   }  

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnAddComponentToSpawner(const QString& spawnerName, const QString& ctypestr)
   {
      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

      dtEntity::ComponentType ctype = dtEntity::SID(ctypestr.toStdString());
      dtEntity::EntitySystem* compsys;
      bool found = mEntityManager->GetEntitySystem(ctype, compsys);
      if(!found)
      {
         if(ms->GetPluginManager().FactoryExists(ctype))
         {
            ms->GetPluginManager().StartEntitySystem(ctype);
            found = mEntityManager->GetEntitySystem(ctype, compsys);
            assert(found);
         }
         else
         {
            LOG_WARNING("Cannot add component to spawner: Entity system not found");
            return;
         }
      }
      
      dtEntity::Spawner* spawner;
      found = ms->GetSpawner(spawnerName.toStdString(), spawner);
      if(!found)
      {
         LOG_WARNING("Selected spawner not found!");
         return;
      }
      
      if(spawner->HasComponent(ctype))
      {
         LOG_WARNING("Component already exists in spawner!");
         return;
      }
      
      dtEntity::DynamicPropertyContainer dynprops = compsys->GetComponentProperties();
      spawner->AddComponent(ctype, dynprops);
      emit(ComponentRetrieved(ctype, dynprops));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnAddComponentToEntity(dtEntity::EntityId id, const QString& ctypestr)
   {
      dtEntity::EntitySystem* es;
      dtEntity::ComponentType ctype = dtEntity::SID(ctypestr.toStdString());
      bool found = mEntityManager->GetEntitySystem(ctype, es);
      if(!found)
      {
         dtEntity::MapSystem* ms;
         mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

         if(ms->GetPluginManager().FactoryExists(ctype))
         {
            ms->GetPluginManager().StartEntitySystem(ctype);
            found = mEntityManager->GetEntitySystem(ctype, es);
            if(!found)
            {
               LOG_WARNING("Cannot find entity system of type " + dtEntity::GetStringFromSID(ctype));
            }
         }
         else
         {
            LOG_WARNING("Cannot add component to entity: Entity system not found");
            return;
         }
      }

      if(es->HasComponent(id))
      {
         std::ostringstream os;
         os << "Could not create component, already exists for entity " << id << ": ";
         os << ctypestr.toStdString();
         LOG_WARNING(os.str());
         return;
      }
      dtEntity::Component* comp;
      found = es->CreateComponent(id, comp);

      if(!found)
      {
         LOG_WARNING("Could not create component " + ctypestr.toStdString());
         return;
      }

      // special case: If layer component was created, send layer system
      // message informing it to add the entity to its layer
      if(ctype == dtEntity::LayerComponent::TYPE)
      {
         dtEntity::EntityAddedToSceneMessage m;
         m.SetAboutEntityId(id);
         static_cast<dtEntity::LayerSystem*>(es)->OnEnterWorld(m);
      }

      comp->Finished();

      dtEntity::PropertyContainer::ConstPropertyMap pmap;
      comp->GetProperties(pmap);  
      dtEntity::DynamicPropertyContainer props;
      props.SetProperties(pmap);
      emit(ComponentRetrieved(ctype, props));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRemoveComponentFromSpawner(const QString& spawnername, const QString& ctypestr)
   {
	  dtEntity::ComponentType ctype = dtEntity::SID(ctypestr.toStdString());
	  
	  dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

	  dtEntity::Spawner* spawner;
	  bool found = ms->GetSpawner(spawnername.toStdString(), spawner);
      if(!found)
      {
         LOG_WARNING("Spawner not found, could not remove component of type " + ctypestr.toStdString());
         return;
      }

	  
      found = spawner->RemoveComponent(ctype);

      if(!found)
      {
		 LOG_WARNING("Could not delete component from spawner: " + ctypestr.toStdString());
      }

      emit(ComponentDeleted(ctype));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRemoveComponentFromEntitySystem(const QString& es, const QString& ctype)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRemoveComponentFromEntity(dtEntity::EntityId id, const QString& ctypestr)
   {
      dtEntity::EntitySystem* es;
      dtEntity::ComponentType ctype = dtEntity::SID(ctypestr.toStdString());
      bool found = mEntityManager->GetEntitySystem(ctype, es);
      if(!found)
      {
         LOG_WARNING("Entity system not found, could not remove component of type " + ctypestr.toStdString());
         return;
      }

      // special case: If layer component was created, send layer system
      // message informing it to add the entity to its layer
      if(ctype == dtEntity::LayerComponent::TYPE)
      {
         dtEntity::EntityRemovedFromSceneMessage m;
         m.SetAboutEntityId(id);
         static_cast<dtEntity::LayerSystem*>(es)->OnLeaveWorld(m);
      }

      found = es->DeleteComponent(id);

      if(!found)
      {
         LOG_WARNING("Could not delete component " + ctypestr.toStdString());
      }

      emit(ComponentDeleted(ctype));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRequestUpdateComponent(dtEntity::EntityId id, dtEntity::StringId componentType)
   {
      dtEntity::Component* component;
      bool found = mEntityManager->GetComponent(id, componentType, component);
      if(found)
      {
         dtEntity::PropertyContainer::ConstPropertyMap pmap;
         component->GetProperties(pmap);
         dtEntity::DynamicPropertyContainer props;
         props.SetProperties(pmap);
         emit ComponentRetrieved(componentType, props);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRequestUpdateEntitySystem(const QString& name)
   {
      dtEntity::StringId sid = dtEntity::SID(name.toStdString());
      dtEntity::EntitySystem* es;
      bool found = mEntityManager->GetEntitySystem(sid, es);
      if(found)
      {
         dtEntity::PropertyContainer::ConstPropertyMap pmap;
         es->GetProperties(pmap);
         dtEntity::DynamicPropertyContainer props;
         props.SetProperties(pmap);
         emit ComponentRetrieved(sid, props);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnRequestUpdateAll(dtEntity::EntityId id)
   {
      dtEntity::RequestEntitySelectMessage m;
      m.SetAboutEntityId(id);
      OnEntitySelected(m);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorController::OnSpawnerAdditionalPropertiesChanged(const QString& spawnerName,
                                             bool addToStore, const QString& category, const QString& iconPath)
   {
      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

      dtEntity::Spawner* spawner;
      bool found = ms->GetSpawner(spawnerName.toStdString(), spawner);
      if(!found)
      {
         LOG_WARNING("Cannot set additional properties on spawner: Spawner not found!");
         return;
      }
      std::string oldcategory = spawner->GetGUICategory();
      spawner->SetAddToSpawnerStore(addToStore);
      spawner->SetGUICategory(category.toStdString());
      spawner->SetIconPath(iconPath.toStdString());
      dtEntity::SpawnerModifiedMessage msg;
      msg.SetName(spawner->GetName());
      msg.SetMapName(spawner->GetMapName());
      msg.SetOldCategory(oldcategory);
      msg.SetNewCategory(spawner->GetGUICategory());
      mEntityManager->EmitMessage(msg);
   }
} 
