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

#include <dtEntityOSG/nodecomponent.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntityOSG/nodemaskvisitor.h>
#include <osg/NodeVisitor>
#include <osg/Group>

namespace dtEntityOSG
{
   ////////////////////////////////////////////////////////////////////////////////
   NodeStore::NodeStore()
      : mNode(new osg::Node())
      , mParentComponent(dtEntity::StringId())     
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   NodeStore::NodeStore(osg::Node* node)
      : mNode(node)
      , mParentComponent(dtEntity::StringId())
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   NodeStore::~NodeStore()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeStore::SetNodeEntity(dtEntity::Entity* e)
   {
      mNode->setUserData(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Entity* NodeStore::GetNodeEntity()
   {
      return dynamic_cast<dtEntity::Entity*>(mNode->getUserData());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeStore::ClearFromParent()
   {
      dtEntity::Entity* entity = GetNodeEntity();
      if(entity == NULL)
      {
         LOG_ERROR("Cannot clear from parent: Does not belong to an entity!");
         return;
      }
      if(GetParentComponent() != dtEntity::StringId())
      {
         dtEntity::Component* comp;
         if(entity->GetComponent(GetParentComponent(), comp))
         {
            GroupComponent* grp = dynamic_cast<GroupComponent*>(comp);
            if(grp)
            {
               grp->GetAttachmentGroup()->removeChild(GetNode());
            }
         }
      }
      SetNodeEntity(NULL);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Node* NodeStore::GetNode() const
   {
      return mNode;
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeStore::SetNode(osg::Node* node)
   {  
      // remove current node from all its parents
      while(mNode->getNumParents() != 0)
      {
         mNode->getParent(0)->removeChild(mNode);
      }
      dtEntity::Entity* entity = GetNodeEntity();
      mNode->setUserData(0);

      mNode = node;     
      
      if(entity)
      {
         mNode->setUserData(entity);

         // if node is already attached to a parent node owned by a component:
         if(GetParentComponent() != dtEntity::StringId())
         {       
            dtEntity::Component* parent;
            if(entity->GetComponent(GetParentComponent(), parent))
            {
               if(GetParentComponent() == LayerComponent::TYPE)
               {
                  LayerComponent* layercomp = static_cast<LayerComponent*>(parent);
                  if(layercomp->IsAddedToScene())
                  {
                     layercomp->SetLayer(layercomp->GetLayer());
                  }
               }
               else
               {
                  assert(dynamic_cast<GroupComponent*>(parent) != NULL);
                  
                  bool success = static_cast<GroupComponent*>(parent)->GetAttachmentGroup()->addChild(mNode);                  
                  assert(success);
               }
            }            
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeStore::SetNodeMask(unsigned int nodemask, bool recursive)
   {
      if(recursive)
      {
         NodeMaskVisitor nv(nodemask);
         GetNode()->accept(nv);
      }
      else
      {
         GetNode()->setNodeMask(nodemask);
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////
   unsigned int NodeStore::GetNodeMask() const
   {
      return GetNode()->getNodeMask();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId NodeComponent::TYPE(dtEntity::SID("Node"));
   
   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::NodeComponent()
   {
      GetNode()->setName("NodeComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::NodeComponent(osg::Node* node)
      : NodeStore(node)      
   {
      GetNode()->setName("NodeComponent");
   }
    
   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::~NodeComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      SetNodeEntity(&entity);
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::OnRemovedFromEntity(dtEntity::Entity& entity)
   {
      ClearFromParent();
   }
}
