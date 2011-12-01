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

#include <dtEntityWrappers/guiwrapper.h>

#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/guiwidgetwrapper.h>
#include <dtEntity/gui.h>
#include <dtEntity/applicationcomponent.h>
#include <iostream>
#include <sstream>
#include <osgViewer/GraphicsWindow>

#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUICoordConverter.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIFontManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/elements/CEGUIScrollablePane.h>
#include <CEGUI/elements/CEGUIScrolledContainer.h>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_guiTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   void MakeContextCurrent()
   {
      dtEntity::ApplicationSystem* appsys;
      GetEntityManager()->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      appsys->GetPrimaryWindow()->makeCurrent();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIToString(const Arguments& args)
   {
      return String::New("<GUI>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUILoadScheme(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() < 1)
      {
         return ThrowException(Exception::SyntaxError(String::New("loadScheme usage: loadScheme(path)")));
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);

      try
      {
         gui->LoadScheme(ToStdString(args[0]));
      }
      catch(CEGUI::Exception& e)
      {
         LOG_ERROR("Exception when loading CEGUI scheme!");
         LOG_ERROR(e.what());
      }
      catch(std::exception& e)
      {
         LOG_ERROR("Exception when loading CEGUI scheme!");
         LOG_ERROR(e.what());
      }
      catch(...)
      {
         LOG_ERROR("Exception when loading CEGUI scheme!");
      }
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUISetMouseCursor(const Arguments& args)
   {
      if(args.Length() < 2)
      {
         return ThrowException(Exception::SyntaxError(String::New("setMouseCursor usage: setMouseCursor(imageSetName, imageName)")));
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      gui->SetMouseCursor(ToStdString(args[0]), ToStdString(args[1]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIShowCursor(const Arguments& args)
   {
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      gui->ShowCursor();
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIHideCursor(const Arguments& args)
   {
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      gui->HideCursor();
      return Undefined();
   }
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUILoadLayout(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() < 1)
      {
         return ThrowException(Exception::SyntaxError(String::New("loadLayout usage: loadLayout(layoutname)")));
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);

      try
      {
         if(args.Length() > 1)
         {
            gui->LoadLayout(ToStdString(args[0]), ToStdString(args[1]));
         }
         else
         {
            gui->LoadLayout(ToStdString(args[0]));
         }

      }
      catch(CEGUI::Exception& e)
      {
         LOG_ERROR("Exception when loading CEGUI layout!");
         LOG_ERROR(e.what());
      }
      catch(std::exception& e)
      {
         LOG_ERROR("Exception when loading CEGUI layout!");
         LOG_ERROR(e.what());
      }
      catch(...)
      {
         LOG_ERROR("Exception when loading CEGUI layout!");
      }

      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIGetWidget(const Arguments& args)
   {
      if(args.Length() < 1)
      {
         return ThrowException(Exception::SyntaxError(String::New("getWidget usage: getWidget(widgetname)")));
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);

      std::string name = ToStdString(args[0]);
      if(!gui->IsWindowPresent(name))
      {
         return Null();
      }
      CEGUI::Window* w = gui->GetWidget(name);
      return WrapGuiWidget(w);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUICreateWidget(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() < 2 || (!args[0]->IsNull() && !IsGuiWidget(args[0])) || !args[1]->IsString())
      {
         return ThrowSyntaxError("createWidget usage: createWidget(widget parent, string typename, [string name])");
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);


      CEGUI::Window* parent = NULL;
      if(!args[0]->IsNull())
      {
         parent = UnwrapGuiWidget(args[0]);
      }

      std::string typeName = ToStdString(args[1]);
      std::string name = "";
      if(args.Length() > 2 )
      {
         name = ToStdString(args[2]);
      }
      CEGUI::Window* newwindow = gui->CreateWidget(parent, typeName, name);

      if(newwindow == NULL)
      {
         return Null();
      }
      return WrapGuiWidget(newwindow);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIIsImagesetPresent(const Arguments& args)
   {
      if(args.Length() < 1 || !args[0]->IsString())
      {
         return ThrowSyntaxError("usage: isImagesetPresent(name)");
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      return Boolean::New(gui->IsImagesetPresent(ToStdString(args[0])));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUICreateImageset(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString())
      {
         return ThrowSyntaxError("usage: createImageset(imagesetName, filename [, resourceGroupName])");
      }

      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      if(args.Length() > 2 && args[2]->IsString())
      {
         gui->CreateImageset(ToStdString(args[0]), ToStdString(args[1]), ToStdString(args[2]));
      }
      else
      {
         gui->CreateImageset(ToStdString(args[0]), ToStdString(args[1]));
      }
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIDestroyImageset(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() < 1 || !args[0]->IsString())
      {
         return ThrowSyntaxError("usage: destroyImageset(imagesetName)");
      }

      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      gui->DestroyImageset(ToStdString(args[0]));

      return Undefined();
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> GUIDestroyWidget(const Arguments& args)
   {
      MakeContextCurrent();
      if(args.Length() != 1 || !IsGuiWidget(args[0]))
      {
         return ThrowException(Exception::SyntaxError(String::New("destroyWidget usage: destroyWidget(widget toDestroy)")));
      }
      dtEntity::GUI* gui; GetInternal(args.This(), 0, gui);
      CEGUI::Window* toDestroy = UnwrapGuiWidget(args[0]);

      gui->DestroyWidget(toDestroy);
      return Undefined();
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   void GUIDestructor(v8::Persistent<Value> v, void*)
   {
      dtEntity::GUI* gui = UnwrapGui(v);
      gui->unref();
      Handle<Object> obj = Handle<Object>::Cast(v);
      v.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Object> WrapGui(dtEntity::GUI* v)
   {
      v->ref();
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(GetGlobalContext());

      if(s_guiTemplate.IsEmpty())
      {
        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_guiTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("GUI"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("createImageset", FunctionTemplate::New(GUICreateImageset));
        proto->Set("createWidget", FunctionTemplate::New(GUICreateWidget));
        proto->Set("destroyImageset", FunctionTemplate::New(GUIDestroyImageset));
        proto->Set("destroyWidget", FunctionTemplate::New(GUIDestroyWidget));
        proto->Set("getWidget", FunctionTemplate::New(GUIGetWidget));
        proto->Set("hideCursor", FunctionTemplate::New(GUIHideCursor));
        proto->Set("isImagesetPresent", FunctionTemplate::New(GUIIsImagesetPresent));
        proto->Set("loadLayout", FunctionTemplate::New(GUILoadLayout));
        proto->Set("loadScheme", FunctionTemplate::New(GUILoadScheme));
        proto->Set("setMouseCursor", FunctionTemplate::New(GUISetMouseCursor));
        proto->Set("showCursor", FunctionTemplate::New(GUIShowCursor));
        proto->Set("toString", FunctionTemplate::New(GUIToString));
        
      }
      Local<Object> instance = s_guiTemplate->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      
      Persistent<v8::Object> pobj = v8::Persistent<v8::Object>::New(instance);
      pobj.MakeWeak(NULL, &GUIDestructor);
      return handle_scope.Close(instance);

   }

   
   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::GUI* UnwrapGui(v8::Handle<v8::Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);

      dtEntity::GUI* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsGui(v8::Handle<v8::Value> val)
   {
      if(s_guiTemplate.IsEmpty())
     {
       return false;
     }
     return s_guiTemplate->HasInstance(val);
   }
}
