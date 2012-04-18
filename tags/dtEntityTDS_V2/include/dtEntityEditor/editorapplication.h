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

#include <dtEntity/entitymanager.h>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QObject>
#include <osg/Timer>
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
       * Load scene from path
       */
      void LoadScene(const QString& path);

      /**
       * Add and save empty new scene
       */
      void AddScene(const QString& datapath, const QString& mappath);

      void SaveScene();
      void SaveAll();

      void ViewResized(const QSize& size);

      void InitializeScripting();

   private slots:

      /**
       * Do a single iteration of simulation and redraw display.
       * Is called by Qt timer
       */
      void StepGame();
      
   signals: 
      
      void ErrorOccurred(const QString&);
      void SceneLoaded(const QString& path);
      void DataPathsChanged(const QStringList& paths);

   private:

      void CreateCameraEntityIfNotExists();
      EditorMainWindow* mMainWindow;
      dtEntity::EntityManager* mEntityManager;
      osg::ref_ptr<osgViewer::Viewer> mViewer;
      std::vector<std::string> mPluginPaths;
      osg::Timer_t mStartOfFrameTick;
      double mTimeScale;
    };
}
