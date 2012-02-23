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
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>
#include <osg/Group>

namespace dtEntity
{
   /**
    * Various components / systems for OSG nodes
    */


   ///////////////////////////////////////////////////////////////////////////
   /**
    * Holds a single OSG node. Subclass this if you want to wrap an object
    * derived from osg::Node. NodeComponent instances can be attached to
    * the scene by the Layer system.
    */
   class DT_ENTITY_EXPORT NodeComponent : public Component
   {

   public:
      
      static const ComponentType TYPE;
      
      NodeComponent();
      NodeComponent(osg::Node* node);
      virtual ~NodeComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void OnAddedToEntity(Entity& entity);
      virtual void OnRemovedFromEntity(Entity& entity);

      /**
       * returns encapsulated node
       */
      virtual osg::Node* GetNode() const;

      /**
       * Sets encapsulated node. Also, if the currently held node
       * is attached as a child to another component then it is
       * removed from that parent and the passed node is added as a child
       */
      virtual void SetNode(osg::Node* node);
   
      /**
       * type of compponent that this node component is attached to.
       * Is 0 if not attached to a parent
       */
      ComponentType GetParentComponent() const { return mParentComponent; }
      void SetParentComponent(ComponentType c) { mParentComponent = c; }

      void SetNodeMask(unsigned int nodemask, bool recursive = false);
      unsigned int GetNodeMask() const;

   protected:

      Entity* mEntity;

   private:

      osg::ref_ptr<osg::Node> mNode;
      ComponentType mParentComponent;
      
   };

  
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT NodeSystem
      : public DefaultEntitySystem<NodeComponent>
   {
   public:
      NodeSystem(EntityManager& em)
         : DefaultEntitySystem<NodeComponent>(em)
      {

      }
   };

   ///////////////////////////////////////////////////////////////////////////
   /**
    * Holds a single OSG group.
    */
   class DT_ENTITY_EXPORT GroupComponent : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const ComponentType TYPE;      
      static const StringId ChildrenId;
   
      /**
       * CTor, creates a new group component
       */
      GroupComponent();

      /**
       * CTor, uses passed group component
       */
      GroupComponent(osg::Group* group);

      virtual ~GroupComponent();

      virtual ComponentType GetType() const 
      { 
         return TYPE; 
      }

      virtual void Finished();

      virtual osg::Group* GetGroup() const;
      
      // return group to attach children to
      virtual osg::Group* GetAttachmentGroup() const { return GetGroup(); }

      bool AddChildComponent(ComponentType c);
      bool RemoveChildComponent(ComponentType c);

      /**
       * Expects an array of stringid properties containing component types
       * of children. These will be attached.
       */
      void SetChildren(const PropertyArray& arr);
      PropertyArray GetChildren() const { return mChildren.Get(); }

   private:
      
      ArrayProperty mChildren;
   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT GroupSystem
      : public DefaultEntitySystem<GroupComponent>
   {
   public:
      GroupSystem(EntityManager& em)
         : DefaultEntitySystem<GroupComponent>(em, NodeComponent::TYPE)
      {

      }
   };
   
   
   ///////////////////////////////////////////////////////////////////////////
   /**
    * Loads a static mesh from a path
    */
   class DT_ENTITY_EXPORT StaticMeshComponent 
      : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const ComponentType TYPE;
      static const StringId MeshId;
      static const StringId CacheHintId;
      static const StringId OptimizeId;
      static const StringId IsTerrainId;

      static const StringId CacheNoneId;
      static const StringId CacheAllId;
      static const StringId CacheNodesId;
      static const StringId CacheHardwareMeshesId;
      
      StaticMeshComponent();     
      virtual ~StaticMeshComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void OnPropertyChanged(StringId propname, Property &prop);
      virtual void Finished();

      // set existing geometry as static mesh
      void SetStaticMesh(osg::Node* node);
    
      /**
       * return path of currently loaded mesh
       */
      std::string GetMesh() const;

      /**
       * Load mesh from path
       */
      virtual void SetMesh(const std::string& path, StringId cacheHint = CacheAllId);

      void SetCacheHint(StringId v) { mCacheHint.Set(v); }
      StringId GetCacheHint() const { return mCacheHint.Get(); }

      bool GetOptimize() const { return mOptimize.Get(); }
      void SetOptimize(bool v) { mOptimize.Set(v); }

      bool GetIsTerrain() const { return mIsTerrain.Get(); }
      void SetIsTerrain(bool v) { mIsTerrain.Set(v); }

   protected:
     
      // path to loaded script file
      StringProperty mMeshPathProperty;
      StringIdProperty mCacheHint;
      BoolProperty mOptimize;
      BoolProperty mIsTerrain;
      std::string mLoadedMesh;
   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT StaticMeshSystem
      : public DefaultEntitySystem<StaticMeshComponent>
   {
   public:

      StaticMeshSystem(EntityManager& em);

      void ClearCache();
   };


   ///////////////////////////////////////////////////////////////////////////
   /**
    * abstract base class for transform components
    */
   class DT_ENTITY_EXPORT TransformComponent
      : public GroupComponent
   {
      typedef GroupComponent BaseClass;

   public:

      static const ComponentType TYPE;

      TransformComponent();
      TransformComponent(osg::Transform* trans);

      virtual ~TransformComponent();

      virtual ComponentType GetType() const {
         return TYPE;
      }


      virtual osg::Vec3d GetTranslation() const = 0;
      virtual void SetTranslation(const osg::Vec3d&) = 0;

      virtual osg::Vec3d GetScale() const { return osg::Vec3d(1,1,1); }
      virtual void SetScale(const osg::Vec3d&) {}

      virtual osg::Quat GetRotation() const = 0;
      virtual void SetRotation(const osg::Quat&) = 0;

      virtual osg::Matrix GetMatrix() const
      {
         osg::Matrix matrix;
         matrix.setTrans(GetTranslation());
         matrix.setRotate(GetRotation());
         return matrix;
      }

      virtual void SetMatrix(const osg::Matrix& mat)
      {
         SetTranslation(mat.getTrans());
         SetRotation(mat.getRotate());
      }


   private:

   };

   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT TransformSystem
      : public EntitySystem
   {
   public:
      TransformSystem(EntityManager& em)
         : EntitySystem(em)
      {
      }

      ComponentType GetComponentType() const { return TransformComponent::TYPE; }
   };

   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single matrix transform
    */
   class DT_ENTITY_EXPORT MatrixTransformComponent 
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId MatrixId;
      
      MatrixTransformComponent();
      MatrixTransformComponent(osg::MatrixTransform* trans);
     
      virtual ~MatrixTransformComponent();

      virtual ComponentType GetType() const { 
         return TYPE; 
      }


      const osg::MatrixTransform* GetMatrixTransform() const;
      osg::MatrixTransform* GetMatrixTransform();

      virtual osg::Matrix GetMatrix() const;
      virtual void SetMatrix(const osg::Matrix& m);
      
      virtual osg::Vec3d GetTranslation() const
      {
         osg::Matrix m = GetMatrix();
         return m.getTrans();
      }

      virtual void SetTranslation(const osg::Vec3d& t)
      {
         osg::Matrix m = GetMatrix();
         m.setTrans(t);
         SetMatrix(m);
      }

      virtual osg::Quat GetRotation() const
      {
         osg::Matrix m = GetMatrix();
         return m.getRotate();
      }

      virtual void SetRotation(const osg::Quat& q)
      {
         osg::Matrix m = GetMatrix();
         m.setRotate(q);
         SetMatrix(m);
      }

      virtual osg::Vec3d GetScale() const
      {
         osg::Matrix m = GetMatrix();
         return m.getScale();
      }

      virtual void SetScale(const osg::Vec3d& v)
      {
         osg::Matrix m = GetMatrix();
         m = m.scale(v);
         SetMatrix(m);
      }

   private:

      DynamicMatrixProperty mMatrix;
   };

   
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT MatrixTransformSystem
      : public DefaultEntitySystem<MatrixTransformComponent>
   {
   public:
      MatrixTransformSystem(EntityManager& em)
         : DefaultEntitySystem<MatrixTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

   ///////////////////////////////////////////////////////////////////////////
   /**
    * holds a single PositionAttitude transform
    */
   class DT_ENTITY_EXPORT PositionAttitudeTransformComponent 
      : public TransformComponent
   {
      typedef TransformComponent BaseClass;

   public:

      static const ComponentType TYPE;
      static const StringId PositionId;
      static const StringId AttitudeId;
      static const StringId ScaleId;
      
      PositionAttitudeTransformComponent();
     
      virtual ~PositionAttitudeTransformComponent();

      virtual ComponentType GetType() const { 
         return TYPE; 
      }

      const osg::PositionAttitudeTransform* GetPositionAttitudeTransform() const;
      osg::PositionAttitudeTransform* GetPositionAttitudeTransform();

      osg::Vec3d GetPosition() const;
      void SetPosition(const osg::Vec3d& p);

      osg::Quat GetAttitude() const;
      void SetAttitude(const osg::Quat& r);

      osg::Vec3d GetScale() const;
      void SetScale(const osg::Vec3d& q);

      virtual osg::Vec3d GetTranslation() const  { return GetPosition(); }
      virtual void SetTranslation(const osg::Vec3d& t) { SetPosition(t); }

      virtual osg::Quat GetRotation() const { return GetAttitude(); }
      virtual void SetRotation(const osg::Quat& q) { SetAttitude(q); }

      virtual osg::Matrix GetMatrix() const;
      virtual void SetMatrix(const osg::Matrix& mat);

   private:

      DynamicVec3dProperty mPosition;
      DynamicVec3dProperty mScale;
      DynamicQuatProperty mAttitude;
   };

   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT PositionAttitudeTransformSystem
      : public DefaultEntitySystem<PositionAttitudeTransformComponent>
   {
   public:
      PositionAttitudeTransformSystem(EntityManager& em)
         : DefaultEntitySystem<PositionAttitudeTransformComponent>(em, TransformComponent::TYPE)
      {

      }
   };

}
