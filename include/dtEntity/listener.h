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

#ifndef  DTENTITY_LISTENER
#define  DTENTITY_LISTENER

#include <dtEntity/export.h>

#include <osg/Referenced>
#include <dtEntity/property.h>

namespace dtEntity
{
   /** dtEntity::Listener
    *
    * dtEntity::Listener is just an interface to the global listener
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
   class DT_ENTITY_EXPORT Listener : public osg::Referenced
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
         void SetPosition(const Vec3f& position);
         /// Set listener orientation
         /**
         *  Requires an "at" vector (listening direction) and an "up" vector
            (defining the UP dir)
         */
         void SetOrientation(const Vec3f& atVec, const Vec3f& upVec);
         /// Set listener velocity
         void SetVelocity(const Vec3f& velocity);

         /**
          * Get the velocity of the listener.
          *
          * @param velocity to get
          */
         void GetVelocity(Vec3f& velocity) const;

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

      Vec3f               mPosition;
      Vec3f               mDirection;
      Vec3f               mUpVector;
      Vec3f               mVelocity;

   };
} // namespace dtEntity

#endif   // DTENTITY_LISTENER
