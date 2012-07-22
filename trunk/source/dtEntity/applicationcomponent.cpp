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

#include <dtEntity/applicationcomponent.h>

#include <dtEntity/cameracomponent.h>
#include <dtEntity/dtentity_config.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/core.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/uniqueid.h>
#include <dtEntity/osgsysteminterface.h>
#include <dtEntity/osginputinterface.h>

#include <dtEntity/systemmessages.h>
#include <assert.h>

#include <sstream>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <iostream>

namespace dtEntity
{   

   ////////////////////////////////////////////////////////////////////////////////



   ////////////////////////////////////////////////////////////////////////////////
   const StringId ApplicationSystem::TYPE(dtEntity::SID("Application"));
   const StringId ApplicationSystem::TimeScaleId(dtEntity::SID("TimeScale"));
   const StringId ApplicationSystem::CmdLineArgsId(dtEntity::SID("CmdLineArgs"));
 
   ////////////////////////////////////////////////////////////////////////////////
   ApplicationSystem::ApplicationSystem(EntityManager& em)
      : EntitySystem(em)
      , mTimeScale(
           DynamicFloatProperty::SetValueCB(this, &ApplicationSystem::SetTimeScale),
           DynamicFloatProperty::GetValueCB(this, &ApplicationSystem::GetTimeScale)
        )
   {

      // generate a unique ID
      mApplicationSystemInfo.mUniqueID = CreateUniqueIdString();

      Register(TimeScaleId, &mTimeScale);
      Register(CmdLineArgsId, &mArgvArray);

      mTimeScale.Set(1);

      AddScriptedMethod("getTimeScale", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetTimeScale));
      AddScriptedMethod("getSimulationTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimulationTime));
      AddScriptedMethod("getSimulationClockTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimulationClockTime));
      AddScriptedMethod("getRealClockTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetRealClockTime));
      AddScriptedMethod("changeTimeSettings", ScriptMethodFunctor(this, &ApplicationSystem::ScriptChangeTimeSettings));

      mSetComponentPropertiesFunctor = MessageFunctor(this, &ApplicationSystem::OnSetComponentProperties);
      em.RegisterForMessages(SetComponentPropertiesMessage::TYPE, mSetComponentPropertiesFunctor, "ApplicationSystem::OnSetComponentProperties");

      mSetSystemPropertiesFunctor = MessageFunctor(this, &ApplicationSystem::OnSetSystemProperties);
      em.RegisterForMessages(SetSystemPropertiesMessage::TYPE, mSetSystemPropertiesFunctor, "ApplicationSystem::OnSetSystemPropertie");

      mCameraAddedFunctor = MessageFunctor(this, &ApplicationSystem::OnCameraAdded);
      em.RegisterForMessages(CameraAddedMessage::TYPE, mCameraAddedFunctor, "ApplicationSystem::OnCameraAdded");


   }

   ////////////////////////////////////////////////////////////////////////////
   ApplicationSystem::~ApplicationSystem()
   {
      GetEntityManager().UnregisterForMessages(SetComponentPropertiesMessage::TYPE, mSetComponentPropertiesFunctor);
      GetEntityManager().UnregisterForMessages(SetSystemPropertiesMessage::TYPE, mSetSystemPropertiesFunctor);
      GetEntityManager().UnregisterForMessages(CameraAddedMessage::TYPE, mCameraAddedFunctor);
   }

   //////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::EmitTickMessagesAndQueuedMessages()
   {
      SystemInterface* iface = GetSystemInterface();
      float deltasimtime = iface->GetDeltaSimTime();
      float deltarealtime = iface->GetDeltaRealTime();
      float simtimescale = iface->GetTimeScale();
      double simulationtime = iface->GetSimulationTime();

      EntityManager& em = GetEntityManager();
      
      {
         dtEntity::PostFrameMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         em.EmitMessage(msg);
      }

      {
         dtEntity::TickMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         em.EmitMessage(msg);
      }

      em.EmitQueuedMessages(simulationtime);

      {
         dtEntity::EndOfFrameMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         em.EmitMessage(msg);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   float ApplicationSystem::GetTimeScale() const
   {
      return GetSystemInterface()->GetTimeScale();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::SetTimeScale(float v)
   {
      GetSystemInterface()->SetTimeScale(v);
   }

   ///////////////////////////////////////////////////////////////////////////////
   double ApplicationSystem::GetSimulationTime() const
   {
      return GetSystemInterface()->GetSimulationTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Timer_t ApplicationSystem::GetSimulationClockTime() const
   {
      return GetSystemInterface()->GetSimulationClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Timer_t ApplicationSystem::GetRealClockTime()
   {
      return GetSystemInterface()->GetRealClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::ChangeTimeSettings(double newTime, float newTimeScale, const Timer_t& newClockTime)
   {
      mTimeScale.Set(newTimeScale);

      Timer_t newstarttick = osg::Timer::instance()->tick() - newTime / osg::Timer::instance()->getSecondsPerTick();
      osgViewer::ViewerBase* viewer = static_cast<OSGSystemInterface*>(GetSystemInterface())->GetViewer();
      osgViewer::CompositeViewer* cv = dynamic_cast<osgViewer::CompositeViewer*>(viewer);
      if(cv)
      {
         cv->setStartTick(newstarttick);
         // calendar time is ignored for now
      }
      else
      {
         osgViewer::Viewer* v = dynamic_cast<osgViewer::Viewer*>(viewer);
         if(v != NULL)
         {
            v->setStartTick(newstarttick);
         }
      }
      GetSystemInterface()->SetSimulationClockTime(newClockTime);
      TimeChangedMessage msg;
      msg.SetSimulationTime(newTime);
      msg.SetSimulationClockTime(newClockTime);
      msg.SetTimeScale(newTimeScale);
      GetEntityManager().EmitMessage(msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   Property* ApplicationSystem::ScriptChangeTimeSettings(const PropertyArgs& args)
   {
      if(args.size() < 3) 
      {
         LOG_ERROR("Script error: changeTimeSettings expects three arguments");    
         return NULL;
      }
      double newtime = args[0]->DoubleValue();
      float newtimescale = args[1]->FloatValue();
      Timer_t newclocktime = args[2]->DoubleValue();
      ChangeTimeSettings(newtime, newtimescale, newclocktime);
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnSetComponentProperties(const Message& m)
   {
      const SetComponentPropertiesMessage& msg = static_cast<const SetComponentPropertiesMessage&>(m);
      
      ComponentType ctype = dtEntity::SIDHash(msg.GetComponentType());
      std::string uniqueid = msg.GetEntityUniqueId();

      MapSystem* ms;
      GetEntityManager().GetEntitySystem(MapComponent::TYPE, ms);
      EntityId id = ms->GetEntityIdByUniqueId(uniqueid);

      Component* component;      
      bool found = GetEntityManager().GetComponent(id, ctype, component);
         
      if(!found)
      {
         LOG_WARNING("Cannot process SetComponentProperties message. Component not found: " 
            + msg.GetComponentType());
         return;
      }
      const PropertyGroup& props = msg.GetComponentProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = component->Get(i->first);
         if(!target)
         {
            LOG_ERROR(
                "Cannot process SetComponentProperties message. Component "
                << msg.GetComponentType()
                << " has no property named "
                << GetStringFromSID(i->first)
            );
            continue;
         }
         target->SetFrom(*i->second);
#if CALL_ONPROPERTYCHANGED_METHOD
         component->OnPropertyChanged(i->first, *target);
#endif
      }
      component->Finished();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnSetSystemProperties(const Message& m)
   {
      const SetSystemPropertiesMessage& msg = static_cast<const SetSystemPropertiesMessage&>(m);
      EntitySystem* sys = GetEntityManager().GetEntitySystem(SIDHash(msg.GetComponentType()));
      if(sys == NULL)
      {
         LOG_WARNING("Cannot process SetSystemProperties message. Entity system not found: " 
            << msg.GetComponentType());
         return;
      }
      const PropertyGroup& props = msg.GetSystemProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = sys->Get(i->first);
         if(!target)
         {
            LOG_ERROR("Cannot process SetSystemProperties message. Entity system "
                << msg.GetComponentType()
                << " has no property named "
                << GetStringFromSID(i->first));
            continue;
         }
         target->SetFrom(*i->second);
#if CALL_ONPROPERTYCHANGED_METHOD
         sys->OnPropertyChanged(i->first, *target);
#endif
      }
      sys->Finished();
      
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnCameraAdded(const Message& m)
   {
      const CameraAddedMessage& msg = static_cast<const CameraAddedMessage&>(m);
      CameraComponent* camcomp;

      OSGSystemInterface* iface = static_cast<OSGSystemInterface*>(GetSystemInterface());
      OSGInputInterface* wface = static_cast<OSGInputInterface*>(GetInputInterface());
      
      if(!GetEntityManager().GetComponent(msg.GetAboutEntityId(), camcomp))
      {
         LOG_ERROR("Camera not found!");
         return;
      }
      
      osgViewer::View* view = dynamic_cast<osgViewer::View*>(camcomp->GetCamera()->getView());
      if(view)
      {
         osgViewer::View::EventHandlers& eh = view->getEventHandlers();
         if(std::find(eh.begin(), eh.end(), wface) ==  eh.end())
         {
            eh.push_back(wface);
         }
      }
      else
      {
         LOG_ERROR("Encountered unknown view type!");
      }

      LayerAttachPointSystem* lsys;
      GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, lsys);
      if(camcomp->GetLayerAttachPoint() != LayerAttachPointSystem::RootId)
      {
         LayerAttachPointComponent* lc;
         if(lsys->GetByName(camcomp->GetLayerAttachPoint(), lc))
         {

            iface->InstallUpdateCallback(lc->GetNode());
         }
         else
         {
            LOG_ERROR("Cannot install update callback for layer attach point " << GetStringFromSID(camcomp->GetLayerAttachPoint()));
         }
      }
   }

}
