#pragma once

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

#include <dtEntityWrappers/export.h>

#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/property.h>
#include <osgViewer/View>
#include <dtEntity/inputhandler.h>

namespace dtEntityWrappers
{
     
   ////////////////////////////////////////////////////////////////////////////
   class DTENTITY_WRAPPERS_EXPORT ScriptSystem
      : public dtEntity::EntitySystem
   {
      static const dtEntity::StringId ScriptsId;
      static const dtEntity::StringId DebugPortId;
      static const dtEntity::StringId DebugEnabledId;
      
   public:

      static const dtEntity::ComponentType TYPE;

      ScriptSystem(dtEntity::EntityManager& em);
      ~ScriptSystem();

      virtual void Finished();
      void OnSceneLoaded(const dtEntity::Message& msg);
      void OnResetSystem(const dtEntity::Message& msg);
      void OnLoadScript(const dtEntity::Message& msg);
      void Tick(const dtEntity::Message& msg);

      void ExecuteFile(const std::string& path);
      void ExecuteScript(const std::string& script);

      // look in given directory for scripts ending with "*.js",
      // execute them and try to add entity system with name
      // equal to the file name
      void LoadAutoStartScripts(const std::string& path);

      virtual bool StorePropertiesToScene() const { return true; }

   private:
      void SetupContext();
      
      dtEntity::MessageFunctor mSceneLoadedFunctor;
      dtEntity::MessageFunctor mResetSystemFunctor;
      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mLoadScriptFunctor;
      osg::observer_ptr<osgViewer::View> mView;
      
      dtEntity::BoolProperty mDebugEnabled;
      bool mDebugPortOpened;
      dtEntity::ArrayProperty mScripts;
      dtEntity::UIntProperty mDebugPort;
      
   };

}
