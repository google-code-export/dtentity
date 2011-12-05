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

#pragma once

#include <QtGui/QtGui>
#include <dtEntity/messagepump.h>

namespace dtEntityQtWidgets
{
   class OSGGraphicsWindowQt;
}

namespace dtEntityEditor
{
   class EditorApplication;
   
   ////////////////////////////////////////////////////////////////////////////////
   class ResizerWidget : public QWidget
   {
      Q_OBJECT

   public:
      ResizerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
      void SetWidget(QWidget* widget);
      void EmitCurrentSize();
   signals:

      void Resized(const QSize&);
      void Closing();
      void LoadScene(const QString&);

   protected:

      virtual void resizeEvent(QResizeEvent* evt);
 
   };

   ////////////////////////////////////////////////////////////////////////////////
   class EditorMainWindow 
      : public QMainWindow
   {
     Q_OBJECT

   public:

      EditorMainWindow(EditorApplication* app, QWidget *parent = 0);
      virtual ~EditorMainWindow();

      void ToolsUpdated(const dtEntity::Message& msg);

   signals:      

      void LoadScene(const QString& path);
      void AddScene(const QString& name);
      void Closed(bool);
      void ViewResized(const QSize& size);
      void ViewClosing();
      void DataPathsChanged(const QStringList&);

      void SaveScene(const QString& path);
      void SaveAll(const QString& path);

      void TextDroppedOntoGLWidget(const QPointF& pos, const QString&);

   public slots:

      void OnDisplayError(const QString& msg);
      void SetOSGWindow(dtEntityQtWidgets::OSGGraphicsWindowQt*);

      void OnViewResized(const QSize& size);
      void OnViewClosing();

      // interpret line in jump tool bar and do jump
      void DoJump();

      void OnSceneLoaded(const QString& path);

      void OnTextDroppedOntoGLWidget(const QPointF& pos, const QString&);
      void ShutDown();

      void CreateEntityTree();
      void CreateMessageStore();
      void CreateSpawners();
      void CreatePropertyEditor();

   protected slots:

      void OnToolActionTriggered(QAction* action );
      void OnChooseScene();
      void OnNewScene();
      void OnChooseDataPaths();
      void OnSaveScene();
      void OnSaveAll();
      void OnResetSystem();
      void EmitQueuedMessages();

   protected:
      virtual void closeEvent(QCloseEvent* e);
      void ReadSettings();
      void WriteSettings();
      void createActions();
      void createMenus();
      void createToolBars();
      void CreateDockWidgets();


   private:

      EditorApplication* mApplication;

      QMenu* mFileMenu;
      QMenu* mEditMenu;
      QMenu* mViewMenu;

      QToolBar* mToolsToolbar;
      QActionGroup* mToolsActionGroup;

      QAction* mChangeDataPathsAct;
      QAction* mChangeSceneAct;
      QAction* mNewSceneAct;
      QAction* mSaveSceneAct;
      QAction* mSaveAllAct;
      QAction* mResetSystemAct;
      QAction* mExitAct;

      // line edit for jump line in tool box
      QLineEdit* mJumpEdit;

      // The scene currently being edited
      QString mCurrentScene;

      dtEntity::MessagePump mMessagePump;
      QTimer* mUpdateTimer;

      QDockWidget* mEntityTreeDock;
      QDockWidget* mMessageStoreDock;
      QDockWidget* mSpawnersDock;
      QDockWidget* mPropertyEditorDock;
   };

}

