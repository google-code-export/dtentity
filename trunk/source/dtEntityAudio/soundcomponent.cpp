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

#include <dtEntityAudio/soundcomponent.h>

#include <dtEntity/core.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/systemmessages.h>
#include <dtEntityAudio/audiomanager.h>
#include <dtEntityAudio/sound.h>
#include <osg/Camera>
#include <assert.h>
#include <iostream>

namespace dtEntityAudio
{
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId SoundComponent::TYPE(dtEntity::SID("Sound"));
   const dtEntity::StringId SoundComponent::SoundPathId(dtEntity::SID("SoundPath"));
   const dtEntity::StringId SoundComponent::AutoPlayId(dtEntity::SID("AutoPlay"));
   const dtEntity::StringId SoundComponent::GainId(dtEntity::SID("Gain"));
   const dtEntity::StringId SoundComponent::PitchId(dtEntity::SID("Pitch"));
   const dtEntity::StringId SoundComponent::RollOffId(dtEntity::SID("RollOff"));
   const dtEntity::StringId SoundComponent::LoopingId(dtEntity::SID("Looping"));
   
   
   ////////////////////////////////////////////////////////////////////////////////
   SoundComponent::SoundComponent()
      : mOwner(NULL)
   {
      Register(SoundPathId, &mSoundPath);
      Register(AutoPlayId, &mAutoPlay);
      Register(GainId, &mGain);
      Register(PitchId, &mPitch);
      Register(RollOffId, &mRollOff);
      Register(LoopingId, &mLooping);

      mGain.Set(1.0f);
      mPitch.Set(1.0f);
      mRollOff.Set(1.0f);
   }

   ////////////////////////////////////////////////////////////////////////////////
   SoundComponent::~SoundComponent()
   {
      FreeSound();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundComponent::Finished()
   {
      BaseClass::Finished();
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

   ////////////////////////////////////////////////////////////////////////////////
   void SoundComponent::Update(float dt)
   {
      if(!IsPlaying() || mOwner == NULL)
      {
         return;
      }

      // retrieve current PAT and set it to OpenAL
      static const dtEntity::StringId patsid = dtEntity::SID("PositionAttitudeTransform");
      static const dtEntity::StringId matrixsid = dtEntity::SID("MatrixTransform");
      static const dtEntity::StringId possid = dtEntity::SID("Position");
      static const dtEntity::StringId matsid = dtEntity::SID("Matrix");
      dtEntity::Component* comp;
      if(mOwner->GetEntityManager().GetComponent(mOwner->GetId(), patsid, comp, true))
      {
        osg::Vec3d currPos = comp->GetVec3d(possid);
        osg::Vec3f oldPos = mCurrentSound->GetPosition();
        osg::Vec3f velocity = (currPos - oldPos) * (1 / dt);
        mCurrentSound->SetVelocity(velocity);
        mCurrentSound->SetPosition(currPos);
        // TODO - need to set orientation as well...
         
      }
      else if(mOwner->GetEntityManager().GetComponent(mOwner->GetId(), matrixsid, comp, true))
      {
         dtEntity::Matrix mat = comp->GetMatrix(matsid);

         osg::Vec3d currPos = mat.getTrans();
         osg::Vec3f oldPos = mCurrentSound->GetPosition();
         osg::Vec3f velocity = (currPos - oldPos) * (1 / dt);
         mCurrentSound->SetVelocity(velocity);
         mCurrentSound->SetPosition(currPos);
         // TODO - need to set orientation as well...

      }
      // also flush all sound commands
      mCurrentSound->RunAllCommandsInQueue();

   }

   ////////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////////
   void SoundComponent::SetSoundPath(const std::string& p) 
   {  
      mSoundPath.Set(p);      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundComponent::PlaySound() 
   {  
      if(mCurrentSound != NULL)
      {
         mCurrentSound->Play();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundComponent::StopSound() 
   {  
      if(mCurrentSound != NULL)
      {
         mCurrentSound->Stop();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::IsPlaying() const
   {
      if(!mCurrentSound.valid())
      {
         return false;
      }
      return (mCurrentSound->IsPlaying() != 0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId SoundSystem::TYPE(dtEntity::SID("Sound"));
   const dtEntity::StringId SoundSystem::ListenerGainId(dtEntity::SID("ListenerGain"));
   const dtEntity::StringId SoundSystem::ListenerEntityId(dtEntity::SID("ListenerEntity"));

   ////////////////////////////////////////////////////////////////////////////////
   SoundSystem::SoundSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<SoundComponent>(em)
      , mListenerEntity(
           dtEntity::DynamicUIntProperty::SetValueCB(this, &SoundSystem::SetListenerEntity),
           dtEntity::DynamicUIntProperty::GetValueCB(this, &SoundSystem::GetListenerEntity)
        )
     , mListenerEntityTrans(0)
     , mListenerEntityVal(0)
   {
      Register(ListenerGainId, &mListenerGain);
      Register(ListenerEntityId, &mListenerEntity);

      mListenerGain = 1.0f;

      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &SoundSystem::OnEnterWorld);
      em.RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "SoundSystem::OnEnterWorld");

      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &SoundSystem::OnLeaveWorld);
      em.RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "SoundSystem::OnLeaveWorld");

      mTickFunctor = dtEntity::MessageFunctor(this, &SoundSystem::OnTick);
      em.RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor, "SoundSystem::OnTick");

      dtEntityAudio::AudioManager::GetInstance().Init();

   }

   ////////////////////////////////////////////////////////////////////////////////
   SoundSystem::~SoundSystem()
   {
      dtEntityAudio::AudioManager::DestroyInstance();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::Finished()
   {
      BaseClass::Finished();
      
      CopyEntityTransformToListener();      
     
      // set global gain
      dtEntityAudio::AudioManager::GetListener()->SetGain(mListenerGain.Get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         SoundComponent* sc = i->second;
         sc->FreeSound();
      }
      em.UnregisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      em.UnregisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);
      em.UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
      em.UnregisterForMessages(dtEntity::WindowClosedMessage::TYPE, mWindowClosedFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnEnterWorld(const dtEntity::Message& msg)
   {
      dtEntity::EntityId eid = static_cast<dtEntity::EntityId>(msg.GetUInt(dtEntity::EntityAddedToSceneMessage::AboutEntityId));
      SoundComponent* sc;
      if(GetEntityManager().GetComponent(eid, sc))
      {
         if(sc->GetAutoPlay() && sc->GetSoundPath() != "")
         {
            this->PlaySound(eid);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnLeaveWorld(const dtEntity::Message& msg)
   {
      dtEntity::EntityId eid = static_cast<dtEntity::EntityId>(msg.GetUInt(dtEntity::EntityRemovedFromSceneMessage::AboutEntityId));
      SoundComponent* sc;
      if(GetEntityManager().GetComponent(eid, sc))
      {  
         sc->FreeSound();
      }
      if(eid == mListenerEntityVal)
      {
         mListenerEntityVal = 0;
         mListenerEntityTrans = 0;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnTick(const dtEntity::Message& msg)
   {
      // shortcut
      if(mComponents.empty())
      {
         return;
      }

      // 2 - update sound component (set position, flush commands)
      const dtEntity::TickMessage& tm = static_cast<const dtEntity::TickMessage&>(msg);

      // copy current camera position to listener...
      CopyEntityTransformToListener();
       

      // update all sound components now
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         // 1 - check if any sound file needs to be loaded
         SoundComponent* currSoundComp = i->second;
         dtEntityAudio::Sound* soundObj = currSoundComp->GetCurrentSound();
         if (soundObj && soundObj->GetMustLoadBuffer())
         {
            AudioManager::GetInstance().LoadSoundBuffer(*soundObj);
            soundObj->SetMustLoadBuffer(false);
         }

         currSoundComp->Update(tm.GetDeltaSimTime());
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::SetSoundPath(dtEntity::EntityId eid, const std::string& p)
   {

      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->SetSoundPath(p);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::PlaySound(dtEntity::EntityId eid)
   {
      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->PlaySound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::StopSound(dtEntity::EntityId eid)
   {
      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->StopSound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::CopyEntityTransformToListener()
   {
      static const dtEntity::StringId possid = dtEntity::SID("Position");
      static const dtEntity::StringId attsid = dtEntity::SID("Attitude");
      static const dtEntity::StringId eyedirsid = dtEntity::SID("EyeDirection");
      static const dtEntity::StringId upsid = dtEntity::SID("Up");

      if(mListenerEntityTrans)
      {
         osg::Vec3d pos = mListenerEntityTrans->GetVec3d(possid);
         dtEntityAudio::AudioManager::GetListener()->SetPosition(pos);
         
         if(mListenerEntityTrans->Has(attsid))
         {
            osg::Quat att = mListenerEntityTrans->GetQuat(attsid);
            osg::Vec3 eyedir = att * osg::Vec3(0,1,0);
            osg::Vec3 up = att * osg::Vec3(0,0,1);
            dtEntityAudio::AudioManager::GetListener()->SetOrientation(eyedir, up);
         }
         else if(mListenerEntityTrans->Has(eyedirsid) &&
            mListenerEntityTrans->Has(upsid))
         {
            osg::Vec3d eyedir = mListenerEntityTrans->GetVec3d(eyedirsid);
            osg::Vec3d up = mListenerEntityTrans->GetVec3d(upsid);
            dtEntityAudio::AudioManager::GetListener()->SetOrientation(eyedir, up);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::SetListenerEntity(dtEntity::EntityId id)  
   { 
      
      bool success = GetEntityManager().GetComponent(id, dtEntity::SID("Transform"), mListenerEntityTrans, true);
      if(success)
      {  
         mListenerEntityVal = id; 
      }
      else
      {
         mListenerEntityTrans = NULL;
         mListenerEntityVal = 0;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId SoundSystem::GetListenerEntity() const 
   { 
      return mListenerEntityVal; 
   }
}
