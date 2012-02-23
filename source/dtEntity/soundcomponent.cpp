/*
* Delta3D Open Source Game and Simulation Engine
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

#include <dtEntity/soundcomponent.h>

#include <dtEntity/audiomanager.h>
#include <dtEntity/sound.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/applicationcomponent.h>
#include <osg/Camera>
#include <assert.h>
#include <iostream>

namespace dtEntity
{
   
   const StringId SoundComponent::TYPE(SID("Sound"));
   const StringId SoundComponent::SoundPathId(SID("SoundPath"));
   const StringId SoundComponent::AutoPlayId(SID("AutoPlay"));
   const StringId SoundComponent::GainId(SID("Gain"));
   const StringId SoundComponent::PitchId(SID("Pitch"));
   const StringId SoundComponent::RollOffId(SID("RollOff"));
   const StringId SoundComponent::LoopingId(SID("Looping"));
   
   
   ////////////////////////////////////////////////////////////////////////////
   SoundComponent::SoundComponent()
      : mOwner(NULL)
   {
      Register(SoundPathId, &mSoundPath);
      Register(AutoPlayId, &mAutoPlay);
      Register(GainId, &mGain);
      Register(PitchId, &mPitch);
      Register(RollOffId, &mRollOff);
      Register(LoopingId, &mLooping);

      GetNode()->setName("SoundComponent");

      mGain.Set(1.0f);
      mPitch.Set(1.0f);
      mRollOff.Set(1.0f);
   }

   ////////////////////////////////////////////////////////////////////////////
   SoundComponent::~SoundComponent()
   {
      FreeSound();
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::Finished()
   {
      FreeSound();
           
      if(mSoundPath.Get() == "")
      {
         return;
      }
      mCurrentSound = AudioManager::GetInstance().NewSound();
      assert(mCurrentSound);

      mCurrentSound->LoadFile(mSoundPath.Get().c_str());
      mCurrentSound->SetGain(mGain.Get());
      mCurrentSound->SetRolloffFactor(mRollOff.Get());
      mCurrentSound->SetPitch(mPitch.Get());
      mCurrentSound->SetLooping(mLooping.Get());
      
      if(mAutoPlay.Get())
      {
         PlaySound();
      }      
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::Update(float dt)
   {
      if(!IsPlaying() || mOwner == NULL)
      {
         return;
      }

      // retrieve current PAT and set it to OpenAL
      dtEntity::TransformComponent* comp;
	  if(mOwner->GetEntityManager().GetComponent(mOwner->GetId(), comp, true))
      {
        osg::Vec3 currPos = comp->GetTranslation();
        osg::Vec3 oldPos = mCurrentSound->GetPosition();
        osg::Vec3 velocity = (currPos - oldPos) * (1 / dt);
        mCurrentSound->SetVelocity(velocity);
        mCurrentSound->SetPosition(currPos);
        // TODO - need to set orientation as well...
         
      }
      // also flush all sound commands
      mCurrentSound->RunAllCommandsInQueue();

   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::FreeSound()
   {
      if(mCurrentSound.valid())
      {
         if(mCurrentSound->IsPlaying())
         {
            mCurrentSound->Stop();            
         }
         mCurrentSound->UnloadFile();
         mCurrentSound->ReleaseSource();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::SetSoundPath(const std::string& p) 
   {  
      mSoundPath.Set(p);      
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::PlaySound() 
   {  
      if(mCurrentSound != NULL)
      {
         mCurrentSound->Play();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundComponent::StopSound() 
   {  
      if(mCurrentSound != NULL)
      {
         mCurrentSound->Stop();
      }
   }

   ///////////////////////////////////////////////////////////////////
   bool SoundComponent::IsPlaying() const
   {
      if(!mCurrentSound.valid())
      {
         return false;
      }
      return (mCurrentSound->IsPlaying() != 0);
   }

   ///////////////////////////////////////////////////////////////////
   const StringId SoundSystem::ListenerGainId(SID("ListenerGain"));
   const StringId SoundSystem::ListenerLinkToCameraId(SID("ListenerLinkToCamera"));

   SoundSystem::SoundSystem(EntityManager& em)
      : DefaultEntitySystem<SoundComponent>(em)
      , mListenerLinkToCamera(true)
   {
      Register(ListenerGainId, &mListenerGain);
      Register(ListenerLinkToCameraId, &mListenerLinkToCamera);

      mListenerGain = 1.0f;

      mEnterWorldFunctor = MessageFunctor(this, &SoundSystem::OnEnterWorld);
      em.RegisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "SoundSystem::OnEnterWorld");

      mLeaveWorldFunctor = MessageFunctor(this, &SoundSystem::OnLeaveWorld);
      em.RegisterForMessages(EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "SoundSystem::OnLeaveWorld");

      mTickFunctor = MessageFunctor(this, &SoundSystem::OnTick);
      em.RegisterForMessages(TickMessage::TYPE, mTickFunctor, "SoundSystem::OnTick");

      dtEntity::AudioManager::GetInstance().Init();

   }

   ////////////////////////////////////////////////////////////////////////////
   SoundSystem::~SoundSystem()
   {
      // TODO - Should we move this to the OnRemoveFromEntitymanager?
      dtEntity::AudioManager::DestroyInstance();
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::Finished()
   {
      if (mListenerLinkToCamera.Get())
      {
         CopyCamTransformToListener();
      }
     
      // set global gain
      dtEntity::AudioManager::GetListener()->SetGain(mListenerGain.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         SoundComponent* sc = i->second;
         sc->FreeSound();
      }
      em.UnregisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      em.UnregisterForMessages(EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);
      em.UnregisterForMessages(TickMessage::TYPE, mTickFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnEnterWorld(const Message& msg)
   {
      EntityId eid = (EntityId)msg.GetUInt(EntityAddedToSceneMessage::AboutEntityId);
      SoundComponent* sc;
      if(GetEntityManager().GetComponent(eid, sc))
      {
         if(sc->GetAutoPlay() && sc->GetSoundPath() != "")
         {
            this->PlaySound(eid);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnLeaveWorld(const Message& msg)
   {
      EntityId eid = (EntityId)msg.GetUInt(EntityRemovedFromSceneMessage::AboutEntityId);
      SoundComponent* sc;
      if(GetEntityManager().GetComponent(eid, sc))
      {  
         sc->FreeSound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnTick(const Message& msg)
   {
      if (mListenerLinkToCamera.Get())
      {
	      // copy current camera position to listener...
         CopyCamTransformToListener();
      } 

      // update all sound components now
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         // 1 - check if any sound file needs to be loaded
         SoundComponent* currSoundComp = i->second;
         dtEntity::Sound* soundObj = currSoundComp->GetCurrentSound();
         if (soundObj && soundObj->GetMustLoadBuffer())
         {
            AudioManager::GetInstance().LoadSoundBuffer(*soundObj);
            soundObj->SetMustLoadBuffer(false);
         }

         // 2 - update sound component (set position, flush commands)
         float dt = msg.GetFloat(dtEntity::TickMessage::DeltaSimTimeId);
         currSoundComp->Update(dt);
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::SetSoundPath(EntityId eid, const std::string& p)
   {

      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->SetSoundPath(p);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::PlaySound(EntityId eid)
   {
      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->PlaySound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::StopSound(EntityId eid)
   {
      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->StopSound();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SoundSystem::CopyCamTransformToListener()
   {
      dtEntity::ApplicationSystem* pAppSys;
      if (GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, pAppSys) )
      {
         osg::Camera* currCam = pAppSys->GetPrimaryCamera();
         if(currCam)
         {
            osg::Vec3 camPos, camLookAt, camUp;
            currCam->getViewMatrixAsLookAt(camPos, camLookAt, camUp);
            dtEntity::AudioManager::GetListener()->SetPosition(camPos);
            dtEntity::AudioManager::GetListener()->SetOrientation(camLookAt - camPos, camUp);
         }
         // TODO add velocity to listener
      }
   }
}
