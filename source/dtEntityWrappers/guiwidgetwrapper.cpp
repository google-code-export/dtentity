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

#include <dtEntityWrappers/guiwidgetwrapper.h>

#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>
#include <dtEntity/scriptmodule.h>
#include <osg/Referenced>
#include <iostream>
#include <sstream>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindow.h>
#include <dtEntity/applicationcomponent.h>
#include <osgViewer/GraphicsWindow>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_guiWidgetTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   void MakeCEGUIContextCurrent(Handle<Value> val)
   {
      osgViewer::GraphicsWindow* win = static_cast<osgViewer::GraphicsWindow*>(External::Unwrap(val));
      win->makeCurrent();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   /** helper class that provides c++ callbacks to CEGUI and forwards the
     * callbacks to JavaScript
     */
   class EventHandler
   {
   public:
      EventHandler(v8::Handle<v8::Object> holder, const std::string& eventName);
      ~EventHandler();

      bool onEvent(const CEGUI::EventArgs& e);
      std::string GetId();
      std::string GetEventName() const { return mEventName; }

      void SetConnection(CEGUI::Event::Connection c) { mConnection = c; }
      CEGUI::Event::Connection GetConnection() const { return mConnection; }

   private:

      v8::Persistent<v8::Object> mObject;
      std::string mEventName;
      CEGUI::Event::Connection mConnection;
   };

   ////////////////////////////////////////////////////////////////////////////////
   struct EventHandlerStore
   {
      typedef std::map<std::string, EventHandler*> EventMap;
      EventMap mHandlers;
   };

   static EventHandlerStore s_eventHandlerStore;

   ////////////////////////////////////////////////////////////////////////////////
   void AddEventHandler(CEGUI::Window* window, EventHandler* e)
   {
      std::string id = e->GetId();

      dtEntity::ScriptModule* sm;

      CEGUI::ScriptModule* cgsm = CEGUI::System::getSingleton().getScriptingModule();
      if(cgsm == NULL)
      {
         sm = new dtEntity::ScriptModule();
         CEGUI::System::getSingleton().setScriptingModule(sm);
      }
      else
      {
         sm = dynamic_cast<dtEntity::ScriptModule*>(cgsm);
         assert(sm != NULL);
      }

      EventHandlerStore::EventMap::const_iterator i = s_eventHandlerStore.mHandlers.find(e->GetId());
      if(i != s_eventHandlerStore.mHandlers.end())
      {
         i->second->GetConnection()->disconnect();
      }

      CEGUI::SubscriberSlot slot(&EventHandler::onEvent, e);
      sm->AddCallback(id, slot);
      CEGUI::String evtname = e->GetEventName();
      CEGUI::Event::Connection connection = sm->subscribeEvent(window, evtname, 0, id);
      e->SetConnection(connection);

      s_eventHandlerStore.mHandlers[e->GetId()] = e;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EventHandler::EventHandler(v8::Handle<v8::Object> holder, const std::string& eventName)
   : mObject(Persistent<Object>::New(holder))
   , mEventName(eventName)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventHandler::~EventHandler()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string EventHandler::GetId()
   {
      int unique = mObject->GetIdentityHash();
      std::ostringstream stream;
      stream << "evt_" << unique << "_" << mEventName;
      return stream.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EventHandler::onEvent(const CEGUI::EventArgs& e)
   {
      HandleScope handle_scope;
      Handle<Context> context = mObject->CreationContext();
      Context::Scope context_scope(context);
      std::string evtName = "on" + mEventName;
      Handle<String> name = String::New(evtName.c_str());
      if(!mObject->Has(name))
      {
         return false;
      }
      Handle<Value> fun = mObject->Get(name);
      if(!fun->IsFunction())
      {
         return true;
      }
     
      // call the callback
      TryCatch try_catch;
      Handle<Value> result = Handle<Function>::Cast(fun)->Call(mObject, 0, NULL);

      if(result.IsEmpty())
      {
         ReportException(&try_catch);
         return false;
      }

      return true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIWidgetToString(const Arguments& args)
   {
      return String::New("<GUIWidget>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   static Handle<Value> GUIWidgetNamedPropertyGetter(Local<String> namestr, const AccessorInfo& info)
   {
      std::string propname = ToStdString(namestr);

      CEGUI::Window* window; GetInternal(info.Holder(), 0, window);

      if(window->isPropertyPresent(propname))
      {
         try
         {
            return String::New(window->getProperty(propname).c_str());
         }
         catch(std::exception& e)
         {
            std::string err = e.what();
            return ThrowError("Error in getProperty: " + err);
         }
      }
      return Handle<Value>();
   }

   ////////////////////////////////////////////////////////////////////////////////
   // when actor.somevalue = x is executed in JavaScript
   Handle<Value> GUIWidgetNamedPropertySetter(Local<String> namestr, Local<Value> value, const AccessorInfo& info)
   {
      
      std::string propname = ToStdString(namestr);
            
      CEGUI::Window* window; GetInternal(info.Holder(), 0, window);

      if(window == NULL)
      {
         return Handle<Value>();
      }

      if(propname.substr(0, 2) == "on")
      {
         MakeCEGUIContextCurrent(info.Data());
         std::string eventName = propname.substr(2, propname.length());
         EventHandler* eh = new EventHandler(info.Holder(), eventName);
         AddEventHandler(window, eh);
         return Handle<Value>();
      }
      else
      {
         MakeCEGUIContextCurrent(info.Data());
         window->setProperty(ToStdString(namestr),ToStdString(value));
         return value;
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Integer> GUIWidgetNamedPropertyQuery(Local<String> prop,const AccessorInfo& info)   
   {
      CEGUI::Window* window; GetInternal(info.Holder(), 0, window);
      std::string propname = ToStdString(prop);
      if(window->PropertySet::isPropertyPresent(propname.c_str()))
      {
         return Integer::New(DontDelete);
      }
      return Integer::New(None);
   }
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Array> GUIWidgetNamedPropertyEnumerator(const AccessorInfo& info)   
   {
      CEGUI::Window* window; GetInternal(info.Holder(), 0, window);
      CEGUI::PropertySet::Iterator prp_iter = window->PropertySet::getIterator();

      HandleScope scope;
      Handle<Array> arr = Array::New();

      int count = 0;
      while(!prp_iter.isAtEnd()) 
      {
         CEGUI::String str = prp_iter.getCurrentKey();
         arr->Set(Integer::New(count++), String::New(str.c_str()));
         ++prp_iter;
      }
      return scope.Close(arr);
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUISetVisible(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsBoolean())
      {
         return ThrowSyntaxError("setVisible usage: setVisible(bool)");
      }
      MakeCEGUIContextCurrent(args.Data());
      CEGUI::Window* window; GetInternal(args.Holder(), 0, window);
      window->setVisible(args[0]->BooleanValue());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUISetAlpha(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsNumber())
      {
         return ThrowSyntaxError("setAlpha usage: setAlpha(number)");
      }
      MakeCEGUIContextCurrent(args.Data());
      CEGUI::Window* window; GetInternal(args.Holder(), 0, window);
      window->setAlpha(args[0]->NumberValue());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUISetProperty(const Arguments& args)
   {
      if(args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString())
      {
         return ThrowSyntaxError("setProperty usage: setProperty(string, string)");
      }
      CEGUI::Window* window; GetInternal(args.Holder(), 0, window);
      try
      {
         MakeCEGUIContextCurrent(args.Data());
         window->setProperty(ToStdString(args[0]),ToStdString(args[1]));
      }
      catch(std::exception& e)
      {
         std::string err = e.what();
         return ThrowError("Error in setProperty: " + err);
      }
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIGetProperty(const Arguments& args)
   {
      if(args.Length() != 1 || !args[0]->IsString())
      {
         return ThrowSyntaxError("getProperty usage: setProperty(string name)");
      }
      CEGUI::Window* window; GetInternal(args.Holder(), 0, window);
      try
      {
         return String::New(window->getProperty(ToStdString(args[0])).c_str());
      }
      catch(std::exception& e)
      {
         std::string err = e.what();
         return ThrowError("Error in getProperty: " + err);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIAddChildWidget(const Arguments& args)
   {
      if(args.Length() != 1 || !IsGuiWidget(args[0]))
      {
         return ThrowSyntaxError("addChildWidget usage: addChildWidget(window child)");
      }
      MakeCEGUIContextCurrent(args.Data());
      CEGUI::Window* window; GetInternal(args.Holder(), 0, window);
      CEGUI::Window* child = UnwrapGuiWidget(args[0]);
      window->addChildWindow(child);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConstructGUIWidget(const v8::Arguments& args)
   {
      Handle<External> ext = Handle<External>::Cast(args[0]);
      CEGUI::Window* widget = static_cast<CEGUI::Window*>(ext->Value());
      args.Holder()->SetInternalField(0, External::New(widget));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapGuiWidget(ScriptSystem* ss, CEGUI::Window* v)
   {

      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(ss->GetGlobalContext());

      if(s_guiWidgetTemplate.IsEmpty())
      {
        dtEntity::ApplicationSystem* appsys;
        ss->GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
        osgViewer::GraphicsWindow* win = appsys->GetPrimaryWindow();

        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_guiWidgetTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("GUIWidget"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(GUIWidgetToString, External::New(win)));
        proto->Set("setVisible", FunctionTemplate::New(GUISetVisible, External::New(win)));
        proto->Set("setAlpha", FunctionTemplate::New(GUISetAlpha, External::New(win)));
        proto->Set("setProperty", FunctionTemplate::New(GUISetProperty, External::New(win)));
        proto->Set("getProperty", FunctionTemplate::New(GUIGetProperty, External::New(win)));
        proto->Set("addChildWidget", FunctionTemplate::New(GUIAddChildWidget, External::New(win)));
        
        templt->InstanceTemplate()->SetNamedPropertyHandler(
           GUIWidgetNamedPropertyGetter,
           GUIWidgetNamedPropertySetter,
           GUIWidgetNamedPropertyQuery,
           0, // no deleter
           GUIWidgetNamedPropertyEnumerator,
           External::New(win)
        );
      }
      Local<Object> instance = s_guiWidgetTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);

   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Window* UnwrapGuiWidget(v8::Handle<v8::Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      CEGUI::Window* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsGuiWidget(v8::Handle<v8::Value> val)
   {
      if(s_guiWidgetTemplate.IsEmpty())
      {
         return false;
      }
      return s_guiWidgetTemplate->HasInstance(val);
   }
}
