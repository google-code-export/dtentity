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
   void SCRToString(const FunctionCallbackInfo<Value>& info)
   {
      info.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "<Screen>"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetLockCursor(Local<String> propname, Local<Value> value, const PropertyCallbackInfo<void>& info)
   {
      Isolate* isolate = v8::Isolate::GetCurrent();
      HandleScope handle_scope(isolate);
      Handle<Context> context = info.Holder()->CreationContext();
      Handle<Value> ih = context->Global()->Get(String::NewFromUtf8(isolate, "Input"));
      dtEntity::InputInterface* input = UnwrapInputInterface(ih);
      input->SetLockCursor(value->BooleanValue());

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetLockCursor(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      Isolate* isolate = v8::Isolate::GetCurrent();
      HandleScope handle_scope(isolate);
      Handle<Context> context = info.Holder()->CreationContext();
      Handle<Value> ih = context->Global()->Get(String::NewFromUtf8(isolate, "Input"));
      dtEntity::InputInterface* input = UnwrapInputInterface(ih);
      info.GetReturnValue().Set(Boolean::New(isolate, input->GetLockCursor()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetShowCursor(const FunctionCallbackInfo<Value>& args)
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
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetWidth(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      int w, h, x, y;
      dtEntity::GetWindowInterface()->GetWindowGeometry(0, x, y, w, h);
      info.GetReturnValue().Set(Integer::New(Isolate::GetCurrent(), w));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetHeight(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      int w, h, x, y;
      dtEntity::GetWindowInterface()->GetWindowGeometry(0, x, y, w, h);
      info.GetReturnValue().Set( Integer::New(Isolate::GetCurrent(), h));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetFullScreen(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      info.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), dtEntity::GetWindowInterface()->GetFullscreen(0)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetFullScreen(Local<String> propname, Local<Value> value, const PropertyCallbackInfo<void>& info)
   {
      dtEntity::GetWindowInterface()->SetFullscreen(0, value->BooleanValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetPickRay(const FunctionCallbackInfo<Value>& info)
   {
      if(info.Length() < 3)
      {
         ThrowError("getPickRay expects three arguments!");
         return;
      }
      if(!info[0]->IsInt32())
      {
         ThrowError("Usage: getPickRay(contextid, x, y[, bool usePixels])");
         return;
      }

      unsigned int contextid = info[0]->Uint32Value();
      float x = (float)info[1]->NumberValue();
      float y = (float)info[2]->NumberValue();
      bool usePixels = true;
      if(info.Length() > 3)
      {
         usePixels = info[4]->BooleanValue();
      }

      dtEntity::Vec3f pr = dtEntity::GetWindowInterface()->GetPickRay(contextid, x, y, usePixels);
      info.GetReturnValue().Set(WrapVec3(pr));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRPickEntity(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() < 2)
      {
		  ThrowError("usage: pickEntity(x, y, [nodemask, contextid])");
        return;
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
      args.GetReturnValue().Set( Uint32::New(Isolate::GetCurrent(), eid));      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRIntersect(const FunctionCallbackInfo<Value>& args)
   {

      if(args.Length() < 2 || !IsVec3(args[0]) || ! IsVec3(args[1]))
      {
         ThrowError("usage: intersect(float[3] from, float[3] to, [int32 nodemask])");
         return;
      }

      unsigned int nodemask = dtEntity::NodeMasks::PICKABLE | dtEntity::NodeMasks::TERRAIN;
      if(args.Length() >= 3)
      {
         nodemask = args[2]->Uint32Value();
      }
      
      
      dtEntity::Vec3d from = UnwrapVec3(args[0]);
      dtEntity::Vec3d to = UnwrapVec3(args[1]);

      dtEntity::WindowInterface::Intersections isects;
      bool found = dtEntity::GetWindowInterface()->GetIntersections(from, to, isects, nodemask);

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Array> ret = Array::New(isolate);

      if(!found)
      {
         args.GetReturnValue().Set(ret);
         return;
      }

      Handle<String> entityid = String::NewFromUtf8(isolate, "EntityId");
      Handle<String> normal = String::NewFromUtf8(isolate, "Normal");
      Handle<String> position = String::NewFromUtf8(isolate, "Position");

      unsigned int count = 0;
      for(dtEntity::WindowInterface::Intersections::const_iterator i = isects.begin(); i != isects.end(); ++i)
      {
         dtEntity::WindowInterface::Intersection isect = *i;
         Handle<Object> obj = Object::New(isolate);
         obj->Set(entityid, Uint32::New(isolate, isect.mEntityId));
         obj->Set(normal, WrapVec3(isect.mNormal));
         obj->Set(position, WrapVec3(isect.mPosition));
         ret->Set(count++, obj); 
      }

      args.GetReturnValue().Set(ret);
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   void SCRConvertWorldToScreenCoords(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::Vec3d screenXYZ(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
      dtEntity::Vec3d c = dtEntity::GetWindowInterface()->ConvertWorldToScreenCoords(0, screenXYZ);
      args.GetReturnValue().Set(WrapVec3(c));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCROpenWindow(const FunctionCallbackInfo<Value>& args)
   {

      osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

      traits->windowDecoration = true;
      traits->doubleBuffer = true;
      traits->sharedContext = 0;

      Isolate* isolate = Isolate::GetCurrent();

      if(args.Length() > 1)
      {
         Handle<Object> traitsin = Handle<Object>::Cast(args[1]);
         if(traitsin->Has(String::NewFromUtf8(isolate, "x"))) traits->x = traitsin->Get(String::NewFromUtf8(isolate, "x"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(isolate, "y"))) traits->y = traitsin->Get(String::NewFromUtf8(isolate, "y"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(isolate, "width"))) traits->width = traitsin->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"))) traits->height = traitsin->Get(String::NewFromUtf8(isolate, "height"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(isolate, "windowDecoration"))) traits->windowDecoration = traitsin->Get(String::NewFromUtf8(isolate, "windowDecoration"))->BooleanValue();
         if(traitsin->Has(String::NewFromUtf8(isolate, "hostName"))) traits->hostName = ToStdString(traitsin->Get(String::NewFromUtf8(isolate, "hostName")));
         if(traitsin->Has(String::NewFromUtf8(isolate, "displayNum"))) traits->displayNum = traitsin->Get(String::NewFromUtf8(isolate, "displayNum"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(isolate, "screenNum"))) traits->screenNum = traitsin->Get(String::NewFromUtf8(isolate, "screenNum"))->Int32Value();
         if(traitsin->Has(String::NewFromUtf8(isolate, "vsync"))) traits->vsync = traitsin->Get(String::NewFromUtf8(isolate, "vsync"))->BooleanValue();
      }

      
      unsigned int contextid;
      // TODO how to pass traits???
      //dtEntityOSG::OSGWindowInterface* wface = static_cast<dtEntityOSG::OSGWindowInterface*>(dtEntity::GetWindowInterface());
      //wface->SetTraits(traits);
      bool success = dtEntity::GetWindowInterface()->OpenWindow(ToStdString(args[0]), contextid);
      assert(success);
      args.GetReturnValue().Set(Uint32::New(isolate, contextid));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRCloseWindow(const FunctionCallbackInfo<Value>& args)
   {     
      if(args[0]->IsString())
      {
         ThrowError("closeWindow expects context id as first argument!");
         return;
      }

      dtEntity::GetWindowInterface()->CloseWindow(args[0]->Uint32Value());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRGetWindowGeometry(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() == 0)
      {
         ThrowError("Usage: getWindowGeometry(contextId)");
         return;
      }

      unsigned int contextId = args[0]->Uint32Value();
      int x, y, w, h;
      
      dtEntity::GetWindowInterface()->GetWindowGeometry(contextId, x, y, w, h);

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Array> arr = Array::New(isolate);
      arr->Set(Integer::New(isolate, 0), Integer::New(isolate, x));
      arr->Set(Integer::New(isolate, 1), Integer::New(isolate, y));
      arr->Set(Integer::New(isolate, 2), Integer::New(isolate, w));
      arr->Set(Integer::New(isolate, 3), Integer::New(isolate, h));

      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetWindowGeometry(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 2)
      {
         ThrowError("Usage: setWindowGeometry(contextId, array[x,y,w,h])");
         return;
      }

      unsigned int contextId = args[0]->Uint32Value();
      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Handle<Array>::Cast(args[1]);
      int x = arr->Get(0)->Int32Value();
      int y = arr->Get(1)->Int32Value();
      int w = arr->Get(2)->Int32Value();
      int h = arr->Get(3)->Int32Value();

      dtEntity::GetWindowInterface()->SetWindowGeometry(contextId, x, y, w, h);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRSetContinuousRedraw(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 2)
      {
         ThrowError("Usage: setContinuousRedraw(contextId, bool)");
         return;
      }

      unsigned int contextId = args[0]->Uint32Value();
      bool v = args[1]->BooleanValue();

      dtEntity::GetWindowInterface()->SetContinuousRedraw(contextId, v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SCRRequestRedraw(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 1)
      {
         ThrowError("Usage: requestRedraw(contextId)");
         return;
      }

      unsigned int contextId = args[0]->Uint32Value();
      dtEntity::GetWindowInterface()->RequestRedraw(contextId);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapScreen(ScriptSystem* ss)
   {
      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope handle_scope(isolate);
      Handle<Context> context = ss->GetGlobalContext();
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = FunctionTemplate::New(isolate);
      templt->SetClassName(String::NewFromUtf8(isolate, "Screen"));

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, SCRToString));
      proto->Set(String::NewFromUtf8(isolate, "getPickRay"), FunctionTemplate::New(isolate, SCRGetPickRay));
      proto->Set(String::NewFromUtf8(isolate, "intersect"), FunctionTemplate::New(isolate, SCRIntersect));
      proto->Set(String::NewFromUtf8(isolate, "pickEntity"), FunctionTemplate::New(isolate, SCRPickEntity));
      proto->Set(String::NewFromUtf8(isolate, "convertWorldToScreenCoords"), FunctionTemplate::New(isolate, SCRConvertWorldToScreenCoords));
      proto->Set(String::NewFromUtf8(isolate, "openWindow"), FunctionTemplate::New(isolate, SCROpenWindow));
      proto->Set(String::NewFromUtf8(isolate, "closeWindow"), FunctionTemplate::New(isolate, SCRCloseWindow));
      proto->Set(String::NewFromUtf8(isolate, "getWindowGeometry"), FunctionTemplate::New(isolate, SCRGetWindowGeometry));
      proto->Set(String::NewFromUtf8(isolate, "setWindowGeometry"), FunctionTemplate::New(isolate, SCRSetWindowGeometry));
      proto->Set(String::NewFromUtf8(isolate, "setShowCursor"), FunctionTemplate::New(isolate, SCRSetShowCursor));
      proto->Set(String::NewFromUtf8(isolate, "setContinuousRedraw"), FunctionTemplate::New(isolate, SCRSetContinuousRedraw));
      proto->Set(String::NewFromUtf8(isolate, "requestRedraw"), FunctionTemplate::New(isolate, SCRRequestRedraw));

      Local<Object> instance = templt->GetFunction()->NewInstance();

      instance->SetAccessor(String::NewFromUtf8(isolate, "lockCursor"), SCRGetLockCursor, SCRSetLockCursor);
      instance->SetAccessor(String::NewFromUtf8(isolate, "width"), SCRGetWidth);
      instance->SetAccessor(String::NewFromUtf8(isolate, "height"), SCRGetHeight);
      instance->SetAccessor(String::NewFromUtf8(isolate, "fullScreen"), SCRGetFullScreen, SCRSetFullScreen);

      return handle_scope.Escape(instance);
   }


}
