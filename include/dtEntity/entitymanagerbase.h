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

#ifndef DELTA_ENTITY_ENTITYMANAGERBASE
#define DELTA_ENTITY_ENTITYMANAGERBASE

#include <dtCore/base.h>
#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <OpenThreads/Mutex>

namespace dtEntity
{
   class EntityManager;

   /**
    * Feeds Delta3D events to the entity manager
    */
   class DT_ENTITY_EXPORT EntityManagerBase
       : public dtCore::Base
   {
      DECLARE_MANAGEMENT_LAYER(EntityManagerBase)

   public:

      EntityManagerBase();
      ~EntityManagerBase();

      EntityManager& GetEntityManager() const;

      /**
      * Called by the dtCore::Base class
      * @param The data from the message
      * @see dtCore::Base
      */
      virtual void OnMessage(dtCore::Base::MessageData* data);

	  // Traversal mutex can be locked to get exclusive access to the
	  // entity system / component properties from outside thread
      void LockTraversalMutex();
      void UnlockTraversalMutex();

   private:

      osg::ref_ptr<EntityManager> mEntityManager;

      OpenThreads::Mutex mTraversalMutex;

   };
}
#endif // DELTA_ENTITY_ENTITYMANAGERBASE
