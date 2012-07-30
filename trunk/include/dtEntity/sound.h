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
* Luca Vezzadini
*/

#ifndef DTENTITY_SOUND
#define DTENTITY_SOUND

// sound.h: Declaration of the Sound class.
//
////////////////////////////////////////////////////////////////////////////////

#include <queue>

#include <dtEntity/export.h>

#ifdef __APPLE__
  #include <OpenAL/alut.h>
#else
  #include <AL/alut.h>
#endif

// definitions
#if !defined(BIT)
# define BIT(a) (1L<<a)
#endif

#include <osg/Referenced>
#include <dtEntity/property.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace dtEntity
{
   static const std::string ERROR_CLEARING_STRING = "Clearing Error code "
            "system at start of method, if this appears then an error occurred before this "
            "method was called.";

   /** dtEntity::Sound
    *
    * dtEntity::Sound is a little more than just an interface to an object
    * held within (and protected) by the dtEntity::AudioManager.
    *
    * Sound objects are not usually created directly by the user (new/delete).
    * Instead the user requests a new sound from the AudioManager:
    *
    *    Sound* mysound = AudioManager::GetInstance().NewSound();
    *
    * The user can then call any of the Sound interface functions.
    * After the user is finished with Sound, it should be returned
    * to the AudioManager for recycling:
    *
    *    AudioManager::GetInstance().FreeSound( mysound );
    *
    * Sounds do not usually directly call the underlying sound-engine functions,
    * but rather send commands to their command queue.  The AudioManager will
    * run through all Sounds that have been initialized by it and process
    * their queues at frame time.
    *
    * Since the Sound commands (play, stop, pitch, etc.) may not happen
    * immediately, Sound has two callback functions which, if set by
    * the user, will get fired off when the Sound actually starts
    * playing and when it actually stops playing.
    *
    *
    * The current version is heavily inspired from the Delta3D one
    *
    */
   class DT_ENTITY_EXPORT Sound : public osg::Referenced
   {
   public:
      typedef void (*CallBack)(Sound* sound, void* param);  ///callback function type

      class FrameData : public osg::Referenced
      {
      public:
         FrameData();
         FrameData(float gain, float pitch, unsigned int playing = 0);

      protected:
         ~FrameData();

      private:
         FrameData(const FrameData& d);              /// not implemented by design
         FrameData& operator =(const FrameData& d);  /// not implemented by design

         friend class Sound;
         float mGain, mPitch;
         unsigned int mPlaying;
      };

      enum  Command
      {
         NONE = 0L,
         LOAD,
         UNLOAD,
         PLAY,
         PAUSE,
         STOP,
         REWIND,
         LOOP,
         UNLOOP,
         QUEUE,
         GAIN,
         PITCH,
         POSITION,
         DIRECTION,
         VELOCITY,
         ABS,
         REL,
         REF_DIST,
         MAX_DIST,
         ROL_FACT,
         MIN_GAIN,
         MAX_GAIN,

         kNumCommands
      };

   public:
      static const char* kCommand[kNumCommands];

      /// Utility function used to work with OpenAL's error messaging system. 
      /**
         It's used in multiple places throughout dtEntity. It's not in AudioManager
         because we don't want things like Sound to directly access the AudioManager.
         @return True on error, false if no error
      */
      static bool CheckForError(const std::string& userMessage, const std::string& msgFunction, int lineNumber);


      /// Constructor
      /**
       * Typically, user does not create directly
       * instead requests a sound from AudioManager.  The AudioManager
       * facilitates sound data buffer management.
       */
      Sound();




   protected:
      /**
       * Destructor. Typically, user does not delete directly
       * instead frees sound to the AudioManager so that any buffer(s)
       * that need to be freed also are freed (or not freed... in case
       * other Sounds are using them).
       */
      virtual ~Sound();

      /**
       * Message handler.
       */
      virtual void OnPostFrame();

      // Get the state of the indicated flag.
      unsigned int GetState(unsigned int flag) const { return mCommandState & BIT(flag); }

      // set indicated flags in mCommandState
      void SetState(unsigned int flag);      

      // Reset indicated flags:
      void ResetState(unsigned int flag) { mCommandState &= ~BIT(flag); }

      // Restore source state information... usually called right before
	   // source is to be played.  Sources are usually deallocated on 
      // stop in order to conserve sound hardware resources.
      //
      // Returns false on failure to restore source.
      bool RestoreSource();

   public:

      bool GetMustLoadBuffer()         {return mMustLoadBuffer; }
      void SetMustLoadBuffer(bool val) {mMustLoadBuffer = val; }

      /**
       * Loads the specified sound file.
       *
       * @param file the name of the file to load
       */
      void LoadFile(const char* file);

      /**
       * Unloads the specified sound file.
       */
      void UnloadFile();

      ///clean up Sound for recyclying
      void Clear();      

      /* 
       * Stops sound and releases sound source memory.
       *
       * @return false if something went wrong while releasing.
       */
      bool ReleaseSource();

      ///Run all commands in the Sound's command queue (also empties the queue)
      void RunAllCommandsInQueue();

      /// Get this sound's OpenAL buffer ID
      ALint GetBuffer();

      /**
       * Returns the name of the loaded sound file.
       *
       * @return the name of the loaded file
       */
      const char* GetFilename() { return mFilename.c_str(); }
      /// Returns the OpenAL Source ID associated with the loaded Sound object.
      ALuint GetSource() { return mSource; }

      /// Get the IsInitialized flag
      bool IsInitialized() { return mIsInitialized; }

      /// get the IsLooping flag
      bool IsLooping() const;
      ///Get the IsListenerRelative flag
      bool IsListenerRelative() const;
      ///Get the IsPaused flag
      int IsPaused() const { return GetState(PAUSE); }

      ///Get the IsPlaying flag
      int IsPlaying() const { return GetState(PLAY); }

      ///Get the IsStopped flag
      int IsStopped() const { return GetState(STOP); }

      /** Set the Sound's OpenAL buffer ID without going through the
       *  AudioManager.  The typical case is to go through the AudioManager,
       *  but this method is provided for exception cases.
       *
       *  NOTE: This is an advanced operation!! Typically a Sound's OpenAL
       *  buffer is set automatically via the AudioManager.  Only tinker with
       *  OpenAL buffers directly if you know what you are doing.
       */
      void SetBuffer(ALint b);
      /// Set the IsInitialized flag
      void SetInitialized(bool isInit) { mIsInitialized = isInit; }

      /**
       * Set callback for when sound starts playing.
       *
       * @param cb callback function pointer
       * @param param any supplied user data
       */
      virtual void SetPlayCallback(CallBack cb, void* param);

      /**
       * Set callback for when sound stops playing.
       *
       * @param cb callback function pointer
       * @param param any supplied user data
       */
      virtual void SetStopCallback(CallBack cb, void* param);

      /**
       * Sets the OpenAL Source ID associated with the loaded Sound object.
       *
       * NOTE: This is an advanced operation!! Typically sounds are loaded
       * and sources are set automatically via the AudioManager.  Only tinker
       * with OpenAL sources directly if you know what you are doing.
       */
      void SetSource(ALuint s);

      /**
       * Tells the AudioManager to start playing this on the next frame step.
       */
      void Play();

      /* 
       * Starts playing a sound without reference to the AudioManager.
       *
       * @return true if playing successfully started, false otherwise
       */
      bool PlayImmediately();

      /**
       * Tells the AudioManager to start/stop playing this sound on the next
       * frame step.  Note that this does NOT have an effect on the return 
       * value of the IsPlaying method (only on the value of IsPaused).
       */
      void Pause();
      
      /*
       * Start/stops playing this sound without reference to the AudioManager.
       * Note that this does NOT have an effect on the return 
       * value of the IsPlaying method (only on the value of IsPaused).
       */
      void PauseImmediately();

      /**
       * Tells the Audio Manager to stop playing the sound at the next frame step.
       */
      void Stop();
      /**
       * Stops the sound without reference to the AudioManager. Usually you
       * want to go through the AudioManager but this method is provided for
       * exceptions to that rule.
       */
      void StopImmediately();

      /**
       * Tells the AudioManager to rewind to the beginning of this sound
       * at the next frame step.  The sound is also stopped.
       */
      void Rewind();

      /* 
	   * Rewinds a sound immediately without referencing the AudioManager.
	   * Note that the sound is also stopped.
	   */
      void RewindImmediately();

      /**
       * Sets whether or not to play the sound in a continuous loop.
       *
       * NOTE that if you set a sound to loop when it is stopped, it will
       * start playing.  To prevent this, use the IsStopped() method before
       * firing the SetLooping method if you want stopped sounds to stay
       * stopped (you would then need to fire a Play() method sometime
       * thereafter to get the stopped looping Sound started up).
       *
       * @param loop : True to play the sound in a loop, otherwise false
       *               (plays sound one time then stops)
       */
      void SetLooping(bool loop = true);

      /**
       * Sets the gain of the sound source.
       *
       * @param gain the new gain
       */
      void SetGain(float gain);

      /**
       * Returns the gain of the sound source.
       *
       * @return the current gain
       */
      float GetGain() const;

      /**
       * Sets the time offset into the sound sample to start playing from (default 0).
       *
       * @param seconds the playback position, expressed in seconds (the value will loop back to zero for looping sources)
       */
      void SetPlayTimeOffset(float seconds);

      /**
       * Gets the time offset into the sound sample to start playing from (default 0).
       *
       * @return the playback position, expressed in seconds (the value will loop back to zero for looping sources)
       */
      float GetPlayTimeOffset() const;

      /**
       * Sets the pitch multiplier of the sound source.
       * The value is clamped between 0.000001 and 128,
       * but some implementations inexplicably won't take a pitch greater than 2.0, so if this is called
       * with greater than 2.0, the code will attempt to set it, but if fails, it will attempt to set to 2.0.
       *
       * @param pitch the new pitch
       */
      void SetPitch(float pitch);

      /**
       * Returns the pitch multipier of the sound source.
       *
       * @return the current pitch
       */
      float GetPitch() const;

      /**
       * Flags sound to be relative to listener position.
       *
       * IT IS IMPORTANT TO UNDERSTAND EXACTLY WHAT THIS MEANS, otherwise
       * confusion ensues:
       *
       * When you enable Relative mode on a Sound source then its Position, Velocity
       * and Orientation all become relative to the Listener's parameters rather
       * than absolute values.
       *
       * Therefore: calling this function has no effect on the Listnener it
       * ONLY affects the Sound source.
       *
       * You almost never want to set ListenerRelative to be true-- if a
       * Sound is in relative mode then when the Listener is moved
       * the Sound moves with it. For that reason, dtEntity defaults all Sounds
       * to relative = false when Sounds are created.
       *
       */
      void SetListenerRelative(bool relative);

      /**
       * Set the position of the Sound
       *
       * @param position to set
       */
      void SetPosition(const Vec3f& position);

      /**
       * Get the position of sound.
       *
       * @param position to get
       */
      void GetPosition(Vec3f& position) const;

      /**
       * Returns the position of the Sound.
       *       
       */
      Vec3f& GetPosition();

      /**
       * Set the direction of sound.
       *
       * If this is not zero, then the source is automatically considered as
       * "directional" by OpenAL, which means its intensity is not the same
       * in all directions. Check OpenAL specs for details (see AL_DIRECTION).
       *
       * @param direction to set
       */
      void SetDirection(const Vec3f& direction);


      /**
       * Get the direction of sound.
       *
       * @param direction to get
       */
      void GetDirection(Vec3f& direction) const;

      /*
       * Return the direction of the Sound
       */
      Vec3f GetDirection();

      /**
       * Set the velocity of sound.
       *
       * @param velocity to set
       */
      void SetVelocity(const Vec3f& velocity);

      /**
       * Get the velocity of sound.
       *
       * @param velocity to get
       */
      void GetVelocity(Vec3f& velocity) const;

      /// Returns the velocity vector of the Sound.
      Vec3f GetVelocity();

      /**
       * Sets the distance where there will no longer be any attenuation of
       * the source.  Used with the Inverse Clamped Distance model.
       *
       * @see dmINVCLAMP of the AudioConfigData struct in AudioManager.h
       * @param dist the maximum distance
       */
      void SetMaxDistance(float dist);

      /**
       * Get the maximum distance that sound plays at min_gain.
       *
       * @return distance maximum
       */
      float GetMaxDistance() const;

      /**
       * Sets the distance from the source position where there will be no 
       * attenuation of the source. 
       * Used with the Inverse Clamped Distance model.
       *
       * @param dist the reference distance
       */
      void SetReferenceDistance(float dist); 

      /**
       * Get the reference distance that a sound plays at max_gain.
       *
       * @return distance reference
       */
      float GetReferenceDistance() const;

      /**
       * Set the rolloff factor describing attenuation curve.
       *
       * @param rolloff factor to set
       */
      void SetRolloffFactor(float rolloff);

      /**
       * Get the rolloff factor describing attenuation curve.
       *
       * @return rolloff factor
       */
      float GetRolloffFactor() const;

      /**
       * Set the minimum gain that sound plays at.
       * Attenuation is clamped to this gain.  Range is 0.0 - 1.0
       *
       * @param gain set to minimum
       */
      void SetMinGain(float gain);

      /**
       * Get the minimum gain that sound plays at.
       *
       * @return gain minimum
       */
      float GetMinGain() const;

      /**
       * Set the maximum gain that sound plays at.
       * Attenuation is clamped to this gain.  Range is 0.0 - 1.0
       *
       * @param gain set to maximum
       */
      void SetMaxGain(float gain);

      /**
       * Get the maximum gain that sound plays at.
       *
       * @return gain maximum
       */
      float GetMaxGain() const;

      /**
       * Generates and returns a key frame that represents the
       * complete recordable state of this object.
       *
       * @return a new key frame
       */
      FrameData* CreateFrameData() const;

      /** 
       * @param data The Sound::FrameData containing the Sound's state information.
       */
      void UseFrameData(const FrameData* data);


      /**
       * Get the duration of time (in seconds) it takes to play this sound.
       *
       * This method assumes that the entire sound is buffered (so this
       * will be inaccurate if the sound is streamed). Also this does NOT
       * take any Doppler effects into account. This does however take the
       * currently set pitch into account, and scales accordingly.
       *
       * @return The duration, in seconds, that the sound would play.
       */
      float GetDurationOfPlay() const;

   protected:
      std::string mFilename;

      /// Flag indicating if this sound is waiting for its buffer to be loaded
      /**
         If set to true, at the next update the audio manager will take care of
         loading the proper data file so this sound object will be ready to play.
      */
      bool mMustLoadBuffer;

      CallBack    mPlayCB;
      void*       mPlayCBData;
      CallBack    mStopCB;
      void*       mStopCBData;

      std::queue<const char*> mCommand;
      unsigned int            mCommandState;

      ALuint                  mSource;
      ALint                   mBuffer;
      bool                    mIsInitialized;

      ALfloat                 mGain;
      ALfloat                 mPitch;
      ALfloat                 mSecondOffset;
      ALfloat                 mMaxDistance;
      ALfloat                 mReferenceDistance;
      ALfloat                 mRolloffFactor;
      ALfloat                 mMinGain;
      ALfloat                 mMaxGain;
      bool                    mListenerRelative;
      Vec3f                   mPosition;
      Vec3f                   mDirection;
      Vec3f                   mVelocity;

      bool                    mUserDefinedSource;
   };
} // namespace dtEntity

////////////////////////////////////////////////////////////////////////////////

#endif // DTENTITY_SOUND
