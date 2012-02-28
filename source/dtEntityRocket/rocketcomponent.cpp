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

#include "rocketcomponent.h"

#include "eventinstancer.h"
#include "eventlistenerinstancer.h"
#include "rocketsystemwrapper.h"

#include <dtEntity/entity.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/windowmanager.h>
#include <iostream>
#include <osgLibRocket/GuiNode>
#include <osgViewer/CompositeViewer>
#include <osgLibRocket/RenderInterface>
#include <osgLibRocket/SystemInterface>
#include <osgLibRocket/FileInterface>
#include <osgLibRocket/GuiNode>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <Rocket/Controls/Controls.h>

namespace dtEntityRocket
{

   using namespace v8;
   using namespace dtEntityWrappers;

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::String ToRocketString(const v8::Handle<v8::Value>& val)
   {
      v8::String::Utf8Value str(val);
      return *str ? *str : "<string conversion failed>";
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* RocketVariantToProperty(const Rocket::Core::Variant& value)
   {
      switch(value.GetType())
      {
      case Rocket::Core::Variant::BYTE: return new dtEntity::IntProperty(value.Get<int>());break;
      case Rocket::Core::Variant::CHAR: return new dtEntity::CharProperty(value.Get<char>());
      case Rocket::Core::Variant::FLOAT: return new dtEntity::FloatProperty(value.Get<float>());
      case Rocket::Core::Variant::INT: return new dtEntity::IntProperty(value.Get<int>());
      case Rocket::Core::Variant::STRING: return new dtEntity::StringProperty(value.Get<Rocket::Core::String>().CString());
      case Rocket::Core::Variant::WORD: return new dtEntity::UIntProperty(value.Get<Rocket::Core::word>());;
      case Rocket::Core::Variant::VECTOR2: 
      {
         Rocket::Core::Vector2f v = value.Get<Rocket::Core::Vector2f>();
         return new dtEntity::Vec2Property(osg::Vec2(v.x, v.y));
      }
      case Rocket::Core::Variant::COLOURF: 
      {
         Rocket::Core::Colourf c = value.Get<Rocket::Core::Colourf>();
         return new dtEntity::Vec4Property(osg::Vec4(c.red, c.green, c.blue, c.alpha));
      }
      case Rocket::Core::Variant::COLOURB:
      {
         Rocket::Core::Colourb c = value.Get<Rocket::Core::Colourb>();
         return new dtEntity::Vec4Property(osg::Vec4(c.red / 256.0f, c.green / 256.0f, c.blue / 256.0f, c.alpha / 256.0f));
      }
      case Rocket::Core::Variant::SCRIPTINTERFACE: return NULL;
      case Rocket::Core::Variant::VOIDPTR:  return NULL;
      default: return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> RocketVariantToVal(const Rocket::Core::Variant& value)
   {
      switch(value.GetType())
      {
      case Rocket::Core::Variant::BYTE: return Int32::New(value.Get<int>());
      case Rocket::Core::Variant::CHAR: return Int32::New(value.Get<char>());
      case Rocket::Core::Variant::FLOAT: return Number::New(value.Get<float>());
      case Rocket::Core::Variant::INT: return Int32::New(value.Get<int>());
      case Rocket::Core::Variant::STRING: return String::New(value.Get<Rocket::Core::String>().CString());
      case Rocket::Core::Variant::WORD: return Uint32::New(value.Get<Rocket::Core::word>());;
      case Rocket::Core::Variant::VECTOR2: 
      {
         Rocket::Core::Vector2f v = value.Get<Rocket::Core::Vector2f>();
         
         HandleScope scope;
         Handle<Array> a = Array::New();
         a->Set(Integer::New(0), Number::New(v.x));
         a->Set(Integer::New(1), Number::New(v.y));
         return scope.Close(a);
      }
      case Rocket::Core::Variant::COLOURF: 
      {
         Rocket::Core::Colourf c = value.Get<Rocket::Core::Colourf>();
         
         HandleScope scope;
         Handle<Array> a = Array::New();
         a->Set(Integer::New(0), Number::New(c.red));
         a->Set(Integer::New(1), Number::New(c.green));
         a->Set(Integer::New(2), Number::New(c.blue));
         a->Set(Integer::New(3), Number::New(c.alpha));
         return scope.Close(a);
      }
      case Rocket::Core::Variant::COLOURB:
      {
         Rocket::Core::Colourf c = value.Get<Rocket::Core::Colourf>();
         
         HandleScope scope;
         Handle<Array> a = Array::New();
         a->Set(Integer::New(0), Number::New(c.red / 256.0f));
         a->Set(Integer::New(1), Number::New(c.green / 256.0f));
         a->Set(Integer::New(2), Number::New(c.blue / 256.0f));
         a->Set(Integer::New(3), Number::New(c.alpha / 256.0f));
         return scope.Close(a);
      }
      case Rocket::Core::Variant::SCRIPTINTERFACE: return Null();
      case Rocket::Core::Variant::VOIDPTR:  return Null();
      default: return Null();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   Rocket::Core::Variant ValToRocketVariant(v8::Handle<v8::Value> value)
   {
      Rocket::Core::Variant ret;
      if(value->IsString())
      {
         ret.Set(ToRocketString(value));
      }
      else if(value->IsBoolean())
      {
         ret.Set(value->BooleanValue());
      }
      else if(value->IsUint32())
      {
         int v = value->Uint32Value();
         ret.Set(v);
      }
      else if(value->IsInt32())
      {
         int v = value->Int32Value();
         ret.Set(v);
      }
      else if(value->IsNumber())
      {
         ret.Set((float)value->NumberValue());
      }
      else if(value->IsNull())
      {
         void* v = 0;
         ret.Set(v);
      }
      else
      {
         LOG_ERROR("Cannot convert rocket variant to javascript value!");
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId RocketComponent::TYPE(dtEntity::SID("Rocket"));
   const dtEntity::StringId RocketComponent::FullScreenId(dtEntity::SID("FullScreen"));
   const dtEntity::StringId RocketComponent::ContextNameId(dtEntity::SID("ContextName"));
   const dtEntity::StringId RocketComponent::DebugId(dtEntity::SID("Debug"));


   ////////////////////////////////////////////////////////////////////////////
   RocketComponent::RocketComponent()
      : mEntity(NULL)
   {
      Register(FullScreenId, &mFullScreen);
      Register(ContextNameId, &mContextName);
      Register(DebugId, &mDebug);
      mContextName.Set("RocketContext");
      mDebug.Set(false);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   RocketComponent::~RocketComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void RocketComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      BaseClass::OnAddedToEntity(e);
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void RocketComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
     BaseClass::OnRemovedFromEntity(e);
     osgLibRocket::GuiNode* gui = dynamic_cast<osgLibRocket::GuiNode*>(GetNode());
     if(gui)
     {
       dtEntity::ApplicationSystem* appsys;
       mEntity->GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
       appsys->GetPrimaryView()->removeEventHandler(gui->GetGUIEventHandler());
     }
   }

   ////////////////////////////////////////////////////////////////////////////
   Rocket::Core::Context* RocketComponent::GetRocketContext()
   {
      osgLibRocket::GuiNode* gui = static_cast<osgLibRocket::GuiNode*>(GetNode());
      if(gui)
      {
         return gui->getContext();
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   void RocketComponent::Finished()
   {

     // GuiNode is responsible for rendering the LibRocket GUI to OSG
     osgLibRocket::GuiNode* gui = new osgLibRocket::GuiNode(mContextName.Get(), mDebug.Get());
     SetNode(gui);

     dtEntity::ApplicationSystem* appsys;
     mEntity->GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);

     appsys->GetPrimaryView()->removeEventHandler(gui->GetGUIEventHandler());
     appsys->GetPrimaryView()->getEventHandlers().push_front(gui->GetGUIEventHandler());

     if(mFullScreen.Get())
     {
        // create a camera that will be rendered after the main OSG scene.
       // This is adapted from osghud example
       osg::ref_ptr<osg::Camera> cam = new osg::Camera();
       cam->setName("Rocket HUD");
       cam->setClearMask(GL_DEPTH_BUFFER_BIT);
       cam->setRenderOrder(osg::Camera::POST_RENDER, 100);
       cam->setAllowEventFocus(false);

       // set the view matrix
       cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
       
       osg::GraphicsContext* gc = appsys->GetPrimaryCamera()->getGraphicsContext();
       // same graphics context as main camera
       cam->setGraphicsContext(gc);

       // passing the camera to the gui node makes it render to that cam
       // and adapt the camera settings in accord to the window size
       gui->setCamera(cam);

       // add gui as child to cam
       cam->addChild(gui);

       
       //appsys->GetPrimaryView()->addSlave(cam, false);
       dtEntity::LayerAttachPointSystem* layersys;
       mEntity->GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
       //dtEntity::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();

       // add as first child of scene graph root. This is to make sure
       // that the rocket node is the first to handle event callback.
       layersys->GetSceneGraphRoot()->insertChild(0, cam);
     }
   }

  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   RocketSystem::RocketSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
     // create and set libRocket to OSG interfaces
     osgLibRocket::RenderInterface* renderer = new osgLibRocket::RenderInterface();
     Rocket::Core::SetRenderInterface(renderer);

     osgLibRocket::SystemInterface* system_interface = new osgLibRocket::SystemInterface();
     Rocket::Core::SetSystemInterface(system_interface);

     osgLibRocket::FileInterface* file_interface = new osgLibRocket::FileInterface();
     Rocket::Core::SetFileInterface(file_interface);

     Rocket::Core::Initialise();
     Rocket::Controls::Initialise();

     // EventInstancer* ei = new EventInstancer(GetEntityManager().GetMessagePump());
     // Rocket::Core::Factory::RegisterEventInstancer(ei)->RemoveReference();

     
     dtEntityWrappers::ScriptSystem* scriptsys;
     if(em.GetEntitySystem(dtEntityWrappers::ScriptSystem::TYPE, scriptsys))
     {
        EventListenerInstancer* eli = new EventListenerInstancer(scriptsys->GetGlobalContext(), GetEntityManager().GetMessagePump());
        Rocket::Core::Factory::RegisterEventListenerInstancer(eli)->RemoveReference();
     }
     
	  

   }

   ////////////////////////////////////////////////////////////////////////////
   RocketSystem::~RocketSystem()
   {
   }
}
