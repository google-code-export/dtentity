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

#include <dtEntityQtWidgets/entitylist.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntityQtWidgets/messages.h>

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   EntityListModel::EntityListModel()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityListModel::~EntityListModel()
   {
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityListModel::EntityJoined(const TableEntry& data)
   {
      beginInsertRows(QModelIndex(), mEntries.size(), mEntries.size());  
      mEntries.push_back(data);
      endInsertRows();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListModel::EntityLeft(dtEntity::EntityId id)
   {
      unsigned int count = 0;
      std::vector<TableEntry>::iterator i = mEntries.begin();
      while(i != mEntries.end())
      {
         TableEntry entry = *i;
         if(entry.mEntityId == id)
         { 
            beginRemoveRows(QModelIndex(), count, count);
            i = mEntries.erase(i);
            endRemoveRows();         
         }
         else
         {
            ++i;
            ++count;
         }      
      }   
   }

   ////////////////////////////////////////////////////////////////////////////////
   int EntityListModel::rowCount(const QModelIndex &parent) const
   {
      return mEntries.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int EntityListModel::columnCount(const QModelIndex &parent) const
   {
      return 2;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant EntityListModel::data(const QModelIndex &index, int role) const
   {

      if (!index.isValid() || index.row() >= (int)mEntries.size() || index.row() < 0)
      {
         return QVariant();
      }

      if (role == Qt::DisplayRole)
      {
         if(index.row() > (int)mEntries.size() - 1)
         {
            return QVariant();
         }

         TableEntry entry = mEntries[index.row()];
         
         switch(index.column())
         {
            case 0:
            {
               return entry.mEntityId;
            }
            case 1:
            {
               return entry.mName;
            }
         }
      }
      return QVariant();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant EntityListModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      if (role != Qt::DisplayRole)
         return QVariant();

      if (orientation == Qt::Horizontal)
      {
         switch(section)
         {
            case 0: return "ID";
            case 1: return "Name";
            default: return "No Name for column";
         }
      }
      return QVariant();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EntityListModel::setData(const QModelIndex &index, const QVariant &value, int role)
   {
      return false;
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EntityListView::EntityListView(QWidget* parent)
      : QTableView(parent)
      , mSelectedEntity(0)
   {
      connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnClick(const QModelIndex&)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityListView::~EntityListView()
   {
   }


   ////////////////////////////////////////////////////////////////////////////////
   void EntityListView::ShowContextMenu(const QPoint&)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListView::OnClick(const QModelIndex& idx)
   {
      dtEntity::EntityId id(this->model()->data(this->model()->index(idx.row(), 0)).toInt());
      emit EntitySelected(id);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EntityListController::EntityListController(dtEntity::EntityManager* entityManager)
      : mEntityManager(entityManager)
   {      
   }

   ////////////////////////////////////////////////////////////////////////////////
   EntityListController::~EntityListController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);
   
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListController::SetupSlots(EntityListModel* model, EntityListView* view)
   {
      connect(view, SIGNAL(EntitySelected(dtEntity::EntityId)), this, SLOT(EntitySelected(dtEntity::EntityId)));

      connect(this, SIGNAL(EntityJoined(const TableEntry&)), model, SLOT(EntityJoined(const TableEntry&)));
      connect(this, SIGNAL(EntityLeft(dtEntity::EntityId)), model, SLOT(EntityLeft(dtEntity::EntityId)));        

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListController::Init()
   {
      qRegisterMetaType<TableEntry>("TableEntry");

      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &EntityListController::OnEnterWorld);
      mEntityManager->RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "EntityListController::OnEnterWorld");
         
      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &EntityListController::OnLeaveWorld);
      mEntityManager->RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "EntityListController::OnLeaveWorld");

      std::list<dtEntity::EntityId> ids;
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      if(success)
      {  
         mtsystem->GetEntitiesInSystem(ids);
      }

      if(!ids.empty())
      {
         for(std::list<dtEntity::EntityId>::iterator i = ids.begin(); i != ids.end(); ++i)
         {
            dtEntity::EntityId eid = *i;
            TableEntry entry;
            entry.mEntityId = eid;

            dtEntity::MapComponent* mcomp;
            if(mEntityManager->GetComponent(eid, mcomp))
            {
               entry.mName = mcomp->GetEntityName().c_str();
            }
            emit EntityJoined(entry);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListController::OnEnterWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityAddedToSceneMessage& msg = 
         static_cast<const dtEntity::EntityAddedToSceneMessage&>(m);
      dtEntity::EntityId eid = msg.GetAboutEntityId();

      TableEntry entry;
      entry.mEntityId = eid;

      dtEntity::MapComponent* mcomp;
      if(mEntityManager->GetComponent(eid, mcomp))
      {
         entry.mName = mcomp->GetEntityName().c_str();
      }
      emit(EntityJoined(entry));   
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EntityListController::OnLeaveWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityRemovedFromSceneMessage& msg = 
         static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(m);
      dtEntity::EntityId eid = msg.GetAboutEntityId();
      emit(EntityLeft(eid));   
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void EntityListController::EntitySelected(dtEntity::EntityId id)
   {
      dtEntity::RequestEntitySelectMessage msg;
      msg.SetAboutEntityId(id);
      mEntityManager->EmitMessage(msg);
   }
      
}
