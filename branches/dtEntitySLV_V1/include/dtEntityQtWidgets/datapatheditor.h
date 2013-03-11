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

#include <QtGui/QDialog>
#include <QtGui/QListWidget>
#include <QtCore/QModelIndex>
#include <dtEntityQtWidgets/export.h>

namespace dtEntityQtWidgets
{
   class ENTITYQTWIDGETS_EXPORT DataPathEditor
      : public QDialog
   {
      Q_OBJECT

   public:
      
      DataPathEditor(QWidget* parent = 0);

      QStringList GetSelectedDataPaths();

   private slots:

      void CurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
      void ItemDoubleClicked(QListWidgetItem*);
      void AddClicked(bool);
      void EditClicked(bool);
      void RemoveClicked(bool);
      void UpClicked(bool);
      void DownClicked(bool);

   private:


      QListWidget* mList;
      QPushButton* mAddButton;
      QPushButton* mEditButton;
      QPushButton* mRemoveButton;
      QPushButton* mUpButton;
      QPushButton* mDownButton;
      QString mLastSelectedDir;
   };

}
