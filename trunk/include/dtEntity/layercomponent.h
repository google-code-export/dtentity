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

#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace dtEntity
{ 

   class LayerAttachPointComponent;
   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Each visible entity has to have a layer component. The layer component
    * defines which node component of the entity is attached to which layer.
    */
   class DT_ENTITY_EXPORT LayerComponent 
      : public Component
   {
   
      friend class LayerSystem;

   public:

      static const ComponentType TYPE;
      static const StringId LayerId;
      static const StringId AttachedComponentId;
      static const StringId VisibleId;
      
      LayerComponent();
      virtual ~LayerComponent();

      virtual ComponentType GetType() const
      {
         return TYPE; 
      }

      /**
       * The attached component is the component that should be attached
       * to the layer. A component with this type has to be present on
       * the entity and it has to derive from NodeComponent.
       */
      void SetAttachedComponent(ComponentType ctype);
      ComponentType GetAttachedComponent() const;

      virtual void OnAddedToEntity(Entity &entity);
      virtual void OnPropertyChanged(StringId propname, Property& prop);

      void OnAddedToScene();
      void OnRemovedFromScene();

      /**
       * Attach the entity to the layer with this name. Default value is DefaultLayerId
       */
      StringId GetLayer() const;
      void SetLayer(StringId);

      /**
       * Is the AttachedComponent currently attached to the layer node?
       */
      bool IsVisible() const;

      /**
       * Set to false to detach AttachedComponent from the layer node.
       * Re-attach by setting to true.
       */
      void SetVisible(bool);

      /**
       * Is the entity already added to scene? Can be used to query current initialization status
       */
      bool IsAddedToScene() const { return mAddedToScene; }


      /**
       * Get OSG node of attached component
       */
      osg::Node* GetAttachedComponentNode() const;

   private:

      StringIdProperty mLayerProperty;
      StringIdProperty mAttachedComponent;
      StringId mCurrentlyAttachedComponent;
      StringId mAttachPoint;
      BoolProperty mVisible;
      bool  mCurrentlyVisible;
      bool mAddedToScene;
      Entity* mEntity;
   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT LayerSystem
      : public DefaultEntitySystem<LayerComponent>
      , public ScriptAccessor
   {
   public:
      
      static const ComponentType TYPE;

      LayerSystem(EntityManager& em);

      void OnEnterWorld(const Message&);
      void OnLeaveWorld(const Message&);

      void AddVisibleBoundingBox(dtEntity::EntityId id);
      void RemoveVisibleBoundingBox(dtEntity::EntityId id);
      void RemoveAllBoundingBoxes();

   private:

      Property* ScriptAddVisibleBoundingBox(const PropertyArgs& args)
      {
         AddVisibleBoundingBox(args[0]->UIntValue());
         return NULL;
      }

      Property* ScriptRemoveVisibleBoundingBox(const PropertyArgs& args)
      {
         RemoveVisibleBoundingBox(args[0]->UIntValue());
         return NULL;
      }

      Property* ScriptRemoveAllBoundingBoxes(const PropertyArgs& args)
      {
         RemoveAllBoundingBoxes();
         return NULL;
      }

      Property* ScriptGetBoundingSphere(const PropertyArgs& args);

      MessageFunctor mEnterWorldFunctor;
      MessageFunctor mLeaveWorldFunctor;
      
   };
}
