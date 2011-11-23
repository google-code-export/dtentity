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
   {
      setDragEnabled(true);
      setIconSize(QSize(80, 80));
      setMovement(QListView::Snap);
      setWrapping(true);
      setGridSize(QSize(80,100));
      setViewMode(QListView::IconMode);
      setWordWrap(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::mousePressEvent(QMouseEvent *event)
    {
        QListWidgetItem* item = itemAt(event->pos());
        if (!item)
            return;

        //QPoint hotSpot = event->pos() - item->pos();

        QByteArray itemData;
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-spawner", itemData);
        mimeData->setText(QString("SPAWNER|%1").arg(item->text()));
		
		//TODO memory leak?
        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(item->icon().pixmap(QSize(40, 40)));

        drag->exec(Qt::CopyAction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::SpawnerStoreView(QWidget* parent)
      : QWidget(parent)
   {
      Ui_SpawnerStore ui;
      ui.setupUi(this);
      mSpawnerList = new SpawnerList();
      layout()->addWidget(mSpawnerList);
   }

   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::~SpawnerStoreView()
   {
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

      mSpawnerList->addItem(item);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnRemoveSpawner(const QString& name)
   {
      QList<QListWidgetItem*> items = mSpawnerList->findItems(name, Qt::MatchExactly);
      QListWidgetItem* item;
      foreach(item, items)
      {
         mSpawnerList->removeItemWidget(item);
         delete item;
      }
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
      connect(this, SIGNAL(AddSpawner(const QString&, const QString&, const QString&)),
              view, SLOT(OnAddSpawner(const QString&, const QString&, const QString&)));

      connect(this, SIGNAL(RemoveSpawner(const QString&)),
              view, SLOT(OnRemoveSpawner(const QString&)));
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
      mEntityManager->GetComponent(mtsystem->GetEntityIdByUniqueId("defaultCam"), cam);
      osg::Vec3 start = cam->GetPosition();
     
      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start, start + pickray * 100000);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
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

      dtEntity::SpawnerRemovedMessage m1;
      m1.SetName(msg.GetName());
      m1.SetMapName(msg.GetMapName());
      OnSpawnerRemoved(m1);

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

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(msg.GetName(), spawner))
      {
         return;
      }

      emit RemoveSpawner(spawner->GetName().c_str());
   }
} 

