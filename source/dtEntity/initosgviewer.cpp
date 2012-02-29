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

#include <dtEntity/initosgviewer.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/componentfactories.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/logmanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/windowmanager.h>

#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   // a simple log handler that forwards log messages to osg log system
   class ConsoleLogHandler
      : public LogListener
   {
    public:
      virtual void LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg)
      {
		 std::ostream* strm;
		 switch(level)
         {
		 case  LogLevel::LVL_DEBUG   : strm = &osg::notify(osg::DEBUG_INFO); break;
		 case  LogLevel::LVL_INFO    : strm = &osg::notify(osg::INFO); break;
		 case  LogLevel::LVL_WARNING : strm = &osg::notify(osg::WARN); break;
		 case  LogLevel::LVL_ERROR   : strm = &osg::notify(osg::FATAL); break;
		 case  LogLevel::LVL_ALWAYS  : strm = &osg::notify(osg::ALWAYS); break;
		 default: strm = &osg::notify(osg::ALWAYS);
         }
		 (*strm) << "File: " << filename << " Line: " << linenumber << " Message: " << msg << std::endl; 
		 strm->flush();
         
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   bool SetupDataPaths(int argc, char** argv, bool checkPaths)
   {
      std::string projectassets = "";
      std::string baseassets = "";

      if(osgDB::fileExists("ProjectAssets")) 
      {
         projectassets = osgDB::getFilePath(argv[0]) + osgDB::getNativePathSeparator() + "ProjectAssets";
      }

      if(osgDB::fileExists("BaseAssets")) 
      {
         projectassets = osgDB::getFilePath(argv[0]) + osgDB::getNativePathSeparator() + "BaseAssets";
      }

      const char* env_projectassets = getenv("DTENTITY_PROJECTASSETS");
      if(env_projectassets != NULL)
      {
         projectassets = env_projectassets;
      }
      const char* env_baseassets = getenv("DTENTITY_BASEASSETS");
      if(env_baseassets != NULL)
      {
         baseassets = env_baseassets;
      }

      int curArg = 1;

      while (curArg < argc)
      {
         std::string curArgv = argv[curArg];
         if (!curArgv.empty())
         {
            if (curArgv == "--projectAssets")
            {
               ++curArg;
               if (curArg < argc)
               {
                  projectassets = argv[curArg];
               }

            }
            else if (curArgv == "--baseAssets")
            {
               ++curArg;
               if (curArg < argc)
               {
                  baseassets = argv[curArg];
               }
            }
         }
         ++curArg;
      }

      if((projectassets == "" || baseassets == "") && checkPaths)
      {
         std::cout << "Please give argument --projectAssets and --baseAssets with path to project assets dir!";
         return false;
      }

      projectassets = osgDB::convertFileNameToUnixStyle(projectassets);
      baseassets = osgDB::convertFileNameToUnixStyle(baseassets);

      osgDB::FilePathList paths = osgDB::getDataFilePathList();

      if(!baseassets.empty() && (std::find(paths.begin(), paths.end(), baseassets) == paths.end())) 
      {
         paths.push_back(baseassets);
      }

      if(!projectassets.empty())
      {
         std::vector<std::string> pathsplit = dtEntity::split(projectassets, ';');
         for(unsigned int i = 0; i < pathsplit.size(); ++i)
         {
            if(std::find(paths.begin(), paths.end(), pathsplit[i]) == paths.end())
            {
               paths.push_back(pathsplit[i]);
            }
         }
      }
      
      osgDB::setDataFilePathList(paths);
      return true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DT_ENTITY_EXPORT InitOSGViewer(int argc,  
                      char** argv, 
                      osgViewer::ViewerBase& viewer,
                      dtEntity::EntityManager& em,
                      bool addStatsHandler, 
                      bool checkPaths, 
                      bool addConsoleLog,
                      osg::Group* pSceneNode)
   {
      if(pSceneNode == NULL)
      {
         pSceneNode = new osg::Group();
      }

      assert(&viewer != NULL);
      assert(&em != NULL);

      if(addConsoleLog)
      {
         LogManager::GetInstance().AddListener(new ConsoleLogHandler());
      }

      

      SetupDataPaths(argc, argv, checkPaths);

      AddDefaultEntitySystemsAndFactories(argc, argv, em);
     
      // give application system access to viewer
      dtEntity::ApplicationSystem* appsystem;
      em.GetEntitySystem(ApplicationSystem::TYPE, appsystem);
      appsystem->SetViewer(&viewer);

      bool success = DoScreenSetup(argc, argv, viewer, em);
      if(!success)
      {
         LOG_ERROR("Error setting up screens! exiting");
         return false;
      }

      SetupSceneGraph(viewer, em, pSceneNode);
    
      if(addStatsHandler)
      {
         osgViewer::ViewerBase::Views views;
         viewer.getViews(views);
         for(osgViewer::ViewerBase::Views::iterator i = views.begin(); i != views.end(); ++i)
         {
            osgViewer::StatsHandler* stats = new osgViewer::StatsHandler();
            stats->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_Insert);
            stats->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_Undo);
            (*i)->addEventHandler(stats);
         }
      }

      StartSystemMessage msg;
      em.EnqueueMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////// 
   void SetupSceneGraph(osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em, osg::Group* pSceneNode)
   {
      assert(pSceneNode != NULL);

      dtEntity::ApplicationSystem* appsystem;
      em.GetEntitySystem(ApplicationSystem::TYPE, appsystem);

      if(appsystem->GetViewer() == NULL)
      {
         appsystem->SetViewer(&viewer);
      }
      
      // install update traversal callback of application system into scene graph
      // root node. Used for sending tick messages etc.
      appsystem->InstallUpdateCallback(pSceneNode);

      // set scene graph root node as scene data for viewer. This is done again in camera setup,
      // but to make it accessible immediately we add it here, first
      appsystem->GetPrimaryView()->setSceneData(pSceneNode);

      // add input handler as callback to primary camera. This is also done again in Camera setup,
      // but is done here first so everything runs fine without a camera.
      appsystem->GetPrimaryView()->addEventHandler(&appsystem->GetWindowManager()->GetInputHandler());

      // create an entity holding the scene graph root as an attach point
      LayerAttachPointSystem* layerattachsys;
      bool found = em.GetEntitySystem(LayerAttachPointComponent::TYPE, layerattachsys);
      assert(found);
      layerattachsys->CreateSceneGraphRootEntity(pSceneNode);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   bool DoScreenSetup(int argc, char** argv,
      osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em)
   {
      int curArg = 1;
      int screenNum = -1;
      int winx = 100;
      int winy = 100;
      int winw = 800;
      int winh = 600;

      while (curArg < argc)
      {
         std::string curArgv = argv[curArg];
         if (!curArgv.empty())
         {
            if (curArgv == "--screen")
            {
               ++curArg;
               if (curArg < argc)
               {
                  std::istringstream iss(argv[curArg]);
                  iss >> screenNum;
               }
            }
            else if (curArgv == "--window")
            {
               ++curArg;
               if (curArg < argc)
               {
                  std::istringstream iss(argv[curArg]);
                  iss >> winx;
               }
               ++curArg;
               if (curArg < argc)
               {
                  std::istringstream iss(argv[curArg]);
                  iss >> winy;
               }
               ++curArg;
               if (curArg < argc)
               {
                  std::istringstream iss(argv[curArg]);
                  iss >> winw;
               }
               ++curArg;
               if (curArg < argc)
               {
                  std::istringstream iss(argv[curArg]);
                  iss >> winh;
               }
               std::ostringstream os;
               os << "OSG_WINDOW=" << winx << " " << winy << " " << winw << " " << winh;

               // unix putenv only accepts non-const char. No idea why.
               char osgwinstr[256];
               strcpy(osgwinstr, os.str().c_str());
               putenv(osgwinstr);

            }
         }
         ++curArg;
      }

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

      traits->windowDecoration = true;
      traits->doubleBuffer = true;
      traits->sharedContext = 0;

      traits->x = winx;
      traits->y = winy;
      traits->width = winw;
      traits->height = winh;
      if(screenNum != -1)
      {
         traits->screenNum = screenNum;
      }
      
      dtEntity::ApplicationSystem* appsystem;
      em.GetEntitySystem(ApplicationSystem::TYPE, appsystem);

      unsigned int contextId;
      bool success = appsystem->GetWindowManager()->OpenWindow("defaultView", SID("root"), *traits, contextId);
      if(!success)
      {
         LOG_ERROR("Could not open window, exiting!");
         return false;
      }

      if(screenNum != -1)
      {
         appsystem->GetWindowManager()->SetFullscreen(contextId, true);
      } 
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void AddDefaultEntitySystemsAndFactories(int argc, char** argv, EntityManager& em)
   {
      
      // this is a required component system, so add it immediately
      MapSystem* mapSystem = new MapSystem(em);
      em.AddEntitySystem(*mapSystem);

      em.AddEntitySystem(*new CameraSystem(em));
      em.AddEntitySystem(*new LayerSystem(em));
      em.AddEntitySystem(*new GroupSystem(em));
      em.AddEntitySystem(*new StaticMeshSystem(em));
      em.AddEntitySystem(*new TransformSystem(em));
      em.AddEntitySystem(*new MatrixTransformSystem(em));
      em.AddEntitySystem(*new PositionAttitudeTransformSystem(em));
      em.AddEntitySystem(*new LayerAttachPointSystem(em));
      
      ApplicationSystem* appsystem = new ApplicationSystem(em);
      em.AddEntitySystem(*appsystem);
      for(int i = 0; i < argc; ++i)
      {
         appsystem->AddCmdLineArg(argv[i]);
      }

      // add factories for aditional entity systems, they will be started lazily
      RegisterStandardFactories(ComponentPluginManager::GetInstance());

   }
}
