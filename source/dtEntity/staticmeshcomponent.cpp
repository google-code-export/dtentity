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

#include <dtEntity/staticmeshcomponent.h>

#include <dtEntity/nodemasks.h>
#include <dtEntity/resourcemanager.h>
#include <dtEntity/systemmessages.h>




namespace dtEntity
{

 

   ////////////////////////////////////////////////////////////////////////////////
   /*class ModelCache
   {
      typedef std::map<std::string, osg::ref_ptr<osg::Node> > ModelMap;

   public:

      osg::ref_ptr<osg::Node> GetNode(const std::string& path, CacheMode::e cachemode, bool optimize)
      {
         switch(cachemode)
         {
            case CacheMode::Nodes:
            {
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
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
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
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
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
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
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
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
            mModels[path] = node;
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

      void Clear()
      {
         mModels.clear();
      }

   private:
      ModelMap mModels;
   };

   ////////////////////////////////////////////////////////////////////////////////
   static ModelCache s_modelCache;*/

   ////////////////////////////////////////////////////////////////////////////
   const StringId StaticMeshComponent::TYPE(dtEntity::SID("StaticMesh"));
   const StringId StaticMeshComponent::MeshId(dtEntity::SID("Mesh"));
   const StringId StaticMeshComponent::OptimizeId(dtEntity::SID("Optimize"));
   const StringId StaticMeshComponent::IsTerrainId(dtEntity::SID("IsTerrain"));
   const StringId StaticMeshComponent::CacheHintId(dtEntity::SID("CacheHint"));

   const StringId StaticMeshComponent::CacheNoneId(dtEntity::SID("None"));
   const StringId StaticMeshComponent::CacheAllId(dtEntity::SID("All"));
   const StringId StaticMeshComponent::CacheNodesId(dtEntity::SID("Nodes"));
   const StringId StaticMeshComponent::CacheHardwareMeshesId(dtEntity::SID("CacheHardwareMeshes"));

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::StaticMeshComponent()
      : BaseClass(new osg::Node())
      , mCacheHint(CacheNoneId)   
   {
      Register(MeshId, &mMeshPathProperty);
      Register(CacheHintId, &mCacheHint);
      Register(OptimizeId, &mOptimize);
      Register(IsTerrainId, &mIsTerrain);
      
      GetNode()->setName("StaticMeshComponent Initial");
      GetNode()->setNodeMask(
         NodeMasks::VISIBLE  |
         NodeMasks::PICKABLE |
         NodeMasks::RECEIVES_SHADOWS |
         NodeMasks::CASTS_SHADOWS
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::~StaticMeshComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::OnPropertyChanged(StringId propname, Property &prop)
   {
      if(propname == IsTerrainId)
      {
         if(prop.BoolValue())
         {
            SetNodeMask(GetNode()->getNodeMask() | NodeMasks::TERRAIN);
         }
         else
         {
            SetNodeMask(GetNode()->getNodeMask() & ~NodeMasks::TERRAIN);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::Finished()
   {
      SetMesh(mMeshPathProperty.Get(), mCacheHint.Get());
      BaseClass::Finished();
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetMesh(const std::string& path, StringId cacheHint)
   {
      if(mLoadedMesh == path)
      {
         return;
      }
      mLoadedMesh = path;
      mMeshPathProperty.Set(path);
      if(path == "")
      {
         osg::Node* node = new osg::Node();
         node->setName("StaticMeshComponent Empty");
         SetNode(node);
      }
      else
      {
         CacheMode::e cm = CacheMode::None;
         if(cacheHint == CacheAllId)
         {
           cm = CacheMode::All;
         }
         else if(cacheHint == CacheNodesId)
         {
           cm = CacheMode::Nodes;
         }
         else if(cacheHint == CacheHardwareMeshesId)
         {
           cm = CacheMode::HardwareMeshes;
         }
         osg::ref_ptr<osg::Node> meshnode = ResourceManager::GetInstance().GetNode(path, cm, GetOptimize());
         if(meshnode == NULL)
         {
            LOG_ERROR("Could not load static mesh from path " + path);
            meshnode = new osg::Node();
            meshnode->setName("StaticMeshComponent NotFound");
         }
         else
         {
            meshnode->setName("StaticMeshComponent " + path);
         }

         SetStaticMesh(meshnode);    
      }

      MeshChangedMessage msg;
      msg.SetAboutEntityId(mEntity->GetId());
      msg.SetFilePath(path);
      mEntity->GetEntityManager().EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string StaticMeshComponent::GetMesh() const
   {
      return mMeshPathProperty.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetStaticMesh(osg::Node* node)
   {
      assert(mEntity != NULL);

      unsigned int nm = node->getNodeMask() |
            NodeMasks::VISIBLE | NodeMasks::PICKABLE |
             NodeMasks::CASTS_SHADOWS | NodeMasks::RECEIVES_SHADOWS;
      if(mIsTerrain.Get())
      {
         nm |= NodeMasks::TERRAIN;
      }
      else
      {
        nm &= ~dtEntity::NodeMasks::TERRAIN;
      }
      node->setNodeMask(nm);
      SetNode(node);
      OnPropertyChanged(IsTerrainId, mIsTerrain);

   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId StaticMeshSystem::TYPE(dtEntity::SID("StaticMesh"));

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshSystem::StaticMeshSystem(EntityManager& em)
      : DefaultEntitySystem<StaticMeshComponent>(em, NodeComponent::TYPE)
   {
   }

}
