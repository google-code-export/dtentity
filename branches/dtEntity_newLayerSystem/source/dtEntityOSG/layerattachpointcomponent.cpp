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

#include <dtEntityOSG/layerattachpointcomponent.h>

#include <dtEntityOSG/layercomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <osg/PolygonMode>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <assert.h>

namespace dtEntityOSG
{
   ////////////////////////////////////////////////////////////////////////////////
   void DetachLayerNodes(LayerSystem* ls, dtEntity::StringId attachcompname, LayerAttachPointComponent* attachcomp)
   {
      for(LayerSystem::ComponentStore::iterator j = ls->begin(); j != ls->end(); ++j)
      {
         LayerComponent* lcomp = j->second;
         osg::Node* attached = lcomp->GetAttachedComponentNode();         
         if(lcomp->IsAddedToScene() && attached->getNumParents() != 0 && lcomp->GetLayer() == attachcompname)
         {
            lcomp->Detach(attachcomp);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReattachLayerNodes(LayerSystem* ls, LayerAttachPointComponent* attachcomp)
   {
      // Loop through all layer components and attach those as children
      // that have a layer name equal to layer name of this layer attach point
      for(LayerSystem::ComponentStore::iterator j = ls->begin(); j != ls->end(); ++j)
      {
         LayerComponent* lcomp = j->second;
         dtEntity::StringId layer = lcomp->GetLayer();
         if(lcomp->IsAddedToScene() && layer == attachcomp->GetName())
         {
            lcomp->Attach(attachcomp);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId LayerAttachPointComponent::TYPE(dtEntity::SID("LayerAttachPoint"));
   const dtEntity::StringId LayerAttachPointComponent::NameId(dtEntity::SID("Name"));
   
   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent::LayerAttachPointComponent()    
      : mEntityManager(NULL)
      , mCurrentName(dtEntity::StringId())
   {      
      Register(NameId, &mName);
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent::~LayerAttachPointComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
      mEntityManager = &entity.GetEntityManager();
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::OnRemovedFromEntity(dtEntity::Entity& entity)
   {      
      LayerSystem* ls;
      bool found = mEntityManager->GetES(ls);
      assert(found);
      DetachLayerNodes(ls, GetName(), this);
      BaseClass::OnRemovedFromEntity(entity);
   }

    ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::Finished()
   {
      BaseClass::Finished();
      assert(mEntityManager != NULL);
      if(mName.Get() != mCurrentName)
      {
         SetName(mName.Get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::SetName(dtEntity::StringId name)
   {      
      LayerAttachPointSystem* lps;
      mEntityManager->GetES(lps);

      LayerSystem* ls;
      bool found = mEntityManager->GetES(ls);
      assert(found);

      GetNode()->setName("Layer Attach Point" + name);

      if(name != mCurrentName)
      {
         LayerAttachPointComponent* old = NULL;
         if(lps->GetByName(name, old) && old == this)
         {
            return;
         }
         if(old != NULL)
         {
            DetachLayerNodes(ls, name, old);
         }
         mName.Set(name);
         mCurrentName = name;
         lps->RegisterByName(name, this);
         
         ReattachLayerNodes(ls, this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::SetNode(osg::Group* node)
   {
      assert(mEntityManager);
      LayerSystem* ls;
      mEntityManager->GetES(ls);
      DetachLayerNodes(ls, mCurrentName, this);
      node->setName("Layer Attach Point " + GetName());
      GroupComponent::SetNode(node);      

      ReattachLayerNodes(ls, this);
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId LayerAttachPointSystem::TYPE(dtEntity::SID("LayerAttachPoint"));
   const dtEntity::StringId LayerAttachPointSystem::DefaultLayerId(dtEntity::SID("default"));
   const dtEntity::StringId LayerAttachPointSystem::RootId(dtEntity::SID("root"));


   ////////////////////////////////////////////////////////////////////////////////
   LayerAttachPointSystem::LayerAttachPointSystem(dtEntity::EntityManager& em)
      : DefaultEntitySystem<LayerAttachPointComponent>(em)
      , mBaseEntityId(0)
   {
      AddScriptedMethod("getByName", dtEntity::ScriptMethodFunctor(this, &LayerAttachPointSystem::ScriptGetByName));
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerAttachPointSystem::DeleteComponent(dtEntity::EntityId eid)
   {
      ComponentStore::iterator i = mComponents.find(eid);
      if(i != mComponents.end())
      {
         dtEntity::StringId name = i->second->GetName();
         LayerByNameMap::iterator j = mLayerByNameMap.find(name);
         if(j != mLayerByNameMap.end())
         {
            mLayerByNameMap.erase(j);
            if(name == DefaultLayerId)
            {
               mLayerByNameMap[DefaultLayerId] = GetRootLayer();
            }
         }          
      }      
   
      return BaseClass::DeleteComponent(eid);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerAttachPointSystem::GetByName(dtEntity::StringId name, LayerAttachPointComponent*& recipient)
   {
      LayerByNameMap::iterator i = mLayerByNameMap.find(name);
      if(i == mLayerByNameMap.end())
         return false;

      recipient = i->second;
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent* LayerAttachPointSystem::GetDefaultLayer()
   {
      LayerAttachPointComponent* comp;
      GetByName(DefaultLayerId, comp);
      return comp;
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent* LayerAttachPointSystem::GetRootLayer()
   {
      LayerAttachPointComponent* comp;
      GetByName(RootId, comp);
      return comp;
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointSystem::CreateSceneGraphRootEntity(osg::Group* root)
   {
     mSceneGraphRoot = root;
      if(mBaseEntityId == 0 || !GetEntityManager().EntityExists(mBaseEntityId))
      {
         dtEntity::Entity* base;
         GetEntityManager().CreateEntity(base);
         mBaseEntityId = base->GetId();
         LayerAttachPointComponent* layerattach;
         bool success = GetEntityManager().CreateComponent(mBaseEntityId, layerattach);
         assert(success);
         layerattach->SetName(RootId);
         layerattach->SetNode(root);
         RegisterByName(DefaultLayerId, layerattach);
      }
      else
      {
         LayerAttachPointComponent* layerattach;
         GetEntityManager().GetComponent(mBaseEntityId, layerattach);
         layerattach->SetName(RootId);
         layerattach->SetNode(root);
         RegisterByName(DefaultLayerId, layerattach);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointSystem::RegisterByName(dtEntity::StringId id, LayerAttachPointComponent* c)
   {
      mLayerByNameMap[id] = c;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* LayerAttachPointSystem::ScriptGetByName(const dtEntity::PropertyArgs& args)
   {
      std::string name = args[0]->StringValue();
      LayerByNameMap::iterator i = mLayerByNameMap.find(dtEntity::SID(name));
      if(i == mLayerByNameMap.end())
      {
         return new dtEntity::StringIdProperty(0);
      }
      return new dtEntity::StringIdProperty(i->first);
   }
}
