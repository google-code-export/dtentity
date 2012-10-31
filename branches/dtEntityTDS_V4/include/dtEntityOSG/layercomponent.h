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

#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/export.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace dtEntityOSG
{ 

   class LayerAttachPointComponent;
   ////////////////////////////////////////////////////////////////////////////////

   /**
    * Each visible entity has to have a layer component. The layer component
    * defines which node component of the entity is attached to which layer.
    */
   class DTENTITY_OSG_EXPORT LayerComponent
      : public dtEntity::Component
   {
   
      friend class LayerSystem;

   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId LayerId;
      static const dtEntity::StringId AttachedComponentId;
      static const dtEntity::StringId VisibleId;
      
      LayerComponent();
      virtual ~LayerComponent();

      virtual dtEntity::ComponentType GetType() const
      {
         return TYPE; 
      }

      /**
       * The attached component is the component that should be attached
       * to the layer. A component with this type has to be present on
       * the entity and it has to derive from NodeComponent.
       */
      void SetAttachedComponent(dtEntity::ComponentType ctype);
      dtEntity::ComponentType GetAttachedComponent() const;

      virtual void OnAddedToEntity(dtEntity::Entity &entity);
      
      void OnAddedToScene();
      void OnRemovedFromScene();

      /**
       * Attach the entity to the layer with this name. Default value is DefaultLayerId
       */
      dtEntity::StringId GetLayer() const;
      void SetLayer(dtEntity::StringId);

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

      /**
       * @return bool if geometry is attached to layer component
       */
      bool GetBoundingBox(dtEntity::Vec3d& min, dtEntity::Vec3d& max);

   private:

      dtEntity::Entity* mEntity;
      dtEntity::DynamicBoolProperty mVisible;
      bool mVisibleVal;
      dtEntity::DynamicStringIdProperty mLayer;
      dtEntity::StringId mLayerVal;
      dtEntity::DynamicStringIdProperty mAttachedComponent;
      dtEntity::StringId mAttachedComponentVal;
      bool mAddedToScene;

   };

   
   ////////////////////////////////////////////////////////////////////////////////

   class DTENTITY_OSG_EXPORT LayerSystem
      : public dtEntity::DefaultEntitySystem<LayerComponent>
      , public dtEntity::ScriptAccessor
   {
   public:
      
      static const dtEntity::ComponentType TYPE;

      static const dtEntity::StringId VisibilityBitsId;

      LayerSystem(dtEntity::EntityManager& em);
      ~LayerSystem();

      void OnEnterWorld(const dtEntity::Message&);
      void OnLeaveWorld(const dtEntity::Message&);

      void AddVisibleBoundingBox(dtEntity::EntityId id);
      void RemoveVisibleBoundingBox(dtEntity::EntityId id);
      void RemoveAllBoundingBoxes();

      // set or clear these bits when visibility of component is toggled
      void SetVisibilityBits(unsigned int bits) { mVisibilityBits.Set(bits); }
      unsigned int GetVisibilityBits() const { return mVisibilityBits.Get(); }

   private:

      dtEntity::Property* ScriptAddVisibleBoundingBox(const dtEntity::PropertyArgs& args)
      {
         AddVisibleBoundingBox(args[0]->UIntValue());
         return NULL;
      }

      dtEntity::Property* ScriptRemoveVisibleBoundingBox(const dtEntity::PropertyArgs& args)
      {
         RemoveVisibleBoundingBox(args[0]->UIntValue());
         return NULL;
      }

      dtEntity::Property* ScriptRemoveAllBoundingBoxes(const dtEntity::PropertyArgs& args)
      {
         RemoveAllBoundingBoxes();
         return NULL;
      }

      dtEntity::Property* ScriptGetBoundingSphere(const dtEntity::PropertyArgs& args);

      dtEntity::MessageFunctor mEnterWorldFunctor;
      dtEntity::MessageFunctor mLeaveWorldFunctor;

      dtEntity::UIntProperty mVisibilityBits;
      
   };
}
