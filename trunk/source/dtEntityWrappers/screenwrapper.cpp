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

#include <dtEntity/core.h>
#include <dtEntity/entity.h>
#include <dtEntity/inputinterface.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/property.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/windowinterface.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <iostream>
#include <v8.h>
#include <osg/DisplaySettings>
#include <osg/GraphicsContext>

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
      dtEntity::InputInterface* input = UnwrapInputInterface(ih);
      input->SetLockCursor(value->BooleanValue());

   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetLockCursor(Local<String> propname, const AccessorInfo& info)
   {
      HandleScope handle_scope;
      Handle<Context> context = info.Holder()->CreationContext();
      Handle<Value> ih = context->Global()->Get(String::New("Input"));
      dtEntity::InputInterface* input = UnwrapInputInterface(ih);
      return Boolean::New(input->GetLockCursor());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRSetShowCursor(const Arguments& args)
   {
      unsigned int cid = 0;
      bool show = true;
      if(args.Length() > 0)
      {
         cid = args[0]->Uint32Value();
      }

      if(args.Length() > 1)
      {
         show = args[1]->BooleanValue();
      }
      dtEntity::GetWindowInterface()->SetShowCursor(cid, show);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetWidth(Local<String> propname, const AccessorInfo& info)
   {
      int w, h, x, y;
      dtEntity::GetWindowInterface()->GetWindowGeometry(0, x, y, w, h);
      return Integer::New(w);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetHeight(Local<String> propname, const AccessorInfo& info)
   {
      int w, h, x, y;
      dtEntity::GetWindowInterface()->GetWindowGeometry(0, x, y, w, h);
      return Integer::New(h);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetFullScreen(Local<String> propname, const AccessorInfo& info)
   {
      return Boolean::New(dtEntity::GetWindowInterface()->GetFullscreen(0));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetFullScreen(Local<String> propname, Local<Value> value, const AccessorInfo& info)
   {
      dtEntity::GetWindowInterface()->SetFullscreen(0, value->BooleanValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetPickRay(const Arguments& args)
   {
      if(args.Length() < 3)
      {
         return ThrowError("getPickRay expects three arguments!");
      }
      if(!args[0]->IsInt32())
      {
         return ThrowError("Usage: getPickRay(contextid, x, y[, bool usePixels])");
      }
      unsigned int contextid = args[0]->Uint32Value();
      float x = (float)args[1]->NumberValue();
      float y = (float)args[2]->NumberValue();
      bool usePixels = true;
      if(args.Length() > 3)
      {
         usePixels = args[4]->BooleanValue();
      }

      dtEntity::Vec3f pr = dtEntity::GetWindowInterface()->GetPickRay(contextid, x, y, usePixels);
      return WrapVec3(pr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRPickEntity(const Arguments& args)
   {
      if(args.Length() < 2)
      {
		  return ThrowError("usage: pickEntity(x, y, [nodemask, contextid])");
      }

      unsigned int nodemask = dtEntity::NodeMasks::PICKABLE | dtEntity::NodeMasks::VISIBLE;
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
	 
      dtEntity::EntityId eid = dtEntity::GetWindowInterface()->PickEntity(x, y, nodemask, contextid);
      return Uint32::New(eid);      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRIntersect(const Arguments& args)
   {

      if(args.Length() < 2 || !IsVec3(args[0]) || ! IsVec3(args[1]))
      {
         return ThrowError("usage: intersect(float[3] from, float[3] to, [int32 nodemask])");
      }

      unsigned int nodemask = dtEntity::NodeMasks::PICKABLE | dtEntity::NodeMasks::TERRAIN;
      if(args.Length() >= 3)
      {
         nodemask = args[2]->Uint32Value();
      }
      
      
      dtEntity::Vec3d from = UnwrapVec3(args[0]);
      dtEntity::Vec3d to = UnwrapVec3(args[1]);

      dtEntity::SystemInterface::Intersections isects;
      bool found = dtEntity::GetSystemInterface()->GetIntersections(from, to, isects, nodemask);

      HandleScope scope;
      Handle<Array> ret = Array::New();

      if(!found)
      {
         return scope.Close(ret);
      }

      Handle<String> entityid = String::New("EntityId");
      Handle<String> normal = String::New("Normal");
      Handle<String> position = String::New("Position");

      unsigned int count = 0;
      for(dtEntity::SystemInterface::Intersections::const_iterator i = isects.begin(); i != isects.end(); ++i)
      {
         dtEntity::SystemInterface::Intersection isect = *i;
         Handle<Object> obj = Object::New();
         obj->Set(entityid, Uint32::New(isect.mEntityId));
         obj->Set(normal, WrapVec3(isect.mNormal));
         obj->Set(position, WrapVec3(isect.mPosition));
         ret->Set(count++, obj); 
      }

      return scope.Close(ret);
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRConvertWorldToScreenCoords(const Arguments& args)
   {
      dtEntity::Vec3d screenXYZ(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
      dtEntity::Vec3d c = dtEntity::GetWindowInterface()->ConvertWorldToScreenCoords(0, screenXYZ);
      return WrapVec3(c);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCROpenWindow(const Arguments& args)
   {

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

      traits->windowDecoration = true;
      traits->doubleBuffer = true;
      traits->sharedContext = 0;

      if(args.Length() > 1)
      {
         Handle<Object> traitsin = Handle<Object>::Cast(args[1]);
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

      
      unsigned int contextid;
      // TODO how to pass traits???
      //dtEntityOSG::OSGWindowInterface* wface = static_cast<dtEntityOSG::OSGWindowInterface*>(dtEntity::GetWindowInterface());
      //wface->SetTraits(traits);
      bool success = dtEntity::GetWindowInterface()->OpenWindow(ToStdString(args[0]), contextid);
      assert(success);
      return Uint32::New(contextid);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRCloseWindow(const Arguments& args)
   {     
      if(args[0]->IsString())
      {
         return ThrowError("closeWindow expects context id as first argument!");
      }
      dtEntity::GetWindowInterface()->CloseWindow(args[0]->Uint32Value());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRGetWindowGeometry(const Arguments& args)
   {
      if(args.Length() == 0)
      {
         return ThrowError("Usage: getWindowGeometry(contextId)");
      }
      unsigned int contextId = args[0]->Uint32Value();
      int x, y, w, h;
      
      dtEntity::GetWindowInterface()->GetWindowGeometry(contextId, x, y, w, h);

      HandleScope scope;
      Handle<Array> arr = Array::New();
      arr->Set(Integer::New(0), Integer::New(x));
      arr->Set(Integer::New(1), Integer::New(y));
      arr->Set(Integer::New(2), Integer::New(w));
      arr->Set(Integer::New(3), Integer::New(h));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRSetWindowGeometry(const Arguments& args)
   {
      if(args.Length() != 2)
      {
         return ThrowError("Usage: setWindowGeometry(contextId, array[x,y,w,h])");
      }
      unsigned int contextId = args[0]->Uint32Value();
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(args[1]);
      int x = arr->Get(0)->Int32Value();
      int y = arr->Get(1)->Int32Value();
      int w = arr->Get(2)->Int32Value();
      int h = arr->Get(3)->Int32Value();

      dtEntity::GetWindowInterface()->SetWindowGeometry(contextId, x, y, w, h);

      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRSetContinuousRedraw(const Arguments& args)
   {
      if(args.Length() != 2)
      {
         return ThrowError("Usage: setContinuousRedraw(contextId, bool)");
      }
      unsigned int contextId = args[0]->Uint32Value();
      bool v = args[1]->BooleanValue();

      dtEntity::GetWindowInterface()->SetContinuousRedraw(contextId, v);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SCRRequestRedraw(const Arguments& args)
   {
      if(args.Length() != 1)
      {
         return ThrowError("Usage: requestRedraw(contextId)");
      }
      unsigned int contextId = args[0]->Uint32Value();

      dtEntity::GetWindowInterface()->RequestRedraw(contextId);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapScreen(ScriptSystem* ss)
   {
      HandleScope handle_scope;
      Handle<Context> context = ss->GetGlobalContext();
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      templt->SetClassName(String::New("Screen"));

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("toString", FunctionTemplate::New(SCRToString));
      proto->Set("getPickRay", FunctionTemplate::New(SCRGetPickRay));
      proto->Set("intersect", FunctionTemplate::New(SCRIntersect));
      proto->Set("pickEntity", FunctionTemplate::New(SCRPickEntity));
      proto->Set("convertWorldToScreenCoords", FunctionTemplate::New(SCRConvertWorldToScreenCoords));
      proto->Set("openWindow", FunctionTemplate::New(SCROpenWindow));
      proto->Set("closeWindow", FunctionTemplate::New(SCRCloseWindow));
      proto->Set("getWindowGeometry", FunctionTemplate::New(SCRGetWindowGeometry));
      proto->Set("setWindowGeometry", FunctionTemplate::New(SCRSetWindowGeometry));
      proto->Set("setShowCursor", FunctionTemplate::New(SCRSetShowCursor));
      proto->Set("setContinuousRedraw", FunctionTemplate::New(SCRSetContinuousRedraw));
      proto->Set("requestRedraw", FunctionTemplate::New(SCRRequestRedraw));

      Local<Object> instance = templt->GetFunction()->NewInstance();

      instance->SetAccessor(String::New("lockCursor"), SCRGetLockCursor, SCRSetLockCursor);
      instance->SetAccessor(String::New("width"), SCRGetWidth);
      instance->SetAccessor(String::New("height"), SCRGetHeight);
      instance->SetAccessor(String::New("fullScreen"), SCRGetFullScreen, SCRSetFullScreen);

      return handle_scope.Close(instance);
   }


}
