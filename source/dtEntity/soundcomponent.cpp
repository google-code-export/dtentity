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

#include <dtEntity/soundcomponent.h>

#include <dtEntity/core.h>
#include <dtEntity/audiomanager.h>
#include <dtEntity/sound.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/transformcomponent.h>
#include <osg/Camera>
#include <assert.h>
#include <iostream>

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const StringId SoundComponent::TYPE(dtEntity::SID("Sound"));
   const StringId SoundComponent::SoundPathId(dtEntity::SID("SoundPath"));
   const StringId SoundComponent::AutoPlayId(dtEntity::SID("AutoPlay"));
   const StringId SoundComponent::GainId(dtEntity::SID("Gain"));
   const StringId SoundComponent::PitchId(dtEntity::SID("Pitch"));
   const StringId SoundComponent::RollOffId(dtEntity::SID("RollOff"));
   const StringId SoundComponent::LoopingId(dtEntity::SID("Looping"));
   
   
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

      GetNode()->setName("SoundComponent");

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
   const StringId SoundSystem::TYPE(dtEntity::SID("Sound"));
   const StringId SoundSystem::ListenerGainId(dtEntity::SID("ListenerGain"));
   const StringId SoundSystem::ListenerEntityId(dtEntity::SID("ListenerEntity"));

   ////////////////////////////////////////////////////////////////////////////////
   SoundSystem::SoundSystem(EntityManager& em)
      : DefaultEntitySystem<SoundComponent>(em)
      , mListenerEntity(
           DynamicUIntProperty::SetValueCB(this, &SoundSystem::SetListenerEntity),
           DynamicUIntProperty::GetValueCB(this, &SoundSystem::GetListenerEntity)
        )
     , mListenerEntityTrans(0)
     , mListenerEntityVal(NULL)
   {
      Register(ListenerGainId, &mListenerGain);
      Register(ListenerEntityId, &mListenerEntity);

      mListenerGain = 1.0f;

      mEnterWorldFunctor = MessageFunctor(this, &SoundSystem::OnEnterWorld);
      em.RegisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "SoundSystem::OnEnterWorld");

      mLeaveWorldFunctor = MessageFunctor(this, &SoundSystem::OnLeaveWorld);
      em.RegisterForMessages(EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "SoundSystem::OnLeaveWorld");

      mTickFunctor = MessageFunctor(this, &SoundSystem::OnTick);
      em.RegisterForMessages(TickMessage::TYPE, mTickFunctor, "SoundSystem::OnTick");

      dtEntity::AudioManager::GetInstance().Init();

   }

   ////////////////////////////////////////////////////////////////////////////////
   SoundSystem::~SoundSystem()
   {
      dtEntity::AudioManager::DestroyInstance();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::Finished()
   {
      BaseClass::Finished();
      
      CopyEntityTransformToListener();      
     
      // set global gain
      dtEntity::AudioManager::GetListener()->SetGain(mListenerGain.Get());
   }

   ////////////////////////////////////////////////////////////////////////////////
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
      em.UnregisterForMessages(WindowClosedMessage::TYPE, mWindowClosedFunctor);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnEnterWorld(const Message& msg)
   {
      EntityId eid = static_cast<EntityId>(msg.GetUInt(EntityAddedToSceneMessage::AboutEntityId));
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
   void SoundSystem::OnLeaveWorld(const Message& msg)
   {
      EntityId eid = static_cast<EntityId>(msg.GetUInt(EntityRemovedFromSceneMessage::AboutEntityId));
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
   void SoundSystem::OnTick(const Message& msg)
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
         dtEntity::Sound* soundObj = currSoundComp->GetCurrentSound();
         if (soundObj && soundObj->GetMustLoadBuffer())
         {
            AudioManager::GetInstance().LoadSoundBuffer(*soundObj);
            soundObj->SetMustLoadBuffer(false);
         }

         currSoundComp->Update(tm.GetDeltaSimTime());
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::SetSoundPath(EntityId eid, const std::string& p)
   {

      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->SetSoundPath(p);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::PlaySound(EntityId eid)
   {
      SoundComponent* sc = GetComponent(eid);
      if(sc)
      {
         sc->PlaySound();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::StopSound(EntityId eid)
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
      static const StringId possid = dtEntity::SID("Position");
      static const StringId attsid = dtEntity::SID("Attitude");
      static const StringId eyedirsid = dtEntity::SID("EyeDirection");
      static const StringId upsid = dtEntity::SID("Up");

      if(mListenerEntityTrans)
      {
         osg::Vec3d pos = mListenerEntityTrans->GetVec3d(possid);
         dtEntity::AudioManager::GetListener()->SetPosition(pos);
         
         if(mListenerEntityTrans->Has(attsid))
         {
            osg::Quat att = mListenerEntityTrans->GetQuat(attsid);
            osg::Vec3 eyedir = att * osg::Vec3(0,1,0);
            osg::Vec3 up = att * osg::Vec3(0,0,1);
            dtEntity::AudioManager::GetListener()->SetOrientation(eyedir, up);
         }
         else if(mListenerEntityTrans->Has(eyedirsid) &&
            mListenerEntityTrans->Has(upsid))
         {
            osg::Vec3d eyedir = mListenerEntityTrans->GetVec3d(eyedirsid);
            osg::Vec3d up = mListenerEntityTrans->GetVec3d(upsid);
            dtEntity::AudioManager::GetListener()->SetOrientation(eyedir, up);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundSystem::SetListenerEntity(EntityId id)  
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
   EntityId SoundSystem::GetListenerEntity() const 
   { 
      return mListenerEntityVal; 
   }
}
