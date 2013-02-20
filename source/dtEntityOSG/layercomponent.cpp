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

#include <dtEntityOSG/layercomponent.h>

#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/systemmessages.h>
#include <dtEntityOSG/transformcomponent.h>
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

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId LayerComponent::TYPE(dtEntity::SID("Layer"));
   const dtEntity::StringId LayerComponent::LayerId(dtEntity::SID("Layer"));
   const dtEntity::StringId LayerComponent::AttachedComponentId(dtEntity::SID("AttachedComponent"));
   const dtEntity::StringId LayerComponent::VisibleId(dtEntity::SID("Visible"));
   
   ////////////////////////////////////////////////////////////////////////////
   LayerComponent::LayerComponent()
      : mEntity(NULL)
      , mVisible(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &LayerComponent::SetVisible),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &LayerComponent::IsVisible)
        )
      , mVisibleVal(true)
      , mLayer(
           dtEntity::DynamicStringIdProperty::SetValueCB(this, &LayerComponent::SetLayer),
           dtEntity::DynamicStringIdProperty::GetValueCB(this, &LayerComponent::GetLayer)
        )
      , mLayerVal(dtEntity::SIDHash("default"))
      , mAttachedComponent(
           dtEntity::DynamicStringIdProperty::SetValueCB(this, &LayerComponent::SetAttachedComponent),
           dtEntity::DynamicStringIdProperty::GetValueCB(this, &LayerComponent::GetAttachedComponent)
        )
      , mAttachedComponentVal(dtEntity::StringId())
      , mAddedToScene(false)
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
   void LayerComponent::OnAddedToEntity(dtEntity::Entity &entity)
   {
      mEntity = &entity;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::StringId LayerComponent::GetLayer() const
   {
      return mLayerVal;;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::CanAttach()
   {
      return (mEntity != NULL && 
         mLayerVal != dtEntity::StringId() && 
         mAttachedComponentVal != dtEntity::StringId() && 
         IsAddedToScene());
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetLayer(dtEntity::StringId layername)
   {
      assert(mEntity != NULL);

      if(CanAttach() && GetAttachedComponentNode()->getNumParents() != 0)
      {
         Detach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
      }

      //dtEntity::MapComponent* mc;      
      //std::string name = mEntity->GetComponent(mc) ? mc->GetEntityName() : "unnamed";
      //LOG_ALWAYS("Changing layer of " << name << " from " << dtEntity::GetStringFromSID(mLayerVal) << " to " << dtEntity::GetStringFromSID(layername))
      mLayerVal = layername;

      if(CanAttach())
      {
         Attach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerAttachPointComponent* GetAttachPoint(dtEntity::EntityManager& em, dtEntity::StringId name)
   {
      LayerAttachPointComponent* ret = NULL;
      if(name != dtEntity::StringId())
      {
         LayerAttachPointSystem* layerattsystem;
         bool success = em.GetES(layerattsystem);
         assert(success);
         layerattsystem->GetByName(name, ret);         
      }
      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetAttachedComponent(dtEntity::ComponentType handle)
   {
      // if nothing changed or not yet added to scene
      if(CanAttach())
      {
         Detach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
      }
      
      mAttachedComponentVal = handle;
      if(CanAttach())
      {
         Attach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
      }
      
      SetVisible(mVisibleVal);      
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::Detach(LayerAttachPointComponent* current)
   {
      if(current == NULL) 
      {
         return false;
      }

      /*dtEntity::MapComponent* mc;
      if(mEntity->GetComponent(mc))
      {
         LOG_ALWAYS("Detaching entity " << mc->GetEntityName() << " from " << current->GetName());
      }
      else
      {
         LOG_ALWAYS("Detaching unnamed entity from " << current->GetName());
      }

      if(!CanAttach())
      {
         LOG_ERROR("Huch");
      }*/
      assert(CanAttach());      

      // fetch attached component
      osg::Node* attachedNode = GetAttachedComponentNode();      
      if(attachedNode == NULL)
      {
         // attached component no longer exists
         return false;
      }      
      osg::Group* grp = current->GetAttachmentGroup();
      bool success = grp->removeChild(attachedNode);  

      //assert(success);
      if(attachedNode->getNumParents() != 0)
      {
         LOG_ERROR("Detaching: attached node still has parents!");
      }
      return true;
   
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::Attach(LayerAttachPointComponent* current)
   {
      if(current == NULL) 
      {
         return false;
      }

      /*dtEntity::MapComponent* mc;
      if(mEntity->GetComponent(mc))
      {
         LOG_ALWAYS("Attaching entity " << mc->GetEntityName() << " to " << dtEntity::GetStringFromSID(current->GetName()));
      }
      else
      {
         LOG_ALWAYS("Attaching unnamed entity to " << current->GetName());
      }

      if(!CanAttach())
      {
         LOG_ERROR("Huch");
      }*/
      assert(CanAttach());
      
      // add new attachment
      osg::Node* attachedNode = GetAttachedComponentNode();
      if(attachedNode == NULL)
      {
         // component to attach does not yet exist
         return false;
      }
      
      if(attachedNode->getNumParents() != 0)
      {
         LOG_ERROR("Node already attached!");
      }
      osg::Group* attchgrp = current->GetAttachmentGroup();
      bool success = attchgrp->addChild(attachedNode);
      assert(success);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::ComponentType LayerComponent::GetAttachedComponent() const
   {
      return mAttachedComponentVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Node* LayerComponent::GetAttachedComponentNode() const
   {
      assert(mEntity != NULL);
      if(mAttachedComponentVal == dtEntity::StringId()) 
      {
         return NULL;
      }

      Component* ac;
      bool found = mEntity->GetComponent(static_cast<dtEntity::ComponentType>(mAttachedComponentVal), ac);
      if(!found)
      {
         LOG_DEBUG("LayerComponent: Attached component does not exist: " 
            + dtEntity::GetStringFromSID(mAttachedComponentVal));
         return NULL;
      }
      NodeStore* nc = dynamic_cast<NodeStore*>(ac);
      if(nc == NULL) return NULL;
      return nc->GetNode();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::GetBoundingBox(dtEntity::Vec3d& min, dtEntity::Vec3d& max)
   {
      osg::Node* att = GetAttachedComponentNode();
      if(att == NULL)
      {
          return false;
      }

      LayerSystem* sys;
      mEntity->GetEntityManager().GetES(sys);
      unsigned int visiblemask = sys->GetVisibilityBits();

      osg::ComputeBoundsVisitor v;
      v.setTraversalMask(visiblemask);
      att->accept(v);
      min = v.getBoundingBox()._min;
      max = v.getBoundingBox()._max;
      return v.getBoundingBox().valid();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool LayerComponent::IsVisible() const
   {
      return mVisibleVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::SetVisible(bool visible)
   {      
      // overwrite node mask even when mVisibleVal == visible, because
      // attached node may have changed and this call is to re-apply the visibility property
      if(GetAttachedComponentNode() != NULL)
      {
         LayerSystem* sys;
         mEntity->GetEntityManager().GetES(sys);
         unsigned int visiblemask = sys->GetVisibilityBits();

         if(visible)
         {
            GetAttachedComponentNode()->setNodeMask(GetAttachedComponentNode()->getNodeMask() | visiblemask);
         }
         else
         {
            GetAttachedComponentNode()->setNodeMask(GetAttachedComponentNode()->getNodeMask() & ~visiblemask);
         }
      }

      if(mVisibleVal != visible)
      {
         mVisibleVal = visible;
         dtEntity::VisibilityChangedMessage msg;
         msg.SetAboutEntityId(mEntity->GetId());
         msg.SetVisible(visible);
         mEntity->GetEntityManager().EmitMessage(msg);
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::OnAddedToScene()
   {
      if(mAddedToScene) return;
      mAddedToScene = true;
      if(!CanAttach())
      {
         return;
      }
      Attach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerComponent::OnRemovedFromScene()
   {
      if(!CanAttach())
      {
         return;
      }
      Detach(GetAttachPoint(mEntity->GetEntityManager(), mLayerVal));
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
   const dtEntity::StringId LayerSystem::TYPE(dtEntity::SID("Layer"));
   const dtEntity::StringId LayerSystem::VisibilityBitsId(dtEntity::SID("VisibilityBits"));

   ////////////////////////////////////////////////////////////////////////////
   LayerSystem::LayerSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<LayerComponent>(em)
      , mVisibilityBits(dtEntity::NodeMasks::VISIBLE)
   {
      Register(VisibilityBitsId, &mVisibilityBits);

      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &LayerSystem::OnEnterWorld);
      em.RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor,"LayerSystem::OnEnterWorld");

      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &LayerSystem::OnLeaveWorld);
      em.RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor, "LayerSystem::OnLeaveWorld");

      AddScriptedMethod("addVisibleBoundingBox", dtEntity::ScriptMethodFunctor(this, &LayerSystem::ScriptAddVisibleBoundingBox));
      AddScriptedMethod("removeVisibleBoundingBox", dtEntity::ScriptMethodFunctor(this, &LayerSystem::ScriptRemoveVisibleBoundingBox));
      AddScriptedMethod("removeAllBoundingBoxes", dtEntity::ScriptMethodFunctor(this, &LayerSystem::ScriptRemoveAllBoundingBoxes));
      AddScriptedMethod("getBoundingSphere", dtEntity::ScriptMethodFunctor(this, &LayerSystem::ScriptGetBoundingSphere));
   }

   ////////////////////////////////////////////////////////////////////////////
   LayerSystem::~LayerSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerSystem::OnEnterWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityAddedToSceneMessage& msg = static_cast<const dtEntity::EntityAddedToSceneMessage&>(m);

      ComponentStore::iterator i = mComponents.find(msg.GetAboutEntityId());
      if(i != mComponents.end())
      {
         i->second->OnAddedToScene();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void LayerSystem::OnLeaveWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityRemovedFromSceneMessage& msg = static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(m);

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
      v.setTraversalMask(GetVisibilityBits());
      node->accept(v);
      if(!v.mVisited)
      {
         return;
      }
      osg::BoundingBox b = v.mBoundingBox;

      float radius = b.radius();
      if(radius == 0.0f )
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

      geode->setNodeMask(GetVisibilityBits());

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
      TransformComponent* tcomp;
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


      LayerAttachPointSystem* ls;
      GetEntityManager().GetES(ls);
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
   dtEntity::Property* LayerSystem::ScriptGetBoundingSphere(const dtEntity::PropertyArgs& args)
   {
      if(args.size() < 1)
      {
         LOG_ERROR("Usage: getBoundingSphere(entityid)");
         return NULL;
      }
      dtEntity::EntityId id = args[0]->UIntValue();
      LayerComponent* lc = GetComponent(id);
      if(lc == NULL)
      {
         return NULL;
      }
      osg::Vec3d min, max;
      bool success = lc->GetBoundingBox(min, max);
      if(!success)
      {
         return new dtEntity::Vec4dProperty(osg::Vec4d());
      }
      osg::Vec3d center = (min + max) * 0.5;
      double radius = (max - min).length();
      return new dtEntity::Vec4dProperty(center[0], center[1], center[2], radius);

   }
}
