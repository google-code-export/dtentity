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
            dtEntity::Entity* entity = dynamic_cast<dtEntity::Entity*>(child->getUserData());
            if(entity != NULL)
            {
               LayerComponent* lc;
               if(entity->GetComponent(lc) &&
                  lc->GetLayer() == name) 
               {
                  dtEntity::MapComponent* mc;
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
      bool found = mEntityManager->GetES(ls);
      assert(found);
      std::list<dtEntity::EntityId> eids;
      ls->GetEntitiesInSystem(eids);

      for(std::list<dtEntity::EntityId>::iterator i = eids.begin(); i != eids.end(); ++i)
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
