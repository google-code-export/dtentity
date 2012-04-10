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
#include <dtEntity/groupcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>
#include <osg/Group>

namespace dtEntity
{

   
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
}
