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

#include <dtEntityWrappers/scriptcomponent.h>

#include <dtEntityWrappers/inputhandlerwrapper.h>
#include <dtEntityWrappers/globalfunctions.h>
#include <dtEntityWrappers/wrappers.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/stringid.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/screenwrapper.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <iostream>
#include <sstream>
#include <v8.h>
#include <v8-debug.h>

#include <osg/Version>

#define OSG_2_9_VER (defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR >= 2 && OSG_VERSION_MINOR > 8)

using namespace v8;

namespace dtEntityWrappers
{
   
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   void GCStartCallback(GCType type, GCCallbackFlags flags)
   {
      LOG_DEBUG("V8 garbage collection started");
   };

   ////////////////////////////////////////////////////////////////////////////
   void GCEndCallback(GCType type, GCCallbackFlags flags)
   {
      LOG_DEBUG("V8 garbage collection ended");
      /*HeapStatistics stats;
      V8::GetHeapStatistics(&stats);
      std::cout << "Total: " << stats.total_heap_size() << " Used: " << stats.used_heap_size() <<"\n";
      */
   };

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ScriptSystem::TYPE(dtEntity::SID("Script")); 
   const dtEntity::StringId ScriptSystem::ScriptsId(dtEntity::SID("Scripts"));
   const dtEntity::StringId ScriptSystem::DebugPortId(dtEntity::SID("DebugPort"));
   const dtEntity::StringId ScriptSystem::DebugEnabledId(dtEntity::SID("DebugEnabled"));

   ScriptSystem::ScriptSystem(dtEntity::EntityManager& em)
      : dtEntity::EntitySystem(TYPE, em)
      , mDebugPortOpened(false)
   {      

      V8::Initialize();
      RegisterGlobalFunctions();
      Register(ScriptsId, &mScripts);
      Register(DebugPortId, &mDebugPort);
      Register(DebugEnabledId, &mDebugEnabled);
      mDebugPort.Set(9222);
      
      HandleScope handle_scope;

      Handle<Context> context = GetGlobalContext();
      Context::Scope context_scope(context);
     
      SetupContext();
      V8::AddGCPrologueCallback(GCStartCallback);
      V8::AddGCEpilogueCallback(GCEndCallback);

      mSceneLoadedFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnSceneLoaded);
      GetEntityManager().RegisterForMessages(dtEntity::SceneLoadedMessage::TYPE, mSceneLoadedFunctor, "ScriptSystem::OnSceneLoaded");

      mResetSystemFunctor = dtEntity::MessageFunctor(this, &ScriptSystem::OnResetSystem);
      GetEntityManager().RegisterForMessages(dtEntity::ResetSystemMessage::TYPE, mResetSystemFunctor,
                             dtEntity::FilterOptions::PRIORITY_HIGHER, "ScriptSystem::OnResetSystem");

   }  

   ////////////////////////////////////////////////////////////////////////////
   ScriptSystem::~ScriptSystem()
   {
      V8::RemoveGCPrologueCallback(GCStartCallback);
      V8::RemoveGCEpilogueCallback(GCEndCallback);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::SetupContext()
   {
      HandleScope handle_scope;

      InitializeAllWrappers(GetEntityManager());

      Handle<Context> context = GetGlobalContext();
      Context::Scope context_scope(context);
      Handle<FunctionTemplate> tmplt = FunctionTemplate::New();
      tmplt->InstanceTemplate()->SetInternalFieldCount(2);

      tmplt->SetClassName(String::New("ScriptSystem"));

      dtEntity::ApplicationSystem* as;
      GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, as);
      mView = as->GetPrimaryView();
      osgViewer::GraphicsWindow* window = as->GetPrimaryWindow();


      context->Global()->Set(String::New("Axis"), WrapAxes(&as->GetInputHandler()));
      context->Global()->Set(String::New("Input"), WrapInputHandler(&as->GetInputHandler()));
      context->Global()->Set(String::New("Key"), WrapKeys(&as->GetInputHandler()));
      context->Global()->Set(String::New("MouseWheelState"), WrapMouseWheelStates());
      context->Global()->Set(String::New("Screen"), WrapScreen(mView, window));
      context->Global()->Set(String::New("TouchPhase"), WrapTouchPhases());
      context->Global()->Set(String::New("Priority"), WrapPriorities());

   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnFinishedSettingProperties()
   {
      if(mDebugEnabled.Get() && !mDebugPortOpened)
      {
         mDebugPortOpened = true;
         unsigned int dbgport = mDebugPort.Get();
         std::string flags = "--debugger-agent";
         v8::V8::SetFlagsFromString(flags.c_str(), flags.length());
         v8::Debug::EnableAgent("DtScript", dbgport);
         std::ostringstream os; 
         os << "Enabling JavaScript debugging on port " << dbgport;
         LOG_DEBUG(os.str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnSceneLoaded(const dtEntity::Message& msg)
   {
      dtEntity::PropertyArray scripts = mScripts.Get();
      dtEntity::PropertyArray::const_iterator i;
      for(i = scripts.begin(); i != scripts.end(); ++i)
      {
         std::string script = (*i)->StringValue();
         v8::HandleScope scope;
         v8::Handle<v8::Value> val = WrapperManager::GetInstance().ExecuteFile(script);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::OnResetSystem(const dtEntity::Message& msg)
   {
      UnregisterJavaScriptFromMessages();
      WrapperManager::GetInstance().ResetGlobalContext();

      SetupContext();
      LoadAutoStartScripts("AutoStartScripts");
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::ExecuteFile(const std::string& path)
   {
      v8::HandleScope scope;
      WrapperManager::GetInstance().ExecuteFile(path);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::ExecuteScript(const std::string& script)
   {
      v8::HandleScope scope;
      WrapperManager::GetInstance().ExecuteJS(script);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ScriptSystem::LoadAutoStartScripts(const std::string& path)
   {
      HandleScope handle_scope;
      Handle<Context> global = GetGlobalContext();
      Context::Scope context_scope(global);

      std::set<std::string> executed;

      osgDB::FilePathList paths = osgDB::getDataFilePathList();
      for(osgDB::FilePathList::iterator i = paths.begin(); i != paths.end(); ++i)
      {
         std::ostringstream autostartpath;
         autostartpath << *i << "/" << path;
         
         osgDB::FilePathList currentPathList;
         currentPathList.push_back(autostartpath.str());
         const std::string absPath = osgDB::findDataFile(autostartpath.str());

         if(!osgDB::fileExists(absPath))
         {
            continue;
         }         
         
         osgDB::DirectoryContents contents = osgDB::getDirectoryContents(absPath);

         osgDB::DirectoryContents::const_iterator j;
         for(j = contents.begin(); j != contents.end(); ++j)
         {
            std::string filepath = *j;
            if(osgDB::getFileExtension(filepath).compare("js") != 0) continue;
            if(executed.find(filepath) == executed.end())
            {
               ExecuteFile(path + "/" + filepath);
               executed.insert(filepath);
            }
         }
      }      
   }
}
