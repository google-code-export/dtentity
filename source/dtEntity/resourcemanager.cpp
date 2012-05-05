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

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgUtil/Optimizer>

namespace dtEntity
{
   ///////////////////////////////////////////////////////////////////////////
   osg::ref_ptr<osg::Node> ResourceManager::GetNode(const std::string& path, CacheMode::e cachemode, bool optimize)
   {
      switch(cachemode)
      {
         case CacheMode::Nodes:
         {
            NodeStore::iterator i = mNodeStore.find(path);
            if(i != mNodeStore.end())
            {
               osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                  osg::CopyOp::DEEP_COPY_OBJECTS        |
                  osg::CopyOp::DEEP_COPY_NODES          |
                  osg::CopyOp::DEEP_COPY_USERDATA
               ));
               n->setUserData(NULL);
               return n;
            }
         }
         break;
         case CacheMode::All:
         {
            NodeStore::iterator i = mNodeStore.find(path);
            if(i != mNodeStore.end())
            {
               osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                  osg::CopyOp::DEEP_COPY_USERDATA
               ));
               n->setUserData(NULL);
               return n;
            }
         }break;
         case CacheMode::HardwareMeshes:
         {
            NodeStore::iterator i = mNodeStore.find(path);
            if(i != mNodeStore.end())
            {
               osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                  osg::CopyOp::DEEP_COPY_ALL
                  & ~osg::CopyOp::DEEP_COPY_PRIMITIVES
                  & ~osg::CopyOp::DEEP_COPY_ARRAYS
                  & ~osg::CopyOp::DEEP_COPY_TEXTURES
                  & ~osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
                  & ~osg::CopyOp::DEEP_COPY_IMAGES
                  &  ~osg::CopyOp::DEEP_COPY_SHAPES
                  & ~osg::CopyOp::DEEP_COPY_UNIFORMS
               ));
               n->setUserData(NULL);
               return n;
            }
         }
         break;
         case CacheMode::None:
         {
            NodeStore::iterator i = mNodeStore.find(path);
            if(i != mNodeStore.end())
            {
               osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                  osg::CopyOp::DEEP_COPY_ALL
               ));
               n->setUserData(NULL);
               return n;
            }
         }
         break;
      }
      
      osg::Node* node = osgDB::readNodeFile(path);

      if(node == NULL)
      {
         return NULL;
      }

      if(optimize)
      {
         osgUtil::Optimizer optimizer;
         optimizer.optimize(node);
      }
      if(cachemode != CacheMode::None)
      {
         mNodeStore[path] = node;
      }

      if(cachemode == CacheMode::HardwareMeshes)
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
}
