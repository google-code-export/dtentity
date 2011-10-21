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
#include <osgViewer/CompositeViewer>

#include <assert.h>
#include <sstream>

#ifdef BUILD_WITH_DELTA3D 
	#include <dtABC/application.h>
	#include <dtCore/camera.h>
	#include <dtCore/deltawin.h>
	#include <dtCore/scene.h>
	#include <dtCore/system.h>
	#include <dtCore/view.h>
	#include <dtUtil/datetime.h>
   
#endif


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
      appsys->GetViewer()->getWindows(wins);

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
      :WindowManager(em)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OSGWindowManager::OpenWindow(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {
      OpenWindowInternal(name, layername, traits);
      WindowCreatedMessage msg;
      msg.SetName(name);
      mEntityManager->EmitMessage(msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OSGWindowManager::OpenWindowInternal(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);

      osgViewer::CompositeViewer* compviewer = dynamic_cast<osgViewer::CompositeViewer*>(appsys->GetViewer());
      if(compviewer == NULL)
      {
         LOG_ERROR("Cannot open window, use CompositeViewer class!");
         return;
      }
      LayerAttachPointSystem* lsys;
      mEntityManager->GetEntitySystem(LayerAttachPointComponent::TYPE, lsys);

      dtEntity::LayerAttachPointComponent* target;
      if(!lsys->GetByName(layername, target))
      {
         LOG_ERROR("Layer attach point not found, cannot open window!");
         return;
      }

      //osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(&traits);
      
      //osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
      //window->setName(name);
      //assert(window != NULL);
      osgViewer::View* view = new osgViewer::View();
      view->setName(name);
      view->setSceneData(target->GetGroup());
      
      
      osg::Camera* cam = new osg::Camera();   
      cam->setName(name);
      view->setCamera(cam);
//      cam->setGraphicsContext(window);
      //appsys->AddCameraToSceneGraph(cam);
//      gc->realize();
      view->setUpViewInWindow(100,100,800,600);
      compviewer->addView(view);
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OSGWindowManager::CloseWindow(const std::string& name)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);

      osgViewer::CompositeViewer* compview = dynamic_cast<osgViewer::CompositeViewer*>(appsys->GetViewer());
      if(compview == NULL)
      {
         LOG_ERROR("Cannot open window, use CompositeViewer class!");
         return;
      }

      osgViewer::View* view = GetViewByName(name);
      if(!view) 
      {
         LOG_ERROR("Cannot close view, not found: " + name);
         return;
      }
      compview->removeView(view);
   }


   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 OSGWindowManager::GetPickRay(const std::string& name, float x, float y)
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

      osg::Vec2 windowCoord((w/2) * (x + 1), (h/2) * (y + 1));

      // calculate pick ray
      osg::Vec3 start = osg::Vec3(windowCoord, 0);
      osg::Vec3 end(windowCoord, 1);

      osg::Matrix matrix;
      matrix.preMult(cam->getViewport()->computeWindowMatrix());
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

#ifdef BUILD_WITH_DELTA3D 

   ///////////////////////////////////////////////////////////////////////////////
   D3DWindowManager::D3DWindowManager(EntityManager& em)
      : WindowManager(em)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void D3DWindowManager::OpenWindow(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {
      OpenWindowInternal(name, layername, traits);
      WindowCreatedMessage msg;
      msg.SetName(name);
      mEntityManager->EmitMessage(msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void D3DWindowManager::OpenWindowInternal(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {
      LayerAttachPointSystem* lsys;
      mEntityManager->GetEntitySystem(LayerAttachPointComponent::TYPE, lsys);

      dtEntity::LayerAttachPointComponent* target;
      if(!lsys->GetByName(layername, target))
      {
         LOG_ERROR("Layer attach point not found, cannot open window!");
         return;
      }

      dtCore::DeltaWin::DeltaWinTraits t;
      t.name = name;
      t.x = traits.x;
      t.y = traits.y;
      t.width = traits.width;
      t.height = traits.height;
      t.showCursor = traits.useCursor;
      //t.fullScreen = traits.fullScreen;
      t.windowDecoration = traits.windowDecoration;
      t.supportResize = traits.supportsResize;
      t.inheritedWindowData = traits.inheritedWindowData;
      t.contextToShare = traits.sharedContext;
      t.hostName = traits.hostName;
      t.displayNum = traits.displayNum;
      t.screenNum = traits.screenNum;
      //t.realizeUponCreate = traits.realizeUponCreate;
      t.vsync = traits.vsync;

      dtCore::DeltaWin* window = new dtCore::DeltaWin(t);

      dtCore::View* view = new dtCore::View();
      view->SetName(name);
      view->GetOsgViewerView()->setName(name);
      dtCore::Scene* scene = new dtCore::Scene();
      scene->SetSceneNode(target->GetGroup());
      view->SetScene(scene);
      view->SetName(name);
      
      ApplicationSystem* appSys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appSys);
      appSys->GetApplication()->AddView(*view);
      
      dtCore::Camera* cam = new dtCore::Camera(name);   
      cam->SetWindow(window);
      view->SetCamera(cam);
      cam->GetOSGCamera()->setName(name);
      appSys->AddCameraToSceneGraph(cam->GetOSGCamera());
     
      cam->SetPerspectiveParams(60.0, (float)traits.width / (float) traits.height, 1.0, 40000.0);
      cam->GetOSGCamera()->setViewport(new osg::Viewport(0.0, 0.0, traits.width, traits.height));
      cam->SetAspectRatio((float)traits.width / (float) traits.height);

   }

   ///////////////////////////////////////////////////////////////////////////////
   void D3DWindowManager::CloseWindow(const std::string& name)
   {

      ApplicationSystem* appSys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appSys);
      dtCore::View* v = GetD3DViewByName(name);
      if(v)
      {
         appSys->GetApplication()->RemoveView(*v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::View* D3DWindowManager::GetD3DViewByName(const std::string& name)
   {
      ApplicationSystem* appSys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appSys);
      int num = appSys->GetApplication()->GetNumberOfViews();
      for(int i = 0; i < num; ++i)
      {
         if(name == appSys->GetApplication()->GetView(i)->GetName())
         {
            return appSys->GetApplication()->GetView(i);
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaWin* D3DWindowManager::GetD3DWindowByName(const std::string& name)
   {
      dtCore::View* view = GetD3DViewByName(name);
      if(view) return view->GetCamera()->GetWindow();
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 D3DWindowManager::GetPickRay(const std::string& name, float x, float y)
   {
      osgViewer::GraphicsWindow* window = GetD3DWindowByName(name)->GetOsgViewerGraphicsWindow();
      
      if(!window) 
      {
         LOG_ERROR("Cannot get pickray for window " + name);
         return osg::Vec3(0,1,0);
      }

      osg::Camera* cam = GetD3DViewByName(name)->GetOsgViewerView()->getCamera();
      
      int wx, wy, w, h;
      window->getWindowRectangle(wx, wy, w, h);

      osg::Vec2 windowCoord((w/2) * (x + 1), (h/2) * (y + 1));

      // calculate pick ray
      osg::Vec3 start = osg::Vec3(windowCoord, 0);
      osg::Vec3 end(windowCoord, 1);

      osg::Matrix matrix;
      matrix.preMult(cam->getViewport()->computeWindowMatrix());
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
#endif
}
