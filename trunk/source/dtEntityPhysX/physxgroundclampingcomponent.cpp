/*
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

#include <dtEntityPhysX/physxgroundclampingcomponent.h>

#include <dtEntityPhysX/physxcomponent.h>
#include <dtEntityPhysX/utils.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/stringid.h>
#include <sstream>
#include <dtEntity/profile.h>

namespace dtEntityPhysX
{

   const dtEntity::StringId PhysXGroundClampingComponent::TYPE(dtEntity::SID("PhysXGroundClamping"));   
   const dtEntity::StringId PhysXGroundClampingComponent::ClampingModeId(dtEntity::SID("ClampingMode"));   
   const dtEntity::StringId PhysXGroundClampingComponent::VerticalOffsetId(dtEntity::SID("VerticalOffset"));   

   const dtEntity::StringId PhysXGroundClampingComponent::ModeDisabledId(dtEntity::SID("Disabled"));      
   const dtEntity::StringId PhysXGroundClampingComponent::ModeKeepAboveTerrainId(dtEntity::SID("KeepAboveTerrain"));      
   const dtEntity::StringId PhysXGroundClampingComponent::ModeSetHeightToTerrainHeightId(dtEntity::SID("SetHeightToTerrainHeight")); 
   const dtEntity::StringId PhysXGroundClampingComponent::ModeSetHeightAndRotationToTerrainId(dtEntity::SID("SetHeightAndRotationToTerrain")); 

   ////////////////////////////////////////////////////////////////////////////
   PhysXGroundClampingComponent::PhysXGroundClampingComponent()
   {
      Register(VerticalOffsetId, &mVerticalOffset);
      Register(ClampingModeId, &mClampingMode);
      mClampingMode.Set(ModeSetHeightAndRotationToTerrainId);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   PhysXGroundClampingComponent::~PhysXGroundClampingComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////


   class RaycastQueryReport : public NxSceneQueryReport
   {
      PhysXGroundClampingSystem* mPhysXGroundClampingSystem;

   public:
      RaycastQueryReport(PhysXGroundClampingSystem* ps)
      {
         mPhysXGroundClampingSystem = ps;
      }
	   virtual	NxQueryReportResult	onBooleanQuery(void* userData, bool result){ return NX_SQR_CONTINUE; };
	   virtual	NxQueryReportResult	onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits){ return NX_SQR_CONTINUE; };
	   virtual	NxQueryReportResult	onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits){ return NX_SQR_CONTINUE; };

	   virtual	NxQueryReportResult	onRaycastQuery(void* userData, NxU32 numberOfHits, const NxRaycastHit* hits)
	   {
         if(numberOfHits > 0)
         {
            dtEntity::EntityId eid = reinterpret_cast<dtEntity::EntityId>(userData);
            mPhysXGroundClampingSystem->DoClamping(eid, ConvertVec3(hits[0].worldImpact), ConvertVec3(hits[0].worldNormal));
         }
		   return NX_SQR_CONTINUE;
	   }
   };


   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId PhysXGroundClampingSystem::EnabledId(dtEntity::SID("Enabled"));      

   PhysXGroundClampingSystem::PhysXGroundClampingSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mSceneQueryObject(NULL)
      , mRaycastQueryReport(new RaycastQueryReport(this))
      , mIsEnabled(true)
   {
      Register(EnabledId, &mEnabled);
      mEnabled.Set(true);

      if(!em.HasEntitySystem(PhysXComponent::TYPE))
      {
         LOG_ERROR("Cannot do ground clamping: No physx system found!");
         return;
      }
      // make ground clamping happen early
      mBeforeFunctor = dtEntity::MessageFunctor(this, &PhysXGroundClampingSystem::BeforePhysicsStep);
      unsigned int options = dtEntity::FilterOptions::PRIORITY_HIGHER;
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mBeforeFunctor, options);

      mAfterFunctor = dtEntity::MessageFunctor(this, &PhysXGroundClampingSystem::AfterPhysicsStep);
      options = dtEntity::FilterOptions::PRIORITY_LOWEST;
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mAfterFunctor, options);
      
      PhysXSystem* ps;
      bool success = GetEntityManager().GetEntitySystem(PhysXComponent::TYPE, ps);
      assert(success);
      
      NxSceneQueryDesc sceneQueryDesc;
	   sceneQueryDesc.executeMode = NX_SQE_ASYNCHRONOUS;
      sceneQueryDesc.report = mRaycastQueryReport;
	   mSceneQueryObject = ps->GetPhysXScene()->createSceneQuery(sceneQueryDesc);
      
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXGroundClampingSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == EnabledId)
      {
         bool enabled = prop.BoolValue();
   
         if(enabled && !mIsEnabled)
         {
            unsigned int options = dtEntity::FilterOptions::PRIORITY_HIGHER;
            GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mBeforeFunctor, options);

            options = dtEntity::FilterOptions::PRIORITY_LOWEST;
            GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mAfterFunctor, options);
         }
         else if(!enabled && mIsEnabled)
         {
            GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mBeforeFunctor);
            GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mAfterFunctor);
         }
         mIsEnabled = enabled;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   PhysXGroundClampingSystem::~PhysXGroundClampingSystem()
   {
      delete mRaycastQueryReport;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXGroundClampingSystem::BeforePhysicsStep(const dtEntity::Message& msg)
   {
      CProfileManager::Start_Profile(dtEntity::SID("PhysXGroundClampingSystem::BeforePhysicsStep"));
      PhysXSystem* ps;
      bool success = GetEntityManager().GetEntitySystem(PhysXComponent::TYPE, ps);
      assert(success);
      if(!ps->GetEnabled())
      {
         return;
      }
      NxRay worldRay;
	   worldRay.dir = NxVec3(0.0f, 0.0f, -1.0f);
      ComponentStore::iterator i;
      dtEntity::PositionAttitudeTransformComponent* patcomp;
      dtEntity::MatrixTransformComponent* mtcomp;
      for(i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         osg::Vec3 translation;
         if(GetEntityManager().GetComponent(i->first, patcomp))
         {
             translation = patcomp->GetPosition();
         }
         else if(GetEntityManager().GetComponent(i->first, mtcomp))
         {
            osg::Matrix m;
            mtcomp->GetMatrix(m);
            translation = m.getTrans();
         }
         else
         {
            LOG_ERROR("Cannot ground clamp entity without transform component!");
            dtEntity::MapComponent* mc;
            if(GetEntityManager().GetComponent(i->first, mc))
            {
               LOG_ERROR("Name of entity: " + mc->GetEntityName());
            }
            continue;
         }
        
         translation[2] += 1000000;
         worldRay.orig = ConvertVec3(translation);
		   
         NxRaycastHit hit;
         NxU32 raycastBit = NX_RAYCAST_IMPACT | NX_RAYCAST_NORMAL;
         mSceneQueryObject->raycastClosestShape(worldRay, NX_STATIC_SHAPES, hit, 0xFFFFFFFF, NX_MAX_F32, raycastBit, NULL, NULL, (void*)(i->first));
      }
      mSceneQueryObject->execute();      
      CProfileManager::Stop_Profile();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXGroundClampingSystem::AfterPhysicsStep(const dtEntity::Message& msg)
   {
      CProfileManager::Start_Profile(dtEntity::SID("PhysXGroundClampingSystem::AfterPhysicsStep"));
      PhysXSystem* ps;
      bool success = GetEntityManager().GetEntitySystem(PhysXComponent::TYPE, ps);
      assert(success);
      if(!ps->GetEnabled())
      {
         return;
      }
      mSceneQueryObject->finish(true);
      CProfileManager::Stop_Profile();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXGroundClampingSystem::DoClamping(dtEntity::EntityId id, const osg::Vec3& groundpos, const osg::Vec3& normal)
   {
      PhysXGroundClampingComponent* gccomp;
      GetEntityManager().GetComponent(id, gccomp);
      osg::Vec3 offsetPos = groundpos;
      offsetPos[2] += gccomp->GetVerticalOffset();
      dtEntity::StringId clampingMode = gccomp->GetClampingMode();

      dtEntity::PositionAttitudeTransformComponent* patcomp = NULL;
      dtEntity::MatrixTransformComponent* mtcomp = NULL;

      osg::Vec3 translation, nexttranslation;
      if(GetEntityManager().GetComponent(id, patcomp))
      {
         translation = patcomp->GetPosition();
      }
      else if(GetEntityManager().GetComponent(id, mtcomp))
      {
         osg::Matrix m;
         mtcomp->GetMatrix(m);
         translation = m.getTrans();
      }
      else
      {
         LOG_ERROR("Cannot ground clamp entity without transform component!");
         return;
      }

      if(clampingMode == PhysXGroundClampingComponent::ModeDisabledId)
      {
         return;
      }
      if(clampingMode == PhysXGroundClampingComponent::ModeKeepAboveTerrainId)
      {
         nexttranslation = translation;
         if(translation[2] < offsetPos[2])
         {            
            nexttranslation[2] = offsetPos[2];
         }
      }
      else if(clampingMode == PhysXGroundClampingComponent::ModeSetHeightToTerrainHeightId)
      {
         nexttranslation = offsetPos;
      }
      else if(clampingMode == PhysXGroundClampingComponent::ModeSetHeightAndRotationToTerrainId)
      {
         nexttranslation = offsetPos;
         osg::Quat rot;
         if(mtcomp)
         {
            osg::Matrix m; mtcomp->GetMatrix(m);
            rot = m.getRotate();
         }
         else
         {
            rot = patcomp->GetAttitude();
         }

         // get current up vector of entity in global coords
         osg::Vec3 up(0, 0, 1);
         up = rot * up;

         // make rotation from global up vec to surface normal
         osg::Quat addrot;
         addrot.makeRotate(up, normal);

         // apply that rotation to current rotation
         if(mtcomp)
         {
            osg::Matrix mat; mat.makeRotate(rot * addrot);
            mtcomp->SetMatrix(mat);
         }
         else
         {
            patcomp->SetAttitude(rot * addrot);
         }
      }
      else
      {
         LOG_ERROR("Unknown clamping mode: " + dtEntity::GetStringFromSID(clampingMode));
         return;
      }
      if(mtcomp)
      {
         osg::Matrix m; mtcomp->GetMatrix(m);
         m.setTrans(nexttranslation);
         mtcomp->SetMatrix(m);
      }
      else
      {
         patcomp->SetPosition(nexttranslation);
      }
   }
}
