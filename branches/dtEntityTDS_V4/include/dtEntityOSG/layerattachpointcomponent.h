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
#include <dtEntityOSG/export.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>
#include <osg/Group>

namespace dtEntityOSG
{

   class DTENTITY_OSG_EXPORT LayerAttachPointComponent
      : public GroupComponent
   {  
    typedef GroupComponent BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId NameId;
      
      LayerAttachPointComponent();
      virtual ~LayerAttachPointComponent();
      virtual void Finished();
      void OnAddedToEntity(dtEntity::Entity& entity);

      virtual void SetNode(osg::Node* node);

      dtEntity::StringId GetName() const { return mName.Get(); }
      void SetName(dtEntity::StringId name);

      virtual dtEntity::ComponentType GetType() const
      {
         return TYPE; 
      }

   private:
      dtEntity::EntityManager* mEntityManager;
      dtEntity::StringIdProperty mName;
      dtEntity::StringId mCurrentName;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DTENTITY_OSG_EXPORT LayerAttachPointSystem
      : public dtEntity::DefaultEntitySystem<LayerAttachPointComponent>
      , public dtEntity::ScriptAccessor
   {
      friend class LayerAttachPointComponent;

      typedef dtEntity::DefaultEntitySystem<LayerAttachPointComponent> BaseClass;

   public:
    
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId DefaultLayerId;
      static const dtEntity::StringId RootId;

      LayerAttachPointSystem(dtEntity::EntityManager& em);

      virtual bool DeleteComponent(dtEntity::EntityId eid);

      bool GetByName(dtEntity::StringId name, LayerAttachPointComponent*& recipient);

      LayerAttachPointComponent* GetDefaultLayer();
      LayerAttachPointComponent* GetRootLayer();

      osg::Group* GetSceneGraphRoot()
      {
         return mSceneGraphRoot.get();
      }

      void CreateSceneGraphRootEntity(osg::Group* root);
      
      dtEntity::Property* ScriptGetByName(const dtEntity::PropertyArgs& args);
      
   private:

      void RegisterByName(dtEntity::StringId, LayerAttachPointComponent*);
      typedef std::map<dtEntity::StringId, LayerAttachPointComponent*> LayerByNameMap;
      LayerByNameMap mLayerByNameMap;
      osg::ref_ptr<osg::Group> mSceneGraphRoot;
      dtEntity::EntityId mBaseEntityId;
   };

  
}
