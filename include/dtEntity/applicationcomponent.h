#pragma once

/* -*-c++-*-
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



#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/component.h>
#include <dtEntity/inputhandler.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>
#include <osg/Group>
#include <osg/Timer>

namespace osgViewer
{
   class View;
   class Window;
   class ViewerBase;
}

namespace dtEntity
{ 

   ////////////////////////////////////////////////////////////////////////////////
   class ApplicationImpl;  
   class DtEntityUpdateCallback;
   class WindowManager;


	/**
    * An entity system for holding the Delta3D application
    * and providing some time management methods
	 */
   class DT_ENTITY_EXPORT ApplicationSystem
      : public EntitySystem
      , public ScriptAccessor
   {

      friend class DtEntityUpdateCallback;

   public:
      static const ComponentType TYPE;
      static const StringId TimeScaleId;
      static const StringId CmdLineArgsId;

      ApplicationSystem(EntityManager& em);
      ~ApplicationSystem();

      virtual void OnPropertyChanged(StringId propname, Property& prop);

      /**
       * @return The scale of realtime the GameManager is running at.
       */
      double GetTimeScale() const;

      /**
       * @return the current simulation time. This is in SECONDs.
       */
      double GetSimulationTime() const;

      /**
       * @return the current simulation time. This is in SECONDS.
       */
      double GetSimTimeSinceStartup() const;

      /**
       * @return the current simulation wall-clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       */
      osg::Timer_t GetSimulationClockTime() const;

      /**
       * @return The current real clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       */
      osg::Timer_t GetRealClockTime() const;

      /**
       * Change the time settings.
       * @param newTime The new simulation time. In SECONDS.
       * @param newTimeScale the new simulation time progression as a factor of real time.
       * @param newClockTime  The new simulation wall-clock time. In MICRO SECONDs (seconds * 1000000LL).
       */
      void ChangeTimeSettings(double newTime, double newTimeScale, const osg::Timer_t& newClockTime);

	  /** Functor reacting to SetComponentPropertiesMessage */
      void OnSetComponentProperties(const Message& msg);
	  
	  /** Functor reacting to SetSystemPropertiesMessageandling is not perfect; the  */
      void OnSetSystemProperties(const Message& msg);

      /** reacts to ResetSystemMessage */
      void OnResetSystem(const Message& msg);
      
      /** create entities for scene graph root and default camera */
      void CreateSceneGraphEntities();

      //void AddCameraToSceneGraph(osg::Camera*);

      InputHandler& GetInputHandler() { return *mInputHandler.get(); }

      void SetWindowManager(WindowManager* wm);
      WindowManager* GetWindowManager() const;

      osgViewer::ViewerBase* GetViewer() const;
      void SetViewer(osgViewer::ViewerBase*);

      osgViewer::View* GetPrimaryView() const;      
      osgViewer::GraphicsWindow* GetPrimaryWindow() const;
      osg::Camera* GetPrimaryCamera() const;

      void AddCmdLineArg(const std::string& arg)
      {
         mArgvArray.Add(new StringProperty(arg));
      }

      int GetNumCmdLineArgs() const { return  mArgvArray.Size(); }
      std::string GetCmdLineArg(int i) { return mArgvArray.Get()[i]->StringValue(); }

   private:

      DoubleProperty mTimeScale;
      ArrayProperty mArgvArray;

      ApplicationImpl* mImpl;

      Property* ScriptGetTimeScale(const PropertyArgs& args)
      {
         return new DoubleProperty(GetTimeScale());
      }

      Property* ScriptGetSimulationTime(const PropertyArgs& args)
      {
         return new DoubleProperty(GetSimulationTime());
      }

      Property* ScriptGetSimTimeSinceStartup(const PropertyArgs& args)
      {
         return new DoubleProperty(GetSimTimeSinceStartup());
      }

      Property* ScriptGetSimulationClockTime(const PropertyArgs& args)
      {
         return new DoubleProperty(GetSimulationClockTime());
      }

      Property* ScriptGetSimulationClockTimeString(const PropertyArgs& args);

      Property* ScriptGetRealClockTime(const PropertyArgs& args)
      {
         return new DoubleProperty(GetRealClockTime());
      }   

      Property* ScriptChangeTimeSettings(const PropertyArgs& args);

      osg::ref_ptr<InputHandler> mInputHandler;
      
      // string holding country code
      //StringProperty mLocale;
      MessageFunctor mSetComponentPropertiesFunctor;
      MessageFunctor mSetSystemPropertiesFunctor;
      MessageFunctor mResetSystemFunctor;
      
   };
}
