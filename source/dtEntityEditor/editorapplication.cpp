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

#include <dtEntityEditor/editorapplication.h>

#include <assert.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntityEditor/editormainwindow.h>
#include <dtEntityQtWidgets/messages.h>
#include <dtEntityQtWidgets/osggraphicswindowqt.h>
#include <dtEntityQtWidgets/osgadapterwidget.h>
#include <osgViewer/GraphicsWindow>
#include <osg/MatrixTransform>
#include <osgViewer/View>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <GL/gl.h>
#include <iostream>
#include <QtCore/QDir>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

namespace dtEntityEditor
{

   ////////////////////////////////////////////////////////////////////////////////
   EditorApplication::EditorApplication(int argc, char *argv[])
      : mMainWindow(NULL)
      , mTimer(NULL)
      , mEntityManager(new dtEntity::EntityManager())
      , mStartOfFrameTick(osg::Timer::instance()->tick())
      , mTimeScale(1)
   {

      // default plugin dir
      mPluginPaths.push_back("dteplugins");

      osg::ArgumentParser arguments(&argc,argv);
      mViewer = new osgViewer::Viewer(arguments);
      mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
      // don't exit on escape
      mViewer->setKeyEventSetsDone(0);

      static const char* winvar = "OSG_WINDOW=0 0 800 600";
      putenv(const_cast<char*>(winvar));

      dtEntity::InitOSGViewer(argc, argv, mViewer, mEntityManager, false, false);

      dtEntity::MapSystem* ms;
      mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, ms);
      dtEntityQtWidgets::RegisterMessageTypes(ms->GetMessageFactory());
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorApplication::~EditorApplication()
   {
      mEntityManager = NULL;
      if(mTimer)
      {
         mTimer->stop();
         mTimer->deleteLater();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityManager& EditorApplication::GetEntityManager() const
   {
      return *mEntityManager;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::SetMainWindow(EditorMainWindow* mw)
   {

      assert(mMainWindow == NULL);
      mMainWindow = mw;
      
      connect(mMainWindow, SIGNAL(Closed(bool)), this, SLOT(ShutDownGame(bool)));
      connect(mMainWindow, SIGNAL(ViewResized(const QSize&)), this, SLOT(ViewResized(const QSize&)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::SetAdditionalPluginPath(const QString &path, bool bBeforeDefaultPath)
   {
      if(bBeforeDefaultPath)
         mPluginPaths.insert(mPluginPaths.begin(), path.toStdString());
      else
         mPluginPaths.push_back(path.toStdString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::StartGame(const QString& sceneToLoad)
   {
      assert(mMainWindow != NULL);
      
      osgViewer::ViewerBase::Windows wins;
      mViewer->getWindows(wins);


      dtEntityQtWidgets::OSGGraphicsWindowQt* osgGraphWindow =
            dynamic_cast<dtEntityQtWidgets::OSGGraphicsWindowQt*>(wins.front());


      if(osgGraphWindow->thread() == thread())
      {
         mMainWindow->SetOSGWindow(osgGraphWindow);
      }
      else
      {
         QMetaObject::invokeMethod(mMainWindow, "SetOSGWindow",  Qt::BlockingQueuedConnection,
            Q_ARG(dtEntityQtWidgets::OSGGraphicsWindowQt*, osgGraphWindow));
      }

      try
      { 

         mTimer = new QTimer(this);
         mTimer->setInterval(10);
         connect(mTimer, SIGNAL(timeout()), this, SLOT(StepGame()), Qt::QueuedConnection);

         mTimer->start();

         connect(this, SIGNAL(ErrorOccurred(const QString&)),
                 mMainWindow, SLOT(OnDisplayError(const QString&)));

         dtEntity::MapSystem* mapSystem;
         mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);
         for(unsigned int i = 0; i < mPluginPaths.size(); ++i) 
         {
            LOG_DEBUG("Looking for plugins in directory " + mPluginPaths[i]);
            // load and start all entity systems in plugins
            mapSystem->GetPluginManager().LoadPluginsInDir(mPluginPaths[i]);
         }
         // add new factories to list of known ones
         std::set<dtEntity::ComponentType> newTypes;
         dtEntity::ComponentPluginManager::PluginFactoryMap& factories = mapSystem->GetPluginManager().GetFactories();
         dtEntity::ComponentPluginManager::PluginFactoryMap::const_iterator j;
         for(j = factories.begin(); j != factories.end(); ++j)
            newTypes.insert(j->first);
         mMainWindow->AddToKnownComponentList(newTypes);


      }
      catch(const std::exception& e)
      {
         emit(ErrorOccurred(QString("Error starting application: %1").arg(e.what())));
         LOG_ERROR("Error starting application:" + std::string(e.what()));
      }
      catch(...)
      {
         emit(ErrorOccurred("Unknown error starting application"));
         LOG_ERROR("Unknown error starting application");
      }

      InitializeScripting();

      if(sceneToLoad != "")
      {
         LoadScene(sceneToLoad);
      }

      dtEntity::StartSystemMessage msg;
      mEntityManager->EmitMessage(msg);

   }

   //////////////////////////////////////////////////////////////////////////

   void EditorApplication::AddPluginLibrary(std::string fileName)
   {
      // get Map system
      dtEntity::MapSystem* mapSystem;
      if (mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem))
      {
         // load plugin, set it to be saved to scene file. Also start all entity systems in it
         std::set<dtEntity::ComponentType> newTypes = 
            mapSystem->GetPluginManager().AddPlugin(fileName, true);
         // notify GUI that new types are now available
         mMainWindow->AddToKnownComponentList(newTypes);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::StepGame()
   {
      if(!mViewer->done())
      {
         mViewer->frame();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::ShutDownGame(bool)
   {
      mViewer->setDone(true);
      
      if(mTimer)
      {
         mTimer->stop();
         mTimer->deleteLater();
         mTimer = NULL;
      }

      // delete entity manager now before EditorApplication object is moved to main thread.
      mEntityManager = NULL;
      QMetaObject::invokeMethod(mMainWindow, "ShutDown", Qt::QueuedConnection);
      QThread::currentThread()->quit();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::ViewResized(const QSize& size)
   {
      osgViewer::ViewerBase::Windows wins;
      mViewer->getWindows(wins);
      wins.front()->resized(0, 0, size.width(), size.height());
      wins.front()->getEventQueue()->windowResize(0, 0, size.width(), size.height());

      osgViewer::ViewerBase::Cameras cams;
      mViewer->getCameras(cams);
      cams.front()->setViewport(new osg::Viewport(0, 0, size.width(), size.height()));

      double vfov, aspectRatio, nearClip, farClip;
      cams.front()->getProjectionMatrixAsPerspective(vfov, aspectRatio, nearClip, farClip);
      cams.front()->setProjectionMatrixAsPerspective(vfov, (double)size.width() / (double) size.height(), nearClip, farClip);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QStringList EditorApplication::GetDataPaths() const
   {
      QStringList out;
      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      for(osgDB::FilePathList::iterator i = paths.begin(); i != paths.end(); ++i)
      {
         out.push_back(i->c_str());
      }
      return out;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::SetDataPaths(const QStringList& paths)
   {
      // make sure we don't loose the paths used by OSG...
      osgDB::FilePathList in = osgDB::getDataFilePathList();

      for(QStringList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         QString path = *i;
         if(!QFile::exists(path))
         {
            LOG_ERROR("Project assets folder does not exist: " + path.toStdString());
         }
         else
         {
            in.push_back(path.toStdString());
         }
      }

      osgDB::setDataFilePathList(in);

      QSettings settings;
      settings.setValue("DataPaths", paths);
      emit DataPathsChanged(paths);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::CreateCameraEntityIfNotExists()
   {
      dtEntity::CameraSystem* camsys;
      GetEntityManager().GetEntitySystem(dtEntity::CameraComponent::TYPE, camsys);
      if(camsys->GetNumComponents() == 0)
      {
         std::string cameramapname = "maps/default.dtemap";

         dtEntity::MapSystem* mapSystem;
         GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);

         if(!mapSystem->GetLoadedMaps().empty())
         {
            cameramapname = mapSystem->GetLoadedMaps().front();
         }

         // create a main camera entity if it was not loaded from map
         dtEntity::ApplicationSystem* appsys;
         GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);

         dtEntity::Entity* entity;
         mEntityManager->CreateEntity(entity);

         unsigned int contextId = appsys->GetPrimaryWindow()->getState()->getContextID();
         dtEntity::CameraComponent* camcomp;
         entity->CreateComponent(camcomp);
         camcomp->SetContextId(contextId);
         camcomp->SetClearColor(osg::Vec4(0,0,0,1));
         camcomp->Finished();

         dtEntity::MapComponent* mapcomp;
         entity->CreateComponent(mapcomp);
         std::ostringstream os;
         os << "cam_" << contextId;
         mapcomp->SetEntityName(os.str());
         mapcomp->SetUniqueId(os.str());
         mapcomp->SetMapName(cameramapname);
         mapcomp->Finished();
         GetEntityManager().AddToScene(entity->GetId());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::LoadScene(const QString& path)
   {  
      
      dtEntity::MapSystem* mapSystem;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);

      if(mapSystem->GetCurrentScene().size() != 0)
      {
         mapSystem->UnloadScene();
      }

      mapSystem->LoadScene(path.toStdString());

      //CreateCameraEntityIfNotExists();
      
      emit SceneLoaded(path);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::AddScene(const QString& datapath, const QString& mappath)
   {
      dtEntity::MapSystem* mapSystem;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);

      mapSystem->CreateScene(datapath.toStdString(), mappath.toStdString());
      //CreateCameraEntityIfNotExists();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::SaveScene()
   {
      dtEntity::MapSystem* mapSystem;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);
      std::ostringstream os;
      bool success = mapSystem->SaveCurrentScene(false);
      if(!success)
      {
         ErrorOccurred(tr("Cannot save scene, please check file permissions!"));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::SaveAll()
   {
      dtEntity::MapSystem* mapSystem;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);
      bool success = mapSystem->SaveCurrentScene(true);
      if(!success)
      {
         ErrorOccurred(tr("Cannot save scene or one of the maps, please check file permissions!"));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorApplication::InitializeScripting()
   {

      dtEntity::StringId scriptId = dtEntity::SID("Script");

      dtEntity::MapSystem* mapSystem;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);
      
      
      if(!GetEntityManager().HasEntitySystem(scriptId))
      {
         if(!mapSystem->GetPluginManager().FactoryExists(scriptId))
         {
            LOG_ERROR("Cannot start scripting, script plugin not loaded!");
            return;
         }         
            
         bool success = mapSystem->GetPluginManager().StartEntitySystem(scriptId);
         if(!success)
         {
            LOG_ERROR("Cannot start scripting, script plugin not loaded!");
            return;
         }
      }

      dtEntity::Message* msg;
      bool success = mapSystem->GetMessageFactory().CreateMessage(dtEntity::SID("ExecuteScriptMessage"), msg);
      assert(success);
      msg->Get(dtEntity::SID("IncludeOnce"))->SetBool(true);
      dtEntity::Property* pathprop = msg->Get(dtEntity::SID("Path"));

      pathprop->SetString("Scripts/editorautostart.js");
      GetEntityManager().EmitMessage(*msg);

      delete msg;
   }
}
