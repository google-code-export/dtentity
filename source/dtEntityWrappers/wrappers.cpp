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

#include <dtEntityWrappers/wrappers.h>

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityWrappers/debugdrawmanagerwrapper.h>
#include <dtEntityWrappers/buffer.h>
#include <dtEntityWrappers/file.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/loggerwrapper.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntityWrappers/mapsystemwrapper.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntity/dtentity_config.h>
#include <v8.h>

#if BUILD_CAL3D
  #include <dtEntityWrappers/animationsystemwrapper.h>
#endif

#if BUILD_CEGUI
  #include <dtEntity/gui.h>
  #include <dtEntityWrappers/guiwrapper.h>
  #include <osgViewer/View>
#endif


#if BUILD_OPENAL
  #include <dtEntityWrappers/soundsystemwrapper.h>
#endif

namespace dtEntityWrappers
{
   using namespace v8;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> WrapNodeMasks()
   {
      HandleScope scope;
      Handle<Object> o = Object::New();

      o->Set(String::New("REFLECTION_SCENE"), Uint32::New(dtEntity::NodeMasks::REFLECTION_SCENE));
      o->Set(String::New("REFRACTION_SCENE"), Uint32::New(dtEntity::NodeMasks::REFRACTION_SCENE));
      o->Set(String::New("NORMAL_SCENE"),     Uint32::New(dtEntity::NodeMasks::NORMAL_SCENE));
      o->Set(String::New("SURFACE_MASK"),     Uint32::New(dtEntity::NodeMasks::SURFACE_MASK));
      o->Set(String::New("SILT_MASK"),        Uint32::New(dtEntity::NodeMasks::SILT_MASK));
      o->Set(String::New("VISIBLE"),          Uint32::New(dtEntity::NodeMasks::VISIBLE));
      o->Set(String::New("PICKABLE"),         Uint32::New(dtEntity::NodeMasks::PICKABLE));
      o->Set(String::New("TERRAIN"),          Uint32::New(dtEntity::NodeMasks::TERRAIN));
      o->Set(String::New("CASTS_SHADOWS"),    Uint32::New(dtEntity::NodeMasks::CASTS_SHADOWS));
      o->Set(String::New("RECEIVES_SHADOWS"), Uint32::New(dtEntity::NodeMasks::RECEIVES_SHADOWS));
      o->Set(String::New("MANIPULATOR"),      Uint32::New(dtEntity::NodeMasks::MANIPULATOR));

      return scope.Close(o);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitializeAllWrappers(dtEntity::EntityManager& em)
   {
      
      dtEntity::ApplicationSystem* as;
      if(!em.GetEntitySystem(dtEntity::ApplicationSystem::TYPE, as))
      {
        LOG_ERROR("Could not get application system!");
        return;
      }

      dtEntity::MapSystem* mapsystem;
      if(!em.GetEntitySystem(dtEntity::MapComponent::TYPE, mapsystem))
      {
        LOG_ERROR("Could not get map system!");
        return;
      }

      ScriptSystem* scriptsystem;
      if(!em.GetEntitySystem(ScriptSystem::TYPE, scriptsystem))
      {
        LOG_ERROR("Could not get script system!");
        return;
      }

      HandleScope handle_scope;
      Handle<Context> context = scriptsystem->GetGlobalContext();      
      Context::Scope context_scope(context);
      
#if BUILD_CEGUI
      
      as->GetViewer()->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
      // make main view gl context current before creating gui
      dtEntity::GUI* gui = new dtEntity::GUI(em, as->GetPrimaryCamera());
      context->Global()->Set(String::New("GUI"), WrapGui(scriptsystem, gui));
#endif

      context->Global()->Set(String::New("DebugDrawManager"), CreateDebugDrawManager(context));
      //context->Global()->Set(String::New("Layer"), FunctionTemplate::New(CreateNewLayer)->GetFunction());

      // make entity manager accessible as a global variable
      context->Global()->Set(String::New("EntityManager"), WrapEntityManager(scriptsystem, &em));

      context->Global()->Set(String::New("NodeMasks"), WrapNodeMasks());

      context->Global()->Set(String::New("Buffer"), CreateBuffer());
      context->Global()->Set(String::New("File"), CreateFile());
      context->Global()->Set(String::New("Log"), WrapLogger(context));

      InitMapSystemWrapper(scriptsystem);
      
#if BUILD_OPENAL
      InitSoundSystemWrapper(scriptsystem);
#endif

#if BUILD_CAL3D
      InitAnimationSystemWrapper(scriptsystem);
#endif
   }

}
