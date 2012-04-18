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


#include <dtEntity/audiomanager.h>
#include <dtEntity/log.h>


#include <cassert>
#include <stack>

#include <osg/Vec3>
#include <osg/io_utils>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

#ifdef __APPLE__
  #include <OpenAL/alut.h>
#else
  #include <AL/alut.h>
#endif


#include <iostream>



namespace dtEntity 
{
   AudioManager::LOB_PTR AudioManager::_Mic(NULL);
   const char*           AudioManager::_EaxVer = "EAX2.0";
   const char*           AudioManager::_EaxSet = "EAXSet";
   const char*           AudioManager::_EaxGet = "EAXGet";


   /////////////////////////////////////////////////////////////////////////////
   // OSG Object for holding sound buffer data loaded from the following
   // OSG plugin. This will allow the Audio Manager to access buffer information
   // supplied from alut, after the file has been loaded from memory but before
   // the buffer is registered with OpenAL; this is to allow the Audio Manager
   // to have veto power over the loaded file, before its buffer is
   // officially registered.
   /////////////////////////////////////////////////////////////////////////////
   class WrapperOSGSoundObject : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      WrapperOSGSoundObject()
         : BaseClass()
         , mRawData(NULL)
      {}

      explicit WrapperOSGSoundObject(bool threadSafeRefUnref)
         : BaseClass(threadSafeRefUnref)
         , mRawData(NULL)
      {}

      WrapperOSGSoundObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
         , mRawData(NULL)
      {}

      ALvoid* mRawData;
      AudioManager::BufferData mBufferData;

      META_Object("dtEntity", WrapperOSGSoundObject);
   };

   /////////////////////////////////////////////////////////////////////////////
   // OSG Plugin for loading encrypted/non-encrypted sound files through OSG.
   /////////////////////////////////////////////////////////////////////////////
   class ReaderWriterWAV : public osgDB::ReaderWriter
   {
   public:

      //////////////////////////////////////////////////////////////////////////
      ReaderWriterWAV()
      {
         supportsExtension("wav","Wav sound format");
      }

      //////////////////////////////////////////////////////////////////////////
      virtual const char* className() const { return "WAV Sound Reader"; }

      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult readObject(const std::string& file,
         const osgDB::ReaderWriter::Options* options =NULL) const
      {
         std::string ext = osgDB::getLowerCaseFileExtension(file);
         if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

         if (!osgDB::fileExists(file))
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
         }

         std::ifstream confStream(file.c_str(), std::ios_base::binary);

         if (!confStream.is_open())
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         return readObject(confStream, options);
      }

      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& fin,const Options* = NULL) const
      {
         osgDB::ReaderWriter::ReadResult result = osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED);

         if (fin.fail()) return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);

         fin.seekg(0,std::ios_base::end);
         unsigned int ulzipFileLength = fin.tellg();
         fin.seekg(0,std::ios_base::beg);

         char* memBuffer = new (std::nothrow) char [ulzipFileLength];
         if (memBuffer == NULL)
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         fin.read(memBuffer, ulzipFileLength);

         osg::ref_ptr<WrapperOSGSoundObject> userData = new WrapperOSGSoundObject;
         AudioManager::BufferData& bf = userData->mBufferData;

         // NON-DEPRECATED version for ALUT >= 1.0.0
         ALfloat freq(0);
         userData->mRawData = alutLoadMemoryFromFileImage(
            memBuffer, ALsizei(ulzipFileLength), &bf.format, &bf.size, &freq);
         bf.freq = ALsizei(freq);
         Sound::CheckForError("data = alutLoadMemoryFromFileImage", __FUNCTION__, __LINE__);
         delete [] memBuffer;
         
         return ReaderWriter::ReadResult(userData.get(), ReaderWriter::ReadResult::FILE_LOADED);
      }
   };
   REGISTER_OSGPLUGIN(wav, ReaderWriterWAV)


   ////////////////////////////////////////////////////////////////////////////////
   // public member functions
   // default consructor
   AudioManager::AudioManager()
      : mEAXSet(NULL)
      , mEAXGet(NULL)
      , mNumSounds(0)
      , mIsInitialized(false)
      , mDevice(NULL)
      , mContext(NULL)
      , mShutdownContexts(false)
   {
      ;
   }

   ////////////////////////////////////////////////////////////////////////////////
   // destructor
   AudioManager::~AudioManager()
   {
      _Mic = NULL;

      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      //stop and clear all Sounds
      SND_LST::iterator it;
      for (it = mSoundList.begin(); it != mSoundList.end(); ++it)
      {
         ReleaseSoundSource(**it, "Error freeing source in Audio Manager destructor.",
            __FUNCTION__, __LINE__);
      }

      // delete the buffers
      BufferData* bd(NULL);
      for (BUF_MAP::iterator iter(mBufferMap.begin()); iter != mBufferMap.end(); iter++)
      {
         if ((bd = iter->second) == NULL)
         {
            continue;
         }

         iter->second = NULL;

         ReleaseSoundBuffer(bd->buf, "alDeleteBuffers(1, &bd->buf )", __FUNCTION__, __LINE__);
         delete bd;
      }
      mBufferMap.clear();
      mSoundList.clear();

      alutExit();
      Sound::CheckForError("alutExit()", __FUNCTION__, __LINE__);

      if (mShutdownContexts)
      {
         LOG_INFO("Shutting down custom contexts.");
         // alut won't close the context if it's not the one that created it, so it needs to be done after alutExit()
         // or alut exit won't work.
         CloseContext();
         CloseDevice();
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   // create the singleton manager
   void AudioManager::Init(ALCdevice* dev, ALCcontext* cntxt, bool shutdownPassedInContexts)
   {
      if (mIsInitialized)
         return;        // already done...

      _Mic  = new Listener;
      assert(_Mic.get());


      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      if (dev == NULL && cntxt == NULL)
      {
         if (alutInit(NULL, NULL) == AL_FALSE)
         {
            std::cout << "Error initializing alut" << std::cout;
            Sound::CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
         }
         Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

         mContext = alcGetCurrentContext();
         Sound::CheckForError("Trying to get context. ", __FUNCTION__, __LINE__);

         mDevice = alcGetContextsDevice(mContext);
         Sound::CheckForError("Trying to get context's device. ", __FUNCTION__, __LINE__);
      }
      else
      {
         if(alutInitWithoutContext(NULL, NULL) == AL_FALSE)
         {
            std::cout << "Error initializing alut" << std::cout;
            Sound::CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
         }
         Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

         mDevice = dev;
         mContext = cntxt;
         mShutdownContexts = shutdownPassedInContexts;
      }

      mIsInitialized = true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   // destroy the singleton manager
   void AudioManager::Destroy()
   {
      _Mic = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::SetOpenALDevice(const ALCchar* deviceName)
   {
      //first make sure we cleanup the device and context we were using before
      CloseContext();
      CloseDevice();

      OpenDevice(deviceName);
      CreateContext();
   }

   ////////////////////////////////////////////////////////////////////////////////
   // static listener accessor
   Listener* AudioManager::GetListener()
   {
      return static_cast<Listener*>(_Mic.get());
   }


   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::SetDistanceModel(ALenum dm)
   {
      Sound::CheckForError("Cleanup al error.", __FUNCTION__, __LINE__);
      alDistanceModel(dm);
      Sound::CheckForError("alDistanceModel()", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetDopplerFactor(float f)
   {
      Sound::CheckForError("Cleanup OpenAL error.", __FUNCTION__, __LINE__);
      alDopplerFactor(f);
      Sound::CheckForError("alDopplerFactor", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetSpeedOfSound(float s)
   {
      Sound::CheckForError("Cleanup OpenAL error.", __FUNCTION__, __LINE__);
      alSpeedOfSound(s);
      Sound::CheckForError("alSpeedOfSound", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::PauseSounds()
   {
      // Pause all sounds that are currently playing, and
      // save their previous state.
      for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
      {
         Sound* sob = iter->get();
         if (sob == NULL)
         {
            continue;
         }

         if (sob->IsPlaying())
         {
            sob->PauseImmediately();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::UnPauseSounds()
   {
      // Restore all paused sounds to their previous state.
      for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
      {
         Sound* sob = iter->get();
         if (sob == NULL)
         {
            continue;
         }

         if (sob->IsPaused())
         {
            sob->PauseImmediately();
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   Sound* AudioManager::NewSound()
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      SOB_PTR snd(NULL);

      // create a new sound object if we don't have one
      if (snd.get() == NULL)
      {
         snd = new Sound();
         assert(snd.get());
      }

      // save the sound
      mSoundList.push_back(snd);

      // hand out the interface to the sound
      return snd.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::FreeSound(Sound* sound)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      SOB_PTR snd = static_cast<Sound*>(sound);

      if (snd.get() == NULL)
      {
         return;
      }

      // remove sound from list
      SND_LST::iterator iter;
      for (iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
      {
         if (snd != *iter)
         {
            continue;
         }

         mSoundList.erase(iter);
         break;
      }

      // free the sound's source and buffer
      UnloadSound(snd.get());
      snd->Clear();
   }


   ////////////////////////////////////////////////////////////////////////////////
   ALint AudioManager::LoadFile(const std::string& file)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      if (file.empty())
      {
         // no file name, bail...
         return false;
      }

      std::string filename;
      if (osgDB::fileExists(file))
      {
         filename = file;
      }
      else
      {
         filename = osgDB::findDataFile(file);
      }

      if (filename.empty())
      {
         // still no file name, bail...
         std::string msg("AudioManager: can't load file ");
         msg.append(file.c_str());
         LOG_WARNING(msg);
         return AL_NONE;
      }

      BufferData* bd = mBufferMap[file];
      if (bd != 0)
      {
         // file already loaded, bail...
         return bd->buf;
      }

      bd = new BufferData;

      // Clear the errors
      //ALenum err( alGetError() );

      // create buffer for the wave file
      alGenBuffers(1L, &bd->buf);
      if (Sound::CheckForError("AudioManager: alGenBuffers error", __FUNCTION__, __LINE__))
      {
         delete bd;
         return AL_NONE;
      }

      ALvoid* data = NULL;

      // We are trying to support the new version of ALUT as well as the old intergated
      // version. So we have two cases: DEPRECATED and NON-DEPRECATED.

      // This is not defined in ALUT prior to version 1.
#ifndef ALUT_API_MAJOR_VERSION

      // DEPRECATED version for ALUT < 1.0.0

      // Man, are we still in the dark ages here???
      // Copy the std::string to a frickin' ALByte array...
      ALbyte fname[256L];
      unsigned int len = std::min(filename.size(), size_t(255L));
      memcpy(fname, filename.c_str(), len);
      fname[len] = 0L;

      ALsizei freq(0);
#ifdef __APPLE__
      alutLoadWAVFile(fname, &format, &data, &size, &freq);
#else
      alutLoadWAVFile(fname, &format, &data, &size, &freq, &bd->loop);
#endif // __APPLE__

#else

      // Load the sound through OSG so that the sound file
      // may or may not be loaded from an encrypted file.
      osg::ref_ptr<osg::Object> osgObj = osgDB::readRefObjectFile(filename);
      WrapperOSGSoundObject* userData = dynamic_cast<WrapperOSGSoundObject*>(osgObj.get());
      if(userData != NULL)
      {
         data = userData->mRawData;
      }

#endif // ALUT_API_MAJOR_VERSION

      if (data == NULL)
      {
#ifndef ALUT_API_MAJOR_VERSION
         LOG_WARNING("AudioManager: alutLoadWAVFile error on " + file.c_str());
#else
         Sound::CheckForError("AudioManager: alutLoadMemoryFromFile error", __FUNCTION__, __LINE__);
#endif // ALUT_API_MAJOR_VERSION

         ReleaseSoundBuffer(bd->buf, "alDeleteBuffers error", __FUNCTION__, __LINE__);
         delete bd;

         return AL_NONE;
      }

#ifndef ALUT_API_MAJOR_VERSION
      bd->format = format;
      bd->freq   = ALsizei(freq);
      bd->size   = size;
#else
      BufferData& userBD = userData->mBufferData;
      bd->format = userBD.format;
      bd->freq   = userBD.freq;
      bd->size   = userBD.size;
#endif // ALUT_API_MAJOR_VERSION

      alBufferData(bd->buf, bd->format, data, bd->size, bd->freq);

#if !defined (_MSC_VER) || !defined (DONT_ALUT_FREE)
      free(data);
      data = NULL;
#endif

      if (Sound::CheckForError("AudioManager: alBufferData error ", __FUNCTION__, __LINE__))
      {
         ReleaseSoundBuffer(bd->buf, "alDeleteBuffers error prior to deleting data.",
            __FUNCTION__, __LINE__);

         delete bd;
         return AL_NONE;
      }

      mBufferMap[file] = bd;
      bd->file = mBufferMap.find(file)->first.c_str();

      return bd->buf;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool AudioManager::UnloadFile(const std::string& file)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      if (file.empty())
      {
         // no file name, bail...
         return false;
      }

      BUF_MAP::iterator iter = mBufferMap.find(file);
      if (iter == mBufferMap.end())
      {
         // file is not loaded, bail...
         return false;
      }

      BufferData* bd = iter->second;
      if (bd == 0)
      {
         // bd should never be NULL
         // this code should never run
         mBufferMap.erase(iter);
         return false;
      }

      if (bd->use)
      {
         // buffer still in use, don't remove buffer
         return false;
      }

      ReleaseSoundBuffer(bd->buf, "alDeleteBuffers( 1L, &bd->buf );", __FUNCTION__, __LINE__);
      delete bd;

      mBufferMap.erase(iter);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool AudioManager::ConfigEAX(bool eax)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      if (!eax)
      {
         return false;
      }

#ifndef AL_VERSION_1_1
      ALubyte buf[32L];
      memset( buf, 0L, 32L );
      memcpy( buf, _EaxVer, std::min( strlen(_EaxVer), size_t(32L) ) );
#else
      const ALchar* buf = _EaxVer;
#endif

      // check for EAX support
      if (alIsExtensionPresent(buf) == AL_FALSE)
      {
         std::string msg("AudioManager - feature not available: ");
         msg.append(_EaxVer);
         LOG_WARNING(msg);
         return false;
      }

#ifndef AL_VERSION_1_1
      memset(buf, 0L, 32L);
      memcpy(buf, _EaxSet, std::min( strlen(_EaxSet), size_t(32L)));
#else
      buf = _EaxSet;
#endif

      // get the eax-set function
      mEAXSet = alGetProcAddress(buf);
      if (mEAXSet == 0)
      {
         std::string msg("AudioManager - feature not available: ");
         msg.append(_EaxVer);
         LOG_WARNING(msg);
         return false;
      }

#ifndef AL_VERSION_1_1
      memset(buf, 0, 32);
      memcpy(buf, _EaxGet, std::min( strlen(_EaxGet), size_t(32)));
#else
      buf = _EaxVer;
#endif

      // get the eax-get function
      mEAXGet = alGetProcAddress(buf);
      if (mEAXGet == 0)
      {
         std::string msg("AudioManager - feature not available: ");
         msg.append(_EaxVer);
         LOG_WARNING(msg);
         mEAXSet = 0;
         return false;
      }

      Sound::CheckForError("Config eax issue", __FUNCTION__, __LINE__);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int AudioManager::LoadSoundBuffer(Sound& snd)
   {
      const char* file = snd.GetFilename();
      int useCount = 0;

      if (file != NULL)
      {
         // Load a new or an existing sound buffer.
         if (LoadFile(file) != AL_NONE)
         {
            BufferData* bd = mBufferMap[file];
            snd.SetBuffer(bd->buf);
            useCount = (++bd->use);
         }
         else
         {
            std::ostringstream errorMessage;
            errorMessage << "Unable to generate a sound buffer for file \""
               << file << "\"";
            LOG_ERROR(errorMessage.str());
         }
      }
      else
      {
         LOG_ERROR("Sound object does not specify a file name.");
      }

      return useCount;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int AudioManager::UnloadSound(Sound* snd)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
      assert(snd);

      int useCount = -1;

      const char* file = snd->GetFilename();

      if (file == NULL)
      {
         return useCount;
      }

      snd->SetBuffer(AL_NONE);

      BufferData* bd = mBufferMap[file];

      if (bd == NULL)
      {
         return useCount;
      }

      // Decrease the buffer reference count.
      if (bd->use > 0)
      {
         useCount = (--bd->use);
      }
      else // Something is wrong!
      {
         std::ostringstream errorMessage;
         errorMessage << "Sound buffer reference count for file \""
            << file << "\" was already at: " << bd->use;
         LOG_ERROR(errorMessage.str());

         // Ensure that the buffer will be stopped,
         // and deleted in the call to UnloadFile.
         useCount = bd->use = 0;
      }

      // If the buffer is not being used by any other sound object...
      if (useCount == 0)
      {
         ReleaseSoundSource(*snd, "Sound source delete error", __FUNCTION__, __LINE__);
      }

      UnloadFile(file);
      Sound::CheckForError("Unload Sound Error", __FUNCTION__, __LINE__);

      return useCount;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool AudioManager::ReleaseSoundSource(Sound& snd, const std::string& errorMessage,
      const std::string& callerFunctionName, int callerFunctionLineNum )
   {
      return snd.ReleaseSource(); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool AudioManager::ReleaseSoundBuffer(ALuint bufferHandle, const std::string& errorMessage,
      const std::string& callerFunctionName, int callerFunctionLineNum )
   {
      bool success = false;
      if (alIsBuffer(bufferHandle) == AL_TRUE)
      {
         alDeleteBuffers(1L, &bufferHandle);

         // Determine if OpenAL has encountered an error.
         success = Sound::CheckForError(errorMessage, callerFunctionName, callerFunctionLineNum);
      }
      else
      {
         LOG_WARNING("Sound buffer was invalid.");
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::OpenDevice(const ALCchar* deviceName)
   {
      mDevice = alcOpenDevice(deviceName);
      if (mDevice == NULL)
      {
         LOG_ERROR("AudioManager can't open audio device " << deviceName);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::CreateContext()
   {
      mContext = alcCreateContext(mDevice, NULL);
      if (!mContext)
      {
         LOG_ERROR("AudioManager can't create audio context.");
      }

      if (!alcMakeContextCurrent(mContext))
      {
         LOG_ERROR("AudioManager can't make audio context current.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::CloseDevice()
   {
      if (mDevice)
      {
         alcCloseDevice(mDevice);
         Sound::CheckForError("Attempted to close OpenAL device.", __FUNCTION__, __LINE__);
         mDevice = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AudioManager::CloseContext()
   {
      alcMakeContextCurrent(NULL);
      Sound::CheckForError("Attempted to change current OpenAL context to nothing.",
         __FUNCTION__, __LINE__);
      if (mContext)
      {
         alcDestroyContext(mContext);
         Sound::CheckForError("Attempted to destroy current OpenAL context.",
            __FUNCTION__, __LINE__);
         mContext = NULL;
      }
   }

} //namespace dtEntity
