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

#include <dtEntityQtWidgets/datapatheditor.h>
#include <QtGui/QFileDialog>
#include <QtCore/QSettings>
#include <assert.h>
#include "ui_datapatheditor.h"

namespace dtEntityQtWidgets
{
      
   ////////////////////////////////////////////////////////////////////////////////
   DataPathEditor::DataPathEditor(QWidget* parent)
      : QDialog(parent)
   {
      Ui_DataPathEditor ui;
      ui.setupUi(this);

      mList = ui.mPathList;
      mAddButton = ui.mAddButton;
      mEditButton = ui.mEditButton;
      mRemoveButton = ui.mRemoveButton;
      mUpButton = ui.mUpButton;
      mDownButton = ui.mDownButton;

      QSettings settings;
      QStringList datapaths = settings.value("DataPaths").toStringList();

      foreach(const QString& str, datapaths)
      {
         mList->addItem(str);
      }

      connect(mList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
              this, SLOT(CurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
      connect(mList, SIGNAL(itemDoubleClicked (QListWidgetItem*)),
              this, SLOT(ItemDoubleClicked(QListWidgetItem*)));
      connect(mAddButton, SIGNAL(clicked(bool)), this, SLOT(AddClicked(bool)));
      connect(mEditButton, SIGNAL(clicked(bool)), this, SLOT(EditClicked(bool)));
      connect(mRemoveButton, SIGNAL(clicked(bool)), this, SLOT(RemoveClicked(bool)));
      connect(mUpButton, SIGNAL(clicked(bool)), this, SLOT(UpClicked(bool)));
      connect(mDownButton, SIGNAL(clicked(bool)), this, SLOT(DownClicked(bool)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   QStringList DataPathEditor::GetSelectedDataPaths()
   {
      QStringList path;
      for(int i = 0; i < mList->count(); ++i)
      {
         path.push_back(mList->item(i)->text());
      }

      return path;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::ItemDoubleClicked(QListWidgetItem*)
   {
      EditClicked(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::CurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous )
   {
      bool enabled = (current != NULL);
      mEditButton->setEnabled(enabled);
      mRemoveButton->setEnabled(enabled);
      mUpButton->setEnabled(enabled);
      mDownButton->setEnabled(enabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::AddClicked(bool)
   {
      QString path = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), mLastSelectedDir);
      if(path == "")
      {
         return;
      }
      mLastSelectedDir = path;
      mList->addItem(path);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::EditClicked(bool)
   {
      QString currentpath = mList->currentItem()->text();
      QString path = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), currentpath);
      if(path == "" || path == currentpath)
      {
         return;
      }
      mList->selectedItems().front()->setText(path);
      mLastSelectedDir = path;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::RemoveClicked(bool)
   {
      if(mList->currentItem() != NULL)
      {
         delete mList->takeItem(mList->currentRow());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::UpClicked(bool)
   {
      if(mList->currentItem() != NULL)
      {
         int row = mList->currentRow();
         if(row == 0) return;
         QListWidgetItem* tmp = mList->takeItem(row);
         mList->insertItem(row - 1, tmp);
         mList->setCurrentRow(row - 1);

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DataPathEditor::DownClicked(bool)
   {
      if(mList->currentItem() != NULL)
      {
         int row = mList->currentRow();
         if(row == mList->count() - 1) return;
         QListWidgetItem* tmp = mList->takeItem(row);
         mList->insertItem(row + 1, tmp);
         mList->setCurrentRow(row + 1);
      }
   }
}
