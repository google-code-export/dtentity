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

#include <dtEntity/windowmanager.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>

#include <osg/GraphicsContext>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <assert.h>
#include <sstream>


namespace dtEntity
{   


////////////////////////////////////////////////////////////////////////////////
   osgViewer::View* WindowManager::GetViewByName(const std::string& name)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);

      osgViewer::ViewerBase::Views views;
      appsys->GetViewer()->getViews(views);
      osgViewer::ViewerBase::Views::iterator i;

      for(i = views.begin(); i != views.end(); ++i)
      {
         if((*i)->getName() == name)
         {
            return *i;
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* WindowManager::GetWindowByName(const std::string& name)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);

      osgViewer::ViewerBase::Windows wins;
      appsys->GetViewer()->getWindows(wins, false);

      osgViewer::ViewerBase::Windows::iterator i;
      for(i = wins.begin(); i != wins.end(); ++i)
      {
         if((*i)->getName() == name)
         {
            return *i;
         }
      }

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   OSGWindowManager::OSGWindowManager(EntityManager& em)
      : WindowManager(em)
   {
      mCloseWindowFunctor = MessageFunctor(this, &OSGWindowManager::OnCloseWindow);
      mMessagePump.RegisterForMessages(CloseWindowMessage::TYPE, mCloseWindowFunctor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   OSGWindowManager::~OSGWindowManager()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   unsigned int OSGWindowManager::OpenWindow(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {
      unsigned int contextId = 0;
      bool success = OpenWindowInternal(name, layername, traits, contextId);
      
      if(success)
      {
         WindowCreatedMessage msg;
         msg.SetName(name);
         msg.SetContextId(contextId);
         mEntityManager->EmitMessage(msg);
         return contextId;
      }
      //TODO return an error code instead
      return 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool OSGWindowManager::OpenWindowInternal(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits, unsigned int& contextId)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);
      osgViewer::CompositeViewer* compviewer = dynamic_cast<osgViewer::CompositeViewer*>(appsys->GetViewer());      
      
	   if(compviewer == NULL)
      {
         appsys->GetViewer()->realize();
         osgViewer::ViewerBase::Windows windows;
         appsys->GetViewer()->getWindows(windows);
         windows.front()->setName(name);
         contextId = 0;
      }
      else
      {
         traits.readDISPLAY();
         if (traits.displayNum<0) traits.displayNum = 0;

         osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(&traits);
         osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
         if (gw)
         {
             osgViewer::View* view = new osgViewer::View();
             view->getCamera()->setGraphicsContext(gw);
             compviewer->addView(view);
             OSG_INFO<<"View::setUpViewOnSingleScreen - GraphicsWindow has been created successfully."<<std::endl;
             gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(traits.x, traits.y, traits.width, traits.height );
             gw->getEventQueue()->getCurrentEventState()->setMouseYOrientation(osgGA::GUIEventAdapter::Y_INCREASING_DOWNWARDS);
             gw->setName(name);
             
             
             contextId = gw->getState()->getContextID();
             gw->realize();
         }
         else
         {
             LOG_ERROR("GraphicsWindow has not been created successfully.");
             return false;
         }
      }
      return true;      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OSGWindowManager::OnCloseWindow(const Message& m)
   {
      const CloseWindowMessage& msg = static_cast<const CloseWindowMessage&>(m);

      osgViewer::GraphicsWindow* window = GetWindowByName(msg.GetName());

      if(window == NULL)
      {
         LOG_ERROR("Cannot close window!");
         return;
      }

      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);

      osgViewer::CompositeViewer* compview = dynamic_cast<osgViewer::CompositeViewer*>(appsys->GetViewer());
      if(compview == NULL)
      {
         LOG_ERROR("Cannot close window, use CompositeViewer class!");
         return;
      }

      CameraSystem* camsys;
      mEntityManager->GetEntitySystem(CameraComponent::TYPE, camsys);
      EntityId camid = camsys->GetCameraEntityByContextId(window->getState()->getContextID());
      if(camid != EntityId())
      {
         mEntityManager->RemoveFromScene(camid);
         mEntityManager->KillEntity(camid);
      }

      osgViewer::View* view = GetViewByName(msg.GetName());
      if(!view)
      {
         LOG_ERROR("Cannot close view, not found: " + msg.GetName());
         return;
      }
      view->setSceneData(NULL);

      window->close();
      compview->removeView(view);

   }

   ///////////////////////////////////////////////////////////////////////////////
   void OSGWindowManager::CloseWindow(const std::string& name)
   {
      // closing window from an event handler creates a crash.
      // Closing window at time of message processing works OK.
      CloseWindowMessage msg;
      msg.SetName(name);
      mMessagePump.EnqueueMessage(msg);
   }


   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 OSGWindowManager::GetPickRay(const std::string& name, float x, float y, bool usePixels)
   {
      osgViewer::GraphicsWindow* window = GetWindowByName(name);
      
      if(!window) 
      {
         LOG_ERROR("Cannot get pickray for window " + name);
         return osg::Vec3(0,1,0);
      }

      osg::Camera* cam = GetViewByName(name)->getCamera();
      
      int wx, wy, w, h;
      window->getWindowRectangle(wx, wy, w, h);

      osg::Vec2 windowCoord;
	  if(usePixels)
	  {
		  windowCoord.set(x, y);
	  }
	  else
	  {
		  windowCoord.set((w/2) * (x + 1), (h/2) * (y + 1));
	  }

      // calculate pick ray
      osg::Vec3 start = osg::Vec3(windowCoord, 0);
      osg::Vec3 end(windowCoord, 1);

      osg::Matrix matrix;
      if(cam->getViewport()) 
      {
         matrix.preMult(cam->getViewport()->computeWindowMatrix());
      }
      matrix.preMult(cam->getProjectionMatrix());
      matrix.preMult(cam->getViewMatrix());

      osg::Matrix inverse;
      inverse.invert(matrix);

      start = start * inverse;
      end = end * inverse;
      osg::Vec3 pickray = end - start;
      pickray.normalize();
      return pickray;
      
   }
}
