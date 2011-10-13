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
#include <osgDB/FileUtils>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <dtEntity/layercomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/applicationcomponent.h>
#include <osgGA/GUIEventAdapter>

namespace dtEntity
{
   bool InitOSGViewer(int argc, char** argv, osgViewer::ViewerBase* viewer,
      dtEntity::EntityManager* em, bool checkPathsExist, bool addStatsHandler)
   {
      
       std::string projectassets = "";
       std::string baseassets = "";

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

       int curArg = 0;

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
      
      osg::ref_ptr<osg::Group> sceneNode = new osg::Group();
      sceneNode->setName("Scene Graph Root");
      
      osgViewer::ViewerBase::Views views;
      viewer->getViews(views);
      if(views.empty())
      {
         LOG_ERROR("OSG Viewer has to have at least one view!");
         return false;
      }
   
      osgViewer::View* view = views.front();
      view->setName("defaultView");
      view->setSceneData(sceneNode);
      

      if(addStatsHandler)
      {
         osgViewer::StatsHandler* stats = new osgViewer::StatsHandler();
         stats->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_Insert);
         view->addEventHandler(stats);
         
      }
      viewer->realize();
      

      osgViewer::ViewerBase::Windows wins;
      viewer->getWindows(wins);
      wins.front()->setName("defaultView");

      dtEntity::ApplicationSystem* appsystem = new dtEntity::ApplicationSystem(*em);
      appsystem->SetViewer(viewer);
      appsystem->CreateSceneGraphEntities();
    
      em->AddEntitySystem(*appsystem);

      dtEntity::MapSystem* mapSystem;
      em->GetEntitySystem(dtEntity::MapComponent::TYPE, mapSystem);
      mapSystem->GetPluginManager().LoadPluginsInDir("plugins");

      return true;
   }
}
