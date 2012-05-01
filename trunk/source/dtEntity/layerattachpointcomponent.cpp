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

#include <dtEntity/layerattachpointcomponent.h>

#include <dtEntity/layercomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <osg/PolygonMode>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <assert.h>

namespace dtEntity
{


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   const StringId LayerAttachPointComponent::TYPE(dtEntity::SID("LayerAttachPoint"));
   const StringId LayerAttachPointComponent::NameId(dtEntity::SID("Name"));
   
   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent::LayerAttachPointComponent()    
      : mEntityManager(NULL)
      , mCurrentName(StringId())
   {      
      Register(NameId, &mName);
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent::~LayerAttachPointComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::OnAddedToEntity(Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
      mEntityManager = &entity.GetEntityManager();
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
   void LayerAttachPointComponent::SetName(StringId name) 
   {      
      mName.Set(name);
      mCurrentName = name;

      LayerAttachPointSystem* sys;
      mEntityManager->GetEntitySystem(TYPE, sys);      
    
      LayerAttachPointComponent* previous;

      if(sys->GetByName(name, previous) && previous != this)
      {
         std::list<dtEntity::EntityId> childentities;
         for(unsigned int i = 0; i < previous->GetGroup()->getNumChildren(); ++i)
         {
            osg::Node* child = previous->GetAttachmentGroup()->getChild(i);
            Entity* entity = dynamic_cast<Entity*>(child->getUserData());
            if(entity != NULL)
            {
               LayerComponent* lc;
               if(entity->GetComponent(lc) &&
                  lc->GetLayer() == name) 
               {
                  MapComponent* mc;
                  entity->GetComponent(mc);
                  childentities.push_back(entity->GetId());
               }
            }
         }

         for(std::list<dtEntity::EntityId>::iterator i = childentities.begin();
            i != childentities.end(); ++i)
         {
            LayerComponent* lc;
            if(mEntityManager->GetComponent(*i, lc))
            {
               lc->OnRemovedFromScene();
            }
         }

         sys->RegisterByName(name, this);

         for(std::list<dtEntity::EntityId>::iterator i = childentities.begin();
            i != childentities.end(); ++i)
         {
            LayerComponent* lc;
            if(mEntityManager->GetComponent(*i, lc))
            {
               lc->OnAddedToScene();
            }
         }
      }
      else
      {
         sys->RegisterByName(name, this);
      }
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LayerAttachPointComponent::SetNode(osg::Node* node)
   {
      assert(mEntityManager);
      node->setName("Layer Attach Point");
      GroupComponent::SetNode(node);      

      LayerSystem* ls;
      mEntityManager->GetEntitySystem(LayerComponent::TYPE, ls);
      std::list<EntityId> eids;
      ls->GetEntitiesInSystem(eids);

      for(std::list<EntityId>::iterator i = eids.begin(); i != eids.end(); ++i)
      {
         LayerComponent* comp;
         mEntityManager->GetComponent(*i, comp);
         if(comp->GetLayer() == GetName())
         {
            comp->SetLayer(GetName());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const StringId LayerAttachPointSystem::TYPE(dtEntity::SID("LayerAttachPoint"));
   const StringId LayerAttachPointSystem::DefaultLayerId(dtEntity::SID("default"));
   const StringId LayerAttachPointSystem::RootId(dtEntity::SID("root"));


   ////////////////////////////////////////////////////////////////////////////////
   LayerAttachPointSystem::LayerAttachPointSystem(EntityManager& em)
      : DefaultEntitySystem<LayerAttachPointComponent>(em)
      , mBaseEntityId(0)
   {
      AddScriptedMethod("getByName", ScriptMethodFunctor(this, &LayerAttachPointSystem::ScriptGetByName));
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
   bool LayerAttachPointSystem::GetByName(StringId name, LayerAttachPointComponent*& recipient)
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
         Entity* base;
         GetEntityManager().CreateEntity(base);
         mBaseEntityId = base->GetId();
         LayerAttachPointComponent* layerattach;
         GetEntityManager().CreateComponent(mBaseEntityId, layerattach);
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
   void LayerAttachPointSystem::RegisterByName(StringId id, LayerAttachPointComponent* c)
   {
      mLayerByNameMap[id] = c;
   }

   ////////////////////////////////////////////////////////////////////////////
   Property* LayerAttachPointSystem::ScriptGetByName(const PropertyArgs& args)
   {
      std::string name = args[0]->StringValue();
      LayerByNameMap::iterator i = mLayerByNameMap.find(dtEntity::SID(name));
      if(i == mLayerByNameMap.end())
      {
         return new StringIdProperty(0);
      }
      return new StringIdProperty(i->first);
   }
}
