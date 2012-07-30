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

#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace dtEntityOSG
{

   
   ///////////////////////////////////////////////////////////////////////////
   /**
    * Loads a static mesh from a path
    */
   class DTENTITY_OSG_EXPORT StaticMeshComponent
      : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId MeshId;
      static const dtEntity::StringId CacheHintId;
      static const dtEntity::StringId OptimizeId;
      static const dtEntity::StringId IsTerrainId;

      static const dtEntity::StringId CacheNoneId;
      static const dtEntity::StringId CacheAllId;
      static const dtEntity::StringId CacheNodesId;
      static const dtEntity::StringId CacheHardwareMeshesId;
      
      StaticMeshComponent();     
      virtual ~StaticMeshComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }

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
      virtual void SetMesh(const std::string& path, dtEntity::StringId cacheHint = CacheAllId);

      void SetCacheHint(dtEntity::StringId v) { mCacheHint.Set(v); }
      dtEntity::StringId GetCacheHint() const { return mCacheHint.Get(); }

      bool GetOptimize() const { return mOptimize.Get(); }
      void SetOptimize(bool v) { mOptimize.Set(v); }

      bool GetIsTerrain() const { return mIsTerrainVal; }
      void SetIsTerrain(bool v);

   protected:
     
      // path to loaded script file
      dtEntity::StringProperty mMeshPathProperty;
      dtEntity::StringIdProperty mCacheHint;
      dtEntity::BoolProperty mOptimize;
      dtEntity::DynamicBoolProperty mIsTerrain;
      bool mIsTerrainVal;
      std::string mLoadedMesh;
   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT StaticMeshSystem
      : public dtEntity::DefaultEntitySystem<StaticMeshComponent>
   {
   public:

      static const dtEntity::ComponentType TYPE;

      StaticMeshSystem(dtEntity::EntityManager& em);
      ~StaticMeshSystem();

      void OnResourceChanged(const dtEntity::Message& msg);

   private:
      dtEntity::MessageFunctor mResourceChangedFunctor;
   };
}
