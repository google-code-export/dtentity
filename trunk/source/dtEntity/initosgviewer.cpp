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
#include <iostream>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <dtEntity/componentfactories.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/logmanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/windowmanager.h>
#include <dtEntity/systemmessages.h>
#include <osgGA/GUIEventAdapter>
#include <osg/Notify>
#include <stdlib.h>

namespace dtEntity
{
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

   bool InitOSGViewer(int argc, char** argv, osgViewer::ViewerBase* viewer,
      dtEntity::EntityManager* em, bool checkPathsExist, bool addStatsHandler, bool addConsoleLog)
   {
       if(addConsoleLog)
       {
          LogManager::GetInstance().AddListener(new ConsoleLogHandler());
       }
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
             else if (curArgv == "--projectAssets")
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

       if((projectassets == "" || baseassets == "") && checkPathsExist)
       {
           std::cout << "Please give argument --projectAssets and --baseAssets with path to project assets dir!";
           return false;
       }

      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      if(!projectassets.empty())
      {
         std::vector<std::string> pathsplit = dtEntity::split(projectassets, ':');
         for(unsigned int i = 0; i < pathsplit.size(); ++i)
         {
            paths.push_back(pathsplit[i]);
         }
      }
      if(!baseassets.empty()) paths.push_back(baseassets);
      osgDB::setDataFilePathList(paths);

      /////////////////////////////////////////////////////////////////////

      // create new entity manager and register with system
      
      // this is a required component system, so add it immediately
      MapSystem* mapSystem = new MapSystem(*em);
      em->AddEntitySystem(*mapSystem);

      // load and start standard comonent systems
      RegisterStandardFactories(mapSystem->GetPluginManager());

      dtEntity::ApplicationSystem* appsystem = new dtEntity::ApplicationSystem(*em);
      em->AddEntitySystem(*appsystem);

      for(int i = 0; i < argc; ++i)
      {
         appsystem->AddCmdLineArg(argv[i]);
      }
      appsystem->SetViewer(viewer);


      dtEntity::LayerAttachPointSystem* layersys;
      em->GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      layersys->CreateSceneGraphRootEntity(new osg::Group());

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

      traits->windowDecoration = true;
      traits->doubleBuffer = true;
      traits->sharedContext = 0;

      if(screenNum == -1)
      {
         traits->x = winx;
         traits->y = winy;
         traits->width = winw;
         traits->height = winh;
      }
      else
      {
         osg::GraphicsContext::ScreenIdentifier si;
         si.readDISPLAY();

         // displayNum has not been set so reset it to 0.
         if (si.displayNum < 0) si.displayNum = 0;

         si.screenNum = screenNum;

         unsigned int width = 800;
         unsigned int  height = 60;
         osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
         if (wsi)
         {
            wsi->getScreenResolution(si, width, height);
         }
         else
         {
             OSG_NOTICE<<"View::setUpViewOnSingleScreen() : Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
         }

         traits->screenNum = screenNum;
         traits->x = 0;
         traits->y = 0;
         traits->width = width;
         traits->height = height;
         traits->windowDecoration = false;
      }

      appsystem->GetWindowManager()->OpenWindow("defaultView", SID("root"), *traits);
     // appsystem->GetPrimaryView()->setSceneData(layersys->GetSceneGraphRoot());
      //appsystem->InstallUpdateCallback(layersys->GetSceneGraphRoot());

      if(addStatsHandler)
      {
         osgViewer::ViewerBase::Views views;
         viewer->getViews(views);
         for(osgViewer::ViewerBase::Views::iterator i = views.begin(); i != views.end(); ++i)
         {
            osgViewer::StatsHandler* stats = new osgViewer::StatsHandler();
            stats->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_Insert);
            stats->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_Undo);
            (*i)->addEventHandler(stats);
         }
      }

      //mapSystem->GetPluginManager().LoadPluginsInDir("plugins");

      StartSystemMessage msg;
      em->EnqueueMessage(msg);
      return true;
   }
}
