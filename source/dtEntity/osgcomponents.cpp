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

#include <dtEntity/osgcomponents.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/nodemaskvisitor.h>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <dtEntity/nodemasks.h>
#include <osgDB/ReadFile>
#include <assert.h>
#include <osgDB/FileUtils>
#include <osgUtil/Optimizer>

namespace dtEntity
{

  namespace CacheMode
  {
      enum e
      {
         None,
         All,
         Nodes,
         HardwareMeshes
      };
   }

   ////////////////////////////////////////////////////////////////////////////////
   class ModelCache
   {
      typedef std::map<std::string, osg::ref_ptr<osg::Node> > ModelMap;

   public:

      osg::Node* GetNode(const std::string& path, CacheMode::e cachemode, bool optimize)
      {
         switch(cachemode)
         {
            case CacheMode::Nodes:
            {
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
               {
                  osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                     osg::CopyOp::DEEP_COPY_OBJECTS        |
                     osg::CopyOp::DEEP_COPY_NODES          |
                     osg::CopyOp::DEEP_COPY_USERDATA
                  ));
                  n->setUserData(NULL);
                  return n;
               }
            }
            break;
            case CacheMode::All:
            {
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
               {
                  osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                     osg::CopyOp::DEEP_COPY_USERDATA
                  ));
                  n->setUserData(NULL);
                  return n;
               }
            }break;
            case CacheMode::HardwareMeshes:
            {
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
               {
                  osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                     osg::CopyOp::DEEP_COPY_ALL
                     & ~osg::CopyOp::DEEP_COPY_PRIMITIVES
                     & ~osg::CopyOp::DEEP_COPY_ARRAYS
                     & ~osg::CopyOp::DEEP_COPY_TEXTURES
                     & ~osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
                     & ~osg::CopyOp::DEEP_COPY_IMAGES
                     &  ~osg::CopyOp::DEEP_COPY_SHAPES
                     & ~osg::CopyOp::DEEP_COPY_UNIFORMS
                  ));
                  n->setUserData(NULL);
                  return n;
               }
            }
            break;
            case CacheMode::None:
            {
               ModelMap::iterator i = mModels.find(path);
               if(i != mModels.end())
               {
                  osg::Node* n = osg::clone(i->second.get(), osg::CopyOp(
                     osg::CopyOp::DEEP_COPY_ALL
                  ));
                  n->setUserData(NULL);
                  return n;
               }
            }
            break;
         }
         
         osg::Node* node = osgDB::readNodeFile(path);

         if(node == NULL)
         {
            return NULL;
         }

         if(optimize)
         {
            osgUtil::Optimizer optimizer;
            optimizer.optimize(node);
         }
         if(cachemode != CacheMode::None)
         {
            mModels[path] = node;
         }

         if(cachemode == CacheMode::HardwareMeshes)
         {
            return osg::clone(node, osg::CopyOp(
               osg::CopyOp::DEEP_COPY_ALL &
               ~osg::CopyOp::DEEP_COPY_PRIMITIVES &
               ~osg::CopyOp::DEEP_COPY_ARRAYS &
               ~osg::CopyOp::DEEP_COPY_TEXTURES
            ));           
         }
         else
         {
            return node;
         }
      }

      void Clear()
      {
         mModels.clear();
      }

   private:
      ModelMap mModels;
   };

   ////////////////////////////////////////////////////////////////////////////////
   static ModelCache s_modelCache;

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   const StringId NodeComponent::TYPE(SID("Node"));   
   
   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::NodeComponent()
      : mEntity(NULL)
      , mNode(new osg::Node())
      , mParentComponent(StringId())
   {
      GetNode()->setName("NodeComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::NodeComponent(osg::Node* node)
      : mEntity(NULL)
      , mNode(node)
      , mParentComponent(StringId())
   {
   }
    
   ////////////////////////////////////////////////////////////////////////////
   NodeComponent::~NodeComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::OnAddedToEntity(Entity& entity)
   {
      mEntity = &entity;

      // for picking
      mNode->setUserData(mEntity);
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::OnRemovedFromEntity(Entity& entity)
   {
      if(GetParentComponent() != StringId())
      {
         dtEntity::Component* comp;
         if(entity.GetComponent(GetParentComponent(), comp))
         {
            dtEntity::GroupComponent* grp = dynamic_cast<dtEntity::GroupComponent*>(comp);
            if(grp)
            {
               grp->GetAttachmentGroup()->removeChild(GetNode());
            }
         }
      }
      mEntity = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Node* NodeComponent::GetNode() const
   {
      return mNode;
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::SetNode(osg::Node* node)
   {
      GroupComponent* parent = NULL;
      // remove from parent
      if(mNode.valid() && mEntity != NULL && GetParentComponent() != StringId())
      {
         Component* comp;
         if(mEntity->GetComponent(GetParentComponent(), comp))
         {
            if(GetParentComponent() == LayerComponent::TYPE)
            {
               static_cast<LayerComponent*>(comp)->SetAttachedComponent(StringId());
            }
            else
            {
               parent = static_cast<GroupComponent*>(comp);
               parent->GetAttachmentGroup()->removeChild(mNode);
            }
         }
         mNode = node;
         mNode->setUserData(mEntity);
         if(parent == NULL)
         {
            if(GetParentComponent() == LayerComponent::TYPE)
            {
               static_cast<LayerComponent*>(comp)->SetAttachedComponent(GetType());
            }
         }
         else
         {
            parent->GetAttachmentGroup()->addChild(mNode);
         }
      }
      else
      {
         mNode = node;
         mNode->setUserData(mEntity);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void NodeComponent::SetNodeMask(unsigned int nodemask, bool recursive)
   {
      if(recursive)
      {
         dtEntity::NodeMaskVisitor nv(nodemask);
         GetNode()->accept(nv);
      }
      else
      {
         GetNode()->setNodeMask(nodemask);
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////
   unsigned int NodeComponent::GetNodeMask() const
   {
      return GetNode()->getNodeMask();
   }

   ////////////////////////////////////////////////////////////////////////////
   const StringId GroupComponent::TYPE(SID("Group"));   
   const StringId GroupComponent::ChildrenId(SID("Children"));
   
   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent()
      : BaseClass(new osg::Group())
   {
      GetNode()->setName("GroupComponent");
      GetNode()->setDataVariance(osg::Object::DYNAMIC);
      Register(ChildrenId, &mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent(osg::Group* group)
      : BaseClass(group)
   {
       Register(ChildrenId, &mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::~GroupComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Group* GroupComponent::GetGroup() const
   {
      return static_cast<osg::Group*>(BaseClass::GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroupComponent::AddChildComponent(ComponentType ct)
   {
      Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->addChild(nc->GetNode());
      nc->SetParentComponent(this->GetType());
      mChildren.Add(new StringIdProperty(ct));
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroupComponent::RemoveChildComponent(ComponentType ct)
   {
      Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->removeChild(nc->GetNode());
      nc->SetParentComponent(StringId());

      PropertyArray children = mChildren.Get();
      for(PropertyArray::iterator i = children.begin(); i != children.end(); ++i)
      {
         if((*i)->StringIdValue() == ct)
         {
            mChildren.Remove(*i);
            return true;
         }
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::SetChildren(const PropertyArray& arr)
   {
      mChildren.Set(arr);
      this->OnPropertyChanged(ChildrenId, mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::Finished()
   {
      BaseClass::Finished();
      assert(mEntity != NULL && "Please add group component to entity before adding children!");

      GetAttachmentGroup()->removeChild(0, GetAttachmentGroup()->getNumChildren());

      PropertyArray arr = mChildren.Get();
      PropertyArray::const_iterator it;
      for(it = arr.begin(); it != arr.end(); ++it)
      {
         Property* prop = *it;
         StringId componentType = prop->StringIdValue();

         Component* c;
         bool found = mEntity->GetComponent(componentType, c);
         if(!found)
         {
            LOG_WARNING("Could not attach component to group, not found: "
               + GetStringFromSID(componentType));
         }
         else
         {
            NodeComponent* nc = dynamic_cast<NodeComponent*>(c);

            if(nc)
            {
               GetAttachmentGroup()->addChild(nc->GetNode());
               nc->SetParentComponent(this->GetType());
            }
            else
            {
               LOG_ERROR("Cannot attach as child: Not a node component!");
            }
         }
      }
      GetNode()->dirtyBound();
   }
   
   ////////////////////////////////////////////////////////////////////////////
   const StringId StaticMeshComponent::TYPE(SID("StaticMesh"));
   const StringId StaticMeshComponent::MeshId(SID("Mesh"));
   const StringId StaticMeshComponent::OptimizeId(SID("Optimize"));
   const StringId StaticMeshComponent::IsTerrainId(SID("IsTerrain"));
   const StringId StaticMeshComponent::CacheHintId(SID("CacheHint"));

   const StringId StaticMeshComponent::CacheNoneId(SID("None"));
   const StringId StaticMeshComponent::CacheAllId(SID("All"));
   const StringId StaticMeshComponent::CacheNodesId(SID("Nodes"));
   const StringId StaticMeshComponent::CacheHardwareMeshesId(SID("CacheHardwareMeshes"));

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::StaticMeshComponent()
      : BaseClass(new osg::Node())
      , mCacheHint(CacheNoneId)   
   {
      Register(MeshId, &mMeshPathProperty);
      Register(CacheHintId, &mCacheHint);
      Register(OptimizeId, &mOptimize);
      Register(IsTerrainId, &mIsTerrain);
      
      GetNode()->setName("StaticMeshComponent Initial");
      GetNode()->setNodeMask(
         NodeMasks::VISIBLE  |
         NodeMasks::PICKABLE |
         NodeMasks::RECEIVES_SHADOWS |
         NodeMasks::CASTS_SHADOWS
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   StaticMeshComponent::~StaticMeshComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::Finished()
   {
      SetMesh(mMeshPathProperty.Get(), mCacheHint.Get());
      BaseClass::Finished();
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetMesh(const std::string& path, StringId cacheHint)
   {
      if(mLoadedMesh == path)
      {
         return;
      }
      mLoadedMesh = path;
      mMeshPathProperty.Set(path);
      if(path == "")
      {
         osg::Node* node = new osg::Node();
         node->setName("StaticMeshComponent Empty");
         SetNode(node);
      }
      else
      {
         CacheMode::e cm = CacheMode::None;
         if(cacheHint == CacheAllId)
         {
           cm = CacheMode::All;
         }
         else if(cacheHint == CacheNodesId)
         {
           cm = CacheMode::Nodes;
         }
         else if(cacheHint == CacheHardwareMeshesId)
         {
           cm = CacheMode::HardwareMeshes;
         }
         osg::ref_ptr<osg::Node> meshnode = s_modelCache.GetNode(path, cm, GetOptimize());
         if(meshnode == NULL)
         {
            LOG_ERROR("Could not load static mesh from path " + path);
            meshnode = new osg::Node();
            meshnode->setName("StaticMeshComponent NotFound");
         }
         else
         {
            meshnode->setName("StaticMeshComponent " + path);
         }

         SetStaticMesh(meshnode);    
      }

      MeshChangedMessage msg;
      msg.SetAboutEntityId(mEntity->GetId());
      msg.SetFilePath(path);
      mEntity->GetEntityManager().EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string StaticMeshComponent::GetMesh() const
   {
      return mMeshPathProperty.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshComponent::SetStaticMesh(osg::Node* node)
   {
      assert(mEntity != NULL);

      
      SetNode(node); 
      
      unsigned int nm = NodeMasks::VISIBLE | NodeMasks::PICKABLE | NodeMasks::CASTS_SHADOWS |
            NodeMasks::RECEIVES_SHADOWS;
      if(mIsTerrain.Get())
      {
         nm |= NodeMasks::TERRAIN;
      }
      SetNodeMask(nm, true);
    
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   StaticMeshSystem::StaticMeshSystem(EntityManager& em)
      : DefaultEntitySystem<StaticMeshComponent>(em, NodeComponent::TYPE)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void StaticMeshSystem::ClearCache()
   {
      s_modelCache.Clear();
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////


   const StringId TransformComponent::TYPE(SID("Transform"));


   ////////////////////////////////////////////////////////////////////////////
   TransformComponent::TransformComponent()
      : BaseClass()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   TransformComponent::TransformComponent(osg::Transform* t)
      : BaseClass(t)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   TransformComponent::~TransformComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////


   const StringId MatrixTransformComponent::TYPE(SID("MatrixTransform"));
   const StringId MatrixTransformComponent::MatrixId(SID("Matrix"));
   
   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::MatrixTransformComponent()
      : BaseClass(new osg::MatrixTransform())
      , mMatrix(
           DynamicMatrixProperty::SetValueCB(this, &MatrixTransformComponent::SetMatrix),
           DynamicMatrixProperty::GetValueCB(this, &MatrixTransformComponent::GetMatrix)
        )
   {
      Register(MatrixId, &mMatrix);
      GetNode()->setName("MatrixTransformComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::MatrixTransformComponent(osg::MatrixTransform* trans)
      : BaseClass(trans)
      , mMatrix(
           DynamicMatrixProperty::SetValueCB(this, &MatrixTransformComponent::SetMatrix),
           DynamicMatrixProperty::GetValueCB(this, &MatrixTransformComponent::GetMatrix)
        )
   {
      Register(MatrixId, &mMatrix);
      GetNode()->setName("MatrixTransformComponent");
   }

   ////////////////////////////////////////////////////////////////////////////
   MatrixTransformComponent::~MatrixTransformComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::MatrixTransform* MatrixTransformComponent::GetMatrixTransform()
   {
      return static_cast<osg::MatrixTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   const osg::MatrixTransform* MatrixTransformComponent::GetMatrixTransform() const
   {
      return static_cast<osg::MatrixTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Matrix MatrixTransformComponent::GetMatrix() const
   {
      return GetMatrixTransform()->getMatrix();
   }

   ////////////////////////////////////////////////////////////////////////////
   void MatrixTransformComponent::SetMatrix(const osg::Matrix& m)
   {
      GetMatrixTransform()->setMatrix(m);
   }


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////


   const StringId PositionAttitudeTransformComponent::TYPE(SID("PositionAttitudeTransform"));
   const StringId PositionAttitudeTransformComponent::PositionId(SID("Position"));
   const StringId PositionAttitudeTransformComponent::AttitudeId(SID("Attitude"));
   const StringId PositionAttitudeTransformComponent::ScaleId(SID("Scale"));
   
   ////////////////////////////////////////////////////////////////////////////
   PositionAttitudeTransformComponent::PositionAttitudeTransformComponent()
      : BaseClass(new osg::PositionAttitudeTransform())
      , mPosition(
           DynamicVec3dProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetPosition),
           DynamicVec3dProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetPosition)
        )
      , mScale(
           DynamicVec3dProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetScale),
           DynamicVec3dProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetScale)
        )
      , mAttitude(
           DynamicQuatProperty::SetValueCB(this, &PositionAttitudeTransformComponent::SetAttitude),
           DynamicQuatProperty::GetValueCB(this, &PositionAttitudeTransformComponent::GetAttitude)
        )
   {
      Register(PositionId, &mPosition);
      Register(AttitudeId, &mAttitude);
      Register(ScaleId, &mScale);
      mScale.Set(osg::Vec3(1, 1, 1));
      GetNode()->setName("PositionAttitudeTransformComponent");

   }

   ////////////////////////////////////////////////////////////////////////////
   PositionAttitudeTransformComponent::~PositionAttitudeTransformComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Matrix PositionAttitudeTransformComponent::GetMatrix() const
   {
      osg::Matrix t;
      t.makeTranslate(GetTranslation());
      t.setRotate(GetRotation());
      osg::Matrix s;
      s.makeScale(GetScale());
      return s * t;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetMatrix(const osg::Matrix& mat)
   {
      osg::Vec3d trans, scale;
      osg::Quat rot, so;
      mat.decompose(trans, rot, scale, so);
      SetRotation(rot);
      SetTranslation(trans);
      SetScale(scale);

   }

   ////////////////////////////////////////////////////////////////////////////
   osg::PositionAttitudeTransform* PositionAttitudeTransformComponent::GetPositionAttitudeTransform()
   {
      return static_cast<osg::PositionAttitudeTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   const osg::PositionAttitudeTransform* PositionAttitudeTransformComponent::GetPositionAttitudeTransform() const
   {
      return static_cast<osg::PositionAttitudeTransform*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d PositionAttitudeTransformComponent::GetPosition() const
   {
      return GetPositionAttitudeTransform()->getPosition();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetPosition(const osg::Vec3d& p)
   {
      GetPositionAttitudeTransform()->setPosition(p);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Quat PositionAttitudeTransformComponent::GetAttitude() const
   {
      return GetPositionAttitudeTransform()->getAttitude();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetAttitude(const osg::Quat& p)
   {
      GetPositionAttitudeTransform()->setAttitude(p);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3d PositionAttitudeTransformComponent::GetScale() const
   {
      return GetPositionAttitudeTransform()->getScale();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PositionAttitudeTransformComponent::SetScale(const osg::Vec3d& s)
   {
      GetPositionAttitudeTransform()->setScale(s);
   }

}
