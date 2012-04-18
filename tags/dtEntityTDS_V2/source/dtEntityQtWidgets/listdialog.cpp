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

#include <dtEntityQtWidgets/listdialog.h>
#include <QtGui/QtGui>
#include "ui_listdialog.h"

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   ListDialog::ListDialog(const QStringList& l, bool allowMultipleSel)
   { 
      Ui_ListDialog dialog;
      dialog.setupUi(this);
      mListWidget = dialog.mFileList;
      mListWidget->insertItems(0, l);

      if(allowMultipleSel)
      {
         mListWidget->setSelectionMode(QAbstractItemView::MultiSelection);         
      }
      else
      {
         mListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
      }

      connect(mListWidget, SIGNAL(doubleClicked(const QModelIndex&)),
         this, SLOT(ItemDoubleClicked(const QModelIndex&)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   ListDialog::~ListDialog()
   { 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ListDialog::ItemDoubleClicked(const QModelIndex& index)
   {
      done(1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QStringList ListDialog::GetSelectedItems() const
   {
      QStringList ret;
      QList<QListWidgetItem*> selectedItems = mListWidget->selectedItems();
      QList<QListWidgetItem*>::const_iterator i;
      for(i = selectedItems.begin(); i != selectedItems.end(); ++i)
      {
         ret.push_back((*i)->text());
      }
      return ret;
   }
}
