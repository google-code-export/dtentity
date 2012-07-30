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

#include <dtEntityOSG/initosgviewer.h>



#include <dtEntity/core.h>
#include <dtEntity/init.h>

#include <dtEntity/componentpluginmanager.h>
#include <dtEntityOSG/osgdebugdrawinterface.h>
#include <dtEntityOSG/osginputinterface.h>
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntityOSG/osgwindowinterface.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntityOSG/componentfactories.h>
#include <dtEntity/systemmessages.h>

#include <osgGA/GUIEventAdapter>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

namespace dtEntityOSG
{


   ////////////////////////////////////////////////////////////////////////////////
   bool InitOSGViewer(int argc,  
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
        dtEntity:: LogManager::GetInstance().AddListener(new dtEntity::ConsoleLogHandler());
      }

      dtEntity::SetupDataPaths(argc, argv, checkPaths);

      dtEntity::SetSystemInterface(new OSGSystemInterface(em.GetMessagePump()));
      dtEntity::SetInputInterface(new OSGInputInterface(em.GetMessagePump()));
      dtEntity::SetWindowInterface(new OSGWindowInterface(em));

      dtEntity::AddDefaultEntitySystemsAndFactories(argc, argv, em);
      RegisterStandardFactories(dtEntity::ComponentPluginManager::GetInstance());

      // give application system access to viewer
      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      iface->SetViewer(&viewer);
      bool success = DoScreenSetup(argc, argv, viewer, em);
      if(!success)
      {
         LOG_ERROR("Error setting up screens! exiting");
         return false;
      }

      SetupSceneGraph(viewer, em, pSceneNode);

      // need layer system for debug draw interface, so set it up now
      dtEntity::SetDebugDrawInterface(new OSGDebugDrawInterface(em));

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

      dtEntity::StartSystemMessage msg;
      em.EnqueueMessage(msg);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////// 
   void SetupSceneGraph(osgViewer::ViewerBase& viewer, dtEntity::EntityManager& em, osg::Group* pSceneNode)
   {
      assert(pSceneNode != NULL);

      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());

      if(iface->GetViewer() == NULL)
      {
         iface->SetViewer(&viewer);
      }
      
      // install update traversal callback of application system into scene graph
      // root node. Used for sending tick messages etc.
      iface->InstallUpdateCallback(pSceneNode);

      // set scene graph root node as scene data for viewer. This is done again in camera setup,
      // but to make it accessible immediately we add it here, first
      osgViewer::ViewerBase::Views views;
      iface->GetViewer()->getViews(views);
      views.front()->setSceneData(pSceneNode);

      // add input handler as callback to primary camera. This is also done again in Camera setup,
      // but is done here first so everything runs fine without a camera.
      OSGInputInterface* ipface = static_cast<OSGInputInterface*>(dtEntity::GetInputInterface());
      views.front()->addEventHandler(ipface);

      dtEntity::ComponentPluginManager::GetInstance().StartEntitySystem(em, LayerAttachPointSystem::TYPE);
      dtEntity::ComponentPluginManager::GetInstance().StartEntitySystem(em, LayerSystem::TYPE);

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
      

      dtEntity::WindowInterface* wface = dtEntity::GetWindowInterface();
      OSGWindowInterface* o = static_cast<OSGWindowInterface*>(wface);
      o->SetTraits(traits);
      unsigned int contextId;
      bool success = wface->OpenWindow("defaultView", dtEntity::SID("root"), contextId);
      if(!success)
      {
         LOG_ERROR("Could not open window, exiting!");
         return false;
      }

      if(screenNum != -1)
      {
         wface->SetFullscreen(contextId, true);
      } 
      return true;
   }


}
