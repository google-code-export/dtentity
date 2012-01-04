/* -*-c++-*-
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

#ifndef DELTA_ENTITY_SOUNDCOMPONENT
#define DELTA_ENTITY_SOUNDCOMPONENT

#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/component.h>
#include <dtEntity/message.h>
#include <dtEntity/stringid.h>
#include <osg/Group>
#include <dtEntity/sound.h>


namespace dtEntity
{ 
   ////////////////////////////////////////////////////////////////////////////////

   class SoundSystem;

   class DT_ENTITY_EXPORT SoundComponent : public NodeComponent
   {
   
      friend class SoundSystem;

   public:

      static const ComponentType TYPE;
      static const StringId SoundPathId;
      static const StringId AutoPlayId;
      static const StringId GainId;
      static const StringId PitchId;
      static const StringId RollOffId;
      static const StringId LoopingId;
      
      SoundComponent();
     
      virtual ~SoundComponent();

      virtual ComponentType GetType() const { 
         return TYPE; 
      }

      virtual bool IsInstanceOf(ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual void Finished();
      virtual void OnAddedToEntity(Entity& entity) { mOwner = &entity;}
      virtual void OnRemovedFromEntity(Entity& entity) { mOwner = NULL; }

      dtEntity::Sound* GetCurrentSound() const { return mCurrentSound.get(); }

      void SetSoundPath(const std::string& p);
      std::string GetSoundPath() const { return mSoundPath.Get(); }

      void SetAutoPlay(bool p) { mAutoPlay.Set(p); }
      bool GetAutoPlay() const { return mAutoPlay.Get(); }

      bool IsPlaying() const;
      void PlaySound();
      void StopSound();

      void Update(float dt);
      void FreeSound();

   private:

      StringProperty mSoundPath;
      BoolProperty mAutoPlay;
      FloatProperty mGain;
      FloatProperty mPitch;
      FloatProperty mRollOff;
      BoolProperty mLooping;
      osg::ref_ptr<dtEntity::Sound> mCurrentSound;
      Entity* mOwner;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   /// Manages sound effects
   /**
      The Sound system relies on the AudioManager, a singleton object which acts
      as the main interface to OpenAL.
      This systems is in charge of updating all sound components at each frame, via
      the OnTick method.
   */
   class DT_ENTITY_EXPORT SoundSystem : public DefaultEntitySystem<SoundComponent>
   {
   public:

      SoundSystem(EntityManager& em);
      ~SoundSystem();

      static const StringId ListenerGainId;
      static const StringId ListenerLinkToCameraId;
      static const StringId ListenerTranslationId;
      static const StringId ListenerUpId;
      static const StringId ListenerEyeDirectionId;
      static const StringId ListenerVelocityId;

      //void OnAddedToEntityManager(dtEntity::EntityManager& em)
      void OnRemoveFromEntityManager(dtEntity::EntityManager& em);
      void OnEnterWorld(const Message&);
      void OnLeaveWorld(const Message&);
      void OnTick(const Message& msg);

      /// Override base class behavior to save system properties to file
      virtual bool StorePropertiesToScene() const { return true; }


      virtual void Finished();

      void SetSoundPath(EntityId eid, const std::string& p);
      void PlaySound(EntityId eid);
      void StopSound(EntityId eid);

      void SetListenerLinkToCamera(const bool val)  { mListenerLinkToCamera.Set(val); }
      bool GetListenerLinkToCamera() const { return mListenerLinkToCamera.Get(); }

      void SetListenerTranslation(const osg::Vec3d& pos) { mListenerTranslation.Set(pos); }
      osg::Vec3d GetListenerTranslation() const { return mListenerTranslation.Get(); }

      void SetListenerUp(const osg::Vec3d& pos)  { mListenerUp.Set(pos); }
      osg::Vec3d GetListenerUp() const { return mListenerUp.Get(); }

      void SetListenerEyeDirection(const osg::Vec3d& pos)  { mListenerEyeDirection.Set(pos); }
      osg::Vec3d GetListenerEyeDirection() const { return mListenerEyeDirection.Get(); }

      void SetListenerVelocity(const osg::Vec3d& pos)  { mListenerVelocity.Set(pos); }
      osg::Vec3d GetListenerVelocity() const { return mListenerVelocity.Get(); }

   private:

      /// Internal util that copies current camera position and orientation to listener
      void CopyCamTransformToListener();

      MessageFunctor mEnterWorldFunctor;
      MessageFunctor mLeaveWorldFunctor;
      MessageFunctor mTickFunctor;
      FloatProperty mListenerGain;
      BoolProperty mListenerLinkToCamera;
      Vec3dProperty mListenerTranslation;
      Vec3dProperty mListenerUp;
      Vec3dProperty mListenerEyeDirection;
      Vec3Property mListenerVelocity;
   };
}

#endif // DELTA_ENTITY_SOUNDCOMPONENT
