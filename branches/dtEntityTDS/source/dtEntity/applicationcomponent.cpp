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

#include <dtEntity/basemessages.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/windowmanager.h>
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
   class DtEntityUpdateCallback;

   class ApplicationImpl
   {
   public:

      osg::observer_ptr<osgViewer::ViewerBase> mViewer;
      osg::ref_ptr<DtEntityUpdateCallback> mUpdateCallback;
      osg::ref_ptr<const osg::FrameStamp> mLastFrameStamp;
      osg::ref_ptr<WindowManager> mWindowManager;
   };

   ////////////////////////////////////////////////////////////////////////////////

   class DtEntityUpdateCallback : public osg::NodeCallback
   {
      ApplicationSystem* mApplicationSystem;
      osg::Timer_t mStartOfFrameTick;
      osg::Timer_t mSimulationClockTime;
      double mPrevSimTime;
      unsigned int mLastFrameNumber;

   public:
     
      DtEntityUpdateCallback(ApplicationSystem* as)
         : mApplicationSystem(as)
         , mStartOfFrameTick(osg::Timer::instance()->tick())
         , mSimulationClockTime(0)
         , mPrevSimTime(0)
         , mLastFrameNumber(0)
      {
         time_t t;
         time(&t);
         osg::Timer_t time = t;
         mSimulationClockTime = time / osg::Timer::instance()->getSecondsPerTick();
      }

      void SetSimulationClockTime(osg::Timer_t t) { mSimulationClockTime = t; }
      osg::Timer_t GetSimulationClockTime() const { return mSimulationClockTime; }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {

         
         const osg::FrameStamp* fs = nv->getFrameStamp();

         if(fs->getFrameNumber() == mLastFrameNumber)
         {
            return;
         }
         mLastFrameNumber = fs->getFrameNumber();

         double simtime = fs->getSimulationTime();
         osg::Timer_t currentTick = osg::Timer::instance()->tick();

         double deltaTime = osg::Timer::instance()->delta_s(mStartOfFrameTick, currentTick);
         
         double timeScale = mApplicationSystem->GetTimeScale();
         double add = (timeScale * deltaTime) / osg::Timer::instance()->getSecondsPerTick();
         mSimulationClockTime += add;
        // fs->setCalendarTime(mSimulationClockTime);
         double deltaSimTime = simtime - mPrevSimTime;
         mPrevSimTime = simtime;
         mStartOfFrameTick = currentTick;
         
         
         dtEntity::EntityManager& em = mApplicationSystem->GetEntityManager();

         {
            dtEntity::PostFrameMessage msg;
            msg.SetDeltaSimTime(deltaSimTime);
            msg.SetDeltaRealTime(deltaTime);
            msg.SetSimTimeScale(timeScale);
            msg.SetSimulationTime(simtime);
            em.EmitMessage(msg);
         }

         {
            dtEntity::TickMessage msg;
            msg.SetDeltaSimTime(deltaSimTime);
            msg.SetDeltaRealTime(deltaTime);
            msg.SetSimTimeScale(timeScale);
            msg.SetSimulationTime(simtime);
            em.EmitMessage(msg);
         }
         em.EmitQueuedMessages(simtime);
         {
            dtEntity::EndOfFrameMessage msg;
            msg.SetDeltaSimTime(deltaSimTime);
            msg.SetDeltaRealTime(deltaTime);
            msg.SetSimTimeScale(timeScale);
            msg.SetSimulationTime(simtime);
            em.EmitMessage(msg);
         }

         mApplicationSystem->mImpl->mLastFrameStamp = fs;
         traverse(node,nv);

         
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   const StringId ApplicationSystem::TYPE(SID("Application"));
   const StringId ApplicationSystem::TimeScaleId(SID("TimeScale"));
   const StringId ApplicationSystem::CmdLineArgsId(SID("CmdLineArgs"));
 
   ////////////////////////////////////////////////////////////////////////////////
   ApplicationSystem::ApplicationSystem(EntityManager& em)
      : EntitySystem(TYPE, em)
      , mImpl(new ApplicationImpl())
   {

      mImpl->mUpdateCallback = new DtEntityUpdateCallback(this);

      Register(TimeScaleId, &mTimeScale);
      Register(CmdLineArgsId, &mArgvArray);

      mTimeScale.Set(1);

      AddScriptedMethod("getTimeScale", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetTimeScale));
      AddScriptedMethod("getSimulationTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimulationTime));
      AddScriptedMethod("getSimTimeSinceStartup", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimTimeSinceStartup));
      AddScriptedMethod("getSimulationClockTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimulationClockTime));
      AddScriptedMethod("getRealClockTime", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetRealClockTime));
      AddScriptedMethod("changeTimeSettings", ScriptMethodFunctor(this, &ApplicationSystem::ScriptChangeTimeSettings));
      AddScriptedMethod("getSimulationClockTimeString", ScriptMethodFunctor(this, &ApplicationSystem::ScriptGetSimulationClockTimeString));

      mSetComponentPropertiesFunctor = MessageFunctor(this, &ApplicationSystem::OnSetComponentProperties);
      em.RegisterForMessages(SetComponentPropertiesMessage::TYPE, mSetComponentPropertiesFunctor, "ApplicationSystem::OnSetComponentProperties");

      mSetSystemPropertiesFunctor = MessageFunctor(this, &ApplicationSystem::OnSetSystemProperties);
      em.RegisterForMessages(SetSystemPropertiesMessage::TYPE, mSetSystemPropertiesFunctor, "ApplicationSystem::OnSetSystemPropertie");

      mResetSystemFunctor = MessageFunctor(this, &ApplicationSystem::OnResetSystem);
      em.RegisterForMessages(ResetSystemMessage::TYPE, mResetSystemFunctor,
                             FilterOptions::ORDER_DEFAULT, "ApplicationSystem::OnResetSystem");

      mCameraAddedFunctor = MessageFunctor(this, &ApplicationSystem::OnCameraAdded);
      em.RegisterForMessages(CameraAddedMessage::TYPE, mCameraAddedFunctor, "ApplicationSystem::OnCameraAdded");

      SetWindowManager(new OSGWindowManager(em));
   }

   ////////////////////////////////////////////////////////////////////////////
   ApplicationSystem::~ApplicationSystem()
   {
      delete mImpl;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnPropertyChanged(StringId propname, Property& prop)
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::SetWindowManager(WindowManager* wm) 
   { 
      mImpl->mWindowManager = wm; 
   }
         
   //////////////////////////////////////////////////////////////////////////////
   WindowManager* ApplicationSystem::GetWindowManager() const 
   { 
      return mImpl->mWindowManager.get(); 
   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::View* ApplicationSystem::GetPrimaryView() const
   {
      osgViewer::ViewerBase::Views views;
      mImpl->mViewer->getViews(views);
      return views.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* ApplicationSystem::GetPrimaryWindow() const
   {
      osgViewer::ViewerBase::Windows wins;
      mImpl->mViewer->getWindows(wins, false);
      return wins.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Camera* ApplicationSystem::GetPrimaryCamera() const
   {
      osgViewer::CompositeViewer::Cameras cams;
      mImpl->mViewer->getCameras(cams);
      if(cams.empty())
      {
         return NULL;
      }
      return cams.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::SetViewer(osgViewer::ViewerBase* viewer)
   {
      mImpl->mViewer = viewer;
   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::ViewerBase* ApplicationSystem::GetViewer() const
   {
      return mImpl->mViewer.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   double ApplicationSystem::GetTimeScale() const
   {
      return mTimeScale.Get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   double ApplicationSystem::GetSimulationTime() const
   {
      return mImpl->mLastFrameStamp.valid() ? mImpl->mLastFrameStamp->getSimulationTime() : 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   double ApplicationSystem::GetSimTimeSinceStartup() const
   {
      return mImpl->mLastFrameStamp.valid() ? mImpl->mLastFrameStamp->getReferenceTime() : 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Timer_t ApplicationSystem::GetSimulationClockTime() const
   {
      return mImpl->mUpdateCallback->GetSimulationClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Timer_t ApplicationSystem::GetRealClockTime()
   {
      return osg::Timer::instance()->tick();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Property* ApplicationSystem::ScriptGetSimulationClockTimeString(const PropertyArgs& args)
   {
      time_t time = GetSimulationClockTime() * osg::Timer::instance()->getSecondsPerTick();
      struct tm * ptm = gmtime(&time);
      std::string ret = "";

      if(ptm != NULL)
      {
         std::ostringstream os;
         os << time << "___";
         if(ptm->tm_mday < 10)
         {
            os << "0";
         }
         os << ptm->tm_mday<<".";

         if(ptm->tm_mon < 9)
         {
            os << "0";
         }
         os << (ptm->tm_mon + 1) << "." << (ptm->tm_year + 1900);
         os << " ";
         if(ptm->tm_hour < 10)
         {
            os << "0";
         }
         os << ptm->tm_hour << ":";
         if(ptm->tm_min < 10)
         {
            os << "0";
         }
         os << ptm->tm_min << ":";
         if(ptm->tm_sec < 10)
         {
            os << "0";
         }
         os << ptm->tm_sec;
         ret = os.str();
      }
      return new StringProperty(ret);      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::ChangeTimeSettings(double newTime, double newTimeScale, const osg::Timer_t& newClockTime)
   {
      mTimeScale.Set(newTimeScale);

      osg::Timer_t newstarttick = osg::Timer::instance()->tick() - newTime / osg::Timer::instance()->getSecondsPerTick();
      osgViewer::CompositeViewer* cv = dynamic_cast<osgViewer::CompositeViewer*>(GetViewer());
      if(cv)
      {
         cv->setStartTick(newstarttick);
         // calendar time is ignored for now
      }
      else
      {
         osgViewer::Viewer* v = dynamic_cast<osgViewer::Viewer*>(GetViewer());
         if(v != NULL)
         {
            v->setStartTick(newstarttick);
         }
      }

      mImpl->mUpdateCallback->SetSimulationClockTime(newClockTime);
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
      double newtimescale = args[1]->DoubleValue();
      osg::Timer_t newclocktime = args[2]->DoubleValue();
      ChangeTimeSettings(newtime, newtimescale, newclocktime);
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::InstallUpdateCallback(osg::Node* node)
   {
      node->setUpdateCallback(mImpl->mUpdateCallback);
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
      PropertyGroup props = msg.GetProperties();
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
         component->OnPropertyChanged(i->first, *target);
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
      PropertyGroup props = msg.GetProperties();
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
         sys->OnPropertyChanged(i->first, *target);
      }
      sys->Finished();
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnResetSystem(const Message& msg)
   {
      const ResetSystemMessage& m = static_cast<const ResetSystemMessage&>(msg);

      MapSystem* mapsys;
      GetEntityManager().GetEntitySystem(MapComponent::TYPE, mapsys);
      mapsys->UnloadScene();
      GetEntityManager().KillAllEntities();
      GetEntityManager().GetMessagePump().ClearQueue();
      //GetEntityManager().GetMessagePump().UnregisterAll();

      std::vector<EntitySystem*> es;
      GetEntityManager().GetEntitySystems(es);
      for(std::vector<EntitySystem*>::iterator i = es.begin(); i != es.end(); ++i)
      {
         // TODO this should not be hard coded
         ComponentType t = (*i)->GetComponentType();
         if(*i != this &&
               t != dtEntity::SIDHash("Camera") &&
               t != dtEntity::SIDHash("Application") &&
               t != dtEntity::SIDHash("Layer") &&
               t != dtEntity::SIDHash("LayerAttachPoint") &&
               t != dtEntity::SIDHash("Group") &&
               t != dtEntity::SIDHash("StaticMesh") &&
               t != dtEntity::SIDHash("MatrixTransform") &&
               t != dtEntity::SIDHash("Map") &&
               t != dtEntity::SIDHash("Script") &&
               t != dtEntity::SIDHash("PositionAttitudeTransform"))
         {
           GetEntityManager().RemoveEntitySystem(**i);
         }
      }

      if(m.GetSceneName() != "")
      {
         mapsys->LoadScene(m.GetSceneName());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnCameraAdded(const Message& m)
   {
      const CameraAddedMessage& msg = static_cast<const CameraAddedMessage&>(m);
      CameraComponent* camcomp;
      if(GetEntityManager().GetComponent(msg.GetAboutEntityId(), camcomp))
      {
         camcomp->GetCamera()->setEventCallback(&GetWindowManager()->GetInputHandler());
      }

      LayerAttachPointSystem* lsys;
      GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, lsys);
      if(camcomp->GetLayerAttachPoint() == LayerAttachPointSystem::RootId)
      {
         InstallUpdateCallback(lsys->GetSceneGraphRoot());
      }
      else
      {
         LayerAttachPointComponent* lc;
         if(lsys->GetByName(camcomp->GetLayerAttachPoint(), lc))
         {
            InstallUpdateCallback(lc->GetNode());
         }
         else
         {
            LOG_ERROR("Cannot install update callback for layer attach point " << GetStringFromSID(camcomp->GetLayerAttachPoint()));
         }
      }
   }

}
