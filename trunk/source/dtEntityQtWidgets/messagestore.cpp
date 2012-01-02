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

#include <dtEntityQtWidgets/messagestore.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/message.h>
#include <dtEntity/messagefactory.h>
#include <dtEntity/spawner.h>
#include <dtEntityQtWidgets/messages.h>
#include "ui_messagestore.h"

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   MessageStoreView::MessageStoreView(QWidget* parent)
      : QWidget(parent)
      , mContextMenuSelectedItem(NULL)
   {
      Ui_MessageStore ui;
      ui.setupUi(this);
      mMessageList = ui.mMessageList;
      mAddMessageButton = ui.mAddMessageButton;
      mRemoveMessageButton = ui.mRemoveMessageButton;
      mEmitMessageButton = ui.mEmitMessageButton;

      connect(mAddMessageButton, SIGNAL(clicked(bool)), this, SLOT(OnAddMessageButtonClicked(bool)));
      connect(mRemoveMessageButton, SIGNAL(clicked(bool)), this, SLOT(OnRemoveMessageButtonClicked(bool)));
      connect(mEmitMessageButton, SIGNAL(clicked(bool)), this, SLOT(OnEmitMessageButtonClicked(bool)));
      connect(mMessageList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
              this, SLOT(CurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
      connect(mMessageList, SIGNAL(itemClicked(QListWidgetItem*)),
              this, SLOT(ItemClicked(QListWidgetItem*)));

      connect(mMessageList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
              this, SLOT(ItemDoubleClicked(QListWidgetItem*)));
      mMessageList->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(mMessageList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

      mRemoveItemAction = new QAction("Remove Item", this);
      connect(mRemoveItemAction, SIGNAL(triggered(bool)), this, SLOT(OnRemoveItemAction(bool)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageStoreView::~MessageStoreView()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::ShowContextMenu(const QPoint& p)
   {
      mContextMenuSelectedItem = mMessageList->itemAt(p);

      if(mContextMenuSelectedItem != NULL)
      {
         QMenu menu(this);
         menu.addAction(mRemoveItemAction);
         menu.exec(mMessageList->mapToGlobal(p));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnRemoveItemAction(bool)
   {
      if(mContextMenuSelectedItem != NULL)
      {
         emit MessagePrototypeRemoved(mContextMenuSelectedItem->text());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::CurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous )
   {
      bool enabled = (current != NULL);
      mRemoveMessageButton->setEnabled(enabled);
      mEmitMessageButton->setEnabled(enabled);
      if(enabled)
      {
         emit MessagePrototypeSelected(current->text());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::ItemClicked(QListWidgetItem* current)
   {
      bool enabled = (current != NULL);
      mRemoveMessageButton->setEnabled(enabled);
      mEmitMessageButton->setEnabled(enabled);
      emit MessagePrototypeSelected(current->text());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::ItemDoubleClicked(QListWidgetItem* current)
   {
      emit MessagePrototypeEmit(current->text());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnAddMessageButtonClicked(bool)
   {
      if(mMessageTypes.empty())
      {
         QMessageBox::critical(this, "No message types registered in entity manager!",
                               "No message types registered in entity manager!");
         return;
      }
      /*bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new message prototype"),
                                          tr("Enter name for new message prototype:"), QLineEdit::Normal,
                                          "TestMessage", &ok);
                                          */
      QDialog dialog;
      dialog.resize(300, 200);
      QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
      dialog.setLayout(layout);

      layout->addWidget(new QLabel("Select message type", &dialog));
      QComboBox* cbox = new QComboBox();
      foreach(QString mtype, mMessageTypes)
      {
         cbox->addItem(mtype);
      }

      layout->addWidget(cbox);

      layout->addWidget(new QLabel("Enter prototype name", &dialog));
      QLineEdit* line = new QLineEdit(&dialog);
      layout->addWidget(line);

      QPushButton* okbut = new QPushButton(&dialog);
      okbut->setText("OK");
      connect(okbut, SIGNAL(released()), &dialog, SLOT(accept()));
      layout->addWidget(okbut);
      dialog.exec();

      QString protoname = line->text();
      QString msgtype = cbox->currentText();

      if(!mMessageList->findItems(protoname, Qt::MatchExactly).empty())
      {
         QMessageBox::critical(this, "Message prototype with that name already exists!",
                               "Message prototype with that name already exists!");
      }
      else if(protoname == "")
      {
         QMessageBox::critical(this, "Please enter a prototype name!",
                               "Please enter a prototype name!");
      }
      else
      {
         emit MessagePrototypeAdded(protoname, msgtype);
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnAddExistingMessagePrototype(const QString& name)
   {
      int count = mMessageList->count();
      mMessageList->insertItem(count, name);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnRemoveExistingMessagePrototype(const QString& name)
   {
      for(int i = 0; i < mMessageList->count(); ++i)
      {
         if(mMessageList->item(i)->text() == name)
         {
            delete mMessageList->takeItem(i);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnRemoveMessageButtonClicked(bool)
   {
      emit MessagePrototypeRemoved(mMessageList->currentItem()->text());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreView::OnEmitMessageButtonClicked(bool)
   {
      if(mMessageList->currentItem() != NULL)
      {
         emit MessagePrototypeEmit(mMessageList->currentItem()->text());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   MessageStoreController::MessageStoreController(dtEntity::EntityManager* entityManager)
   : mEntityManager(entityManager)
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageStoreController::~MessageStoreController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::SetupSlots(MessageStoreView* view)
   {
      connect(view, SIGNAL(MessagePrototypeAdded(const QString&, const QString&)),
              this, SLOT(OnMessagePrototypeAdded(const QString&, const QString&)));

      connect(view, SIGNAL(MessagePrototypeRemoved(const QString&)),
              this, SLOT(OnMessagePrototypeRemoved(const QString&)));

      connect(view, SIGNAL(MessagePrototypeSelected(const QString&)),
              this, SLOT(OnMessagePrototypeSelected(const QString&)));

      connect(view, SIGNAL(MessagePrototypeEmit(const QString&)),
              this, SLOT(OnMessagePrototypeEmit(const QString&)));

      connect(this, SIGNAL(PassMessageTypes(const QStringList&)),
              view, SLOT(OnPassMessageTypes(const QStringList&)));

      connect(this, SIGNAL(AddExistingMessagePrototype(const QString&)),
              view, SLOT(OnAddExistingMessagePrototype(const QString&)));

      connect(this, SIGNAL(RemoveExistingMessagePrototype(const QString&)),
              view, SLOT(OnRemoveExistingMessagePrototype(const QString&)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::Init()
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::string mapname = "maps/messageprototypes.dtemap";
      if(!mtsystem->IsMapLoaded(mapname))
      {
         if(mtsystem->MapExists(mapname))
         {
            mtsystem->LoadMap(mapname);
         }
         else
         {
            mtsystem->AddEmptyMap(mapname);
         }
      }

      mSpawnerAddedFunctor = dtEntity::MessageFunctor(this, &MessageStoreController::OnSpawnerAdded);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);

      mSpawnerRemovedFunctor = dtEntity::MessageFunctor(this, &MessageStoreController::OnSpawnerRemoved);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor);

      std::vector<dtEntity::MessageType> types;
      mtsystem->GetMessageFactory().GetRegisteredMessageTypes(types);

      QStringList messagetypes;
      foreach(dtEntity::MessageType mtype, types)
      {
         messagetypes.push_back(dtEntity::GetStringFromSID(mtype).c_str());
      }
      messagetypes.sort();
      emit PassMessageTypes(messagetypes);

      std::map<std::string, dtEntity::Spawner*> spawners;
      mtsystem->GetAllSpawners(spawners);
      std::map<std::string, dtEntity::Spawner*>::const_iterator i;
      for(i = spawners.begin(); i != spawners.end(); ++i)
      {
         if(i->second->GetMapName() == "maps/messageprototypes.dtemap")
         {
            emit AddExistingMessagePrototype(i->second->GetName().c_str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnSpawnerAdded(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerAddedMessage& msg =
         static_cast<const dtEntity::SpawnerAddedMessage&>(m);

      if(msg.GetMapName() == "maps/messageprototypes.dtemap")
      {
         emit AddExistingMessagePrototype(msg.GetName().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnSpawnerRemoved(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerRemovedMessage& msg =
         static_cast<const dtEntity::SpawnerRemovedMessage&>(m);

      if(msg.GetMapName() == "maps/messageprototypes.dtemap")
      {
         emit RemoveExistingMessagePrototype(msg.GetName().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnMessagePrototypeAdded(const QString& name, const QString& msgtype)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::string mapname = "maps/messageprototypes.dtemap";
      
      dtEntity::Spawner* spawner = new dtEntity::Spawner(name.toStdString(), mapname);

      dtEntity::Message* msg;
      success = mtsystem->GetMessageFactory().CreateMessage(dtEntity::SIDHash(msgtype.toStdString()), msg);
      if(!success)
      {
         LOG_ERROR("Could not create message, message type not found in registry!");
         return;
      }
      dtEntity::PropertyContainer::ConstPropertyMap props;
      msg->GetProperties(props);
      dtEntity::DynamicPropertyContainer dprops;
      dprops.SetProperties(props);
      delete msg;

      dtEntity::StringProperty msgtypename;
      msgtypename.Set(msgtype.toStdString());
      dprops.AddProperty(dtEntity::SIDHash("MessageTypeName"), msgtypename);

      spawner->AddComponent(dtEntity::SIDHash("Message"), dprops);

      mtsystem->AddSpawner(*spawner);
      mtsystem->SaveMap(mapname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnMessagePrototypeRemoved(const QString& name)
   {
      dtEntity::MapSystem* mtsystem;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      bool success = mtsystem->DeleteSpawner(name.toStdString());
      if(!success)
      {
         LOG_ERROR("Could not delete message prototype spawner!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnMessagePrototypeSelected(const QString& name)
   {
      SpawnerSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageStoreController::OnMessagePrototypeEmit(const QString& name)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      dtEntity::Spawner* spawner;
      success = mtsystem->GetSpawner(name.toStdString(), spawner);
      if(!success)
      {
         LOG_ERROR("Cannot emit message: Spawner not found!");
         return;
      }
      dtEntity::DynamicPropertyContainer props = spawner->GetComponentValues(dtEntity::SIDHash("Message"));
      dtEntity::StringId msgtypenamesid = dtEntity::SIDHash("MessageTypeName");
      std::string msgtypename = props.GetString(msgtypenamesid);

      dtEntity::Message* msg;
      success = mtsystem->GetMessageFactory().CreateMessage(dtEntity::SIDHash(msgtypename), msg);
      if(!success)
      {
         LOG_ERROR("Cannot emit message: Message of this type is not registered: " + msgtypename);
         return;
      }
      dtEntity::PropertyContainer::ConstPropertyMap propvals;
      props.GetProperties(propvals);
      dtEntity::PropertyContainer::ConstPropertyMap::iterator i;
      for(i = propvals.begin(); i != propvals.end(); ++i)
      {
         if(i->first != msgtypenamesid)
         {
            dtEntity::Property* prop = msg->Get(i->first);
            if(prop == NULL)
            {
               LOG_ERROR("Cannot set message property with name " + dtEntity::GetStringFromSID(i->first));
            }
            else
            {
               prop->SetFrom(*i->second);
            }
         }
      }
      mEntityManager->EnqueueMessage(*msg);
      delete msg;

   }
} 
