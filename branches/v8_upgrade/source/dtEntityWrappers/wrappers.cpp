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

#include <dtEntity/core.h>
#include <dtEntity/systeminterface.h>
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


#if BUILD_OPENAL
  #include <dtEntityWrappers/soundsystemwrapper.h>
#endif

namespace dtEntityWrappers
{
   using namespace v8;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> WrapNodeMasks()
   {
      Isolate* isolate = Isolate::GetCurrent();

      EscapableHandleScope scope(isolate);
      Local<Object> o = Object::New(isolate);

      o->Set(String::NewFromUtf8(isolate, "REFLECTION_SCENE"), Uint32::New(isolate, dtEntity::NodeMasks::REFLECTION_SCENE));
      o->Set(String::NewFromUtf8(isolate, "REFRACTION_SCENE"), Uint32::New(isolate, dtEntity::NodeMasks::REFRACTION_SCENE));
      o->Set(String::NewFromUtf8(isolate, "NORMAL_SCENE"),     Uint32::New(isolate, dtEntity::NodeMasks::NORMAL_SCENE));
      o->Set(String::NewFromUtf8(isolate, "SURFACE_MASK"),     Uint32::New(isolate, dtEntity::NodeMasks::SURFACE_MASK));
      o->Set(String::NewFromUtf8(isolate, "SILT_MASK"),        Uint32::New(isolate, dtEntity::NodeMasks::SILT_MASK));
      o->Set(String::NewFromUtf8(isolate, "VISIBLE"),          Uint32::New(isolate, dtEntity::NodeMasks::VISIBLE));
      o->Set(String::NewFromUtf8(isolate, "PICKABLE"),         Uint32::New(isolate, dtEntity::NodeMasks::PICKABLE));
      o->Set(String::NewFromUtf8(isolate, "TERRAIN"),          Uint32::New(isolate, dtEntity::NodeMasks::TERRAIN));
      o->Set(String::NewFromUtf8(isolate, "CASTS_SHADOWS"),    Uint32::New(isolate, dtEntity::NodeMasks::CASTS_SHADOWS));
      o->Set(String::NewFromUtf8(isolate, "RECEIVES_SHADOWS"), Uint32::New(isolate, dtEntity::NodeMasks::RECEIVES_SHADOWS));
      o->Set(String::NewFromUtf8(isolate, "MANIPULATOR"),      Uint32::New(isolate, dtEntity::NodeMasks::MANIPULATOR));

      return scope.Escape(o);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> WrapCmdLineArgs()
   {
      Isolate* isolate = Isolate::GetCurrent();
      dtEntity::SystemInterface* sysinf = dtEntity::GetSystemInterface();

      int argc = sysinf->GetArgC();
      const std::vector<std::string>& argv = sysinf->GetArgV();

      EscapableHandleScope scope(isolate);
      Local<Array> o = Array::New(isolate, argc);

      for(int i = 0; i < argc; ++i)
      {  
         o->Set(i, String::NewFromUtf8(isolate, argv[i].c_str()));
      }
      return scope.Escape(o);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitializeAllWrappers(dtEntity::EntityManager& em)
   {
      
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

      Isolate* isolate = v8::Isolate::GetCurrent();

      HandleScope handle_scope(isolate);
      Handle<Context> context = scriptsystem->GetGlobalContext();      
      Context::Scope context_scope(context);
      
      // TODO - temporary disabled, need porting to newest v8 version
      //context->Global()->Set(String::NewFromUtf8(isolate, "DebugDrawManager"), CreateDebugDrawManager(context));
      
      //context->Global()->Set(String::NewFromUtf8(isolate, "Layer"), FunctionTemplate::New(CreateNewLayer)->GetFunction());

      // make entity manager accessible as a global variable
      context->Global()->Set(String::NewFromUtf8(isolate, "EntityManager"), WrapEntityManager(scriptsystem, &em));

      context->Global()->Set(String::NewFromUtf8(isolate, "NodeMasks"), WrapNodeMasks());

      // TODO - temporary disabled, need porting to newest v8 version
      //context->Global()->Set(String::NewFromUtf8(isolate, "Buffer"), CreateBuffer());
      //context->Global()->Set(String::NewFromUtf8(isolate, "File"), CreateFile());

      context->Global()->Set(String::NewFromUtf8(isolate, "Log"), WrapLogger(context));
      context->Global()->Set(String::NewFromUtf8(isolate, "CmdLineArgs"), WrapCmdLineArgs());

      InitMapSystemWrapper(scriptsystem);
      
#if BUILD_OPENAL
      InitSoundSystemWrapper(scriptsystem);
#endif

   }

}
