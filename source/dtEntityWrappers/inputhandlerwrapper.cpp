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
         Isolate* isolate = Isolate::GetCurrent();
         mObject.Reset(isolate, obj);
         
         if(obj->Has(String::NewFromUtf8(isolate, "keyDown"))) 
         {
            mKeyDownFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "keyDown"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "keyUp"))) 
         {
            mKeyUpFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "keyUp"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "mouseButtonDown"))) 
         {
            mMouseDownFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "mouseButtonDown"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "mouseButtonUp"))) 
         {
            mMouseUpFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "mouseButtonUp"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "mouseWheel"))) 
         {
            mMouseWheelFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "mouseWheel"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "mouseMove"))) 
         {
            mMouseMoveFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "mouseMove"))));
         }
         if(obj->Has(String::NewFromUtf8(isolate, "mouseEnterLeave")))
         {
            mMouseEnterLeaveFunc.Reset(isolate, Handle<Function>::Cast(obj->Get(String::NewFromUtf8(isolate, "mouseEnterLeave"))));
         }
      }

      virtual bool KeyUp(const std::string& name, bool handled, unsigned int contextId) {
         if(!mKeyUpFunc.IsEmpty())
         {
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> keyUpFunc = Handle<Function>::New(isolate, mKeyUpFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(keyUpFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { ToJSString(name), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = keyUpFunc->Call(object, 3, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> keyDownFunc = Handle<Function>::New(isolate, mKeyDownFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(keyDownFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { ToJSString(name), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = keyDownFunc->Call(object, 3, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> mouseUpFunc = Handle<Function>::New(isolate, mMouseUpFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(mouseUpFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(isolate, button), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = mouseUpFunc->Call(object, 3, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> mouseDownFunc = Handle<Function>::New(isolate, mMouseDownFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(mouseDownFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(isolate, button), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = mouseDownFunc->Call(object, 3, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> mouseWheelFunc = Handle<Function>::New(isolate, mMouseWheelFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(mouseWheelFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[3] = { Integer::New(isolate, dir), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = mouseWheelFunc->Call(object, 3, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> mouseWheelFunc = Handle<Function>::New(isolate, mMouseWheelFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(mouseWheelFunc->CreationContext());
            TryCatch try_catch;
            Handle<Value> argv[4] = { Number::New(isolate, x), Number::New(isolate, y), Boolean::New(isolate, handled), Uint32::New(isolate, contextId) };
            Handle<Value> ret = mouseWheelFunc->Call(object, 4, argv);

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
            Isolate* isolate = Isolate::GetCurrent();
            HandleScope scope(isolate);

            Handle<Function> mouseEnterLeaveFunc = Handle<Function>::New(isolate, mMouseEnterLeaveFunc);
            Handle<Object> object = Handle<Object>::New(isolate, mObject);

            Context::Scope context_scope(mouseEnterLeaveFunc->CreationContext());            
            TryCatch try_catch;
            Handle<Value> argv[2] = { Boolean::New(isolate, focused), Uint32::New(isolate, contextId) };
            Handle<Value> ret = mouseEnterLeaveFunc->Call(object, 2, argv);

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
   void IHGetKey(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }

      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetKey(ToStdString(args[0]), contextId)) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetKeyUp(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }

      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetKeyUp(ToStdString(args[0]), contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetKeyDown(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetKeyDown(ToStdString(args[0]), contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHAnyKey(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->AnyKeyDown()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetInputString(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      args.GetReturnValue().Set( ToJSString(input->GetInputString()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetMouseButton(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());

      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetMouseButton(args[0]->Int32Value(), contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetMouseButtonUp(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());

      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetMouseButtonUp(args[0]->Int32Value(), contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetMouseButtonDown(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 1)
      {
         contextId = args[1]->Uint32Value();
      }
      args.GetReturnValue().Set( Boolean::New(Isolate::GetCurrent(), input->GetMouseButtonDown(args[0]->Int32Value(), contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetAxis(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      args.GetReturnValue().Set( Number::New(input->GetAxis(ToStdString(args[0]))));
#else
      args.GetReturnValue().Set( Number::New(Isolate::GetCurrent(), input->GetAxis((dtEntity::StringId)args[0]->Uint32Value())));
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetMouseWheelState(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* input = UnwrapInputInterface(args.This());
      unsigned int contextId = 0;
      if(args.Length() > 0)
      {
         contextId = args[0]->Uint32Value();
      }
      args.GetReturnValue().Set( Integer::New(Isolate::GetCurrent(), (int)input->GetMouseWheelState(contextId)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetMultiTouchEnabled(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      args.GetReturnValue().Set( Boolean::New (Isolate::GetCurrent(), ih->GetMultiTouchEnabled()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetNumTouches(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      args.GetReturnValue().Set( Integer::New(Isolate::GetCurrent(), ih->GetNumTouches()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHGetTouches(const FunctionCallbackInfo<Value>& args)
   {
      Isolate* isolate = Isolate::GetCurrent();
      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      HandleScope scope(isolate);
      Handle<Array> jstouches = Array::New(v8::Isolate::GetCurrent());

      Handle<String> tapcount = String::NewFromUtf8(isolate, "tapcount");
      Handle<String> id = String::NewFromUtf8(isolate, "id");
      Handle<String> x = String::NewFromUtf8(isolate, "x");
      Handle<String> y = String::NewFromUtf8(isolate, "y");
      Handle<String> phase = String::NewFromUtf8(isolate, "phase");

      const std::vector<dtEntity::TouchPoint>& touches = ih->GetTouches();
      unsigned int count = 0;
      for(std::vector<dtEntity::TouchPoint>::const_iterator i = touches.begin(); i != touches.end(); ++i)
      {
         dtEntity::TouchPoint tp = *i;
         Handle<Object> o = Object::New(Isolate::GetCurrent());
         o->Set(id, Uint32::New(isolate, tp.mId));
         o->Set(tapcount, Uint32::New(isolate, tp.mTapCount));
         o->Set(x, Number::New(isolate, tp.mX));
         o->Set(y, Number::New(isolate, tp.mY));
         o->Set(phase, Integer::New(isolate, tp.mTouchPhase));
         jstouches->Set(count++, o);
      }

      args.GetReturnValue().Set(jstouches);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHToString(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set( String::NewFromUtf8(v8::Isolate::GetCurrent(), "<Input>"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHAddInputCallback(const FunctionCallbackInfo<Value>& args)
   {
      Handle<Object> o = Handle<Object>::Cast(args[0]);
      InputCallbackWrapper* wr = new InputCallbackWrapper(o);

      dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
      ih->AddInputCallback(wr);
      s_inputCallbackWrappers.push_back(wr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IHRemoveInputCallback(const FunctionCallbackInfo<Value>& args)
   {
      for(std::vector<InputCallbackWrapper*>::iterator i = s_inputCallbackWrappers.begin();
         i != s_inputCallbackWrappers.end(); ++i)
      {
         if((*i)->mObject == args[0])
         {
            dtEntity::InputInterface* ih; GetInternal(args.This(), 0, ih);
            ih->RemoveInputCallback(*i);
            s_inputCallbackWrappers.erase(i);
            args.GetReturnValue().Set( True(Isolate::GetCurrent()));
         }
      }
      
      args.GetReturnValue().Set( False(Isolate::GetCurrent()) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   /*void IHPrintKeys(const Arguments& args)
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
      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope scope(isolate);
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = FunctionTemplate::New(isolate);
      templt->SetClassName(String::NewFromUtf8(isolate, "Input"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set(String::NewFromUtf8(isolate, "getAxis"), FunctionTemplate::New(isolate, IHGetAxis));
      proto->Set(String::NewFromUtf8(isolate, "getKey"), FunctionTemplate::New(isolate, IHGetKey));
      proto->Set(String::NewFromUtf8(isolate, "getKeyDown"), FunctionTemplate::New(isolate, IHGetKeyDown));
      proto->Set(String::NewFromUtf8(isolate, "getKeyUp"), FunctionTemplate::New(isolate, IHGetKeyUp));
      proto->Set(String::NewFromUtf8(isolate, "anyKeyDown"), FunctionTemplate::New(isolate, IHAnyKey));

      proto->Set(String::NewFromUtf8(isolate, "getInputString"), FunctionTemplate::New(isolate, IHGetInputString));

      proto->Set(String::NewFromUtf8(isolate, "getMouseButton"), FunctionTemplate::New(isolate, IHGetMouseButton));
      proto->Set(String::NewFromUtf8(isolate, "getMouseButtonUp"), FunctionTemplate::New(isolate, IHGetMouseButtonUp));
      proto->Set(String::NewFromUtf8(isolate, "getMouseButtonDown"), FunctionTemplate::New(isolate, IHGetMouseButtonDown));
      proto->Set(String::NewFromUtf8(isolate, "getMouseWheelState"), FunctionTemplate::New(isolate, IHGetMouseWheelState));
      
      proto->Set(String::NewFromUtf8(isolate, "getMultiTouchEnabled"), FunctionTemplate::New(isolate, IHGetMultiTouchEnabled));
      proto->Set(String::NewFromUtf8(isolate, "getNumTouches"), FunctionTemplate::New(isolate, IHGetNumTouches));
      proto->Set(String::NewFromUtf8(isolate, "getTouches"), FunctionTemplate::New(isolate, IHGetTouches));
      //proto->Set(String::NewFromUtf8(isolate, "printKeys"), FunctionTemplate::New(IHPrintKeys));
      proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, IHToString));
      proto->Set(String::NewFromUtf8(isolate, "addInputCallback"), FunctionTemplate::New(isolate, IHAddInputCallback));
      proto->Set(String::NewFromUtf8(isolate, "removeInputCallback"), FunctionTemplate::New(isolate, IHRemoveInputCallback));
      
      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(isolate, v));

      return scope.Escape(instance);
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
      Isolate* isolate = Isolate::GetCurrent();

      EscapableHandleScope scope(isolate);
      Local<Object> obj = Object::New(isolate);
      obj->Set(String::NewFromUtf8(isolate, "Unknown"), Integer::New(isolate, dtEntity::TouchPhase::UNKNOWN));
      obj->Set(String::NewFromUtf8(isolate, "Began"), Integer::New(isolate, dtEntity::TouchPhase::BEGAN));
      obj->Set(String::NewFromUtf8(isolate, "Moved"), Integer::New(isolate, dtEntity::TouchPhase::MOVED));
      obj->Set(String::NewFromUtf8(isolate, "Stationary"), Integer::New(isolate, dtEntity::TouchPhase::STATIONARY));
      obj->Set(String::NewFromUtf8(isolate, "Ended"), Integer::New(isolate, dtEntity::TouchPhase::ENDED));
      return scope.Escape(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapKeys(dtEntity::InputInterface* ih)
   {
      typedef std::map<std::string, unsigned int> KN;
      KN kn;
      ih->GetKeyNames(kn);
      
      EscapableHandleScope scope(Isolate::GetCurrent());
      Local<Object> obj = Object::New(Isolate::GetCurrent());
      
      for(KN::const_iterator i = kn.begin(); i != kn.end(); ++i)
      {
         obj->Set(ToJSString(i->first), Uint32::New(Isolate::GetCurrent(), i->second));
      }

      return scope.Escape(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapAxes(dtEntity::InputInterface* ih)
   {
      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope scope(isolate);
      Local<Object> obj = Object::New(isolate);
      
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseX"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseXId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseY"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseYId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseXRaw"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseXRawId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseYRaw"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseYRawId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseDeltaX"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseDeltaXId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseDeltaY"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseDeltaYId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseDeltaXRaw"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseDeltaXRawId.c_str()));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MouseDeltaYRaw"), String::NewFromUtf8(v8::Isolate::GetCurrent(), dtEntity::InputInterface::MouseDeltaYRawId.c_str()));
#else
      obj->Set(String::NewFromUtf8(isolate, "MouseX"), Uint32::New(isolate, dtEntity::InputInterface::MouseXId));
      obj->Set(String::NewFromUtf8(isolate, "MouseY"), Uint32::New(isolate, dtEntity::InputInterface::MouseYId));
      obj->Set(String::NewFromUtf8(isolate, "MouseXRaw"), Uint32::New(isolate, dtEntity::InputInterface::MouseXRawId));
      obj->Set(String::NewFromUtf8(isolate, "MouseYRaw"), Uint32::New(isolate, dtEntity::InputInterface::MouseYRawId));
      obj->Set(String::NewFromUtf8(isolate, "MouseDeltaX"), Uint32::New(isolate, dtEntity::InputInterface::MouseDeltaXId));
      obj->Set(String::NewFromUtf8(isolate, "MouseDeltaY"), Uint32::New(isolate, dtEntity::InputInterface::MouseDeltaYId));
      obj->Set(String::NewFromUtf8(isolate, "MouseDeltaXRaw"), Uint32::New(isolate, dtEntity::InputInterface::MouseDeltaXRawId));
      obj->Set(String::NewFromUtf8(isolate, "MouseDeltaYRaw"), Uint32::New(isolate, dtEntity::InputInterface::MouseDeltaYRawId));
#endif 
      return scope.Escape(obj);
   }

}
