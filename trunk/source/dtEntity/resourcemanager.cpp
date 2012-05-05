/*
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

#include <dtEntity/resourcemanager.h>

#include <dtEntity/log.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/systemmessages.h>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgUtil/Optimizer>

namespace dtEntity
{
   ///////////////////////////////////////////////////////////////////////////
   osg::ref_ptr<osg::Node> ResourceManager::GetNode(EntityManager& em, const std::string& path, unsigned int options)
   {
      osg::Node* ret = NULL;

      std::string abspath = osgDB::findDataFile(path);

      if(abspath.empty())
      {
         LOG_ERROR("Error loading node, could not find data file: " << path);
         return NULL;
      }

      if((options & ResourceManagerOptions::CopyNodes) != 0)
      {
         NodeStore::iterator i = mNodeStore.find(abspath);
         if(i != mNodeStore.end())
         {
            ret = osg::clone(i->second.get(), osg::CopyOp(
               osg::CopyOp::DEEP_COPY_OBJECTS        |
               osg::CopyOp::DEEP_COPY_NODES          |
               osg::CopyOp::DEEP_COPY_USERDATA
            ));
         }
      }
      else if((options & ResourceManagerOptions::ShallowCopy) != NULL)
      {
         NodeStore::iterator i = mNodeStore.find(abspath);
         if(i != mNodeStore.end())
         {
            ret = osg::clone(i->second.get(), osg::CopyOp(
               osg::CopyOp::DEEP_COPY_USERDATA
            ));
         }
      }
      else if((options & ResourceManagerOptions::CopyHardwareMeshes) != NULL)
      {
         NodeStore::iterator i = mNodeStore.find(abspath);
         if(i != mNodeStore.end())
         {
            ret = osg::clone(i->second.get(), osg::CopyOp(
               osg::CopyOp::DEEP_COPY_ALL
               & ~osg::CopyOp::DEEP_COPY_PRIMITIVES
               & ~osg::CopyOp::DEEP_COPY_ARRAYS
               & ~osg::CopyOp::DEEP_COPY_TEXTURES
               & ~osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
               & ~osg::CopyOp::DEEP_COPY_IMAGES
               &  ~osg::CopyOp::DEEP_COPY_SHAPES
               & ~osg::CopyOp::DEEP_COPY_UNIFORMS
            ));
         }
      }
      else if((options & ResourceManagerOptions::DeepCopy) != NULL)
      {
         NodeStore::iterator i = mNodeStore.find(abspath);
         if(i != mNodeStore.end())
         {
            ret = osg::clone(i->second.get(), osg::CopyOp(
               osg::CopyOp::DEEP_COPY_ALL
            ));
         }
      }
      
      if(ret != NULL)
      {
         ret->setUserData(NULL);
         return ret;
      }
      
      osg::Node* node = osgDB::readNodeFile(abspath);

      if(node == NULL)
      {
         LOG_ERROR("Error loading node, could not interpret data file: " << abspath);
         return NULL;
      }

      if((options & ResourceManagerOptions::DoOptimization) != NULL)
      {
         osgUtil::Optimizer optimizer;
         optimizer.optimize(node);
      }
      if((options & ResourceManagerOptions::DeepCopy) == NULL)
      {
         mNodeStore[abspath] = node;
      }

      ResourceLoadedMessage msg;
      msg.SetPath(abspath);
      em.EmitMessage(msg);

      if((options & ResourceManagerOptions::CopyHardwareMeshes) != NULL)
      {
         return osg::clone(node, osg::CopyOp(
            osg::CopyOp::DEEP_COPY_ALL &
            ~osg::CopyOp::DEEP_COPY_PRIMITIVES &
            ~osg::CopyOp::DEEP_COPY_ARRAYS &
            ~osg::CopyOp::DEEP_COPY_TEXTURES
         ));           
      }
      else
      {
         return node;
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   bool ResourceManager::IsInNodeCache(const std::string& path)
   {
      return (mNodeStore.find(path) != mNodeStore.end());
   }

   ///////////////////////////////////////////////////////////////////////////
   bool ResourceManager::RemoveFromNodeCache(const std::string& path)
   {
      NodeStore::iterator i = mNodeStore.find(path);
      if(i != mNodeStore.end())
      {
         mNodeStore.erase(i);
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////
   void ResourceManager::TriggerReload(const std::string& path, EntityManager& em)
   {
      RemoveFromNodeCache(path);
      
      ResourceChangedMessage msg;
      msg.SetPath(path);
      em.EmitMessage(msg);
   
   }
}
