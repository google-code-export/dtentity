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
   class SpawnerList : public QListWidget
   {
      Q_OBJECT

   public:

      SpawnerList();
      void mousePressEvent(QMouseEvent *event);

      void AddCategory(const QString&);

      QString GetTargetMap() const { return mTargetMap; }
   public slots:

      void SetTargetMap(const QString& v) { mTargetMap = v; }

   protected slots:

      void DeleteSelectedSpawners();
      void SelectBySelectedSpawner();
      void OnReclassify(QAction*);

   signals:

      void spawnerClicked(QListWidgetItem*);
      void DeleteSpawner(const QString& name);
      void SelectBySpawner(const QString& name);
      void ChangeCategory(const QString& spawnername, const QString& oldcat, const QString& newcat);

   private:
       QString mTargetMap;
       QAction* mDeleteSpawnerAction;
       QAction* mSelectBySpawnerAction;
       QListWidgetItem* mSelected;
       QMenu* mReclassifyMeny;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT SpawnerStoreView
      : public QWidget
   {
      Q_OBJECT

   public:

      SpawnerStoreView(const QStringList& loadedMaps, QWidget* parent = NULL);
      virtual ~SpawnerStoreView();

      QWidget* GetButtons() const { return mButtons; }

      QStringList GetCategories();

   public slots:

      void OnAddSpawner(const QString& name, const QString& category, const QString& iconpath);
      void OnRemoveSpawner(const QString& name, const QString& category);
      void CategoryChanged(const QString&);
      void OnItemClicked(QListWidgetItem*);
      void OnAddCategoryButtonClicked();

      void MapLoaded(const QString& map);
      void MapUnloaded(const QString& map);

   signals:
      void SpawnerClicked(const QString& name, const QString& category);
      void DeleteSpawner(const QString&);
      void SelectBySpawner(const QString& name);
      void ChangeSpawnerCategory(const QString& spawnername, const QString& oldcat, const QString& newcat);
   private:

      void ShowHideByCategory();

      SpawnerList* mSpawnerList;
      QComboBox* mCategories;
      QWidget* mButtons;
      QComboBox* mTargetMap;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class ENTITYQTWIDGETS_EXPORT SpawnerStoreController
      : public QObject
   {
      Q_OBJECT

   public:

      SpawnerStoreController(dtEntity::EntityManager*);
      virtual ~SpawnerStoreController();

      void SetupSlots(SpawnerStoreView* view);

      void OnSpawnerAdded(const dtEntity::Message& m);
      void OnSpawnerModified(const dtEntity::Message& m);
      void OnSpawnerRemoved(const dtEntity::Message& m);

   signals:
      void AddSpawner(const QString& name, const QString& category, const QString& iconpath);
      void RemoveSpawner(const QString& name, const QString& category);

   public slots:

      void Init();
      void OnTextDroppedOntoGLWidget(const QPointF& pos, const QString&);
      void OnSpawnerClicked(const QString& name, const QString& category);
      void SpawnerDeleted(const QString&);
      void DoSelectBySpawner(const QString& name);
      void OnChangeSpawnerCategory(const QString& spawnername, const QString& oldcat, const QString& newcat);

   private:
      dtEntity::EntityManager* mEntityManager;
      dtEntity::MessageFunctor mSpawnerAddedFunctor;
      dtEntity::MessageFunctor mSpawnerModifiedFunctor;
      dtEntity::MessageFunctor mSpawnerRemovedFunctor;
   };
}
