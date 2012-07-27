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

#include <dtEntityOSG/staticmeshcomponent.h>

#include <dtEntity/nodemasks.h>
#include <dtEntity/resourcemanager.h>
#include <dtEntity/systemmessages.h>
#include <osgDB/FileUtils>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId StaticMeshComponent::TYPE(dtEntity::SID("StaticMesh"));
   const dtEntity::StringId StaticMeshComponent::MeshId(dtEntity::SID("Mesh"));
   const dtEntity::StringId StaticMeshComponent::OptimizeId(dtEntity::SID("Optimize"));
   const dtEntity::StringId StaticMeshComponent::IsTerrainId(dtEntity::SID("IsTerrain"));
   const dtEntity::StringId StaticMeshComponent::CacheHintId(dtEntity::SID("CacheHint"));

   const dtEntity::StringId StaticMeshComponent::CacheNoneId(dtEntity::SID("None"));
   const dtEntity::StringId StaticMeshComponent::CacheAllId(dtEntity::SID("All"));
   const dtEntity::StringId StaticMeshComponent::CacheNodesId(dtEntity::SID("Nodes"));
   const dtEntity::StringId StaticMeshComponent::CacheHardwareMeshesId(dtEntity::SID("CacheHardwareMeshes"));

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::StaticMeshComponent()
      : BaseClass(new osg::Node())
      , mCacheHint(CacheNoneId)   
      , mIsTerrain(dtEntity::DynamicBoolProperty(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &StaticMeshComponent::SetIsTerrain),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &StaticMeshComponent::GetIsTerrain)))
      , mIsTerrainVal(false)
   {
      Register(MeshId, &mMeshPathProperty);
      Register(CacheHintId, &mCacheHint);
      Register(OptimizeId, &mOptimize);
      Register(IsTerrainId, &mIsTerrain);
      
      GetNode()->setName("StaticMeshComponent Initial");
      GetNode()->setNodeMask(
         dtEntity::NodeMasks::VISIBLE  |
         dtEntity::NodeMasks::PICKABLE |
         dtEntity::NodeMasks::RECEIVES_SHADOWS |
         dtEntity::NodeMasks::CASTS_SHADOWS
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::~StaticMeshComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::Finished()
   {
      SetMesh(mMeshPathProperty.Get(), mCacheHint.Get());
      BaseClass::Finished();
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetIsTerrain(bool v) 
   { 
      mIsTerrainVal = v; 
      if(mIsTerrainVal)
      {
         SetNodeMask(GetNode()->getNodeMask() | dtEntity::NodeMasks::TERRAIN);
      }
      else
      {
         SetNodeMask(GetNode()->getNodeMask() & ~dtEntity::NodeMasks::TERRAIN);
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetMesh(const std::string& path, dtEntity::StringId cacheHint)
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
         unsigned int options = dtEntity::ResourceManagerOptions::DeepCopy;
         if(cacheHint == CacheAllId)
         {
           options = dtEntity::ResourceManagerOptions::ShallowCopy;
         }
         else if(cacheHint == CacheNodesId)
         {
            options = dtEntity::ResourceManagerOptions::CopyNodes;
         }
         else if(cacheHint == CacheHardwareMeshesId)
         {
            options = dtEntity::ResourceManagerOptions::CopyHardwareMeshes;
         }
         if(GetOptimize())
         {
            options |= dtEntity::ResourceManagerOptions::DoOptimization;
         }

         dtEntity::ResourceManager& rm = dtEntity::ResourceManager::GetInstance();
         osg::ref_ptr<osg::Node> meshnode = rm.GetNode(mEntity->GetEntityManager(), path, options);
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

      dtEntity::MeshChangedMessage msg;
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
            dtEntity::NodeMasks::VISIBLE | dtEntity::NodeMasks::PICKABLE |
             dtEntity::NodeMasks::CASTS_SHADOWS | dtEntity::NodeMasks::RECEIVES_SHADOWS;
      if(mIsTerrainVal)
      {
         nm |= dtEntity::NodeMasks::TERRAIN;
      }
      else
      {
        nm &= ~dtEntity::NodeMasks::TERRAIN;
      }
      node->setNodeMask(nm);
      SetNode(node);
      SetIsTerrain(mIsTerrainVal);

   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId StaticMeshSystem::TYPE(dtEntity::SID("StaticMesh"));

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshSystem::StaticMeshSystem(dtEntity::EntityManager& em)
      : DefaultEntitySystem<StaticMeshComponent>(em, dtEntity::NodeComponent::TYPE)
   {
      mResourceChangedFunctor = dtEntity::MessageFunctor(this, &StaticMeshSystem::OnResourceChanged);
      em.RegisterForMessages(dtEntity::ResourceChangedMessage::TYPE, mResourceChangedFunctor, "StaticMeshSystem::OnResourceChanged");
      
   }

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshSystem::~StaticMeshSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::ResourceChangedMessage::TYPE, mResourceChangedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshSystem::OnResourceChanged(const dtEntity::Message& m)
   {
      const dtEntity::ResourceChangedMessage& msg = static_cast<const dtEntity::ResourceChangedMessage&>(m);

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         StaticMeshComponent* c = i->second;
         std::string abspath = osgDB::findDataFile(c->GetMesh());
         if(abspath == msg.GetPath())
         {
            c->SetMesh("");
            c->SetMesh(msg.GetPath());
         }
      }
   }

}
