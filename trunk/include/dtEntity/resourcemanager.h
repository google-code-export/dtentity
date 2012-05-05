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
#include <osg/referenced>
#include <dtEntity/export.h>
#include <dtEntity/singleton.h>
#include <map>

namespace osg
{
   class Node;
}

namespace dtEntity
{
   namespace ResourceManagerOptions
   {
      enum e
      {
         DeepCopy             = 1<<0,
         ShallowCopy          = 1<<1,
         CopyNodes            = 1<<2,
         CopyHardwareMeshes   = 1<<3,
         DoOptimization       = 1<<4,
         Default              = ShallowCopy
      };
   }
   
   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT ResourceManager 
      : public dtEntity::Singleton<ResourceManager>
   {
   public:
      
      typedef std::map<std::string, osg::ref_ptr<osg::Node> > NodeStore;
      osg::ref_ptr<osg::Node> GetNode(const std::string& path, unsigned int options = ResourceManagerOptions::Default);
      void RemoveFromCache(const std::string& path);
      
   private:
      NodeStore mNodeStore;
   };
}
