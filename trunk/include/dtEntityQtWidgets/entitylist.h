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
#include <QtCore/QAbstractTableModel>
#include <QtGui/QTableView>
#include <dtEntityQtWidgets/export.h>
#include <dtEntity/entityid.h>
#include <dtEntity/entitymanager.h>

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   struct TableEntry
   {
      dtEntity::EntityId mEntityId;
      QString mName;    
   };

   
   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityListModel
      : public QAbstractTableModel
   {
      Q_OBJECT

   public:

      EntityListModel();
      ~EntityListModel();

   public slots:      
     
      int rowCount(const QModelIndex &parent) const;
      int columnCount(const QModelIndex &parent) const;
      QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const;
      bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

      void EntityJoined(const TableEntry& data);
      void EntityLeft(dtEntity::EntityId id);
      
   private:
     
      std::vector<TableEntry> mEntries;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityListView
      : public QTableView
   {
      Q_OBJECT

   public:
      
      EntityListView(QWidget *parent = 0);

      virtual ~EntityListView();

   signals:
      void EntitySelected(dtEntity::EntityId);

   protected slots:

      void ShowContextMenu(const QPoint&);     
      void OnClick(const QModelIndex&);
      
   private:

      dtEntity::EntityId          mSelectedEntity;
      
   }; 

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT EntityListController
      : public QObject
   {
      Q_OBJECT

   public:      

      EntityListController(dtEntity::EntityManager*);
      virtual ~EntityListController();

      void OnEnterWorld(const dtEntity::Message& msg);
      void OnLeaveWorld(const dtEntity::Message& msg);
      void SetupSlots(EntityListModel* model, EntityListView*);

   public slots:
      void Init();
      void EntitySelected(dtEntity::EntityId id);

   signals:
      void EntityJoined(const TableEntry& data);
      void EntityLeft(dtEntity::EntityId id);

   private:

      dtEntity::EntityManager* mEntityManager;
      dtEntity::MessageFunctor mEnterWorldFunctor;
      dtEntity::MessageFunctor mLeaveWorldFunctor;
   };

}
