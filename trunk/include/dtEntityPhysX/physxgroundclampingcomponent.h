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

#ifndef DTENTITY_PHYSX_GROUNDCLAMPING_COMPONENT
#define DTENTITY_PHYSX_GROUNDCLAMPING_COMPONENT

#include <dtEntityPhysX/export.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/property.h>
#include <osg/Geode>

class NxSceneQuery;

namespace dtEntityPhysX
{
   
   class DT_ENTITY_PHYSX_EXPORT PhysXGroundClampingComponent : public dtEntity::Component
   {
      friend class PhysXGroundClampingSystem;
      
   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ClampingModeId;      
      static const dtEntity::StringId VerticalOffsetId;      

      static const dtEntity::StringId ModeDisabledId;
      static const dtEntity::StringId ModeKeepAboveTerrainId;      
      static const dtEntity::StringId ModeSetHeightToTerrainHeightId;      
      static const dtEntity::StringId ModeSetHeightAndRotationToTerrainId;      
      
      PhysXGroundClampingComponent();      
      virtual ~PhysXGroundClampingComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }     

      void SetClampingMode(dtEntity::StringId id) { mClampingMode.Set(id); }
      dtEntity::StringId GetClampingMode() const { return mClampingMode.Get(); }

      void SetVerticalOffset(float v) { mVerticalOffset.Set(v); }
      float GetVerticalOffset() const { return mVerticalOffset.Get(); }

   private:

      dtEntity::StringIdProperty mClampingMode;
      dtEntity::FloatProperty mVerticalOffset;

   };


   ////////////////////////////////////////////////////////////////////////////////

   class RaycastQueryReport;
   
   class DT_ENTITY_PHYSX_EXPORT PhysXGroundClampingSystem
      : public dtEntity::DefaultEntitySystem<PhysXGroundClampingComponent>
   {
      typedef dtEntity::DefaultEntitySystem<PhysXGroundClampingComponent> BaseClass;
     
   public:
      static const dtEntity::StringId EnabledId;      
      PhysXGroundClampingSystem(dtEntity::EntityManager& em);
      ~PhysXGroundClampingSystem();

      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      void BeforePhysicsStep(const dtEntity::Message& msg);
      void AfterPhysicsStep(const dtEntity::Message& msg);
      
      void DoClamping(dtEntity::EntityId id, const osg::Vec3& groundpos, const osg::Vec3& normal);

   private:

      dtEntity::MessageFunctor mBeforeFunctor;
      dtEntity::MessageFunctor mAfterFunctor;

      NxSceneQuery* mSceneQueryObject;
      RaycastQueryReport* mRaycastQueryReport;

      dtEntity::BoolProperty mEnabled;
      bool mIsEnabled;
   };
}

#endif // DTENTITY_PHYSX_GROUNDCLAMPING_COMPONENT
