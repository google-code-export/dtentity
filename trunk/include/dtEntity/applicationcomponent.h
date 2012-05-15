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

#include <dtEntity/export.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/message.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>
#include <osg/Timer>

namespace osgViewer
{
   class View;
   class Window;
   class ViewerBase;
   class GraphicsWindow;
}

namespace osg
{
   class Camera;
   class Group;
   class Node;
}

namespace dtEntity
{ 

   ////////////////////////////////////////////////////////////////////////////////
   class ApplicationImpl;  
   class DtEntityUpdateCallback;
   class WindowManager;


	/**
    * An entity system for holding the dtEntity application
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

      void OnPropertyChanged(StringId propname, Property &prop);

      ComponentType GetComponentType() const { return TYPE; }

      void EmitTickMessagesAndQueuedMessages();


      /// Returns the UniqueID of this ApplicationSystem
      std::string GetUniqueID() { return mApplicationSystemInfo.mUniqueID; }


      /**
       * @return The scale of realtime the GameManager is running at.
       */
      float GetTimeScale() const;
      void SetTimeScale(float v);

      /**
       * @return the current simulation time. This is in SECONDs.
       */
      double GetSimulationTime() const;

      /**
       * @return the current simulation wall-clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       */
      osg::Timer_t GetSimulationClockTime() const;

      /**
       * @return The current real clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       */
      static osg::Timer_t GetRealClockTime();

      /**
       * Change the time settings.
       * @param newTime The new simulation time. In SECONDS.
       * @param newTimeScale the new simulation time progression as a factor of real time.
       * @param newClockTime  The new simulation wall-clock time. In MICRO SECONDs (seconds * 1000000LL).
       */
      void ChangeTimeSettings(double newTime, float newTimeScale, const osg::Timer_t& newClockTime);

	  /** Functor reacting to SetComponentPropertiesMessage */
      void OnSetComponentProperties(const Message& msg);
	  
	  /** Functor reacting to SetSystemPropertiesMessageandling is not perfect; the  */
      void OnSetSystemProperties(const Message& msg);

      /** reacts to ResetSystemMessage */
      void OnResetSystem(const Message& msg);
      
      /** adds input callback to cameras */
      void OnCameraAdded(const Message& msg);      

      void InstallUpdateCallback(osg::Node*);


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

      /// Holds basic information about the ApplicationSystem instance
      /**
      *  For now we just include a UniqueID (string); later on this might
      *  include additional fields like a name, the network address, ...
      *  The single members are accessed through specific public methods
      *  like GetUniqueID().
      */
      struct ApplicationSystemInfo
      {
         std::string mUniqueID;
      };

      ApplicationSystemInfo mApplicationSystemInfo;

      FloatProperty mTimeScale;
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

      
      // string holding country code
      //StringProperty mLocale;
      MessageFunctor mSetComponentPropertiesFunctor;
      MessageFunctor mSetSystemPropertiesFunctor;
      MessageFunctor mResetSystemFunctor;
      MessageFunctor mCameraAddedFunctor;
      
   };
}
