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

#include <dtEntity/entitymanager.h>

#include <dtEntityQtWidgets/datapatheditor.h>
#include <dtEntityEditor/editorapplication.h>
#include <dtEntityQtWidgets/entitylist.h>
#include <dtEntityQtWidgets/entitytree.h>
#include <dtEntityQtWidgets/listdialog.h>
#include <dtEntity/basemessages.h>
#include <dtEntityQtWidgets/osgadapterwidget.h>
#include <dtEntityQtWidgets/osggraphicswindowqt.h>
#include <dtEntityQtWidgets/propertyeditor.h>
#include <dtEntityQtWidgets/messagestore.h>
#include <dtEntityQtWidgets/qtguiwindowsystemwrapper.h>
#include <dtEntityQtWidgets/spawnerstore.h>
#include <dtEntity/log.h>
#include <cassert>
#include <iostream>
#include <QtGui/QtGui>
#include <QtOpenGL/QGLWidget>

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
   // TODO What is this? Memory leak?
      QHBoxLayout* rswLayout = new QHBoxLayout(this);
      rswLayout->setContentsMargins(0, 0, 0, 0);
      rswLayout->addWidget(widget);
      widget->adjustSize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorMainWindow::EditorMainWindow(EditorApplication* app, QWidget* parent)
      : QMainWindow(parent) 
      , mApplication(app)
   {

      {
         dtEntity::MessageFunctor functor(this, &EditorMainWindow::ToolsUpdated);
         mMessagePump.RegisterForMessages(dtEntity::ToolsUpdatedMessage::TYPE, functor);
      }

      mUpdateTimer = new QTimer(this);
      connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(EmitQueuedMessages()));
      mUpdateTimer->start(100);

      {
         // register local message pump to receive messages from game message pump
         dtEntity::MessageFunctor functor(&mMessagePump, &dtEntity::MessagePump::EnqueueMessage);
         mApplication->GetEntityManager().RegisterForMessages(dtEntity::ToolsUpdatedMessage::TYPE, functor);
      }

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
      connect(this, SIGNAL(AddScene(const QString&)), app, SLOT(AddScene(const QString&)));
      connect(this, SIGNAL(SaveScene(const QString&)), app, SLOT(SaveScene(const QString&)));
      connect(this, SIGNAL(SaveAll(const QString&)), app, SLOT(SaveAll(const QString&)));
      ReadSettings();
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

      mNewSceneAct = new QAction(tr("New Scene..."), this);
      connect(mNewSceneAct , SIGNAL(triggered()), this, SLOT(OnNewScene()));

      mSaveSceneAct = new QAction(tr("Save scene file only"), this);
      connect(mSaveSceneAct , SIGNAL(triggered()), this, SLOT(OnSaveScene()));
      mSaveSceneAct->setEnabled(false);

      mSaveAllAct = new QAction(tr("Save scene and maps"), this);
      connect(mSaveAllAct , SIGNAL(triggered()), this, SLOT(OnSaveAll()));
      mSaveAllAct->setEnabled(false);

      mResetSystemAct = new QAction(tr("Reset System"), this);
      connect(mResetSystemAct , SIGNAL(triggered()), this, SLOT(OnResetSystem()));

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
      mFileMenu->addAction(mSaveAllAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mChangeDataPathsAct);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mResetSystemAct);
      mFileMenu->addAction(mExitAct);

      //mEditMenu = menuBar()->addMenu(tr("&Edit"));
      //mViewMenu = menuBar()->addMenu(tr("&View"));
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::createToolBars()
   {
      {
         QToolBar* maintoolbar = new QToolBar(this);
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
         mToolsToolbar = new QToolBar(this);
         mToolsToolbar->setObjectName("ToolsToolBar");

         mToolsToolbar->setMovable(true);
         mToolsToolbar->setFloatable(true);
         mToolsToolbar->setAllowedAreas(Qt::AllToolBarAreas);
         addToolBar(Qt::TopToolBarArea, mToolsToolbar);
         mToolsActionGroup = new QActionGroup(this);

         connect(mToolsToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(OnToolActionTriggered(QAction*)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::ToolsUpdated(const dtEntity::Message& m)
   {
      const dtEntity::ToolsUpdatedMessage& msg = static_cast<const dtEntity::ToolsUpdatedMessage&>(m);
      mToolsToolbar->clear();
      dtEntity::PropertyArray props = msg.GetTools();

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

         //:icons/edit-select.png
         QAction* a = mToolsToolbar->addAction(QIcon(icon), name);
         a->setCheckable(true);
         a->setChecked(false);
         a->setData(name);
         a->setShortcut(QKeySequence(shortcut));
         a->setToolTip(QString("%1 (%2)").arg(name).arg(shortcut));


      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnToolActionTriggered(QAction* action)
   {
      QList<QAction*> actions = mToolsToolbar->actions();
      QAction* a;
      foreach(a, actions)
      {
         if(a == action)
         {
            a->setChecked(true);
         }
         else
         {
            a->setChecked(false);
         }
      }

      dtEntity::ToolActivatedMessage msg;
      msg.SetToolName(action->text().toStdString());
      mApplication->GetEntityManager().EnqueueMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::CreateDockWidgets()
   {
      using namespace dtEntityQtWidgets;

      {
         QDockWidget* entityTreeDock = new QDockWidget("Entity Tree");
         entityTreeDock->setObjectName("EntityTreeDock");
         EntityTreeModel* model = new EntityTreeModel();
         EntityTreeView* view = new EntityTreeView();
         view->SetModel(model);
         EntityTreeController* controller = new EntityTreeController(&mApplication->GetEntityManager());

         controller->moveToThread(mApplication->thread());
         controller->SetupSlots(model, view);

         QTimer::singleShot(0, controller, SLOT(Init()));
         
         entityTreeDock->setWidget(view);
         
         addDockWidget(Qt::LeftDockWidgetArea, entityTreeDock);
      }

      {
         QDockWidget* messageStoreDock = new QDockWidget("Message Store");
         messageStoreDock->setObjectName("MessageStoreDock");

         MessageStoreView* view = new MessageStoreView();
         MessageStoreController* controller = new MessageStoreController(&mApplication->GetEntityManager());

         controller->moveToThread(mApplication->thread());
         controller->SetupSlots(view);
         QTimer::singleShot(0, controller, SLOT(Init()));

         messageStoreDock->setWidget(view);
         addDockWidget(Qt::LeftDockWidgetArea, messageStoreDock);
      }

      {
         QDockWidget* spawnerStoreDock = new QDockWidget("Spawners");
         spawnerStoreDock->setObjectName("SpawnerStoreDock");

         SpawnerStoreView* view = new SpawnerStoreView();
         SpawnerStoreController* controller = new SpawnerStoreController(&mApplication->GetEntityManager());

         controller->moveToThread(mApplication->thread());
         controller->SetupSlots(view);
         connect(this, SIGNAL(TextDroppedOntoGLWidget(const QPointF&, const QString&)),
                 controller, SLOT(OnTextDroppedOntoGLWidget(const QPointF&, const QString&)));
         QTimer::singleShot(0, controller, SLOT(Init()));

         spawnerStoreDock->setWidget(view);
         addDockWidget(Qt::LeftDockWidgetArea, spawnerStoreDock);
      }

      {
         QDockWidget* propertyEditorDock = new QDockWidget("Property Editor");
         propertyEditorDock->setObjectName("PropertyEditorDock");
         PropertyEditorModel* model = new PropertyEditorModel();
         PropertyEditorView* view = new PropertyEditorView();
         view->SetModel(model);

         PropertyEditorController* controller = new PropertyEditorController(&mApplication->GetEntityManager());
         
         controller->moveToThread(mApplication->thread());
         controller->SetupSlots(model, view);

         QTimer::singleShot(0, controller, SLOT(Init()));
         
         propertyEditorDock->setWidget(view);
         
         addDockWidget(Qt::RightDockWidgetArea, propertyEditorDock);
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
   void EditorMainWindow::OnSaveAll()
   {
      emit SaveAll(mCurrentScene);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnResetSystem()
   {
      dtEntity::ResetSystemMessage msg;
      msg.SetSceneName(mCurrentScene.toStdString());
      mApplication->GetEntityManager().EnqueueMessage(msg);
      QTimer::singleShot(10, mApplication, SLOT(InitializeScripting()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnChooseScene()
   {
      QSettings settings;
      QStringList paths = settings.value("DataPaths", "ProjectAssets").toStringList();

      QStringList entries;
      for(QStringList::const_iterator i = paths.begin(); i != paths.end(); ++i)
      {
         QDir dir(*i + "/Scenes");
         if(dir.exists())
         {
            entries += dir.entryList(QStringList("*.dtescene"), QDir::Files);
         }
      }
      if(entries.empty())
      {
         QMessageBox::critical(this, "No scenes found", "No scenes found!"
                               " Please edit your data paths!");
         return;
      }

      dtEntityQtWidgets::ListDialog* dialog = new dtEntityQtWidgets::ListDialog(entries);
      entries.sort();
      if(dialog->exec() == QDialog::Accepted)
      {  
         QStringList sel = dialog->GetSelectedItems();
         if(sel.size() != 0)
         {
            mCurrentScene = QString("Scenes/%1").arg(sel.front());
            emit LoadScene(mCurrentScene);
         }
      }

      dialog->close();
      dialog->deleteLater();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnNewScene()
   {
      bool ok;
      QString text = QInputDialog::getText(this, tr("Enter name for new scene"),
                                          tr("Enter name for new scene:"), QLineEdit::Normal,
                                          "NewScene", &ok);
      if (ok && !text.isEmpty())
      {
         emit AddScene(text);
         mSaveAllAct->setEnabled(true);
         mSaveSceneAct->setEnabled(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::DoJump()
   {
      QString text = mJumpEdit->text();
      QStringList l = text.split(" ");
      if(l.size() == 3)
      {
         /*float x = l[0].toFloat();
         float y = l[1].toFloat();
         float z = l[2].toFloat();
         JumpToPositionMessage msg;
         msg.SetPosition(osg::Vec3(x, y, z));
         mApplication->GetEntityManager()->EnqueueMessage(msg);
         */
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
      connect(glwidget, SIGNAL(TextDropped(const QPointF&, const QString&)),
              this, SLOT(OnTextDroppedOntoGLWidget(const QPointF&, const QString&)));

      glwidget->setMinimumSize(400,400);
      
      ResizerWidget* r = new ResizerWidget();
      r->SetWidget(glwidget);  

      QObject::connect(r, SIGNAL(Resized(const QSize&)), this, SLOT(OnViewResized(const QSize&)));
      QObject::connect(r, SIGNAL(Closing()), this, SLOT(OnViewClosing()));

      setCentralWidget(r);

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
   void EditorMainWindow::OnSceneLoaded(const QString& path)
   {
      mCurrentScene = path;
      mSaveAllAct->setEnabled(true);
      mSaveSceneAct->setEnabled(true);
      setWindowTitle(QString("%1 - dtEntityEditor").arg(path));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorMainWindow::OnTextDroppedOntoGLWidget(const QPointF& pos, const QString& txt)
   {
      emit TextDroppedOntoGLWidget(pos, txt);
   }
}
