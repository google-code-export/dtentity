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

#include <dtEntityQtWidgets/assetselector.h>

#include <QtGui/QLabel>
#include <QtGui/QTreeView>
#include <QtCore/QSettings>
#include <assert.h>
#include "ui_assetselector.h"
#include <QtSvg/QSvgRenderer>
#include <QtGui/QImage>
#include <QtGui/QPainter>

namespace dtEntityQtWidgets
{

   // taken from http://www.qtcentre.org/archive/index.php/t-37454.html
   // assumes that path and rootPath use the same dir separator and the same letter case
   void PopulatePathInModel(const QString& path, const QString& rootPath, QTreeView* view,  QFileSystemModel* model)
   {
      QStringList parts = path.split("/");

      QString currPath;
      foreach (QString part, parts)
      {
         if (!currPath.isEmpty()) currPath += "/";
         currPath += part;

         // no need to populate dirs outside of our area of interest
         if (!currPath.startsWith(rootPath)) continue;

         if (QFileInfo(currPath).isDir())
         {
            QModelIndex idx = model->index(currPath);
            if (idx.isValid())
            {
               while (model->canFetchMore(idx))
               {
                  model->fetchMore(idx);
                  view->expand(idx);
               }
            }
         }
      }
   }
      
   ////////////////////////////////////////////////////////////////////////////////
   AssetSelector::AssetSelector(const QString& filters, QWidget* parent, const QString& expandToPath)
      :  mModel(new QFileSystemModel())
      , mPreviewContents(NULL)
   {
      Ui_AssetSelector ui;
      ui.setupUi(this);
      this->setModal(true);
      this->resize(600, 700);

      mFilePathLabel = ui.mFilePathLabel;
      mDataPathSelector = ui.mDataPathSelector;

      mModel->setNameFilters(filters.split(" "));
      mModel->setNameFilterDisables(false);

      ui.mFileTree->setModel(mModel);
      ui.mFileTree->setSortingEnabled(true);
      ui.mFileTree->sortByColumn(0, Qt::AscendingOrder);

      // make file column wider
      ui.mFileTree->header()->resizeSection(0, 300);

      connect(ui.mSelectButton, SIGNAL(clicked()), this, SLOT(Selected()));
      connect(ui.mCancelButton, SIGNAL(clicked()), this, SLOT(Cancel()));

     // connect(ui.mFileTree, SIGNAL(clicked(const QModelIndex&)), this, SLOT(Clicked(const QModelIndex&)));
      connect(ui.mFileTree, SIGNAL(activated(const QModelIndex&)), this, SLOT(Activated(const QModelIndex&)));

      QItemSelectionModel* m = ui.mFileTree->selectionModel();
      connect(m, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
         this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));

      ui.mPreviewWindow->setLayout(new QHBoxLayout());

      mPreviewWindow = ui.mPreviewWindow;
      mFileTree = ui.mFileTree;

      QSettings settings;
      QStringList dataPaths = settings.value("DataPaths").toStringList();
      QString current;

      if(expandToPath == "")
      {
         current = settings.value("AssetSelectorCurrentDataPath", dataPaths.front()).toString();
      }
      else
      {
         QString path;
         foreach(path, dataPaths)
         {
            if(expandToPath.contains(path))
            {
               current = path;
            }
         }
      }

      int i = 0;
      foreach(const QString& str, dataPaths)
      {
         mDataPathSelector->addItem(str);
         if(str == current) 
         {
            mDataPathSelector->setCurrentIndex(i);
         }
         ++i;
      }
      
      OnDataPathChanged(current);   
      QString lastpath = settings.value("AssetSelectorCurrentSelection", current).toString();
      
      PopulatePathInModel(lastpath, current, mFileTree, mModel);

      connect(mDataPathSelector, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnDataPathChanged(const QString&)));

      if(expandToPath != "")
      {
         QModelIndex idx = mModel->index(expandToPath);

         mFileTree->setCurrentIndex(idx);

         // this does not seem to work:
         mFileTree->scrollTo(idx);

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   AssetSelector::~AssetSelector()
   {
      delete mModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString AssetSelector::GetSelected() const
   {
      return mSelected;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString AssetSelector::GetSelectedAbsPath() const
   {
      return mDataPathSelector->currentText() + QDir::separator() + mSelected;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::ClearPreviewWindow()
   {
      if(mPreviewContents != NULL)
      {
         mPreviewWindow->layout()->removeWidget(mPreviewContents);
         delete mPreviewContents;
         mPreviewContents = NULL;
      }      
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::ShowThumbNailImage(const QString& path)
   {
      mFilePathLabel->setText(path);
      QImage img;

      // QImage can load SVGs directly but there seems to be a bug in qt 4.7 that
      // made images from the wrong path appear. So treat SVG as a special case :(
      if(path.endsWith(".svg"))
      {
         img = QImage(200, 200, QImage::Format_ARGB32);
         QSvgRenderer renderer;
         if(!renderer.load(path))
         {
            return;
         }
         QPainter painter(&img);
         painter.setCompositionMode(QPainter::CompositionMode_Source);
         painter.fillRect(img.rect(), Qt::transparent);
         painter.scale(400.0f / 200, 400.0f / 200);
         renderer.render(&painter);
      }
      else
      {
         if(!img.load(path))
         {   
            return;
         }
         img = img.scaled(300, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      }
      
      QLabel* label = new QLabel();
      mPreviewContents = label;
       
      mPreviewWindow->layout()->addWidget(label);

      label->setPixmap(QPixmap::fromImage(img));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
   {
      ClearPreviewWindow();

      QItemSelection::const_iterator i;
      for(i = selected.begin(); i != selected.end(); ++i)
      {
         QModelIndexList indexes = i->indexes();
         for(QModelIndexList::iterator j = indexes.begin(); j != indexes.end(); ++j)
         {
            QModelIndex sel = *j;
            QFileSystemModel* model = static_cast<QFileSystemModel*>(mFileTree->model());
            if(!sel.isValid() || !model->fileInfo(sel).isFile())
            {
               return;
            }
            QString absPath = model->filePath(sel);
            QString thumbPath = QString("%1_thumb.png").arg(absPath);

            QDir dir(thumbPath);
            if(QFile::exists(thumbPath))
            {
               ShowThumbNailImage(thumbPath);  
               return;
            }    
            
            if(absPath.endsWith(".svg") || absPath.endsWith(".jpg") || absPath.endsWith(".png") || absPath.endsWith(".tga"))
            {
               ShowThumbNailImage(absPath);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::Activated(const QModelIndex& index)
   {
      Selected();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::Selected()
   {
      QModelIndexList selected = mFileTree->selectionModel()->selectedIndexes();
      if(selected.size() == 0)
      {
         done(QDialog::Rejected);
         return;
      }
      assert(selected.size() == 1);
      QModelIndex sel = selected.front();
      QFileSystemModel* model = static_cast<QFileSystemModel*>(mFileTree->model());
      if(model->isDir(sel))
      {
         return;
      }
      QString absPath = model->filePath(sel);
      QDir dir(mRootPath);
      mSelected = dir.relativeFilePath(absPath);
      
      QSettings settings;
      settings.setValue("AssetSelectorCurrentSelection", absPath);
      
      done(QDialog::Accepted);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::Cancel()
   {
      mSelected = "";
      done(QDialog::Rejected);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AssetSelector::OnDataPathChanged(const QString& path)
   {
      mRootPath = path;
      QModelIndex idx = mModel->setRootPath(mRootPath);
      mFileTree->setRootIndex(idx);
      QSettings settings;
      settings.setValue("AssetSelectorCurrentDataPath", path);
   }
}
