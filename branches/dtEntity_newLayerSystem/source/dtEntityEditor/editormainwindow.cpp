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

#include <dtEntityEditor/editormainwindow.h>

#include <dtEntity/core.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/log.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/systemmessages.h>
#include <dtEntityQtWidgets/datapatheditor.h>
#include <dtEntityEditor/editorapplication.h>
#include <dtEntityQtWidgets/entitytree.h>
#include <dtEntityQtWidgets/listdialog.h>
#include <dtEntityQtWidgets/assetcreationdialog.h>
#include <dtEntityQtWidgets/osgadapterwidget.h>
#include <dtEntityQtWidgets/osggraphicswindowqt.h>
#include <dtEntityQtWidgets/propertyeditor.h>
#include <dtEntityQtWidgets/qtguiwindowsystemwrapper.h>
#include <dtEntityQtWidgets/spawnerstore.h>

#include <cassert>
#include <iostream>
#include <QtGui/QtGui>
#include <QtOpenGL/QGLWidget>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

namespace dtEntityEditor
{

   ////////////////////////////////////////////////////////////////////////////////
   ResizerWidget::ResizerWidget(QWidget* parent, Qt::WindowFlags f)
         : QWidget(parent, f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResizerWidget::resizeEvent(QResizeEvent* evt)
   {
      QWidget::resizeEvent(evt);
      emit(Resized(evt->size()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResizerWidget::EmitCurrentSize()
   {
      emit(Resized(size()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResizerWidget::SetWidget(QWidget* widget)
   {
      QHBoxLayout* rswLayout = new QHBoxLayout(this);
      rswLayout->setContentsMargins(0, 0, 0, 0);
      rswLayout->addWidget(widget);
      //widget->adjustSize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorMainWindow::EditorMainWindow(EditorApplication* app, QWidget* parent)
      : QMainWindow(parent)
      , mApplication(app)
      , mEntityTreeDock(NULL)
      , mSpawnersDock(NULL)
      , mPropertyEditorDock(NULL)
   {

      mUpdateTimer = new QTimer(this);
      connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(EmitQueuedMessages()));
      mUpdateTimer->start(100);


      // register local message pump to receive messages from game message pump
      dtEntity::MessageFunctor functor(&mMessagePump, &dtEntity::MessagePump::EnqueueMessage);
      dtEntity::EntityManager& em = mApplication->GetEntityManager();
      em.RegisterForMessages(dtEntity::ToolsUpdatedMessage::TYPE, functor);
      em.RegisterForMessages(dtEntity::SceneLoadedMessage::TYPE, functor);
      em.RegisterForMessages(dtEntity::SceneUnloadedMessage::TYPE, functor);
      em.RegisterForMessages(dtEntity::MapLoadedMessage::TYPE, functor);
      em.RegisterForMessages(dtEntity::MapUnloadedMessage::TYPE, functor);

      mMessagePump.RegisterForMessages(dtEntity::ToolsUpdatedMessage::TYPE, dtEntity::MessageFunctor(this, &EditorMainWindow::OnToolsUpdated));
      mMessagePump.RegisterForMessages(dtEntity::SceneLoadedMessage::TYPE, dtEntity::MessageFunctor(this, &EditorMainWindow::OnSceneLoaded));
      mMessagePump.RegisterForMessages(dtEntity::SceneUnloadedMessage::TYPE, dtEntity::MessageFunctor(this, &EditorMainWindow::OnSceneUnloaded));
      mMessagePump.RegisterForMessages(dtEntity::MapLoadedMessage::TYPE, dtEntity::MessageFunctor(this, &EditorMainWindow::OnMapLoaded));
      mMessagePump.RegisterForMessages(dtEntity::MapUnloadedMessage::TYPE, dtEntity::MessageFunctor(this, &EditorMainWindow::OnMapUnloaded));


      setMinimumSize(800, 600);
      layout()->setContentsMargins(0, 0, 0, 0);

      setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks |
                     QMainWindow::VerticalTabs);


      setWindowTitle("dtEntity Editor");

      createActions();
      createMenus();
      createToolBars();
      CreateDockWidgets();

      connect(this, SIGNAL(LoadScene(const QString&)), app, SLOT(LoadScene(const QString&)));
      connect(this, SIGNAL(NewScene()), app, SLOT(NewScene()));
      connect(this, SIGNAL(SaveScene(QString)), app, SLOT(SaveScene(QString)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorMainWindow::~EditorMainWindow()
   {
      mUpdateTimer->stop();
      mMessagePump.EmitQueuedMessages(FLT_MAX);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::ShutDown()
   {
     // mCloseRequested = true;
      close();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::EmitQueuedMessages()
   {
      // emit ALL enqueued messages. No thread-safe way to get simulation time for now
      mMessagePump.EmitQueuedMessages(DBL_MAX);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::ReadSettings()
   {
      QSettings settings;
      settings.beginGroup( "MainWindow" );

      restoreGeometry(settings.value("geometry").toByteArray());
      restoreState(settings.value("windowState").toByteArray());

      resize(settings.value("size", QSize(1024, 800)).toSize());
      move(settings.value("pos").toPoint());


      settings.endGroup();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::WriteSettings()
   {
      QSettings settings;

      settings.beginGroup("MainWindow");

      settings.setValue("geometry", saveGeometry());
      settings.setValue("windowState", saveState());

      settings.setValue("size", size());
      settings.setValue("pos", pos());
      settings.endGroup();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::createActions()
   {

      mChangeDataPathsAct = new QAction(tr("Choose Data Paths..."), this);
      connect(mChangeDataPathsAct , SIGNAL(triggered()), this, SLOT(OnChooseDataPaths()));

      mChangeSceneAct = new QAction(tr("Open scene..."), this);
      connect(mChangeSceneAct , SIGNAL(triggered()), this, SLOT(OnChooseScene()));

      mNewSceneAct = new QAction(tr("New Scene"), this);
      connect(mNewSceneAct , SIGNAL(triggered()), this, SLOT(OnNewScene()));

      mSaveSceneAct = new QAction(tr("Save scene"), this);
      connect(mSaveSceneAct , SIGNAL(triggered()), this, SLOT(OnSaveScene()));
      mSaveSceneAct->setEnabled(false);

      mSaveSceneAsAct = new QAction(tr("Save scene as..."), this);
      connect(mSaveSceneAsAct , SIGNAL(triggered()), this, SLOT(OnSaveSceneAs()));
      mSaveSceneAsAct->setEnabled(true);

      mAddPluginAct = new QAction(tr("Add Plugin/Library..."), this);
      connect(mAddPluginAct, SIGNAL(triggered()), this, SLOT(OnAddPlugin()));

      mExitAct = new QAction(tr("E&xit"), this);

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
      mExitAct->setShortcuts(QKeySequence::Quit);
#endif
      mExitAct->setStatusTip(tr("Exit the application"));
      connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::createMenus()
   {
      mFileMenu = menuBar()->addMenu(tr("&File"));

      mFileMenu->addAction(mNewSceneAct);
      mFileMenu->addAction(mChangeSceneAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mSaveSceneAct);
      mFileMenu->addAction(mSaveSceneAsAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mChangeDataPathsAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mAddPluginAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mExitAct);

      //mEditMenu = menuBar()->addMenu(tr("&Edit"));

      mViewMenu = menuBar()->addMenu(tr("&View"));

   }


   ////////////////////////////////////////////////////////////////////////////////
   QStringList EditorMainWindow::GetLoadedMaps() const
   {
      QStringList ret;
      QString m;
      foreach(m, mCurrentMaps)
      {
         ret.push_back(m);
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::createToolBars()
   {
      {
         QToolBar* maintoolbar = new QToolBar("Commands", this);
         maintoolbar->setObjectName("MainToolBar");
         //tool->setGeometry(0,0,200,20);

         mJumpEdit = new QLineEdit();
         mJumpEdit->setMaximumWidth(100);
         maintoolbar->addWidget(mJumpEdit);
         QPushButton* jumpBut = new QPushButton("Jump");
         jumpBut->setMaximumWidth(40);
         maintoolbar->addWidget(jumpBut);
         connect(jumpBut, SIGNAL(clicked()), this, SLOT(DoJump()));
         connect(mJumpEdit, SIGNAL(returnPressed()), this, SLOT(DoJump()));
         maintoolbar->addSeparator();


         maintoolbar->setMovable(true);
         maintoolbar->setFloatable(true);
         maintoolbar->setAllowedAreas(Qt::AllToolBarAreas);
         addToolBar(Qt::TopToolBarArea, maintoolbar);
      }

      {
         mToolsToolbar = new QToolBar("Tools", this);
         mToolsToolbar->setObjectName("ToolsToolBar");

         mToolsToolbar->setMovable(true);
         mToolsToolbar->setFloatable(true);
         mToolsToolbar->setAllowedAreas(Qt::AllToolBarAreas);
         addToolBar(Qt::TopToolBarArea, mToolsToolbar);
         mToolsActionGroup = new QActionGroup(this);

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnToolsUpdated(const dtEntity::Message& m)
   {
      const dtEntity::ToolsUpdatedMessage& msg = static_cast<const dtEntity::ToolsUpdatedMessage&>(m);
      mToolsToolbar->clear();
      dtEntity::PropertyArray props = msg.GetTools();

      QAction* coordsysaction = mToolsToolbar->addAction(QIcon(":/icons/coordsystems.png"), "Use local coord system");
      coordsysaction->setCheckable(true);
      coordsysaction->setChecked(true);
      connect(coordsysaction, SIGNAL(triggered(bool)), this, SLOT(OnToggleCoordSystem(bool)));
      OnToggleCoordSystem(true);

      QAction* clampaction = mToolsToolbar->addAction(QIcon(":/icons/go-bottom.png"), "Clamp");
      clampaction->setCheckable(true);
      clampaction->setChecked(true);
      connect(clampaction, SIGNAL(triggered(bool)), this, SLOT(OnToggleGroundClamp(bool)));
      OnToggleGroundClamp(true);

      mToolsToolbar->addSeparator();

      QActionGroup* toolsActionGroup = new QActionGroup(mToolsToolbar);

      for(dtEntity::PropertyArray::iterator i = props.begin(); i != props.end(); ++i)
      {
         const dtEntity::GroupProperty* grp = dynamic_cast<const dtEntity::GroupProperty*>(*i);
         dtEntity::PropertyGroup grpprops = grp->Get();
         QString name = "";
         QString icon = "";
         QString shortcut = "";

         {
            dtEntity::PropertyGroup::iterator i = grpprops.find(dtEntity::ToolsUpdatedMessage::ToolNameId);
            if(i != grpprops.end())
            {
               name = i->second->StringValue().c_str();
            }
         }

         {
            dtEntity::PropertyGroup::iterator i = grpprops.find(dtEntity::ToolsUpdatedMessage::IconPathId);
            if(i != grpprops.end())
            {
               icon = i->second->StringValue().c_str();
            }
         }

         {
            dtEntity::PropertyGroup::iterator i = grpprops.find(dtEntity::ToolsUpdatedMessage::ShortcutId);
            if(i != grpprops.end())
            {
               shortcut = i->second->StringValue().c_str();
            }
         }

         //:dtentity/icons/edit-select.png
         QAction* a = mToolsToolbar->addAction(QIcon(icon), name);
         a->setCheckable(true);
         a->setChecked(false);
         a->setData(name);
         a->setShortcut(QKeySequence(shortcut));
         a->setToolTip(QString("%1 (%2)").arg(name).arg(shortcut));
         toolsActionGroup->addAction(a);
         connect(a, SIGNAL(triggered(bool)), this, SLOT(OnToolActionTriggered(bool)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnMapLoaded(const dtEntity::Message& m)
   {
      const dtEntity::MapLoadedMessage& msg =
            static_cast<const dtEntity::MapLoadedMessage&>(m);
      QString mapname = msg.GetMapPath().c_str();
      mCurrentMaps.insert(mapname);
      emit MapLoaded(mapname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnMapUnloaded(const dtEntity::Message& m)
   {
      const dtEntity::MapUnloadedMessage& msg =
            static_cast<const dtEntity::MapUnloadedMessage&>(m);
       QString mapname = msg.GetMapPath().c_str();
      mCurrentMaps.remove(mapname);
      emit MapUnloaded(mapname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnSceneLoaded(const dtEntity::Message& m)
   {
      const dtEntity::SceneLoadedMessage& msg =
            static_cast<const dtEntity::SceneLoadedMessage&>(m);

      std::string curr = msg.GetSceneName();
      mCurrentScene = dtEntity::GetSystemInterface()->FindDataFile(curr).c_str();

      mSaveSceneAct->setEnabled(true);

      setWindowTitle(QString("%1 - dtEntity Editor").arg(curr.c_str()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnSceneUnloaded(const dtEntity::Message& m)
   {
      mCurrentScene = "";
      mSaveSceneAct->setEnabled(false);
      setWindowTitle("dtEntity Editor");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnToggleCoordSystem(bool v)
   {
      dtEntity::BoolProperty b(v);
      dtEntity::SetSystemPropertiesMessage msg("Manipulator", "UseLocalCoords", b);
      mApplication->GetEntityManager().EnqueueMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnToggleGroundClamp(bool v)
   {
      dtEntity::BoolProperty b(v);
      dtEntity::SetSystemPropertiesMessage msg("Manipulator", "UseGroundClamping", b);
      mApplication->GetEntityManager().EnqueueMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnToolActionTriggered(bool)
   {
      QAction* action = dynamic_cast<QAction*>(sender());
      if(!action) return;

      dtEntity::ToolActivatedMessage msg;
      msg.SetToolName(action->text().toStdString());
      mApplication->GetEntityManager().EnqueueMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::CreateEntityTree()
   {

      mEntityTreeDock = new QDockWidget("Entity Tree");
      mEntityTreeDock->setObjectName("EntityTreeDock");
      using namespace dtEntityQtWidgets;

      EntityTreeModel* model = new EntityTreeModel(mApplication->GetEntityManager(), false);
      EntityTreeView* view = new EntityTreeView();
      view->SetModel(model);
      EntityTreeController* controller = new EntityTreeController(&mApplication->GetEntityManager());

      controller->moveToThread(mApplication->thread());
      controller->SetupSlots(model, view);

      QTimer::singleShot(0, controller, SLOT(Init()));

      mEntityTreeDock->setWidget(view);

      addDockWidget(Qt::LeftDockWidgetArea, mEntityTreeDock);
      mViewMenu->addAction(mEntityTreeDock->toggleViewAction());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::CreateSpawners()
   {

      mSpawnersDock = new QDockWidget("Spawners");
      mSpawnersDock->setObjectName("SpawnersDoc");

      using namespace dtEntityQtWidgets;


      SpawnerStoreView* view = new SpawnerStoreView(GetLoadedMaps());
      connect(this, SIGNAL(MapLoaded(QString)), view, SLOT(MapLoaded(QString)));
      connect(this, SIGNAL(MapUnloaded(QString)), view, SLOT(MapUnloaded(QString)));
      SpawnerStoreController* controller = new SpawnerStoreController(&mApplication->GetEntityManager());

      controller->moveToThread(mApplication->thread());
      controller->SetupSlots(view);
      connect(this, SIGNAL(TextDroppedOntoGLWidget(const QPointF&, const QString&)),
              controller, SLOT(OnTextDroppedOntoGLWidget(const QPointF&, const QString&)));
      QTimer::singleShot(0, controller, SLOT(Init()));

      mSpawnersDock->setWidget(view);
      addDockWidget(Qt::LeftDockWidgetArea, mSpawnersDock);

      mViewMenu->addAction(mSpawnersDock->toggleViewAction());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::CreatePropertyEditor()
   {

      mPropertyEditorDock = new QDockWidget("Property Editor");
      mPropertyEditorDock->setObjectName("PropertyEditorDock");

      using namespace dtEntityQtWidgets;


      PropertyEditorModel* model = new PropertyEditorModel();
      PropertyEditorView* view = new PropertyEditorView();
      view->SetModel(model);

      PropertyEditorController* controller = new PropertyEditorController(&mApplication->GetEntityManager());

      controller->moveToThread(mApplication->thread());
      controller->SetupSlots(model, view);

      QTimer::singleShot(0, controller, SLOT(Init()));

      mPropertyEditorDock->setWidget(view);

      addDockWidget(Qt::RightDockWidgetArea, mPropertyEditorDock);

      mViewMenu->addAction(mPropertyEditorDock->toggleViewAction());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::CreateDockWidgets()
   {
      CreateEntityTree();
      CreateSpawners();
      CreatePropertyEditor();
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::AddToKnownComponentList(std::set<dtEntity::ComponentType> newTypes)
   {
      using namespace dtEntityQtWidgets;

      // get the model first
      PropertyEditorView* view = dynamic_cast<PropertyEditorView*>(mPropertyEditorDock->widget());
      if (view)
      {
         PropertyEditorModel* model = dynamic_cast<PropertyEditorModel*>(view->GetModel());
         if (model)
         {
          //  model->EntitySystemAdded();

            // loop over input types and add them
            std::set<dtEntity::ComponentType>::const_iterator itr;
            for (itr = newTypes.begin(); itr != newTypes.end(); ++itr)
            {
               model->EntitySystemAdded(*itr);
            }
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::closeEvent(QCloseEvent* e)
   {
      WriteSettings();
      emit(Closed(false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnDisplayError(const QString& msg)
   {
      QMessageBox::warning(this, "RDEViewer", msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnChooseDataPaths()
   {
      dtEntityQtWidgets::DataPathEditor editor;
      int ret = editor.exec();
      if(ret == QDialog::Accepted)
      {
         QStringList paths = editor.GetSelectedDataPaths();
         QMetaObject::invokeMethod(mApplication, "SetDataPaths",  Qt::QueuedConnection,
            Q_ARG(QStringList, paths));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnSaveScene()
   {
      emit SaveScene(mCurrentScene);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnSaveSceneAs()
   {

      QSettings settings;
      QString currpath = settings.value("AssetSelectorCurrentDataPath", "").toString();

      QStringList paths = settings.value("DataPaths", "ProjectAssets").toStringList();

      QList<QUrl> urls;
      foreach(QString path, paths)
      {
         urls << QUrl::fromLocalFile(path);
      }

      QFileDialog dialog(this, tr("Save Scene"), currpath, tr("Scenes (*.dtescene *.bscene)"));
      dialog.setSidebarUrls(urls);
      dialog.setFileMode(QFileDialog::AnyFile);

      if(dialog.exec())
      {
         mCurrentScene = dialog.selectedFiles().first();
         setWindowTitle(QString("%1 - dtEntity Editor").arg(mCurrentScene));
         OnSaveScene();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RecursiveSearch(QDir dir, QSet<QString>& entries, const QString& extension, const QString& relDir = "")
   {
      dir.setSorting( QDir::Name );
      dir.setFilter( QDir::Files | QDir::Dirs );
      QFileInfoList  qsl = dir.entryInfoList();
      foreach (QFileInfo finfo, qsl)
      {
         if(finfo.isSymLink())
         {
            return;
         }
         if(finfo.isDir())
         {
            QString dirname = finfo.fileName();
            if(dirname == "." || dirname == "..")
            {
               continue;
            }
            QDir sd(finfo.filePath());
            QString rd = relDir.isEmpty() ? sd.dirName() : relDir + "/" + sd.dirName();
            RecursiveSearch(sd, entries, extension, rd);
          }
         else if(finfo.suffix() == extension)
         {
            entries.insert(relDir + "/" + finfo.fileName());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnChooseScene()
   {
      QSettings settings;
      QStringList paths = settings.value("DataPaths", "ProjectAssets").toStringList();

      QSet<QString> entries;
      for(QStringList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         QDir dir(*i);
         RecursiveSearch(dir, entries, "dtescene");
         RecursiveSearch(dir, entries, "bscene");
      }
      if(entries.empty())
      {
         QMessageBox::critical(this, "No scenes found", "No scenes found!"
                               " Please edit your data paths!");
         return;
      }

      QStringList l = entries.toList();
      l.sort();
      dtEntityQtWidgets::ListDialog* dialog = new dtEntityQtWidgets::ListDialog(l);

      if(dialog->exec() == QDialog::Accepted)
      {
         QStringList sel = dialog->GetSelectedItems();
         if(sel.size() != 0)
         {
            QString selected = sel.front();
            emit LoadScene(selected);
         }
      }

      dialog->close();
      dialog->deleteLater();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnNewScene()
   {
      mSaveSceneAct->setEnabled(false);
      emit NewScene();
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnAddPlugin()
   {
      std::string newLib;

      QString fileName = QFileDialog::getOpenFileName(this, tr("Load Library"),
         QString(),
         QString());

//      newLib.assign("D:\\dev\\gilgamesh\\VC_2008\\bin\\gmGamed.dll");
//      newLib.assign("D:\\dev\\dtentity\\VC_2008\\Debug\\plugins\\dtEntityRocketd.dll");
      newLib.assign(fileName.toStdString());

      // invoke method on application to actually load the plugin
      mApplication->AddPluginLibrary(newLib);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::DoJump()
   {
      QString text = mJumpEdit->text();
      QStringList l = text.split(" ");
      if(l.size() == 3)
      {
         float x = l[0].toFloat();
         float y = l[1].toFloat();
         float z = l[2].toFloat();
         dtEntity::MoveCameraToPositionMessage msg;
         msg.SetPosition(osg::Vec3(x, y, z));
         msg.SetLookAt(osg::Vec3(x, y, z));
         mApplication->GetEntityManager().EnqueueMessage(msg);

         return;
      }

      QString msg = "Please enter two or three coordinates or the name of a waypoint!";
      OnDisplayError(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::SetOSGWindow(dtEntityQtWidgets::OSGGraphicsWindowQt* osgGraphWindow)
   {
      dtEntityQtWidgets::OSGAdapterWidget* glwidget =
            dynamic_cast<dtEntityQtWidgets::OSGAdapterWidget*>(osgGraphWindow->GetQGLWidget());
      glwidget->setObjectName("glwidget_main");
      connect(glwidget, SIGNAL(TextDropped(const QPointF&, const QString&)),
              this, SLOT(OnTextDroppedOntoGLWidget(const QPointF&, const QString&)));

      glwidget->setMinimumSize(800,600);
      glwidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
      ResizerWidget* r = new ResizerWidget();
      r->SetWidget(glwidget);

      r->setObjectName("Resizer");
      QObject::connect(r, SIGNAL(Resized(const QSize&)), this, SLOT(OnViewResized(const QSize&)));
      QObject::connect(r, SIGNAL(Closing()), this, SLOT(OnViewClosing()));

      setCentralWidget(r);

      ReadSettings();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnViewResized(const QSize& size)
   {
      emit(ViewResized(size));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnViewClosing()
   {
      emit(ViewClosing());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnTextDroppedOntoGLWidget(const QPointF& pos, const QString& txt)
   {
      emit TextDroppedOntoGLWidget(pos, txt);
   }
}
