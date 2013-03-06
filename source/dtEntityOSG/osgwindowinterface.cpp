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

#include <dtEntityOSG/osgwindowinterface.h>

#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/cameracomponent.h>
#include <dtEntity/core.h>
#include <dtEntity/entity.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntityOSG/osgsysteminterface.h>
#include <dtEntity/systemmessages.h>
#include <osg/GraphicsContext>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <assert.h>
#include <sstream>


namespace dtEntityOSG
{   

   ////////////////////////////////////////////////////////////////////////////////
   osgViewer::View* GetViewByName(const std::string& name)
   {
      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase::Views views;
      iface->GetViewer()->getViews(views);
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
   osgViewer::GraphicsWindow* GetWindowByName(const std::string& name)
   {
      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase::Windows wins;
      iface->GetViewer()->getWindows(wins, false);

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
   OSGWindowInterface::OSGWindowInterface(dtEntity::EntityManager& em)
      : mEntityManager(&em)
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   OSGWindowInterface::~OSGWindowInterface()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* OSGWindowInterface::GetWindowByContextId(unsigned int contextId) const
   {
      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase* viewer = iface->GetViewer();

      osgViewer::ViewerBase::Windows wins;
      viewer->getWindows(wins);
      for(osgViewer::ViewerBase::Windows::iterator i = wins.begin(); i != wins.end(); ++i)
      {
         osgViewer::GraphicsWindow* w = *i;
         if(w && w->getState()->getContextID() == contextId)
         {
            return w;
         }
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osgViewer::View* OSGWindowInterface::GetViewByContextId(unsigned int contextId) const
   {

      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase* viewer = iface->GetViewer();

      osgViewer::ViewerBase::Views views;
      viewer->getViews(views);
      for(osgViewer::ViewerBase::Views::iterator i = views.begin(); i != views.end(); ++i)
      {
         osgViewer::View* v = *i;
         if(v && v->getCamera()->getGraphicsContext()->getState()->getContextID() == contextId)
         {
            return v;
         }
      }
      return NULL;
   }


   ///////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::OpenWindow(const std::string& name, unsigned int& contextId)
   {
      bool success = OpenWindowInternal(name, contextId);
      
      if(success)
      {
         dtEntity::WindowCreatedMessage msg;
         msg.SetName(name);
         msg.SetContextId(contextId);
         mEntityManager->EmitMessage(msg);
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::OpenWindowInternal(const std::string& name, unsigned int& contextId)
   {
      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase* viewer = iface->GetViewer();
      osgViewer::CompositeViewer* compviewer = dynamic_cast<osgViewer::CompositeViewer*>(viewer);
      
	   if(compviewer == NULL)
      {
         viewer->realize();
         if(!viewer->isRealized())
         {
            LOG_ERROR("OSG viewer could not realize window, check your display traits!");
            return false;
         }
         osgViewer::ViewerBase::Windows windows;
         viewer->getWindows(windows);
         windows.front()->setName(name);
         contextId = 0;
      }
      else
      {
         mTraits->readDISPLAY();
         if (mTraits->displayNum < 0) mTraits->displayNum = 0;

         osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(mTraits);
         osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
         if (gw)
         {
             osgViewer::View* view = new osgViewer::View();
             view->getCamera()->setGraphicsContext(gw);
             view->setName(name);
             compviewer->addView(view);

             // wake viewer up if it was asleep (had no windows)

             compviewer->setDone(false);
             OSG_INFO<<"View::setUpViewOnSingleScreen - GraphicsWindow has been created successfully."<<std::endl;
             gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(mTraits->x, mTraits->y, mTraits->width, mTraits->height);
             gw->getEventQueue()->getCurrentEventState()->setMouseYOrientation(osgGA::GUIEventAdapter::Y_INCREASING_DOWNWARDS);
             gw->setName(name);
             
             
             contextId = gw->getState()->getContextID();
             gw->realize();
             if(!gw->isRealized())
             {
                LOG_ERROR("OSG viewer could not realize window, check your display traits!");
                return false;
             }
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
   void OSGWindowInterface::CloseWindow(unsigned int contextid)
   {

      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextid);

      if(window == NULL)
      {
         LOG_ERROR("Cannot close window!");
         return;
      }

      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(dtEntity::GetSystemInterface());
      osgViewer::ViewerBase* viewer = iface->GetViewer();

      osgViewer::CompositeViewer* compview = dynamic_cast<osgViewer::CompositeViewer*>(viewer);
      if(compview == NULL)
      {
         LOG_ERROR("Cannot close window, use CompositeViewer class!");
         return;
      }

      dtEntityOSG::CameraSystem* camsys;
      mEntityManager->GetEntitySystem(dtEntityOSG::CameraComponent::TYPE, camsys);
      dtEntity::EntityId camid = camsys->GetCameraEntityByContextId(window->getState()->getContextID());
      if(camid != dtEntity::EntityId())
      {
         mEntityManager->RemoveFromScene(camid);
         mEntityManager->KillEntity(camid);
      }

      osgViewer::View* view = GetViewByContextId(contextid);
      if(!view)
      {
         LOG_ERROR("Cannot close view, not found: " << contextid);
         return;
      }
      view->setSceneData(NULL);

      window->close();
      compview->removeView(view);
   }


   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Vec3f OSGWindowInterface::GetPickRay(unsigned int contextid, float x, float y, bool usePixels) const
   {
      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextid);
      
      if(!window) 
      {
         LOG_ERROR("Cannot get pickray for context " + contextid);
         return osg::Vec3(0,1,0);
      }

      osg::View* view = GetViewByContextId(contextid);
      if(!view)
      {
         LOG_ERROR("Cannot get pickray for context " + contextid);
         return osg::Vec3(0,1,0);
      }

      osg::Camera* cam = view->getCamera();
      
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

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::GetWindowGeometry(unsigned int contextId, int& x, int& y, int& width, int& height) const
   {

      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextId);
      if(!window)
      {
         return false;
      }
      window->getWindowRectangle(x, y, width, height);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::SetWindowGeometry(unsigned int contextId, int x, int y, int width, int height)
   {
      SetFullscreen(contextId, false);

      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextId);
      if(!window)
      {
         return false;
      }
      window->setWindowRectangle(x, y, width, height);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGWindowInterface::SetFullscreen(unsigned int contextId, bool fullscreen)
   {
      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextId);
      if(!window)
      {
         LOG_ERROR("Cannot set window to fullscreen, no window with that id found! " << contextId);
         return;
      }

      int x, y, w, h;
      window->getWindowRectangle(x, y, w, h);

      osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();

      if (wsi == NULL)
      {
         LOG_WARNING("Error, no WindowSystemInterface available, cannot toggle window fullscreen.");
         return;
      }

      unsigned int screenWidth;
      unsigned int screenHeight;

      wsi->getScreenResolution(*(window->getTraits()), screenWidth, screenHeight);

      if (!fullscreen)
      {
         if(!GetFullscreen(contextId)) return;
         WindowPosMap::iterator i = mWindowPositions.find(contextId);
         if(i == mWindowPositions.end())
         {
            return;
         }
         WindowPos wp = i->second;
         mWindowPositions.erase(i);

         window->setWindowDecoration(wp.mWindowDeco);
         window->setWindowRectangle(wp.mX, wp.mY, wp.mW, wp.mH);
      }
      else
      {
         if(GetFullscreen(contextId)) return;
         WindowPos wp;
         wp.mX = x;
         wp.mY = y;
         wp.mW = w;
         wp.mH = h;
         wp.mWindowDeco = window->getWindowDecoration();
         mWindowPositions[contextId] = wp;

         window->setWindowDecoration(false);
         window->setWindowRectangle(0, 0, screenWidth, screenHeight);
      }

      window->grabFocusIfPointerInWindow();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::GetFullscreen(unsigned int contextId) const
   {
      WindowPosMap::const_iterator i = mWindowPositions.find(contextId);
      return (i != mWindowPositions.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId OSGWindowInterface::PickEntity(double x, double y, unsigned int nodemask,
                                                     unsigned int contextid, bool usePixels) const
   {
      dtEntityOSG::CameraSystem* camsys;
	   mEntityManager->GetES(camsys);
	   dtEntity::EntityId camid = camsys->GetCameraEntityByContextId(contextid);
	   dtEntityOSG::CameraComponent* camcomp;
	   bool success = mEntityManager->GetComponent(camid, camcomp);
	   if(!success)
	   {
	 	  return 0;
	   }
	   osg::Vec3d from = camcomp->GetPosition();
      dtEntity::Vec3f pr = GetPickRay(contextid, x, y, usePixels);
      
      osg::Vec3d to = from + pr * 10000;

      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi;
      lsi = new osgUtil::LineSegmentIntersector(from, to);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setUseKdTreeWhenAvailable(true);
      iv.setTraversalMask(nodemask); 

      LayerAttachPointSystem* laps;
      mEntityManager->GetEntitySystem(LayerAttachPointComponent::TYPE, laps);
      LayerAttachPointComponent* sceneLayer = laps->GetDefaultLayer();
      sceneLayer->GetGroup()->accept(iv);

      if(!lsi->containsIntersections())
      {
         return 0;
      }

      osgUtil::LineSegmentIntersector::Intersections::iterator i;

      for(i = lsi->getIntersections().begin(); i != lsi->getIntersections().end(); ++i)
      {
         osgUtil::LineSegmentIntersector::Intersection isect = *i;
         for(osg::NodePath::const_reverse_iterator j = isect.nodePath.rbegin(); j != isect.nodePath.rend(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();
            if(referenced == NULL) continue;
            const EntityData* ed = dynamic_cast<const EntityData*>(referenced);
            if(ed != NULL)
            {			     
               return ed->mEntity->GetId();
            }
         }
      }
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGWindowInterface::SetShowCursor(unsigned int contextid, bool v)
   {
      osgViewer::GraphicsWindow* window = GetWindowByContextId(contextid);
      if(!window)
      {
         LOG_ERROR("Cannot set show cursor: No window with context id " << contextid);
         return;
      }
      if(v)
      {
         window->useCursor(true);
         window->setCursor(osgViewer::GraphicsWindow::RightArrowCursor);
      }
      else
      {
         window->useCursor(false);
         window->setCursor(osgViewer::GraphicsWindow::NoCursor);         
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Vec3d OSGWindowInterface::ConvertWorldToScreenCoords(unsigned int contextid, const dtEntity::Vec3d& coord)
   {
      osgViewer::View* view = GetViewByContextId(contextid);
      if(view == NULL)
      {
         LOG_ERROR("Cannot convert world to screen coords: context does not exist");
         return dtEntity::Vec3d();
      }
      osg::Camera* cam = view->getCamera();

      osg::Vec4d c(coord, 1);
      c = cam->getViewMatrix().preMult(c);
      c = cam->getProjectionMatrix().preMult(c);
      double w = c [3];
      return osg::Vec3d(c [0] / w, c [1] / w, c [2] / w);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGWindowInterface::SetContinuousRedraw(unsigned int contextid, bool v)
   {
      osgViewer::View* view = GetViewByContextId(contextid);
      if(view)
      {
         view->requestContinuousUpdate(v);
      }
      else
      {
         LOG_ERROR("SetContinuousRedraw: No view with this contextid");
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void OSGWindowInterface::RequestRedraw(unsigned int contextid)
   {
      osgViewer::View* view = GetViewByContextId(contextid);
      if(view)
      {
         view->requestRedraw();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGWindowInterface::GetIntersections(const dtEntity::Vec3d& start, const dtEntity::Vec3d& end,
         std::vector<dtEntity::WindowInterface::Intersection>& isects, unsigned int nodemask, dtEntity::StringId layer) const
   {
      if(layer == dtEntity::StringId())
      {
         layer = dtEntity::SID("default");
      }
      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi;
      lsi = new osgUtil::LineSegmentIntersector(start, end);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setUseKdTreeWhenAvailable(true);
      iv.setTraversalMask(nodemask);

      dtEntityOSG::LayerAttachPointSystem* ls;
      mEntityManager->GetES(ls);
      dtEntityOSG::LayerAttachPointComponent* layercomp;
      if(!ls->GetByName(layer, layercomp))
      {
         LOG_ERROR("Could not execute isect test, layer attach point not found!");
         return false;
      }

      layercomp->GetNode()->accept(iv);

      if(!lsi->containsIntersections())
      {
         return false;
      }

      osgUtil::LineSegmentIntersector::Intersections::iterator i;
      for(i = lsi->getIntersections().begin(); i != lsi->getIntersections().end(); ++i)
      {
         osgUtil::LineSegmentIntersector::Intersection isect = *i;
         for(osg::NodePath::const_iterator j = isect.nodePath.begin(); j != isect.nodePath.end(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();
            if(referenced == NULL) continue;
            const EntityData* ed = dynamic_cast<const EntityData*>(referenced);
            if(ed != NULL)
            {
               dtEntity::WindowInterface::Intersection entry;
               entry.mEntityId = ed->mEntity->GetId();
               entry.mNormal = isect.getWorldIntersectNormal();
               entry.mPosition = isect.getWorldIntersectPoint();
               isects.push_back(entry);
            }
         }
      }
      return true;
   }
}
