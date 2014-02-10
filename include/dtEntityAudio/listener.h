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

#ifndef  DTENTITY_LISTENER
#define  DTENTITY_LISTENER

#include <dtEntityAudio/export.h>

#include <osg/Referenced>
#include <dtEntity/property.h>

namespace dtEntityAudio
{
   /** dtEntityAudio::Listener
    *
    * dtEntityAudio::Listener is just an interface to the global listener
    * object held within (and protected) by the dtEntity::AudioManager.
    *
    * The listener is not usually created directly by the user (new/delete).
    * Instead the user requests the listener from the AudioManager:
    *
    *    Listener* global_ear = AudioManager::GetInstance().GetListener();
    *
    * The user can then call any of the Listener interface functions.
    * After the user is finished with the Listener, there is no need to
    * free it up.  The underlying listener object is a global singular
    * which lasts as long as the AudioManager exists.
    *
    * The current version is heavily inspired from the Delta3D one
    */
   class DTENTITY_AUDIO_EXPORT Listener : public osg::Referenced
   {

      public:
         /**
          * Constructor, user does not create directly
          * instead requests the listener from AudioManager
          */
         Listener();

      protected:
         /**
          * Destructor, user does not delete directly
          * AudioManager handles destruction
          */
         virtual ~Listener();

      public:

         /// Set listener position 
         void SetPosition(const osg::Vec3f& position);
         /// Set listener orientation
         /**
         *  Requires an "at" vector (listening direction) and an "up" vector
            (defining the UP dir)
         */
         void SetOrientation(const osg::Vec3f& atVec, const osg::Vec3f& upVec);
         /// Set listener velocity
         void SetVelocity(const osg::Vec3f& velocity);

         /**
          * Get the velocity of the listener.
          *
          * @param velocity to get
          */
         void GetVelocity(osg::Vec3f& velocity) const;

         /**
          * Sets the master volume of the listener.
          *
          * @param gain the new gain
          */
         void SetGain(float gain);

         /**
          * Returns the master volume of the listener.
          *
          * @return the current volume
          */
         float GetGain() const;

         /// clean up listener
         void Clear(void);


   protected:

      osg::Vec3f               mPosition;
      osg::Vec3f               mDirection;
      osg::Vec3f               mUpVector;
      osg::Vec3f               mVelocity;

   };
} // namespace dtEntityAudio

#endif   // DTENTITY_LISTENER
