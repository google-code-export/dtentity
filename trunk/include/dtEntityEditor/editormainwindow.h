/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#pragma once

#include <QtGui/QtGui>
#include <dtEntity/messagepump.h>
#include <set>

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

      void OnToolsUpdated(const dtEntity::Message& msg);
      void OnMapLoaded(const dtEntity::Message& msg);
      void OnMapUnloaded(const dtEntity::Message& msg);
      void OnSceneLoaded(const dtEntity::Message& msg);
      void OnSceneUnloaded(const dtEntity::Message& msg);

      QStringList GetLoadedMaps() const;

   signals:      

      void LoadScene(const QString& path);
      void UnloadScene();
      void NewScene();
      void Closed(bool);
      void ViewResized(const QSize& size);
      void ViewClosing();
      void DataPathsChanged(const QStringList&);

      void SaveScene(const QString& path);

      void TextDroppedOntoGLWidget(const QPointF& pos, const QString&);

      void MapLoaded(const QString& map);
      void MapUnloaded(const QString& map);

   public slots:

      void OnDisplayError(const QString& msg);
      void SetOSGWindow(dtEntityQtWidgets::OSGGraphicsWindowQt*);

      void OnViewResized(const QSize& size);
      void OnViewClosing();

      // interpret line in jump tool bar and do jump
      void DoJump();


      void OnTextDroppedOntoGLWidget(const QPointF& pos, const QString&);
      void ShutDown();

      void CreateEntityTree();
      void CreateSpawners();
      void CreatePropertyEditor();

      /// Handles changes in the list of known systems/components
      /**
      *  This forces a refresh in the list of available components, by recreating it from
      *  scratch. Make sure this method is called whenever the list of known components
      *  changes and the GUI widget must be updated accordingly.
      */
      void OnComponentListChanged();

      protected slots:

      void OnToggleCoordSystem(bool v);
      void OnToggleGroundClamp(bool v);
      void OnToolActionTriggered(bool);
      void OnChooseScene();
      void OnNewScene();
      void OnChooseDataPaths();
      void OnSaveScene();
      void OnSaveSceneAs();
      void OnAddPlugin();
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
      QMenu* mViewMenu;

      QToolBar* mToolsToolbar;
      QActionGroup* mToolsActionGroup;

      QAction* mChangeDataPathsAct;
      QAction* mChangeSceneAct;
      QAction* mNewSceneAct;
      QAction* mSaveSceneAct;
      QAction* mSaveSceneAsAct;
      QAction* mAddPluginAct;
      QAction* mExitAct;

      // line edit for jump line in tool box
      QLineEdit* mJumpEdit;

      // The scene currently being edited
      QString mCurrentScene;

      QSet<QString> mCurrentMaps;

      dtEntity::MessagePump mMessagePump;
      QTimer* mUpdateTimer;

      QDockWidget* mEntityTreeDock;
      QDockWidget* mSpawnersDock;
      QDockWidget* mPropertyEditorDock;
   };

}

