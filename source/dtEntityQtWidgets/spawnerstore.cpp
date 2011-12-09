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

#include <dtEntityQtWidgets/spawnerstore.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/spawner.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityQtWidgets/messages.h>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/View>
#include <osgDB/FileUtils>
#include "ui_spawnerstore.h"

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerList::SpawnerList()
      : mDeleteSpawnerAction(new QAction(tr("Delete Spawner"), this))
      , mSelected(NULL)
      , mReclassifyMeny(new QMenu(tr("Reclassify"), this))
   {
      setDragEnabled(true);
      setIconSize(QSize(80, 80));
      setMovement(QListView::Snap);
      setWrapping(true);
      setGridSize(QSize(120,120));
      setViewMode(QListView::IconMode);
      setResizeMode(QListView::Adjust);
      setWordWrap(true);

      QFont sansFont("Helvetica [Cronyx]", 8);
      setFont(sansFont);

      connect(mDeleteSpawnerAction, SIGNAL(triggered()), this, SLOT(DeleteSelectedSpawners()));
      connect(mReclassifyMeny, SIGNAL(triggered(QAction*)), this, SLOT(OnReclassify(QAction*)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::AddCategory(const QString& cat)
   {
      mReclassifyMeny->addAction(cat);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::DeleteSelectedSpawners()
   {
      if(mSelected != NULL)
      {
         emit DeleteSpawner(mSelected->text());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::mousePressEvent(QMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton)
        {
           QListWidgetItem* item = itemAt(event->pos());
           if (!item)
           {
              return;
           }
           emit spawnerClicked(item);

           //QPoint hotSpot = event->pos() - item->pos();

           QByteArray itemData;
           QMimeData *mimeData = new QMimeData;
           mimeData->setData("application/x-spawner", itemData);
           mimeData->setText(QString("SPAWNER|%1").arg(item->text()));

           QDrag* drag = new QDrag(this);
           drag->setMimeData(mimeData);
           drag->setPixmap(item->icon().pixmap(QSize(40, 40)));

           drag->exec(Qt::CopyAction);
        }
        else
        {
           QListWidgetItem* item = itemAt(event->pos());
           if (!item)
           {
               return;
           }
           mSelected = item;
           QMenu menu(this);
           menu.addAction(mDeleteSpawnerAction);
           menu.addMenu(mReclassifyMeny);
           menu.exec(mapToGlobal(event->pos()));
        }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::OnReclassify(QAction* a)
   {
      if(mSelected != NULL)
      {
         emit ChangeCategory(mSelected->text(), mSelected->data(Qt::UserRole).toString(), a->text());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::SpawnerStoreView(QWidget* parent)
      : QWidget(parent)
   {
      Ui_SpawnerStore ui;
      ui.setupUi(this);
      mCategories = ui.mCategories;
      mSpawnerList = new SpawnerList();
      mButtons = ui.mButtons;
      ui.mListPlaceholder->setLayout(new QVBoxLayout());
      ui.mListPlaceholder->layout()->addWidget(mSpawnerList);
      connect(mSpawnerList, SIGNAL(spawnerClicked( QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
      connect(mSpawnerList, SIGNAL(DeleteSpawner(QString)), this, SIGNAL(DeleteSpawner(QString)));
      connect(mSpawnerList, SIGNAL(ChangeCategory(QString, QString, QString)), this, SIGNAL(ChangeSpawnerCategory(QString, QString, QString)));
      connect(mCategories, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(CategoryChanged(const QString&)));
      connect(ui.mAddCategoryButton, SIGNAL(clicked()), this, SLOT(OnAddCategoryButtonClicked()));

      mCategories->setInsertPolicy(QComboBox::InsertAlphabetically);
   }

   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::~SpawnerStoreView()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::CategoryChanged(const QString& category)
   {
      ShowHideByCategory();
      QSettings settings;
      settings.setValue("spawnerstore_category", category);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QStringList SpawnerStoreView::GetCategories()
   {
      QStringList ret;
      for(unsigned int i = 0; i < mCategories->count(); ++i)
      {
         QString txt = mCategories->itemText(i);
         if(txt != tr("All"))
         {
            ret.push_back(txt);
         }
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnAddCategoryButtonClicked()
   {
      bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new category"),
                                          tr("Enter name for new category:"), QLineEdit::Normal,
                                          "NewCategory", &ok);
      if (ok && !text.isEmpty())
      {
         if(mCategories->findText(text, Qt::MatchFixedString) != -1)
         {
            QMessageBox::warning(this, "Category already exists!", "Category already exists!");
            return;
         }
         mCategories->addItem(text);
         mSpawnerList->AddCategory(text);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnItemClicked(QListWidgetItem* item)
   {
      emit SpawnerClicked(item->text(), item->data(Qt::UserRole).toString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::ShowHideByCategory()
   {
      QString category = mCategories->currentText();
      for(int i = 0; i < mSpawnerList->count(); ++i)
      {
         QListWidgetItem* item = mSpawnerList->item(i);
         if(category == "All" || item->data(Qt::UserRole).toString() == category)
         {
            item->setHidden(false);
         }
         else
         {
            item->setHidden(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnAddSpawner(const QString& name, const QString& category, const QString& iconpath)
   {
      QListWidgetItem* item;
      if(iconpath != "")
      {
         QString iconrealpath = osgDB::findDataFile(iconpath.toStdString()).c_str();
         QIcon icon(iconrealpath);
         item = new QListWidgetItem(icon, name, mSpawnerList);
      }
      else
      {
         item = new QListWidgetItem(name, mSpawnerList);
      }
      item->setData(Qt::UserRole, category);

      mSpawnerList->addItem(item);

      if(mCategories->findText(category) == -1)
      {
         mCategories->addItem(category);
         mSpawnerList->AddCategory(category);
         QSettings settings;
         QString last = settings.value("spawnerstore_category").toString();
         if(last == category)
         {
            mCategories->setCurrentIndex(mCategories->findText(category));
         }
      }
      ShowHideByCategory();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnRemoveSpawner(const QString& name, const QString& category)
   {
      QList<QListWidgetItem*> items = mSpawnerList->findItems(name, Qt::MatchExactly);
      QListWidgetItem* item;
      foreach(item, items)
      {
         QString txt = item->text();
         QString cat = item->data(Qt::UserRole).toString();
         if(cat == category || (category == "" && cat == "default"))
         {
            mSpawnerList->removeItemWidget(item);
            delete item;
            return;
         }
      }
      ShowHideByCategory();
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreController::SpawnerStoreController(dtEntity::EntityManager* entityManager)
   : mEntityManager(entityManager)
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreController::~SpawnerStoreController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerModifiedMessage::TYPE, mSpawnerModifiedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::SetupSlots(SpawnerStoreView* view)
   {
      connect(this, SIGNAL(AddSpawner(QString, QString, QString)),
              view, SLOT(OnAddSpawner(QString, QString, QString)));

      connect(this, SIGNAL(RemoveSpawner(QString, QString)),
              view, SLOT(OnRemoveSpawner(QString, QString)));

      connect(view, SIGNAL(SpawnerClicked(QString, QString)),
              this, SLOT(OnSpawnerClicked(QString, QString)));

      connect(view, SIGNAL(DeleteSpawner(QString)), this, SLOT(SpawnerDeleted(QString)));

      connect(view, SIGNAL(ChangeSpawnerCategory(QString, QString, QString)), this, SLOT(OnChangeSpawnerCategory(QString, QString, QString)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::Init()
   {

      mSpawnerAddedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerAdded);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);

      mSpawnerRemovedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerRemoved);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor);

      mSpawnerModifiedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerModified);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerModifiedMessage::TYPE, mSpawnerModifiedFunctor);

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      mtsystem->GetAllSpawners(spawners);
      std::map<std::string, dtEntity::Spawner*>::const_iterator i;
      for(i = spawners.begin(); i != spawners.end(); ++i)
      {
         if(i->second->GetAddToSpawnerStore())
         {
            emit AddSpawner(i->second->GetName().c_str(), i->second->GetGUICategory().c_str(),
                            i->second->GetIconPath().c_str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnTextDroppedOntoGLWidget(const QPointF& pos, const QString& text)
   {
      QStringList l = text.split("|");
      if(l.size() != 2 || l.front() != "SPAWNER")
      {
         return;
      }
      QString spawnerName = l.back();

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);


      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(spawnerName.toStdString(), spawner))
      {
         LOG_ERROR("Could not spawn by drag and drop: Spawner not found!");
         return;
      }

      dtEntity::ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      
      osg::Vec3 pickray = appsys->GetWindowManager()->GetPickRay("defaultView", pos.x(), pos.y());
      
      dtEntity::CameraComponent* cam;
      mEntityManager->GetComponent(mtsystem->GetEntityIdByUniqueId("cam_0"), cam);
      osg::Vec3 start = cam->GetPosition();
     
      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start, start + pickray * 100000);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setTraversalMask(dtEntity::NodeMasks::TERRAIN);
      iv.setUseKdTreeWhenAvailable(true);

      dtEntity::LayerAttachPointSystem* layersys;
      mEntityManager->GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      dtEntity::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();
      osg::Node* sceneNode = sceneLayer->GetGroup();
      sceneNode->accept(iv);

      bool found = lsi->containsIntersections();

      osg::Vec3 spawnPosition;
      if(found)
      {
         spawnPosition = lsi->getIntersections().begin()->getWorldIntersectPoint();
      }
      else
      {
         spawnPosition = start + pickray * 10;
      }

      dtEntity::Entity* entity;
      mEntityManager->CreateEntity(entity);
      spawner->Spawn(*entity);

      dtEntity::MapComponent* mc;
      if(entity->GetComponent(mc))
      {
         mc->SetEntityName(spawner->GetName());
      }
      mEntityManager->AddToScene(entity->GetId());

      dtEntity::TransformComponent* tcomp;      
      if(mEntityManager->GetComponent(entity->GetId(), tcomp, true))
      {
         tcomp->SetTranslation(spawnPosition);
      }
      
      dtEntity::RequestEntitySelectMessage msg;
      msg.SetAboutEntityId(entity->GetId());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerClicked(const QString& name, const QString& category)
   {
      SpawnerSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::SpawnerDeleted(const QString& name)
   {
      dtEntity::MapSystem* mtsystem;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      bool success = mtsystem->DeleteSpawner(name.toStdString());
      if(!success)
      {
         LOG_WARNING("Could not delete spawner!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnChangeSpawnerCategory(const QString& spawnername, const QString& oldcat, const QString& newcat)
   {
      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(spawnername.toStdString(), spawner))
      {
         LOG_ERROR("Spawner not found!");
         return;
      }

      spawner->SetGUICategory(newcat.toStdString());

      dtEntity::SpawnerModifiedMessage msg;
      msg.SetName(spawner->GetName());
      msg.SetMapName(spawner->GetMapName());
      msg.SetOldCategory(oldcat.toStdString());
      msg.SetNewCategory(spawner->GetGUICategory());
      mEntityManager->EmitMessage(msg);
      if(!mtsystem->SaveMap(spawner->GetMapName()))
      {
         LOG_ERROR("Could not save map!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerAdded(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerAddedMessage& msg =
         static_cast<const dtEntity::SpawnerAddedMessage&>(m);

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(msg.GetName(), spawner))
      {
         return;
      }


      if(spawner->GetAddToSpawnerStore())
      {
         emit AddSpawner(spawner->GetName().c_str(), spawner->GetGUICategory().c_str(),
                         spawner->GetIconPath().c_str());
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerModified(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerModifiedMessage& msg =
         static_cast<const dtEntity::SpawnerModifiedMessage&>(m);

      emit RemoveSpawner(msg.GetName().c_str(), msg.GetOldCategory().c_str());

      dtEntity::SpawnerAddedMessage m2;
      m2.SetName(msg.GetName());
      m2.SetMapName(msg.GetMapName());
      OnSpawnerAdded(m2);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerRemoved(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerRemovedMessage& msg =
         static_cast<const dtEntity::SpawnerRemovedMessage&>(m);

      emit RemoveSpawner(msg.GetName().c_str(), msg.GetCategory().c_str());
   }
} 

