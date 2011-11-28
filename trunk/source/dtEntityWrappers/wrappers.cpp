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
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/debugdrawmanagerwrapper.h>
#include <dtEntityWrappers/buffer.h>
#include <dtEntityWrappers/file.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntityWrappers/loggerwrapper.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntityWrappers/mapsystemwrapper.h>
#include <dtEntityWrappers/v8helpers.h>

#include <v8.h>

#if BUILD_CAL3D_WRAPPER
  #include <dtEntityWrappers/animationsystemwrapper.h>
#endif

#if BUILD_CEGUI_WRAPPER
  #include <dtEntity/gui.h>
  #include <dtEntityWrappers/guiwrapper.h>
  #include <osgViewer/View>
#endif


#if BUILD_OPENAL_WRAPPER
  #include <dtEntityWrappers/soundsystemwrapper.h>
#endif

namespace dtEntityWrappers
{
   using namespace v8;

   ////////////////////////////////////////////////////////////////////////////////
   void InitializeAllWrappers(dtEntity::EntityManager& em)
   {

      RegisterGlobalFunctions();
      HandleScope handle_scope;
      Handle<Context> context = GetGlobalContext();
      Context::Scope context_scope(context);

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
      
      
#if BUILD_CEGUI_WRAPPER
      
      as->GetViewer()->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
      // make main view gl context current before creating gui
      dtEntity::GUI* gui = new dtEntity::GUI(em, as->GetPrimaryCamera());
      context->Global()->Set(String::New("GUI"), WrapGui(gui));
#endif

      context->Global()->Set(String::New("DebugDrawManager"), CreateDebugDrawManager());
      //context->Global()->Set(String::New("Layer"), FunctionTemplate::New(CreateNewLayer)->GetFunction());

      // make entity manager accessible as a global variable
      context->Global()->Set(String::New("EntityManager"), WrapEntityManager(&em, &mapsystem->GetMessageFactory()));

      context->Global()->Set(String::New("Buffer"), CreateBuffer());
      context->Global()->Set(String::New("File"), CreateFile());
      context->Global()->Set(String::New("Log"), WrapLogger());

      InitMapSystemWrapper();
      
#if BUILD_OPENAL_WRAPPER
      InitSoundSystemWrapper();
#endif

#if BUILD_CAL3D_WRAPPER
      InitAnimationSystemWrapper();
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ExecuteScript(const std::string& path)
   {
      HandleScope scope;
      WrapperManager::GetInstance().ExecuteFile(path);
   }
}
