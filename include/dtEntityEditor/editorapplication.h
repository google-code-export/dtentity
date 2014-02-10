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

#include <dtEntity/entitymanager.h>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtCore/QFileSystemWatcher>
#include <dtEntity/systeminterface.h>
#include <set>

namespace dtEntity
{
   class EntityManager;
}

namespace osgViewer
{
   class Viewer;
}

namespace dtEntityEditor
{   
   class EditorMainWindow;
   
   class EditorApplication 
      : public QObject
      , public osg::Referenced
   {
      
      Q_OBJECT

   public:    

      EditorApplication(int argc, char *argv[]);
      ~EditorApplication();

      /**
       * Get access to the entity system holding actors etc.
       */ 
      dtEntity::EntityManager& GetEntityManager() const; 

      /**
       * Plugins have to have access to the Qt main window. This pointer
       * is passed to the plugins so they can add their GUI
       */
      void SetMainWindow(EditorMainWindow* mw);

      /**
       * Add a path to the list of paths to look for plugins in
       */
      void SetAdditionalPluginPath(const QString &path, bool bBeforeDefaultPath = false);

      /// Adds a specific plugin to the component plugin manager
      void AddPluginLibrary(std::string fileName);

      /**
       * Get list of osg data paths
       */
      QStringList GetDataPaths() const;

      void OnCameraAdded(const dtEntity::Message& msg);
      void OnResourceLoaded(const dtEntity::Message& msg);

   public slots:

      /**
       * Change osgDB data paths. Separate paths with a ';'
       * First listed path will be set as Qt search path 'ProjecAssets:'
       */
      void SetDataPaths(const QStringList&);
      

      /**
       * Start the simulation loop
       */
      void StartGame(const QString& sceneToLoad);

      /**
       * Stop and exit the simulation loop
       */
      void ShutDownGame(bool);
     
      /**
        * Reset scene
        */
      void NewScene();

      /**
       * Load scene from path
       */
      void LoadScene(const QString& path);

      void SaveScene(const QString& path);

      void ViewResized(const QSize& size);

      void InitializeScripting();

      void OnFileChanged(const QString& path);

   private slots:

      /**
       * Do a single iteration of simulation and redraw display.
       * Is called by Qt timer
       */
      void StepGame();
      
   signals: 
      
      void ErrorOccurred(const QString&);
      void EditorSceneLoaded(const QString& path);
      void DataPathsChanged(const QStringList& paths);

   private:

      EditorMainWindow* mMainWindow;
      dtEntity::EntityManager* mEntityManager;
      osg::ref_ptr<osgViewer::Viewer> mViewer;
      std::vector<std::string> mPluginPaths;
      dtEntity::Timer_t mStartOfFrameTick;
      double mTimeScale;
      QFileSystemWatcher* mFileSystemWatcher;

    };
}
