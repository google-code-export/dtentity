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


#include <dtEntity/listener.h>
#include <dtEntity/sound.h>
#include <osg/Matrix>

namespace dtEntity
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
   void Listener::SetPosition(const osg::Vec3& position)
   {
      // store it internally
      mPosition = position;

      // pass it to OpenAL
      ALfloat  pos[3L]  = {mPosition.x(), mPosition.y(), mPosition.z()};
      alListenerfv(AL_POSITION, pos);

      Sound::CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
   }

   //////////////////////////////////////////////////////////////////////////
   void Listener::SetOrientation(const osg::Vec3& atVec, const osg::Vec3& upVec)
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

}  // namespace dtEntity
