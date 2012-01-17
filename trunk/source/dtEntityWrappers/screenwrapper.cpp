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

#include <dtEntityWrappers/screenwrapper.h>

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/inputhandler.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntity/basemessages.h>
#include <iostream>
#include <v8.h>

using namespace v8;

namespace dtEntityWrappers
{

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRToString(const Arguments& args)
   {
      return String::New("<Screen>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetLockCursor(Local<String> propname, Local<Value> value, const AccessorInfo& info)
   {     
      HandleScope handle_scope;
      Handle<Context> context = info.Holder()->CreationContext();
      Handle<Value> ih = context->Global()->Get(String::New("Input"));
      dtEntity::InputHandler* input = UnwrapInputHandler(ih);
      input->SetLockCursor(value->BooleanValue());

   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetLockCursor(Local<String> propname, const AccessorInfo& info)
   {
      HandleScope handle_scope;
      Handle<Context> context = info.Holder()->CreationContext();
      Handle<Value> ih = context->Global()->Get(String::New("Input"));
      dtEntity::InputHandler* input = UnwrapInputHandler(ih);
      return Boolean::New(input->GetLockCursor());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetShowCursor(Local<String> propname, Local<Value> value, const AccessorInfo& info)
   {
      osgViewer::GraphicsWindow* window = UnwrapScreenWindow(info.This());
      bool showCursor = value->BooleanValue();
      if(showCursor)
      {
         window->useCursor(true);
         window->setCursor(osgViewer::GraphicsWindow::RightArrowCursor);
      }
      else
      {
         window->setCursor(osgViewer::GraphicsWindow::NoCursor);
         window->useCursor(false);
      }
      
      info.This()->Set(String::New("__USE_CURSOR__"), Boolean::New(showCursor));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetShowCursor(Local<String> propname, const AccessorInfo& info)
   {
      if(!info.This()->Has(String::New("__USE_CURSOR__")))
      {
         return Boolean::New(true);
      }
      return info.This()->Get(String::New("__USE_CURSOR__"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetWidth(Local<String> propname, const AccessorInfo& info)
   {
     osgViewer::GraphicsWindow* window = UnwrapScreenWindow(info.This());
     int x, y, w, h;
     window->getWindowRectangle(x, y, w, h);
     return Integer::New(w);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetHeight(Local<String> propname, const AccessorInfo& info)
   {
     osgViewer::GraphicsWindow* window = UnwrapScreenWindow(info.This());
     int x, y, w, h;
     window->getWindowRectangle(x, y, w, h);
     return Integer::New(h);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetFullScreen(Local<String> propname, const AccessorInfo& info)
   {
      if(!info.This()->Has(String::New("__FULLSCREEN__")))
      {
         return Boolean::New(false);
      }
      return info.This()->Get(String::New("__FULLSCREEN__"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetFullScreen(Local<String> propname, Local<Value> value, const AccessorInfo& info)
   {
      if(info.This()->Has(String::New("__FULLSCREEN__")) &&
            info.This()->Get(String::New("__FULLSCREEN__"))->BooleanValue() == value->BooleanValue())
      {
         return;
      }

      info.This()->Set(String::New("__FULLSCREEN__"), Boolean::New(value->BooleanValue()));

      osgViewer::GraphicsWindow* window = UnwrapScreenWindow(info.This());

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

      if (!value->BooleanValue())
      {
        // int rx = mLastWindowedWidth;
        // int ry = mLastWindowedHeight;
         int lastw = info.This()->Get(String::New("__LAST_WINDOW_WIDTH"))->Int32Value();
         int lasth = info.This()->Get(String::New("__LAST_WINDOW_HEIGHT"))->Int32Value();


         window->setWindowDecoration(true);
         window->setWindowRectangle((screenWidth - lastw) / 2, (screenHeight - lasth) / 2, lastw, lasth);
      }
      else
      {  info.This()->Set(String::New("__LAST_WINDOW_WIDTH"), Integer::New(w));
         info.This()->Set(String::New("__LAST_WINDOW_HEIGHT"), Integer::New(h));
         window->setWindowDecoration(false);
         window->setWindowRectangle(0, 0, screenWidth, screenHeight);
      }

      window->grabFocusIfPointerInWindow();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetPickRay(const Arguments& args)
   {
      dtEntity::EntityManager* em = GetEntityManager(args.Holder()->CreationContext());

      dtEntity::ApplicationSystem* appsys;
      em->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      osg::Vec3 pr = appsys->GetWindowManager()->GetPickRay("defaultView", args[0]->NumberValue(), args[1]->NumberValue());
      return WrapVec3(pr);
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRPickEntity(const Arguments& args)
   {
      if(args.Length() < 2)
      {
		  return ThrowError("usage: pickEntity(x, y, [nodemask, contextid])");
      }

      unsigned int nodemask = dtEntity::NodeMasks::PICKABLE;
      if(args.Length() > 2)
      {
         nodemask = args[2]->Uint32Value();
      }
      unsigned int contextid = 0;
      if(args.Length() > 3)
      {
         contextid = args[3]->Uint32Value();
      }

      double x = args[0]->NumberValue();
      double y = args[1]->NumberValue();
	 
      HandleScope scope;

      dtEntity::EntityManager* em = GetEntityManager(args.Holder()->CreationContext());

	   dtEntity::CameraSystem* camsys;
	   em->GetEntitySystem(dtEntity::CameraComponent::TYPE, camsys);
	   dtEntity::EntityId camid = camsys->GetCameraEntityByContextId(contextid);
	   dtEntity::CameraComponent* camcomp;
	   bool success = em->GetComponent(camid, camcomp);
	   if(!success)
	   {
	 	  return Null();
	   }
	   osg::Vec3d from = camcomp->GetPosition();

	   dtEntity::ApplicationSystem* appsys;
      em->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      
      osg::Vec3 pr = appsys->GetWindowManager()->GetPickRay("defaultView", x, y, true);
      
      osg::Vec3d to = from + pr * 10000;

      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi;
      lsi = new osgUtil::LineSegmentIntersector(from, to);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setUseKdTreeWhenAvailable(true);
      iv.setTraversalMask(nodemask); 

      dtEntity::LayerAttachPointSystem* laps;
      em->GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, laps);
      dtEntity::LayerAttachPointComponent* sceneLayer = laps->GetDefaultLayer();
      sceneLayer->GetGroup()->accept(iv);

      if(!lsi->containsIntersections())
      {
         return Null();
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
            const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL)
            {			     
               return Uint32::New(entity->GetId());			      
            }
         }
      }

      return Null();
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRIntersect(const Arguments& args)
   {

      if(args.Length() < 2 || !IsVec3(args[0]) || ! IsVec3(args[1]))
      {
         return ThrowError("usage: intersect(float[3] from, float[3] to, [int32 nodemask])");
      }

      HandleScope scope;

      dtEntity::EntityManager* entityManager = GetEntityManager(args.Holder()->CreationContext());

      osg::Vec3d from = UnwrapVec3(args[0]);
      osg::Vec3d to = UnwrapVec3(args[1]);

      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi;
      lsi = new osgUtil::LineSegmentIntersector(from, to);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setUseKdTreeWhenAvailable(true);

      if(args.Length() >= 3)
      {
         iv.setTraversalMask(args[2]->Uint32Value());
      }
      else
      {
         iv.setTraversalMask(dtEntity::NodeMasks::PICKABLE | dtEntity::NodeMasks::TERRAIN);
      }
      dtEntity::LayerAttachPointSystem* laps;
      entityManager->GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, laps);
      dtEntity::LayerAttachPointComponent* sceneLayer = laps->GetDefaultLayer();
      sceneLayer->GetGroup()->accept(iv);

      Handle<Array> ret = Array::New();

      if(!lsi->containsIntersections())
      {
         return scope.Close(ret);
      }

      Handle<String> entityid = String::New("EntityId");
      Handle<String> normal = String::New("Normal");
      Handle<String> position = String::New("Position");

      osgUtil::LineSegmentIntersector::Intersections::iterator i;
      unsigned int count = 0;
      for(i = lsi->getIntersections().begin(); i != lsi->getIntersections().end(); ++i)
      {
         osgUtil::LineSegmentIntersector::Intersection isect = *i;
         for(osg::NodePath::const_iterator j = isect.nodePath.begin(); j != isect.nodePath.end(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();
            if(referenced == NULL) continue;
            const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL)
            {
				   Handle<Object> obj = Object::New();
				   obj->Set(entityid, Uint32::New(entity->GetId()));
				   obj->Set(normal, WrapVec3(isect.getWorldIntersectNormal()));
				   obj->Set(position, WrapVec3(isect.getWorldIntersectPoint()));
				   ret->Set(count++, obj);			   
            }
         }
      }

      return scope.Close(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRConvertWorldToScreenCoords(const Arguments& args)
   {
      osgViewer::View* view = UnwrapScreenView(args.This());
      osg::Camera* cam = view->getCamera();

      osg::Vec4d screenXYZ(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), 1);
      screenXYZ = cam->getViewMatrix().preMult(screenXYZ);
      screenXYZ = cam->getProjectionMatrix().preMult(screenXYZ);
      double w = screenXYZ[3];
      osg::Vec3d ret(screenXYZ[0] / w, screenXYZ[1] / w, screenXYZ[2] / w);
      return WrapVec3(ret);

   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCROpenWindow(const Arguments& args)
   {
     // std::string name = ToStdString(args[0]);
      dtEntity::StringId layername = dtEntity::LayerAttachPointSystem::DefaultLayerId;
      if(args.Length() > 1)
      {
         layername = dtEntity::SIDHash(ToStdString(args[1]));
      }

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

      traits->windowDecoration = true;
      traits->doubleBuffer = true;
      traits->sharedContext = 0;

      if(args.Length() > 2)
      {
         Handle<Object> traitsin = Handle<Object>::Cast(args[2]);
         if(traitsin->Has(String::New("x"))) traits->x = traitsin->Get(String::New("x"))->Int32Value();
         if(traitsin->Has(String::New("y"))) traits->y = traitsin->Get(String::New("y"))->Int32Value();
         if(traitsin->Has(String::New("width"))) traits->width = traitsin->Get(String::New("width"))->Int32Value();
         if(traitsin->Has(String::New("height"))) traits->height = traitsin->Get(String::New("height"))->Int32Value();
         if(traitsin->Has(String::New("windowDecoration"))) traits->windowDecoration = traitsin->Get(String::New("windowDecoration"))->BooleanValue();
         if(traitsin->Has(String::New("hostName"))) traits->hostName = ToStdString(traitsin->Get(String::New("hostName")));
         if(traitsin->Has(String::New("displayNum"))) traits->displayNum = traitsin->Get(String::New("displayNum"))->Int32Value();
         if(traitsin->Has(String::New("screenNum"))) traits->screenNum = traitsin->Get(String::New("screenNum"))->Int32Value();
         if(traitsin->Has(String::New("vsync"))) traits->vsync = traitsin->Get(String::New("vsync"))->BooleanValue();
      }

      dtEntity::EntityManager* entityManager = GetEntityManager(args.Holder()->CreationContext());
      dtEntity::ApplicationSystem* appsys;
      entityManager->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);

      unsigned int contextid = appsys->GetWindowManager()->OpenWindow(ToStdString(args[0]), layername, *traits);
      return Uint32::New(contextid);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRCloseWindow(const Arguments& args)
   {     
      dtEntity::EntityManager* entityManager = GetEntityManager(args.Holder()->CreationContext());
      dtEntity::ApplicationSystem* appsys;
      entityManager->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      appsys->GetWindowManager()->CloseWindow(ToStdString(args[0]));
      return Undefined();
   }


   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapScreen(ScriptSystem* ss, osgViewer::View* v, osgViewer::GraphicsWindow* w)
   {
      HandleScope handle_scope;
      Handle<Context> context = ss->GetGlobalContext();
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      templt->SetClassName(String::New("Screen"));
      templt->InstanceTemplate()->SetInternalFieldCount(2);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("toString", FunctionTemplate::New(SCRToString));
      proto->Set("getPickRay", FunctionTemplate::New(SCRGetPickRay));
      proto->Set("intersect", FunctionTemplate::New(SCRIntersect));
      proto->Set("pickEntity", FunctionTemplate::New(SCRPickEntity));
      proto->Set("convertWorldToScreenCoords", FunctionTemplate::New(SCRConvertWorldToScreenCoords));
      proto->Set("openWindow", FunctionTemplate::New(SCROpenWindow));
      proto->Set("closeWindow", FunctionTemplate::New(SCRCloseWindow));

      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      instance->SetInternalField(1, External::New(w));

      instance->SetAccessor(String::New("lockCursor"), SCRGetLockCursor, SCRSetLockCursor);
      instance->SetAccessor(String::New("showCursor"), SCRGetShowCursor, SCRSetShowCursor);
      instance->SetAccessor(String::New("width"), SCRGetWidth);
      instance->SetAccessor(String::New("height"), SCRGetHeight);
      instance->SetAccessor(String::New("fullScreen"), SCRGetFullScreen, SCRSetFullScreen);

      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgViewer::View* UnwrapScreenView(Handle<Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      osgViewer::View* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* UnwrapScreenWindow(Handle<Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      osgViewer::GraphicsWindow* v;
      GetInternal(obj, 1, v);
      return v;
   }

}
