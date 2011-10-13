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
#include <QtGui/QTreeView>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtCore/QModelIndex>
#include <QtGui/QFileSystemModel>
#include <dtEntityQtWidgets/export.h>

namespace dtEntityQtWidgets
{
   class ENTITYQTWIDGETS_EXPORT AssetSelector 
      : public QDialog
   {
      Q_OBJECT

   public:
      
     AssetSelector(const QString& filters, QWidget* parent = 0);
     ~AssetSelector();

     QString GetSelected() const;

     void ClearPreviewWindow();
     void ShowThumbNailImage(const QString& path);

   private slots:
        void Selected();
        void Cancel();
        void Activated(const QModelIndex&);
        void OnDataPathChanged(const QString&);
        void OnSelectionChanged(const QItemSelection&, const QItemSelection&);
   private:
      QString mSelected;
      QString mRootPath;
      QFileSystemModel* mModel;
      QComboBox* mDataPathSelector;
      QLabel* mFilePathLabel;
      QWidget* mPreviewContents;
      QWidget* mPreviewWindow;
      QTreeView* mFileTree;
   };

}
