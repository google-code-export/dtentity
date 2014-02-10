/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/


#include <dtEntityAudio/listener.h>
#include <dtEntityAudio/sound.h>
#include <osg/Matrix>

namespace dtEntityAudio
{
   ////////////////////////////////////////////////////////////////////////////////
   Listener::Listener()
   {
      Clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Listener::~Listener()
   {
      ;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Listener::SetVelocity(const osg::Vec3f& velocity)
   {
      alListener3f(AL_VELOCITY,
         static_cast<ALfloat>(velocity[0]),
         static_cast<ALfloat>(velocity[1]),
         static_cast<ALfloat>(velocity[2]));
      Sound::CheckForError("OpenAL Listener velocity value changing", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Listener::GetVelocity(osg::Vec3f& velocity) const
   {
      alGetListener3f(AL_VELOCITY, static_cast<ALfloat*>(&velocity[0]),
         static_cast<ALfloat*>(&velocity[1]),
         static_cast<ALfloat*>(&velocity[2]));
      Sound::CheckForError("Getting OpenAL Listener velocity value", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Listener::SetGain(float gain)
   {
      // force gain to range from zero to one
      osg::clampBetween<float>( gain, 0.0f, 1.0f );

      alListenerf(AL_GAIN, gain);
      Sound::CheckForError("OpenAL Listener gain value changing", __FUNCTION__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   float Listener::GetGain(void) const
   {
      float g;
      alGetListenerf(AL_GAIN, &g);
      Sound::CheckForError("OpenAL Listener getting gain value", __FUNCTION__, __LINE__);

      return g;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Listener::Clear(void)
   {
      Sound::CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      union
      {
         ALfloat     ort[6];
         struct
         {
            ALfloat  at[3];
            ALfloat  up[3];
         };
      }  orient;
      orient.ort[0] = 0.0f;
      orient.ort[1] = 1.0f;
      orient.ort[2] = 0.0f;
      orient.ort[3] = 0.0f;
      orient.ort[4] = 0.0f;
      orient.ort[5] = 1.0f;

      ALfloat  pos[3L]  = { 0.0f, 0.0f, 0.0f };

      alListenerfv( AL_POSITION, pos );
      alListenerfv( AL_ORIENTATION, orient.ort );
      Sound::CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);

      SetVelocity(osg::Vec3f(0.0f, 0.0f, 0.0f));
      SetGain(1.0);
   }

   //////////////////////////////////////////////////////////////////////////
   void Listener::SetPosition(const osg::Vec3f& position)
   {
      // store it internally
      mPosition = position;

      // pass it to OpenAL
      ALfloat  pos[3L]  = {mPosition.x(), mPosition.y(), mPosition.z()};
      alListenerfv(AL_POSITION, pos);

      Sound::CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
   }

   //////////////////////////////////////////////////////////////////////////
   void Listener::SetOrientation(const osg::Vec3f& atVec, const osg::Vec3f& upVec)
   {
      // store it internally
      mDirection = atVec;
      mUpVector = upVec;

      // pass it to OpenAL
      ALfloat  orientVec[6]  = {
         mDirection.x(), mDirection.y(), mDirection.z(), // At component
         mUpVector.x(), mUpVector.y(), mUpVector.z()     // UP component
      };
      alListenerfv(AL_ORIENTATION, orientVec);

      Sound::CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
   }

}  // namespace dtEntityAudio
