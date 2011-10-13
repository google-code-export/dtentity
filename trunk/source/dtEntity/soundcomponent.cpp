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

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtCore/transform.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtUtil/stringutils.h>
#include <assert.h>
#include <iostream>

namespace dtEntity
{
   
   const StringId SoundComponent::TYPE(SID("Sound"));
   const StringId SoundComponent::SoundPathId(SID("SoundPath"));
   const StringId SoundComponent::AutoPlayId(SID("AutoPlay"));
   const StringId SoundComponent::MoveWithTransformId(SID("MoveWithTransform"));
   const StringId SoundComponent::GainId(SID("Gain"));
   const StringId SoundComponent::PitchId(SID("Pitch"));
   const StringId SoundComponent::RollOffId(SID("RollOff"));
   const StringId SoundComponent::LoopingId(SID("Looping"));
   
   
   ////////////////////////////////////////////////////////////////////////////
   SoundComponent::SoundComponent()
      : mOwner(NULL)
      , mTransformComponent(NULL)
   {
      Register(SoundPathId, &mSoundPath);
      Register(AutoPlayId, &mAutoPlay);
      Register(MoveWithTransformId, &mMoveWithTransform);
      Register(GainId, &mGain);
      Register(PitchId, &mPitch);
      Register(RollOffId, &mRollOff);
      Register(LoopingId, &mLooping);
      mMoveWithTransform.Set(true);
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
   void SoundComponent::OnFinishedSettingProperties()
   {
      FreeSound();
           
      if(mSoundPath.Get() == "")
      {
         return;
      }
      mCurrentSound = dtAudio::AudioManager::GetInstance().NewSound();
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
      if(!IsPlaying() || !mMoveWithTransform.Get() || mOwner == NULL)
      {
         return;
      }
      
      if(mTransformComponent == NULL)
      {
         if(!mOwner->GetEntityManager().GetComponent(mOwner->GetId(), mTransformComponent, true))
         {
            mMoveWithTransform.Set(false);
            return;
         }
      }
      
      osg::Vec3 oldpos = mCurrentSound->GetPosition();
      osg::Vec3 trans = mTransformComponent->GetTranslation();
      osg::Vec3 velocity = (trans - oldpos) * (1 / dt);
      //sc->mCurrentSound->SetPosition(trans);
      dtCore::Transform xform;
      xform.SetTranslation(trans);
      mCurrentSound->SetTransform(xform);
      mCurrentSound->SetVelocity(velocity);
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
   const StringId SoundSystem::ListenerTranslationId(SID("ListenerTranslation"));
   const StringId SoundSystem::ListenerUpId(SID("ListenerUp"));
   const StringId SoundSystem::ListenerEyeDirectionId(SID("ListenerEyeDirection"));
   const StringId SoundSystem::ListenerVelocityId(SID("ListenerVelocity"));

   SoundSystem::SoundSystem(EntityManager& em)
      : DefaultEntitySystem<SoundComponent>(em)
   {
      Register(ListenerTranslationId, &mListenerTranslation);
      Register(ListenerUpId, &mListenerUp);
      Register(ListenerEyeDirectionId, &mListenerEyeDirection);
      Register(ListenerVelocityId, &mListenerVelocity);

      mEnterWorldFunctor = MessageFunctor(this, &SoundSystem::OnEnterWorld);
      em.RegisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "SoundSystem::OnEnterWorld");

      mLeaveWorldFunctor = MessageFunctor(this, &SoundSystem::OnLeaveWorld);
      em.RegisterForMessages(EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "SoundSystem::OnLeaveWorld");

      mTickFunctor = MessageFunctor(this, &SoundSystem::OnTick);
      em.RegisterForMessages(TickMessage::TYPE, mTickFunctor, "SoundSystem::OnTick");

      //GetEntityManager().RegisterForMessages(TickMessage::TYPE,
      //   mTickFunctor, FilterOptions::PRIORITY_LOWEST, "SoundSystem::OnTick");

      dtAudio::AudioManager::Instantiate();
   }

   ////////////////////////////////////////////////////////////////////////////
   SoundSystem::~SoundSystem()
   {

   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnFinishedSettingProperties()
   {      
      osg::Matrixd mat;
      mat.setTrans(mListenerTranslation.Get());
      osg::Quat q;
      osg::Vec3d rot = mListenerEyeDirection.Get();
      rot[2] = 0;
      rot.normalize();
      q.makeRotate(osg::Vec3d(0,1,0), rot);
      mat.setRotate(q);
      //mat.makeLookAt(mListenerTranslation.Get(), eye, mListenerUp.Get());
      dtCore::Transform xform;
      //xform.SetTranslation(mListenerTranslation.Get());
      
      xform.Set(mat);
      dtAudio::AudioManager::GetListener()->SetTransform(xform);
      dtAudio::AudioManager::GetListener()->SetVelocity(mListenerVelocity.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void SoundSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         SoundComponent* sc = i->second;
         sc->FreeSound();
      }
      dtAudio::AudioManager::Destroy();
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
      float dt = msg.GetFloat(dtEntity::TickMessage::DeltaSimTimeId);
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         SoundComponent* sc = i->second;
         sc->Update(dt);  
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
}
