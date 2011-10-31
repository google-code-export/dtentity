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

#include <dtEntity/basemessages.h>
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
#include <osgViewer/CompositeViewer>

#ifdef BUILD_WITH_DELTA3D 
	#include <dtABC/application.h>
	#include <dtCore/system.h>
#endif

namespace dtEntity
{   
   class DtEntityUpdateCallback;

   class ApplicationImpl
   {


   public:

#ifdef BUILD_WITH_DELTA3D 
      osg::ref_ptr<dtABC::Application> mApplication;
#endif
      osg::ref_ptr<osgViewer::ViewerBase> mViewer;
      osg::ref_ptr<DtEntityUpdateCallback> mUpdateCallback;
      osg::ref_ptr<const osg::FrameStamp> mLastFrameStamp;
      osg::ref_ptr<WindowManager> mWindowManager;
   };

   ////////////////////////////////////////////////////////////////////////////////

   class DtEntityUpdateCallback : public osg::NodeCallback
   {
      ApplicationSystem* mApplicationSystem;
      osg::Timer_t mStartOfFrameTick;
      double mPrevSimTime;

   public:
     
      DtEntityUpdateCallback(ApplicationSystem* as)
         : mApplicationSystem(as)
         , mStartOfFrameTick(osg::Timer::instance()->tick())
         , mPrevSimTime(0)
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {

         const osg::FrameStamp* fs = nv->getFrameStamp();
         double simtime = fs->getSimulationTime();
         osg::Timer_t currentTick = osg::Timer::instance()->tick();
         double deltaTime = osg::Timer::instance()->delta_s(mStartOfFrameTick, currentTick);
         float timeScale = mApplicationSystem->GetTimeScale();
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
 
   ////////////////////////////////////////////////////////////////////////////////
   ApplicationSystem::ApplicationSystem(EntityManager& em)
      : EntitySystem(TYPE, em)
      , mImpl(new ApplicationImpl())
      , mInputHandler(new InputHandler(em))
   {
      Register(TimeScaleId, &mTimeScale);
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
                             FilterOptions::PRIORITY_DEFAULT, "ApplicationSystem::OnResetSystem");

#ifdef BUILD_WITH_DELTA3D 
      SetWindowManager(new D3DWindowManager(em));
#else
      SetWindowManager(new OSGWindowManager(em));
#endif
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
   void ApplicationSystem::SetApplication(dtABC::Application* app)
   {

#ifdef BUILD_WITH_DELTA3D

      assert(mImpl->mApplication == NULL);
      mImpl->mApplication = app;
      SetViewer(app->GetCompositeViewer());
      app->GetCamera()->RemoveSender(&dtCore::System::GetInstance());
      CreateSceneGraphEntities();
#endif
   }

   //////////////////////////////////////////////////////////////////////////////
   dtABC::Application* ApplicationSystem::GetApplication() const
   {
#ifdef BUILD_WITH_DELTA3D
      return mImpl->mApplication;
#else
      return NULL;
#endif
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
   void ApplicationSystem::SetNextStatisticsType() const
   {
#ifdef BUILD_WITH_DELTA3D
      mImpl->mApplication->SetNextStatisticsType();
#else
      LOG_ERROR("TODO: Implement!");
#endif
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
      mImpl->mViewer->getWindows(wins);
      return wins.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Camera* ApplicationSystem::GetPrimaryCamera() const
   {
      osgViewer::CompositeViewer::Cameras cams;
      mImpl->mViewer->getCameras(cams);
      return cams.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::SetViewer(osgViewer::ViewerBase* viewer)
   {
      mImpl->mViewer = viewer;
      
      if(!mImpl->mUpdateCallback.valid())
      {
         mImpl->mUpdateCallback = new DtEntityUpdateCallback(this);
      }
      GetPrimaryView()->getSceneData()->addUpdateCallback(mImpl->mUpdateCallback);

   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::ViewerBase* ApplicationSystem::GetViewer() const
   {
      return mImpl->mViewer;
   }

   ///////////////////////////////////////////////////////////////////////////////
   float ApplicationSystem::GetTimeScale() const
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
      if(!mImpl->mLastFrameStamp.valid()) return 0;
      std::tm tm;
      mImpl->mLastFrameStamp->getCalendarTime(tm);
      return mktime(&tm);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Timer_t ApplicationSystem::GetRealClockTime() const
   {
      return osg::Timer::instance()->tick();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Property* ApplicationSystem::ScriptGetSimulationClockTimeString(const PropertyArgs& args)
   {
      std::string ret = "";
      if(mImpl->mLastFrameStamp.valid())
      {
         std::tm tm;
         mImpl->mLastFrameStamp->getCalendarTime(tm);
         std::ostringstream os;
         os << tm.tm_mday <<"." << tm.tm_mon << "." << (tm.tm_year + 1900);
         os << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec;
         ret = os.str();
      }
      return new StringProperty(ret);      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::ChangeTimeSettings(double newTime, float newTimeScale, const osg::Timer_t& newClockTime)
   {
      mTimeScale.Set(newTimeScale);
#ifdef BUILD_WITH_DELTA3D
      dtCore::System::GetInstance().SetSimulationClockTime(newClockTime);
      dtCore::System::GetInstance().SetSimulationTime(newTime);
      dtCore::System::GetInstance().SetTimeScale(newTimeScale);
#else
      LOG_ERROR("TODO: Implement!");
#endif
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
      osg::Timer_t newclocktime = args[2]->DoubleValue();
      ChangeTimeSettings(newtime, newtimescale, newclocktime);
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::CreateSceneGraphEntities()
   {
      dtEntity::LayerAttachPointSystem* layersys;
      GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      layersys->CreateSceneGraphRootEntity(GetPrimaryView()->getSceneData()->asGroup());      
      
      if(GetPrimaryCamera()->getName() == "")
      {
         GetPrimaryCamera()->setName("defaultCam");
      }
     // AddCameraToSceneGraph(GetPrimaryCamera());
      layersys->GetSceneGraphRoot()->addChild(mInputHandler);
   }

   ///////////////////////////////////////////////////////////////////////////////
   /*void ApplicationSystem::AddCameraToSceneGraph(osg::Camera* cam)
   {
      CameraSystem* camsystem;
      GetEntityManager().GetEntitySystem(CameraComponent::TYPE, camsystem);

      CameraSystem::ComponentStore::iterator i;
      for(i = camsystem->begin(); i != camsystem->end(); ++i)
      {
         CameraComponent* camcomp = i->second;
         if(camcomp->GetIsMainCamera())
         {
            if(camcomp->GetCamera() != cam)
            {
               camcomp->SetCamera(cam);
            }
            return;
         }
      }

      // no camera in map found, wrap camera in entity
      dtEntity::MapSystem* ms;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, ms);

      dtEntity::Entity* entity;
      GetEntityManager().CreateEntity(entity);

      dtEntity::CameraComponent* camcomp;
      GetEntityManager().CreateComponent(entity->GetId(), camcomp);

      camcomp->SetCamera(cam);

      dtEntity::MapComponent* mapComponent;
      GetEntityManager().CreateComponent(entity->GetId(), mapComponent);
      mapComponent->SetUniqueId(cam->getName());
      
      std::string camname = cam->getName() + std::string("_camera");
      mapComponent->SetEntityName(camname);
      mapComponent->SetUniqueId(camname);
   }*/

   ///////////////////////////////////////////////////////////////////////////////
   void ApplicationSystem::OnSetComponentProperties(const Message& m)
   {
      const SetComponentPropertiesMessage& msg = static_cast<const SetComponentPropertiesMessage&>(m);
      
      ComponentType ctype = msg.GetComponentType();
      std::string uniqueid = msg.GetEntityUniqueId();

      MapSystem* ms;
      GetEntityManager().GetEntitySystem(MapComponent::TYPE, ms);
      EntityId id = ms->GetEntityIdByUniqueId(uniqueid);

      Component* component;      
      bool found = GetEntityManager().GetComponent(id, ctype, component);
         
      if(!found)
      {
         LOG_ERROR("Cannot process SetComponentProperties message. Component not found: " 
            + GetStringFromSID(msg.GetComponentType()));
         return;
      }
      PropertyGroup props = msg.GetProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = component->Get(i->first);
         if(!target)
         {
            std::ostringstream os;
            os << "Cannot process SetComponentProperties message. Component ";
            os << GetStringFromSID(msg.GetComponentType());
            os << " has no property named ";
            os << GetStringFromSID(i->first);
            LOG_ERROR(os.str());
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
      EntitySystem* sys = GetEntityManager().GetEntitySystem(msg.GetComponentType());
      if(sys == NULL)
      {
         LOG_ERROR("Cannot process SetSystemProperties message. Entity system not found: " 
            + GetStringFromSID(msg.GetComponentType()));
         return;
      }
      PropertyGroup props = msg.GetProperties();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         Property* target = sys->Get(i->first);
         if(!target)
         {
            std::ostringstream os;
            os << "Cannot process SetSystemProperties message. Entity system ";
            os << GetStringFromSID(msg.GetComponentType());
            os << " has no property named ";
            os << GetStringFromSID(i->first);
            LOG_ERROR(os.str());
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
               t != dtEntity::SID("Camera") &&
               t != dtEntity::SID("Application") &&
               t != dtEntity::SID("Layer") &&
               t != dtEntity::SID("LayerAttachPoint") &&
               t != dtEntity::SID("Group") &&
               t != dtEntity::SID("StaticMesh") &&
               t != dtEntity::SID("MatrixTransform") &&
               t != dtEntity::SID("Map") &&
               t != dtEntity::SID("Script") &&
               t != dtEntity::SID("PositionAttitudeTransform"))
         {
           GetEntityManager().RemoveEntitySystem(**i);
         }
      }

      CreateSceneGraphEntities();

      if(m.GetSceneName() != "")
      {
         mapsys->LoadScene(m.GetSceneName());
      }
   }
}
