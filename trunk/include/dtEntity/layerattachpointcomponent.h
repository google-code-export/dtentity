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
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/export.h>
#include <dtEntity/groupcomponent.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>

namespace dtEntity
{ 

   class DT_ENTITY_EXPORT LayerAttachPointComponent 
      : public GroupComponent
   {  
    typedef GroupComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId NameId;
      
      LayerAttachPointComponent();
      virtual ~LayerAttachPointComponent();
      virtual void Finished();
      void OnAddedToEntity(Entity& entity);

      virtual void SetNode(osg::Node* node);

      StringId GetName() const { return mName.Get(); }
      void SetName(StringId name);

      virtual ComponentType GetType() const
      {
         return TYPE; 
      }

   private:
      EntityManager* mEntityManager;
      StringIdProperty mName;
      StringId mCurrentName;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT LayerAttachPointSystem
      : public DefaultEntitySystem<LayerAttachPointComponent>
      , public ScriptAccessor
   {
      friend class LayerAttachPointComponent;

      typedef DefaultEntitySystem<LayerAttachPointComponent> BaseClass;

   public:
    
      static const ComponentType TYPE;
      static const StringId DefaultLayerId;
      static const StringId RootId;

      LayerAttachPointSystem(EntityManager& em);   

      virtual bool DeleteComponent(dtEntity::EntityId eid);

      bool GetByName(StringId name, LayerAttachPointComponent*& recipient);

      LayerAttachPointComponent* GetDefaultLayer();
      LayerAttachPointComponent* GetRootLayer();

      osg::Group* GetSceneGraphRoot()
      {
         return mSceneGraphRoot.get();
      }

      void CreateSceneGraphRootEntity(osg::Group* root);
      
      Property* ScriptGetByName(const PropertyArgs& args);
      
   private:

      void RegisterByName(StringId, LayerAttachPointComponent*);
      typedef std::map<StringId, LayerAttachPointComponent*> LayerByNameMap;
      LayerByNameMap mLayerByNameMap;
      osg::ref_ptr<osg::Group> mSceneGraphRoot;
      dtEntity::EntityId mBaseEntityId;
   };

  
}
