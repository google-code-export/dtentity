#pragma once

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

#include <osg/ref_ptr>
#include <dtEntityOSG/export.h>
#include <dtEntity/singleton.h>
#include <map>
#include <string>

namespace osg
{
   class Node;
}

namespace dtEntity
{
    class EntityManager;
}

namespace dtEntityOSG
{
   namespace ResourceManagerOptions
   {
      enum e
      {
         DeepCopy             = 1<<0,        // If in cache: Create copy of node and all of its subnodes
         ShallowCopy          = 1<<1,        // If in cache: Create copy of node, share subnodes of cached node
         CopyNodes            = 1<<2,        // If in cache: Create copy of node and subnodes, share vertices, textures etc
         CopyHardwareMeshes   = 1<<3,        // If in cache: Special mode for efficiently sharing osgAnimation objects
         DoOptimization       = 1<<4,        // Run osgUtil::Optimizer on first load
         Default              = ShallowCopy
      };
   }
   
   ///////////////////////////////////////////////////////////////////////////
   class DTENTITY_OSG_EXPORT ResourceManager 
      : public dtEntity::Singleton<ResourceManager>
   {
   public:
      
      typedef std::map<std::string, osg::ref_ptr<osg::Node> > NodeStore;

      /**
       * Try to read node from data paths.
       * @param path Load resource from this path
       * @param options Loading options.
       * @return node, NULL if not found
       */
      osg::ref_ptr<osg::Node> GetNode(dtEntity::EntityManager& em, const std::string& path, unsigned int options = ResourceManagerOptions::Default);

      /**
       * @return true if this resource resides in the cache
       */
      bool IsInNodeCache(const std::string& path);

      /**
       * remove from cache
       * @return true if this resource was in the cache
       */
      bool RemoveFromNodeCache(const std::string& path);
      
      /**
       * Call this method to notify the resource manager that this resource has changed
       * and should be reloaded
       */
      void TriggerReload(const std::string& path, dtEntity::EntityManager& em);

   private:
      NodeStore mNodeStore;
   };
}
