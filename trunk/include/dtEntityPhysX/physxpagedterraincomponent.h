/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#ifndef DTENTITY_PHYSX_PAGEDTERRAIN_COMPONENT
#define DTENTITY_PHYSX_PAGEDTERRAIN_COMPONENT

#include <dtEntityPhysX/export.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntityPhysX/pagedterraincullvisitor.h>
#include <dtEntity/property.h>
#include <osg/Geode>
#include <osgUtil/CullVisitor>

namespace dtEntityPhysX
{

   class DT_ENTITY_PHYSX_EXPORT PhysXPagedTerrainComponent : public dtEntity::Component
   {
      friend class PhysXPagedTerrainSystem;

   public:
      
      static const dtEntity::ComponentType TYPE;
      //static const dtEntity::StringId CollisionTypeId;      
      
      PhysXPagedTerrainComponent();      
      virtual ~PhysXPagedTerrainComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }     
   };


   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////
   // Export symbol not needed, this should not be used
   // by external libraries
   class TerrainNode : public osg::Referenced
   {
      public:
         TerrainNode(osg::Geode* toSet);

      protected:
         ~TerrainNode();
         dtEntity::EntityId mEntityId;

      public:
         ///////////////////////////////////
         enum TILE_TODO
         {
            TILE_TODO_DISABLE = 0,
            TILE_TODO_KEEP,
            TILE_TODO_LOAD
         };

         /////////////////////////////////////////////////////////////////////////
         bool  IsFilled() const {return mFilledBL;}
         void  SetFilled(bool value) {mFilledBL = value;}

         /////////////////////////////////////////////////////////////////////////
         osg::Geode* GetGeodePointer()
         {
            if(mGeodePTR.valid())
               return mGeodePTR.get();
            return NULL;
         }

         /////////////////////////////////////////////////////////////////////////
         void SetFlags(char value)  {mFlags = value;}
         char GetFlags() const {return mFlags;}

         /////////////////////////////////////////////////////////////////////////
         void SetFlagsToKeep()    {mFlags = TILE_TODO_KEEP;}
         void SetFlagsToLoad()    {mFlags = TILE_TODO_LOAD;}
         void SetFlagsToDisable() {mFlags = TILE_TODO_DISABLE;}

         /////////////////////////////////////////////////////////////////////////

         dtEntity::EntityId GetEntityId() const { return mEntityId; }
         void SetEntityId(dtEntity::EntityId id) { mEntityId = id; }
         osg::observer_ptr<osg::Geode>    mGeodePTR;
         bool                             mFilledBL;
         char                             mFlags;
   };


      ////////////////////////////////////////////////////////////////////////////////

   class DT_ENTITY_PHYSX_EXPORT PhysXPagedTerrainSystem
      : public dtEntity::DefaultEntitySystem<PhysXPagedTerrainComponent>
   {
      friend class PagedTerrainCullVisitor;
      typedef dtEntity::DefaultEntitySystem<PhysXPagedTerrainComponent> BaseClass;
      
   public:

      static const dtEntity::StringId CookingRadiusId;
      static const dtEntity::StringId CullRadiusId;
      static const dtEntity::StringId FrameDelayId;
      static const dtEntity::StringId DBPagerDoPrecompileId;
      static const dtEntity::StringId DBPagerTargetFrameRateId;
      static const dtEntity::StringId DBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrameId;

     
      PhysXPagedTerrainSystem(dtEntity::EntityManager& em);
      ~PhysXPagedTerrainSystem();

      void SetTerrain(dtEntity::EntityId id);
      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      void SetCookingRadius(float);
      void SetCullRadius(float);
      void SetFrameDelay(int);

      void Tick(const dtEntity::Message& msg);
      void OnEnterWorld(const dtEntity::Message&);
      void OnLeaveWorld(const dtEntity::Message&);

   private:

      bool UpdateCullVisitor();
      void ResetTerrainIterator();
      bool FinalizeTerrain(int amountOfFrames);
      void CheckGeode(osg::Geode& node, bool loadNow, const osg::Matrix& matrixForTransform);
      dtEntity::EntityId CreatePhysXObject(osg::Node& node);

      osg::ref_ptr<osg::Camera> mCamera;
      dtEntity::FloatProperty mCookingRadius;
      dtEntity::FloatProperty mCullRadius;
      dtEntity::IntProperty mFrameDelay;
      //dtEntity::DoubleProperty mDBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrame;

      dtEntity::BoolProperty mDBPagerDoPrecompile;
      //dtEntity::DoubleProperty mDBPagerTargetFrameRate;

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mEnterWorldFunctor;
      dtEntity::MessageFunctor mLeaveWorldFunctor;
      
      osg::ref_ptr<PagedTerrainCullVisitor> mCullVisitor;

      typedef std::map<osg::Geode*, osg::ref_ptr<TerrainNode> > TerrainMap;
      TerrainMap mTerrainMap;
      std::map<osg::Geode*, osg::ref_ptr<TerrainNode> >::iterator mFinalizeTerrainIter;

      dtEntity::EntityId mTerrainId;
   };
}

#endif // DTENTITY_PHYSX_PAGEDTERRAIN_COMPONENT
