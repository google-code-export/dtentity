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

#include <dtEntity/d3dwindowmanager.h>

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

#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/view.h>




namespace dtEntity
{   


   ///////////////////////////////////////////////////////////////////////////////
   D3DWindowManager::D3DWindowManager(EntityManager& em, dtABC::Application& app)
      : WindowManager(em)
      , mApplication(&app)
   {
      ApplicationSystem* appsys;
      em.GetEntitySystem(ApplicationSystem::TYPE, appsys);
      appsys->SetViewer(mApplication->GetCompositeViewer());
      mApplication->GetCamera()->RemoveSender(&dtCore::System::GetInstance());

      mTimeChangedFunctor = dtEntity::MessageFunctor(this, &D3DWindowManager::OnTimeChange);
      em.RegisterForMessages(dtEntity::TimeChangedMessage::TYPE, mTimeChangedFunctor, "EphemerisSystem::OnTimeChange");

      app.GetScene()->GetSceneNode()->addEventCallback(&GetInputHandler());
   }

   ///////////////////////////////////////////////////////////////////////////////
   D3DWindowManager::~D3DWindowManager()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void D3DWindowManager::OnTimeChange(const dtEntity::Message& m)
   {
      const TimeChangedMessage& msg = static_cast<const TimeChangedMessage&>(m);
      dtCore::System::GetInstance().SetSimulationClockTime(msg.GetSimulationClockTime());
      dtCore::System::GetInstance().SetSimulationTime(msg.GetSimulationTime());
      dtCore::System::GetInstance().SetTimeScale(msg.GetTimeScale());
   }

   ///////////////////////////////////////////////////////////////////////////////
   unsigned int D3DWindowManager::OpenWindow(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits)
   {

      OpenWindowInternal(name, layername, traits);
      osgViewer::GraphicsWindow* gw = GetWindowByName(name);
      if(gw)
      {
         gw->realize();
      }
      WindowCreatedMessage msg;
      msg.SetName(name);
      msg.SetContextId(gw->getState()->getContextID());
      mEntityManager->EmitMessage(msg);
      return gw->getState()->getContextID();
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
      //t.realizeUponCreate = true;
      t.vsync = traits.vsync;

      dtCore::DeltaWin* window = new dtCore::DeltaWin(t);

      dtCore::View* view = new dtCore::View();
      view->SetName(name);
      view->GetOsgViewerView()->setName(name);
      dtCore::Scene* scene = new dtCore::Scene();
      scene->SetSceneNode(target->GetGroup());
      view->SetScene(scene);
      view->SetName(name);
      
      GetApplication()->AddView(*view);
      
      dtCore::Camera* cam = new dtCore::Camera(name);   
      cam->SetWindow(window);
      view->SetCamera(cam);
      cam->GetOSGCamera()->setName(name);
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void D3DWindowManager::CloseWindow(const std::string& name)
   {

      dtCore::View* v = GetD3DViewByName(name);
      if(v)
      {
         GetApplication()->RemoveView(*v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::View* D3DWindowManager::GetD3DViewByName(const std::string& name)
   {

      int num = GetApplication()->GetNumberOfViews();
      for(int i = 0; i < num; ++i)
      {
         if(name == GetApplication()->GetView(i)->GetName())
         {
            return GetApplication()->GetView(i);
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
}
