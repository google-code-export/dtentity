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

#include <dtEntity/inputinterface.h>
#include <dtEntity/dtentity_config.h>
#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <iostream>
#include <v8.h>

using namespace v8;

namespace dtEntityWrappers
{

   ////////////////////////////////////////////////////////////////////////////////
   class InputCallbackWrapper
      : public dtEntity::InputCallbackInterface
   {
   public:

      InputCallbackWrapper(Handle<Object> obj)
      {
         mObject = Persistent<Object>::New(obj);
         
         if(obj->Has(String::New("keyDown"))) 
         {
            mKeyDownFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("keyDown"))));
         }
         if(obj->Has(String::New("keyUp"))) 
         {
            mKeyUpFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("keyUp"))));
         }
         if(obj->Has(String::New("mouseButtonDown"))) 
         {
            mMouseDownFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("mouseButtonDown"))));
         }
         if(obj->Has(String::New("mouseButtonUp"))) 
         {
            mMouseUpFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("mouseButtonUp"))));
         }
         if(obj->Has(String::New("mouseWheel"))) 
         {
            mMouseWheelFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("mouseWheel"))));
         }
         if(obj->Has(String::New("mouseMove"))) 
         {
            mMouseMoveFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("mouseMove"))));
         }
         if(obj->Has(String::New("mouseEnterLeave")))
         {
            mMouseEnterLeaveFunc = Persistent<Function>::New(Handle<Function>::Cast(obj->Get(String::New("mouseEnterLeave"))));
         }
      }

      virtual bool KeyUp(const std::string& name, bool handled, unsigned int contextId) {
         if(!mKeyUpFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mKeyUpFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { ToJSString(name), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mKeyUpFunc->Call(mObject, 3, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual bool KeyDown(const std::string& name, bool handled, unsigned int contextId)
      {
         if(!mKeyDownFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mKeyDownFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { ToJSString(name), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mKeyDownFunc->Call(mObject, 3, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual bool MouseButtonUp(int button, bool handled, unsigned int contextId)
      {
         if(!mMouseUpFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mMouseUpFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(button), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mMouseUpFunc->Call(mObject, 3, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual bool MouseButtonDown(int button, bool handled, unsigned int contextId)
      {
         if(!mMouseDownFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mMouseDownFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(button), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mMouseDownFunc->Call(mObject, 3, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual bool MouseWheel(int dir, bool handled, unsigned int contextId)
      {
         if(!mMouseWheelFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mMouseWheelFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(dir), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mMouseWheelFunc->Call(mObject, 3, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual bool MouseMove(float x, float y, bool handled, unsigned int contextId)
      {
         if(!mMouseMoveFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mMouseMoveFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[4] = { Number::New(x), Number::New(y), Boolean::New(handled), Uint32::New(contextId) };
            Handle<Value> ret = mMouseMoveFunc->Call(mObject, 4, argv);

            if(ret.IsEmpty()) 
            {
               ReportException(&try_catch);
               return false;
            }
            return ret->BooleanValue();
         }
         return false;
      }

      virtual void MouseEnterLeave(bool focused,unsigned int contextId)
      {
         if(!mMouseEnterLeaveFunc.IsEmpty())
         {
            HandleScope scope;
            Context::Scope context_scope(mMouseEnterLeaveFunc->CreationContext());            
            TryCatch try_catch;
            Handle<Value> argv[2] = { Boolean::New(focused), Uint32::New(contextId) };
            Handle<Value> ret = mMouseEnterLeaveFunc->Call(mObject, 2, argv);

            if(ret.IsEmpty())
            {
               ReportException(&try_catch);
            }
         }
      }

      Persistent<Function> mKeyUpFunc;
      Persistent<Function> mKeyDownFunc;
      Persistent<Function> mMouseUpFunc;
      Persistent<Function> mMouseDownFunc;
      Persistent<Function> mMouseWheelFunc;
      Persistent<Function> mMouseMoveFunc;
      Persistent<Function> mMouseEnterLeaveFunc;
      Persistent<Object> mObject;
   };

   std::vector<InputCallbackWrapper*> s_inputCallbackWrappers;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetKey(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetKey(ToStdString(args[0]), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetKeyUp(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetKeyUp(ToStdString(args[0]), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetKeyDown(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetKeyDown(ToStdString(args[0]), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHAnyKey(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      return Boolean::New(input->AnyKeyDown());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetInputString(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      return ToJSString(input->GetInputString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMouseButton(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());

      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetMouseButton(args[0]->Int32Value(), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMouseButtonUp(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());

      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetMouseButtonUp(args[0]->Int32Value(), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMouseButtonDown(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      return Boolean::New(input->GetMouseButtonDown(args[0]->Int32Value(), contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetAxis(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return Number::New(input->GetAxis(ToStdString(args[0])));
#else
      return Number::New(input->GetAxis((dtEntity::StringId)args[0]->Uint32Value()));
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMouseWheelState(const Arguments& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 0)
      {
         contextId = args[0]->Uint32Value();
      }
      return Integer::New((int)input->GetMouseWheelState(contextId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMultiTouchEnabled(const Arguments& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      return Boolean::New(ih->GetMultiTouchEnabled());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetNumTouches(const Arguments& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      return Integer::New(ih->GetNumTouches());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetTouches(const Arguments& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      HandleScope scope;
      Handle<Array> jstouches = Array::New();

      Handle<String> tapcount = String::New("tapcount");
      Handle<String> id = String::New("id");
      Handle<String> x = String::New("x");
      Handle<String> y = String::New("y");
      Handle<String> phase = String::New("phase");

      const std::vector<dtEntity::TouchPoint>& touches = ih->GetTouches();
      unsigned int count = 0;
      for(std::vector<dtEntity::TouchPoint>::const_iterator i = touches.begin(); i != touches.end(); ++i)
      {
         dtEntity::TouchPoint tp = *i;
         Handle<Object> o = Object::New();
         o->Set(id, Uint32::New(tp.mId));
         o->Set(tapcount, Uint32::New(tp.mTapCount));
         o->Set(x, Number::New(tp.mX));
         o->Set(y, Number::New(tp.mY));
         o->Set(phase, Integer::New(tp.mTouchPhase));
         jstouches->Set(count++, o);
      }

      return scope.Close(jstouches);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHToString(const Arguments& args)
   {
      return String::New("<Input>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHAddInputCallback(const Arguments& args)
   {
      Handle<Object> o = Handle<Object>::Cast(args[0]);
      InputCallbackWrapper* wr = new InputCallbackWrapper(o);

      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      ih->AddInputCallback(wr);
      s_inputCallbackWrappers.push_back(wr);

      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHRemoveInputCallback(const Arguments& args)
   {
      for(std::vector<InputCallbackWrapper*>::iterator i = s_inputCallbackWrappers.begin();
         i != s_inputCallbackWrappers.end(); ++i)
      {
         if((*i)->mObject == args[0])
         {
            dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
            ih->RemoveInputCallback(*i);
            s_inputCallbackWrappers.erase(i);
            return True();
         }
      }
      
      return False();
   }

   ////////////////////////////////////////////////////////////////////////////////
   /*Handle<Value> IHPrintKeys(const Arguments& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      const dtEntity::InputInterface::KeyNames k = ih->GetKeyNames();
      
      dtEntity::InputInterface::KeyNames::const_iterator i;
      for(i = k.begin(); i != k.end(); ++i)
      {
         std::cout << i->first << "\n";         
      }
      return Undefined();
   }*/


   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapInputInterface(Handle<Context> context, dtEntity::InputInterface* v)
   {
      HandleScope handle_scope;
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      templt->SetClassName(String::New("Input"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("getAxis", FunctionTemplate::New(IHGetAxis));
      proto->Set("getKey", FunctionTemplate::New(IHGetKey));
      proto->Set("getKeyDown", FunctionTemplate::New(IHGetKeyDown));
      proto->Set("getKeyUp", FunctionTemplate::New(IHGetKeyUp));
      proto->Set("anyKeyDown", FunctionTemplate::New(IHAnyKey));

      proto->Set("getInputString", FunctionTemplate::New(IHGetInputString));

      proto->Set("getMouseButton", FunctionTemplate::New(IHGetMouseButton));
      proto->Set("getMouseButtonUp", FunctionTemplate::New(IHGetMouseButtonUp));
      proto->Set("getMouseButtonDown", FunctionTemplate::New(IHGetMouseButtonDown));
      proto->Set("getMouseWheelState", FunctionTemplate::New(IHGetMouseWheelState));
      
      proto->Set("getMultiTouchEnabled", FunctionTemplate::New(IHGetMultiTouchEnabled));
      proto->Set("getNumTouches", FunctionTemplate::New(IHGetNumTouches));
      proto->Set("getTouches", FunctionTemplate::New(IHGetTouches));
      //proto->Set("printKeys", FunctionTemplate::New(IHPrintKeys));
      proto->Set("toString", FunctionTemplate::New(IHToString));
      proto->Set("addInputCallback", FunctionTemplate::New(IHAddInputCallback));
      proto->Set("removeInputCallback", FunctionTemplate::New(IHRemoveInputCallback));
      
      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));

      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::InputInterface* UnwrapInputInterface(Handle<Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      dtEntity::InputInterface* v;
      GetInternal(obj, 0, v);
      return v;      
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapTouchPhases()
   {
      HandleScope scope;
      Handle<Object> obj = Object::New();
      obj->Set(String::New("Unknown"), Integer::New(dtEntity::TouchPhase::UNKNOWN));
      obj->Set(String::New("Began"), Integer::New(dtEntity::TouchPhase::BEGAN));
      obj->Set(String::New("Moved"), Integer::New(dtEntity::TouchPhase::MOVED));
      obj->Set(String::New("Stationary"), Integer::New(dtEntity::TouchPhase::STATIONARY));
      obj->Set(String::New("Ended"), Integer::New(dtEntity::TouchPhase::ENDED));
      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapKeys(dtEntity::InputInterface* ih)
   {
      typedef std::map<std::string, unsigned int> KN;
      KN kn;
      ih->GetKeyNames(kn);
      
      HandleScope scope;
      Handle<Object> obj = Object::New();
      
      for(KN::const_iterator i = kn.begin(); i != kn.end(); ++i)
      {
         obj->Set(ToJSString(i->first), Uint32::New(i->second));
      }

      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapAxes(dtEntity::InputInterface* ih)
   {
            
      HandleScope scope;
      Handle<Object> obj = Object::New();
      
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      obj->Set(String::New("MouseX"), String::New(dtEntity::InputInterface::MouseXId.c_str()));
      obj->Set(String::New("MouseY"), String::New(dtEntity::InputInterface::MouseYId.c_str()));
      obj->Set(String::New("MouseXRaw"), String::New(dtEntity::InputInterface::MouseXRawId.c_str()));
      obj->Set(String::New("MouseYRaw"), String::New(dtEntity::InputInterface::MouseYRawId.c_str()));
      obj->Set(String::New("MouseDeltaX"), String::New(dtEntity::InputInterface::MouseDeltaXId.c_str()));
      obj->Set(String::New("MouseDeltaY"), String::New(dtEntity::InputInterface::MouseDeltaYId.c_str()));
      obj->Set(String::New("MouseDeltaXRaw"), String::New(dtEntity::InputInterface::MouseDeltaXRawId.c_str()));
      obj->Set(String::New("MouseDeltaYRaw"), String::New(dtEntity::InputInterface::MouseDeltaYRawId.c_str()));
#else
      obj->Set(String::New("MouseX"), Uint32::New(dtEntity::InputInterface::MouseXId));
      obj->Set(String::New("MouseY"), Uint32::New(dtEntity::InputInterface::MouseYId));
      obj->Set(String::New("MouseXRaw"), Uint32::New(dtEntity::InputInterface::MouseXRawId));
      obj->Set(String::New("MouseYRaw"), Uint32::New(dtEntity::InputInterface::MouseYRawId));
      obj->Set(String::New("MouseDeltaX"), Uint32::New(dtEntity::InputInterface::MouseDeltaXId));
      obj->Set(String::New("MouseDeltaY"), Uint32::New(dtEntity::InputInterface::MouseDeltaYId));
      obj->Set(String::New("MouseDeltaXRaw"), Uint32::New(dtEntity::InputInterface::MouseDeltaXRawId));
      obj->Set(String::New("MouseDeltaYRaw"), Uint32::New(dtEntity::InputInterface::MouseDeltaYRawId));
#endif 
      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapMouseWheelStates()
   {            
      HandleScope scope;
      Handle<Object> obj = Object::New();
      
      obj->Set(String::New("None"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_NONE));
      obj->Set(String::New("Left"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_LEFT));
      obj->Set(String::New("Right"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_RIGHT));
      obj->Set(String::New("Up"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_UP));
      obj->Set(String::New("Down"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_DOWN));
      obj->Set(String::New("2D"), Uint32::New(osgGA::GUIEventAdapter::SCROLL_2D));
      
      return scope.Close(obj);
   }
}
