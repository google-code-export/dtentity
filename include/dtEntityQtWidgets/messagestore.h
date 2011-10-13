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
#include <QtGui/QtGui>

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT MessageStoreView
      : public QWidget
   {
      Q_OBJECT

   public:
      
      MessageStoreView(QWidget* parent = NULL);

      virtual ~MessageStoreView();

   public slots:

      void OnPassMessageTypes(const QStringList& types) { mMessageTypes = types; }
      void OnAddExistingMessagePrototype(const QString& name);
      void OnRemoveExistingMessagePrototype(const QString& name);
   protected slots:

      void OnAddMessageButtonClicked(bool);
      void OnRemoveMessageButtonClicked(bool);
      void OnEmitMessageButtonClicked(bool);
      void CurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
      void ItemClicked(QListWidgetItem* current);
      void ItemDoubleClicked(QListWidgetItem* current);
      void ShowContextMenu(const QPoint&);
      void OnRemoveItemAction(bool);

   signals:

      void MessagePrototypeAdded(const QString& name, const QString& msgtype);
      void MessagePrototypeRemoved(const QString& name);
      void MessagePrototypeEmit(const QString& name);
      void MessagePrototypeSelected(const QString& name);

   private:
      QStringList mMessageTypes;
      QListWidget* mMessageList;
      QToolButton* mAddMessageButton;
      QToolButton* mRemoveMessageButton;
      QPushButton* mEmitMessageButton;

      QListWidgetItem* mContextMenuSelectedItem;
      QAction* mRemoveItemAction;
   }; 

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT MessageStoreController
      : public QObject
   {
      Q_OBJECT

   public:
      
      MessageStoreController(dtEntity::EntityManager*);
      virtual ~MessageStoreController();

      void SetupSlots(MessageStoreView* view);

      void OnSpawnerAdded(const dtEntity::Message& m);
      void OnSpawnerRemoved(const dtEntity::Message& m);

   public slots:
   
      void Init();
      void OnMessagePrototypeAdded(const QString& name, const QString& msgtype);
      void OnMessagePrototypeRemoved(const QString& name);
      void OnMessagePrototypeEmit(const QString& name);
      void OnMessagePrototypeSelected(const QString& name);

   signals:

      void PassMessageTypes(const QStringList& types);
      void AddExistingMessagePrototype(const QString& name);
      void RemoveExistingMessagePrototype(const QString& name);


   private:
      dtEntity::EntityManager* mEntityManager;
      dtEntity::MessageFunctor mSpawnerAddedFunctor;
      dtEntity::MessageFunctor mSpawnerRemovedFunctor;
   };
}
