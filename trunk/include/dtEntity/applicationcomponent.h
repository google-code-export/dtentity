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

#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/export.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/message.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systeminterface.h>

namespace dtEntity
{ 

   /**
    * An entity system for holding the dtEntity application
    * and providing some time management methods
	 */
   class DT_ENTITY_EXPORT ApplicationSystem
      : public EntitySystem
      , public ScriptAccessor
   {

   public:
      static const ComponentType TYPE;
      static const StringId TimeScaleId;
      static const StringId CmdLineArgsId;

      ApplicationSystem(EntityManager& em);
      ~ApplicationSystem();

      ComponentType GetComponentType() const { return TYPE; }

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
      Timer_t GetSimulationClockTime() const;

      /**
       * @return The current real clock time. This is in MICRO SECONDS (seconds * 1000000LL).
       */
      static Timer_t GetRealClockTime();

      /**
       * Change the time settings.
       * @param newTime The new simulation time. In SECONDS.
       * @param newTimeScale the new simulation time progression as a factor of real time.
       * @param newClockTime  The new simulation wall-clock time. In MICRO SECONDs (seconds * 1000000LL).
       */
      void ChangeTimeSettings(double newTime, float newTimeScale, const Timer_t& newClockTime);

	  /** Functor reacting to SetComponentPropertiesMessage */
      void OnSetComponentProperties(const Message& msg);
	  
	  /** Functor reacting to SetSystemPropertiesMessageandling is not perfect; the  */
      void OnSetSystemProperties(const Message& msg);

      /** adds input callback to cameras */
      void OnCameraAdded(const Message& msg);      

      void AddCmdLineArg(const std::string& arg)
      {
         mArgvArray.Add(new StringProperty(arg));
      }

      ArrayProperty::size_type GetNumCmdLineArgs() const { return  mArgvArray.Size(); }
      std::string GetCmdLineArg(ArrayProperty::size_type i) { return mArgvArray.Get()[i]->StringValue(); }

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

      ArrayProperty mArgvArray;

      DynamicFloatProperty mTimeScale;

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
