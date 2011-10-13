/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#include <dtEntityWrappers/inputhandler.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntity/basemessages.h>
#include <iostream>
#include <v8.h>

using namespace v8;

namespace dtEntityWrappers
{

   ////////////////////////////////////////////////////////////////////////////////
   std::string TouchPhase::ToString(e v)
   {
      switch(v)
      {
      case BEGAN:       return "BEGAN";
      case MOVED:       return "MOVED";
      case STATIONERY:  return "STATIONERY";
      case ENDED:       return "ENDED";
      }
      return "UNKNOWN";
   }

   ////////////////////////////////////////////////////////////////////////////////
   InputHandler::InputHandler(dtEntity::EntityManager& em)
      : mEntityManager(&em)
      , mMultiTouchEnabled(false)
      , mNumTouches(0)
   {
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   InputHandler::~InputHandler()
   {
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::handle(const osgGA::GUIEventAdapter& ea, 
                             osgGA::GUIActionAdapter& aa, osg::Object*,
                             osg::NodeVisitor*)
   {
      if(ea.getHandled()) 
      {
         return false;
      }
      //returned values in the range of (-1..1)
      float x = ea.getXnormalized();
      float y = ea.getYnormalized();

      switch (ea.getEventType() )
      {
         case osgGA::GUIEventAdapter::PUSH: 
         case osgGA::GUIEventAdapter::DRAG:
         case osgGA::GUIEventAdapter::RELEASE:
         {
            if(!ea.isMultiTouchEvent())
            {
               return false;
            }
            osgGA::GUIEventAdapter::TouchData* data = ea.getTouchData();
            mNumTouches = 0;
            mTouches.resize(data->getNumTouchPoints());

            for(unsigned int i = 0; i < data->getNumTouchPoints(); ++i)
            {
               osgGA::GUIEventAdapter::TouchData::TouchPoint tp = data->get(i);
               TouchPoint mypt;
               mypt.mId = tp.id;
               mypt.mX = tp.x;
               mypt.mY = tp.y;
               mypt.mTapCount = tp.tapCount;

               switch(tp.phase)
               {
               case osgGA::GUIEventAdapter::TOUCH_UNKNOWN: 
                  mypt.mTouchPhase = TouchPhase::UNKNOWN; 
                  break;
               case osgGA::GUIEventAdapter::TOUCH_BEGAN: 
                  mypt.mTouchPhase = TouchPhase::BEGAN;
                  ++mNumTouches;
                  break;
               case osgGA::GUIEventAdapter::TOUCH_MOVED: 
                  mypt.mTouchPhase = TouchPhase::MOVED; 
                  ++mNumTouches;
                  break;
               case osgGA::GUIEventAdapter::TOUCH_STATIONERY: 
                  mypt.mTouchPhase = TouchPhase::STATIONERY; 
                  ++mNumTouches;
                  break;
               case osgGA::GUIEventAdapter::TOUCH_ENDED: 
                  mypt.mTouchPhase = TouchPhase::ENDED; 
                  break;
               }
               mTouches[i] = mypt;
            }
           
            break;
         }
      }
      return false;
   }   

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHToString(const Arguments& args)
   {
      return String::New("<Input>");
   }

   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetMultiTouchEnabled(const Arguments& args)
   {
      InputHandler* ih; GetInternal(args.This(), 0, ih);
      return Boolean::New(ih->GetMultiTouchEnabled());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetNumTouches(const Arguments& args)
   {
      InputHandler* ih; GetInternal(args.This(), 0, ih);
      return Integer::New(ih->GetNumTouches());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> IHGetTouches(const Arguments& args)
   {
      InputHandler* ih; GetInternal(args.This(), 0, ih);
      HandleScope scope;
      Handle<Array> jstouches = Array::New();

      Handle<String> tapcount = String::New("tapcount");
      Handle<String> id = String::New("id");
      Handle<String> x = String::New("x");
      Handle<String> y = String::New("y");
      Handle<String> phase = String::New("phase");
      const std::vector<TouchPoint>& touches = ih->GetTouches();
      unsigned int count = 0;
      for(std::vector<TouchPoint>::const_iterator i = touches.begin(); i != touches.end(); ++i)
      {
         TouchPoint tp = *i;
         Handle<Object> o = Object::New();
         o->Set(id, Uint32::New(tp.mId));
         o->Set(tapcount, Uint32::New(tp.mTapCount));
         o->Set(x, Number::New(tp.mX));
         o->Set(y, Number::New(tp.mY));
         o->Set(phase, String::New(TouchPhase::ToString(tp.mTouchPhase).c_str()));
         jstouches->Set(count++, o);
      }

      return scope.Close(jstouches);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConstructIH(const v8::Arguments& args)
   {
      Handle<External> ext = Handle<External>::Cast(args[0]);
      InputHandler* ih = static_cast<InputHandler*>(ext->Value());
      args.Holder()->SetInternalField(0, External::New(ih));
      return Undefined();
   }

  
   ////////////////////////////////////////////////////////////////////////////////
   InputHandlerWrapper::InputHandlerWrapper()
   {
      HandleScope handle_scope;
      Handle<Context> context = WrapperManager::GetInstance().GetGlobalContext();
      Context::Scope context_scope(context);

      mTemplate->InstanceTemplate()->SetInternalFieldCount(1);

      mTemplate->SetClassName(String::New("Input"));

      Local<Function> constructor = FunctionTemplate::New(ConstructIH)->GetFunction();
      mConstructor = Persistent<Function>::New(constructor);

      AddFunctionToTpl(mTemplate->PrototypeTemplate(), "toString", IHToString);
      AddFunctionToTpl(mTemplate->PrototypeTemplate(), "getMultiTouchEnabled", IHGetMultiTouchEnabled);
      AddFunctionToTpl(mTemplate->PrototypeTemplate(), "getNumTouches", IHGetNumTouches);
      AddFunctionToTpl(mTemplate->PrototypeTemplate(), "getTouches", IHGetTouches);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapInputHandler(InputHandler* v)
   {
      InputHandlerWrapper* wrapper = &InputHandlerWrapper::GetInstance();

      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(WrapperManager::GetInstance().GetGlobalContext());

      // first let wrapper create an object from its template.
      v8::Local<v8::Object> obj = wrapper->Create();

      // call constructor function of wrapper and pass it
      // a single argument with the native pointer wrapped in
      // a v8::External
      v8::Handle<v8::External> ext = v8::External::New(const_cast<void*>(static_cast<const void*>(v)));
      v8::Handle<v8::Value> argv[1] = { ext };

      TryCatch try_catch;
      Handle<Value> result = wrapper->GetConstructor()->Call(obj, 1, argv);

      if(try_catch.HasCaught())
      {
         throw dtUtil::Exception("Cannot wrap, no wrapper found for this type", __FILE__, __LINE__);
      }

      // object was created in local scope. Let the local scope
      // pass the object to the next higher scope on the stack.
      return handle_scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   InputHandler* UnwrapInputHandler(v8::Handle<v8::Value> val)
   {
      const void* v;
      InputHandlerWrapper::GetInstance().Unwrap(val, &v);
      return const_cast<InputHandler*>(static_cast<const InputHandler*>(v));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsInputHandler(v8::Handle<v8::Value> val)
   {
      return InputHandlerWrapper::GetInstance().IsInstanceOf(val);
   }
}
