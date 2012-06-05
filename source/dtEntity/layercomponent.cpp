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

#include <dtEntity/layercomponent.h>

#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/transformcomponent.h>
#include <dtEntity/nodemasks.h>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/PolygonOffset>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <assert.h>
#include <sstream>
#include <osg/ComputeBoundsVisitor>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   const StringId LayerComponent::TYPE(dtEntity::SID("Layer"));
   const StringId LayerComponent::LayerId(dtEntity::SID("Layer"));
   const StringId LayerComponent::AttachedComponentId(dtEntity::SID("AttachedComponent"));
   const StringId LayerComponent::VisibleId(dtEntity::SID("Visible"));   
   
   ////////////////////////////////////////////////////////////////////////////
   LayerComponent::LayerComponent()
      : mAddedToScene(false)
      , mEntity(NULL)
      , mVisible(
           DynamicBoolProperty::SetValueCB(this, &LayerComponent::SetVisible),
           DynamicBoolProperty::GetValueCB(this, &LayerComponent::IsVisible)
        )
      , mVisibleVal(true)
      , mLayer(
           DynamicStringIdProperty::SetValueCB(this, &LayerComponent::SetLayer),
           DynamicStringIdProperty::GetValueCB(this, &LayerComponent::GetLayer)
        )
      , mLayerVal(dtEntity::SIDHash("default"))
      , mAttachedComponent(
           DynamicStringIdProperty::SetValueCB(this, &LayerComponent::SetAttachedComponent),
           DynamicStringIdProperty::GetValueCB(this, &LayerComponent::GetAttachedComponent)
        )
      , mAttachedComponentVal(StringId())
   {
      Register(LayerId, &mLayer);
      Register(AttachedComponentId, &mAttachedComponent);            
      Register(VisibleId, &mVisible);   
      
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerComponent::~LayerComponent()
   {
   }
 
   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::OnAddedToEntity(Entity &entity)
   {
      mEntity = &entity;
   }

   ////////////////////////////////////////////////////////////////////////////
   StringId LayerComponent::GetLayer() const
   {
      return mLayerVal;;
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetLayer(StringId layername)
   {
      if(mLayerVal == layername)
      {
         return;
      }

      assert(mEntity != NULL);

      osg::Node* attachedNode = GetAttachedComponentNode();

      if(attachedNode != NULL && mAddedToScene)
      {
         LayerAttachPointSystem* layerattsystem;
         mEntity->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, layerattsystem);
               
         LayerAttachPointComponent* current;
         if(mLayerVal != StringId() && layerattsystem->GetByName(mLayerVal, current))
         {
            current->GetGroup()->removeChild(attachedNode);           
         }

         LayerAttachPointComponent* next;
         if(layerattsystem->GetByName(layername, next))
         {
            next->GetAttachmentGroup()->addChild(attachedNode);
         }

      }
      mLayerVal = layername;
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetAttachedComponent(ComponentType handle)
   {
      if(mLayerVal == StringId()) 
      {
         mAttachedComponentVal = handle;
         return;
      }

      if(mAttachedComponentVal == handle)
      {
         return;
      }

      LayerAttachPointSystem* layerattsystem;
      mEntity->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, layerattsystem);
      
      LayerAttachPointComponent* current;
      if(!layerattsystem->GetByName(mLayerVal, current))
      {
         LOG_WARNING(os << "Cannot detach node from scene graph. Attach point is "
          << GetStringFromSID(mLayerVal));
         return;
      }

      // remove old attachment
      if(mAddedToScene && mLayerVal != StringId())
      {
         // remove old attachment
         osg::Node* attachedNode = GetAttachedComponentNode();
         
         LayerAttachPointComponent* current;
         if(attachedNode != NULL && layerattsystem->GetByName(mLayerVal, current))
         {
            bool success = current->GetGroup()->removeChild(attachedNode);           
			assert(success);
         }
      }
      
      mAttachedComponentVal = handle;

      // add new attachment
      if(mAddedToScene)
      {
         // remove old attachment
         osg::Node* attachedNode = GetAttachedComponentNode();
         if(attachedNode != NULL)
         {
            current->GetAttachmentGroup()->addChild(attachedNode);
         }
      }
      SetVisible(mVisible.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   ComponentType LayerComponent::GetAttachedComponent() const
   {
      return mAttachedComponentVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Node* LayerComponent::GetAttachedComponentNode() const
   {
      assert(mEntity != NULL);
      if(mAttachedComponentVal == StringId()) return NULL;
      Component* ac;
      bool found = mEntity->GetComponent((ComponentType)mAttachedComponentVal, ac);
      if(!found)
      {
         LOG_DEBUG("LayerComponent: Attached component does not exist: " 
            + GetStringFromSID(mAttachedComponentVal));
         return NULL;
      }
      NodeComponent* nc = dynamic_cast<NodeComponent*>(ac);
      if(nc == NULL) return NULL;
      return nc->GetNode();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::IsVisible() const
   {
      return mVisibleVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetVisible(bool visible)
   {      
      if(mVisibleVal == visible || GetAttachedComponentNode() == NULL) return;

      mVisibleVal = visible;
      if(visible)
      {
         GetAttachedComponentNode()->setNodeMask(GetAttachedComponentNode()->getNodeMask() | dtEntity::NodeMasks::VISIBLE);
      }
      else
      {
         GetAttachedComponentNode()->setNodeMask(GetAttachedComponentNode()->getNodeMask() & ~dtEntity::NodeMasks::VISIBLE);
      }

      VisibilityChangedMessage msg;
      msg.SetAboutEntityId(mEntity->GetId());
      msg.SetVisible(visible);
      mEntity->GetEntityManager().EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::OnAddedToScene()
   {
      if(mAddedToScene) return;
      mAddedToScene = true;

      LayerAttachPointComponent* current = NULL;
      if(mLayerVal != StringId() && mEntity != NULL)
      {
         LayerAttachPointSystem* ls;
         mEntity->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, ls);
         ls->GetByName(mLayerVal, current);
      }

      if(current != NULL && mAttachedComponent.Get() != StringId())
      {  
         osg::Node* attachedNode = GetAttachedComponentNode();
#ifdef _DEBUG
         if(attachedNode == NULL)
         {

            LOG_WARNING("LayerSystem: Cannot attach entity, node to attach is not selected!"
             << " ComponentType: " << GetStringFromSID(mAttachedComponentVal));
         }
         if(attachedNode != NULL && (attachedNode->getUserData() == NULL ||
             dynamic_cast<Entity*>(attachedNode->getUserData()) == NULL))
         {

            LOG_ERROR("Attaching node with no user data to scene graph!"
             << " ComponentType: " << GetStringFromSID(mAttachedComponentVal));
         }
#endif

         if(attachedNode != NULL)
         {
            bool success = current->GetAttachmentGroup()->addChild(attachedNode);
            assert(success);
            Component* ac;
            if(mEntity->GetComponent((ComponentType)mAttachedComponent.Get(), ac))
            {
               static_cast<NodeComponent*>(ac)->SetParentComponent(LayerComponent::TYPE);
            }
            SetVisible(mVisible.Get());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::OnRemovedFromScene()
   {
      if(!mAddedToScene) return;

      if(mLayerVal != StringId() && mAttachedComponentVal != StringId())
      {
         osg::Node* attachedNode = GetAttachedComponentNode();
         LayerAttachPointSystem* ls;
         mEntity->GetEntityManager().GetEntitySystem(LayerAttachPointComponent::TYPE, ls);
         LayerAttachPointComponent* current = NULL;
         ls->GetByName(mLayerVal, current);
         
         if(attachedNode != NULL && current != NULL && current->GetGroup() != NULL)
         {
            bool success = current->GetGroup()->removeChild(attachedNode);
            if(!success)
            {
               LOG_WARNING("Could not remove attached node from scene graph!");
            }
         }
      }
      mAddedToScene = false;
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   // visitor for getting bounds of object relative to its own
   // coordinate system
   class LocalBoundingBoxVisitor : public osg::NodeVisitor
   {
   public:

      LocalBoundingBoxVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
         , mVisited(false)
      {}

      virtual void apply(osg::Node& node)
      {
         if((node.getNodeMask() & dtEntity::NodeMasks::VISIBLE) != 0)
         {
            traverse(node);
         }
      }

      /**
      * Visits the specified geode.
      *
      * @param node the geode to visit
      */
      virtual void apply(osg::Geode& node)
      {
         osg::NodePath p = getNodePath();
         osg::NodePath nodePath;
         osg::NodePath::iterator i = p.begin();
         ++i;
         for(;i != p.end(); ++i)
         {
            nodePath.push_back(*i);
         }

         osg::Matrix matrix = osg::computeLocalToWorld(nodePath);

         for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
         {
            osg::Drawable* dr = node.getDrawable(i);
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(dr);
            if(geometry && geometry->getVertexArray() == NULL)
            {
               continue;
            }
            osg::BoundingBox b = dr->getBound();

            for (unsigned int j = 0; j < 8; ++j)
            {

               osg::Vec3 corner = b.corner(j);
               mBoundingBox.expandBy(corner * matrix);
               mVisited = true;
            }
         }
      }

      /**
      * The aggregate bounding box.
      */
      osg::BoundingBox mBoundingBox;
      bool mVisited;
   };


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId LayerSystem::TYPE(dtEntity::SID("Layer"));


   ////////////////////////////////////////////////////////////////////////////
   LayerSystem::LayerSystem(EntityManager& em)
      : DefaultEntitySystem<LayerComponent>(em)
   {
      mEnterWorldFunctor = MessageFunctor(this, &LayerSystem::OnEnterWorld);
      em.RegisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor,"LayerSystem::OnEnterWorld");

      mLeaveWorldFunctor = MessageFunctor(this, &LayerSystem::OnLeaveWorld);
      em.RegisterForMessages(EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "LayerSystem::OnLeaveWorld");

      AddScriptedMethod("addVisibleBoundingBox", ScriptMethodFunctor(this, &LayerSystem::ScriptAddVisibleBoundingBox));
      AddScriptedMethod("removeVisibleBoundingBox", ScriptMethodFunctor(this, &LayerSystem::ScriptRemoveVisibleBoundingBox));
      AddScriptedMethod("removeAllBoundingBoxes", ScriptMethodFunctor(this, &LayerSystem::ScriptRemoveAllBoundingBoxes));
      AddScriptedMethod("getBoundingSphere", ScriptMethodFunctor(this, &LayerSystem::ScriptGetBoundingSphere));
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerSystem::OnEnterWorld(const Message& m)
   {
      const EntityAddedToSceneMessage& msg = static_cast<const EntityAddedToSceneMessage&>(m);

      ComponentStore::iterator i = mComponents.find(msg.GetAboutEntityId());
      if(i != mComponents.end())
      {
         i->second->OnAddedToScene();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerSystem::OnLeaveWorld(const Message& m)
   {
      const EntityRemovedFromSceneMessage& msg = static_cast<const EntityRemovedFromSceneMessage&>(m);

      ComponentStore::iterator i = mComponents.find(msg.GetAboutEntityId());
      if(i != mComponents.end())
      {
         i->second->OnRemovedFromScene();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LayerSystem::AddVisibleBoundingBox(dtEntity::EntityId id)
   {
      RemoveVisibleBoundingBox(id);

      if(mComponents.find(id) == mComponents.end())
      {
         return;
      }

      LayerComponent* lc = mComponents[id];

      osg::Node* node = lc->GetAttachedComponentNode();
      if(node == NULL || node->asTransform() == NULL)
      {
         return;
      }

      LocalBoundingBoxVisitor v;
      v.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
      node->accept(v);
      if(!v.mVisited)
      {
         return;
      }
      osg::BoundingBox b = v.mBoundingBox;

      float radius = b.radius();
      if(radius == 0 )
      {
         return;
      }

      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
      osg::ref_ptr<osg::Vec3Array> a = new osg::Vec3Array(24);
      geometry->setVertexArray(a);

      float xmax = b.xMax();
      float xmin = b.xMin();
      float ymax = b.yMax();
      float ymin = b.yMin();
      float zmax = b.zMax();
      float zmin = b.zMin();

      osg::Vec3 v0(xmin, ymin, zmin);
      osg::Vec3 v1(xmin, ymax, zmin);
      osg::Vec3 v2(xmax, ymax, zmin);
      osg::Vec3 v3(xmax, ymin, zmin);
      osg::Vec3 v4(xmin, ymin, zmax);
      osg::Vec3 v5(xmin, ymax, zmax);
      osg::Vec3 v6(xmax, ymax, zmax);
      osg::Vec3 v7(xmax, ymin, zmax);

      a->at( 0).set(v0);
      a->at( 1).set(v1);
      a->at( 2).set(v1);
      a->at( 3).set(v2);
      a->at( 4).set(v2);
      a->at( 5).set(v3);
      a->at( 6).set(v3);
      a->at( 7).set(v0);

      a->at( 8).set(v0);
      a->at( 9).set(v4);
      a->at(10).set(v1);
      a->at(11).set(v5);
      a->at(12).set(v2);
      a->at(13).set(v6);
      a->at(14).set(v3);
      a->at(15).set(v7);

      a->at(16).set(v4);
      a->at(17).set(v5);
      a->at(18).set(v5);
      a->at(19).set(v6);
      a->at(20).set(v6);
      a->at(21).set(v7);
      a->at(22).set(v7);
      a->at(23).set(v4);


      osg::Vec4 c[] = { osg::Vec4(1,1,1,1) };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);


      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 24));

      osg::ref_ptr<osg::Geode> geode = new osg::Geode();

      geode->addDrawable(geometry);
      geode->setName("SelectionBounds");
      geode->setNodeMask(dtEntity::NodeMasks::VISIBLE);

      // store pointer to entity to make box identifiable for removal
      dtEntity::Entity* entity;
      GetEntityManager().GetEntity(id, entity);

      geode->setUserData(entity);
      node->asGroup()->addChild(geode);

      // make box wireframe
      osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
      osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth();
      lw->setWidth(2);
      stateset->setAttributeAndModes(lw, osg::StateAttribute::ON);


      osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset();
      polyoffset->setFactor(-1.0f);
      polyoffset->setUnits(-1.0f);
      osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode();
      polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
      stateset->setAttributeAndModes(polyoffset, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
      stateset->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
      stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      stateset->setRenderBinDetails(22000, "RenderBin");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LayerSystem::RemoveVisibleBoundingBox(dtEntity::EntityId id)
   {
      dtEntity::TransformComponent* tcomp;
      if(!GetEntityManager().GetComponent(id, tcomp, true))
      {
         return;
      }
      osg::Group* grp = tcomp->GetNode()->asGroup();

      dtEntity::Entity* entity;
      if(!GetEntityManager().GetEntity(id, entity))
      {
         return;
      }

      for(unsigned int i = 0; i < grp->getNumChildren(); ++i)
      {
         osg::Node* child = grp->getChild(i);
         if(child->getName() == "SelectionBounds" && child->getUserData() == entity)
         {
            grp->removeChild(i, 1);
            return;
         }
      }


      dtEntity::LayerAttachPointSystem* ls;
      GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, ls);
      grp = ls->GetDefaultLayer()->GetNode()->asGroup();

      for(unsigned int i = 0; i < grp->getNumChildren(); ++i)
      {
         osg::Node* child = grp->getChild(i);
         if(child->getName() == "SelectionBounds" && child->getUserData() == entity)
         {
            grp->removeChild(i, 1);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LayerSystem::RemoveAllBoundingBoxes()
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         RemoveVisibleBoundingBox(i->first);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* LayerSystem::ScriptGetBoundingSphere(const PropertyArgs& args)
   {
      if(args.size() < 1)
      {
         LOG_ERROR("Usage: getBoundingSphere(entityid)");
         return NULL;
      }
      EntityId id = args[0]->UIntValue();
      LayerComponent* lc = GetComponent(id);
      if(lc == NULL)
      {
         return NULL;
      }
      osg::Node* att = lc->GetAttachedComponentNode();
      if(att == NULL)
      {
          return new Vec4dProperty(osg::Vec4d());
      }

      osg::ComputeBoundsVisitor v;
      v.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
      att->accept(v);

      return new Vec4dProperty(osg::Vec4d(v.getBoundingBox().center(), v.getBoundingBox().radius()));
   }
}
