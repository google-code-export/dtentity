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

#include <dtEntitySimulation/groundclampingcomponent.h>

#include <dtEntity/cameracomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/stringid.h>
#include <iostream>

#define MINIMUM_MOVEMENT_DISTANCE 0.2

namespace dtEntitySimulation
{


   const dtEntity::StringId GroundClampingComponent::TYPE(dtEntity::SID("GroundClamping"));
   const dtEntity::StringId GroundClampingComponent::ClampingModeId(dtEntity::SID("ClampingMode"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_DisabledId(dtEntity::SID("Disabled"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_KeepAboveTerrainId(dtEntity::SID("KeepAboveTerrain"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_SetHeightToTerrainHeightId(
      dtEntity::SID("SetHeightToTerrainHeight"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId(
      dtEntity::SID("SetHeightAndRotationToTerrain"));
   const dtEntity::StringId GroundClampingComponent::VerticalOffsetId(dtEntity::SID("VerticalOffset"));
   const dtEntity::StringId GroundClampingComponent::MinDistToCameraId(dtEntity::SID("MinDistToCamera"));

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingComponent::GroundClampingComponent()
      : mTransformComponent(NULL)
      , mEntity(NULL)
      , mIntersector(new osgUtil::LineSegmentIntersector(osg::Vec3d(), osg::Vec3d()))
      , mLastClampedPosition(osg::Vec3(FLT_MAX, FLT_MAX, FLT_MAX))
   {
      Register(ClampingModeId, &mClampingMode);
      Register(VerticalOffsetId, &mVerticalOffset);
      Register(MinDistToCameraId, &mMinDistToCamera);

      mMinDistToCamera.Set(500);

   }
    
   ////////////////////////////////////////////////////////////////////////////
   GroundClampingComponent::~GroundClampingComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingComponent::Finished()
   {
      assert(mEntity != NULL);
      bool success = mEntity->GetEntityManager().GetComponent(mEntity->GetId(), mTransformComponent, true);
      if(!success)
      {
         LOG_ERROR("Ground clamping component depends on transform component!");
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId GroundClampingSystem::EnabledId(dtEntity::SID("Enabled"));
   const dtEntity::StringId GroundClampingSystem::IntersectLayerId(dtEntity::SID("IntersectLayer"));

   GroundClampingSystem::GroundClampingSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mRootNode(NULL)
      , mIntersectorGroup(new osgUtil::IntersectorGroup())
      , mCamera(NULL)
   {

      Register(EnabledId, &mEnabled);
      Register(IntersectLayerId, &mIntersectLayer);
      mEnabled.Set(true);

      mTickFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "GroundClampingSystem::Tick");

      mCameraAddedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::CameraAdded);
      GetEntityManager().RegisterForMessages(dtEntity::CameraAddedMessage::TYPE,
         mCameraAddedFunctor, "GroundClampingSystem::CameraAdded");

      mCameraRemovedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::CameraRemoved);
      GetEntityManager().RegisterForMessages(dtEntity::CameraRemovedMessage::TYPE,
         mCameraAddedFunctor, "GroundClampingSystem::CameraRemoved");

	  mMapLoadedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::MapLoaded);
      GetEntityManager().RegisterForMessages(dtEntity::MapLoadedMessage::TYPE,
         mMapLoadedFunctor, "GroundClampingSystem::MapLoaded");
	  GetEntityManager().RegisterForMessages(dtEntity::MapUnloadedMessage::TYPE,
         mMapLoadedFunctor, "GroundClampingSystem::MapUnLoaded");

      AddScriptedMethod("getTerrainHeight", dtEntity::ScriptMethodFunctor(this, &GroundClampingSystem::ScriptGetTerrainHeight));

      dtEntity::MapSystem* mapsys;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapsys);
      dtEntity::EntityId camid = mapsys->GetEntityIdByUniqueId("cam_0");
      if(camid != dtEntity::EntityId())
      {
         GetEntityManager().GetComponent(camid, mCamera);
      }
      else
      {
         dtEntity::CameraSystem* camsys;
         GetEntityManager().GetEntitySystem(dtEntity::CameraComponent::TYPE, camsys);
         if(camsys->GetNumComponents() != 0)
         {
            mCamera = camsys->begin()->second;
         }
      }

      SetIntersectLayer(dtEntity::LayerAttachPointSystem::DefaultLayerId);
   }

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingSystem::~GroundClampingSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property &prop)
   {
      if(propname == IntersectLayerId)
      {
         SetIntersectLayer(prop.StringIdValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
     GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);
     GetEntityManager().UnregisterForMessages(dtEntity::CameraAddedMessage::TYPE, mCameraAddedFunctor);
     GetEntityManager().UnregisterForMessages(dtEntity::CameraRemovedMessage::TYPE, mCameraRemovedFunctor);
	 GetEntityManager().UnregisterForMessages(dtEntity::MapLoadedMessage::TYPE, mMapLoadedFunctor);
	 GetEntityManager().UnregisterForMessages(dtEntity::MapUnloadedMessage::TYPE, mMapLoadedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::SetIntersectLayer(dtEntity::StringId layername)
   {
      mIntersectLayer.Set(layername);
      dtEntity::LayerAttachPointSystem* layersys;
      GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      dtEntity::LayerAttachPointComponent* c;
      if(layersys->GetByName(layername, c))
      {
         mRootNode = c->GetGroup();
      }
      else
      {
         LOG_ERROR("Could not find intersect layer for ground clamping system!");
         mRootNode = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* GroundClampingSystem::ScriptGetTerrainHeight(const dtEntity::PropertyArgs& args)
   {
      osg::Vec3d pos = args[0]->Vec3dValue();
      int voffset = 10000;
      if(args.size() > 1)
      {
         voffset = args[1]->IntValue();
      }
      bool found = ClampToTerrain(pos, voffset);
      if(found)
      {
         return new dtEntity::DoubleProperty(pos[2]);
      }
      else
      {
         return NULL;
      }
   } 

   ////////////////////////////////////////////////////////////////////////////
   bool GroundClampingSystem::ClampToTerrain(osg::Vec3d& v, int voffset)
   {
      if(!mRootNode.valid())
      {
         return false;
      }
      mIntersectionVisitor.reset();
      osgUtil::LineSegmentIntersector* intersector = 
         new osgUtil::LineSegmentIntersector(osg::Vec3d(v[0], v[1], v[2] + voffset),
         osg::Vec3d(v[0], v[1], v[2] - voffset));
      mIntersectionVisitor.setIntersector(intersector);
      mIntersectionVisitor.setTraversalMask(dtEntity::NodeMasks::TERRAIN);

      mRootNode->accept(mIntersectionVisitor);
	  if(intersector->containsIntersections())
	  {
		  v = intersector->getFirstIntersection().getWorldIntersectPoint();
		  return true;
	  }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::CameraAdded(const dtEntity::Message& m)
   {
      const dtEntity::CameraAddedMessage& msg = static_cast<const dtEntity::CameraAddedMessage&>(m);
      if(msg.GetContextId() == 0)
      {
         GetEntityManager().GetComponent(msg.GetAboutEntityId(), mCamera);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::CameraRemoved(const dtEntity::Message& m)
   {
      const dtEntity::CameraAddedMessage& msg = static_cast<const dtEntity::CameraAddedMessage&>(m);
      if(msg.GetContextId() == 0)
      {
         mCamera = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::MapLoaded(const dtEntity::Message& msg)
   {
	  SetIntersectLayer(mIntersectLayer.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::Tick(const dtEntity::Message& msg)
   {
      if(!mRootNode.valid() || mCamera == NULL || !mEnabled.Get() || mComponents.empty() )
      {
         return;
      }

      osg::Vec3d campos = mCamera->GetPosition();

      mIntersectorGroup->clear();
      mIntersectorGroup->reset();
      
      typedef std::list<std::pair<osgUtil::LineSegmentIntersector*, GroundClampingComponent*> > ISectToCompMap;
      ISectToCompMap iSectToCompMap;

      ComponentStore::iterator i = mComponents.begin();
      for(; i != mComponents.end(); ++i)
      {
         GroundClampingComponent* component = i->second;

         dtEntity::StringId mode = component->GetClampingMode();
         if(mode == GroundClampingComponent::ClampingMode_DisabledId)
         {
            //std::cout << "Disabled\n";
            continue;
         }

         dtEntity::TransformComponent* transformcomp = component->GetTransformComponent();
         assert(transformcomp != NULL);
         osg::Vec3d translation = transformcomp->GetTranslation();

         // don't do clamping if camera is too far away
         float distx = translation[0] - campos[0];
         float disty = translation[1] - campos[1];
         float distToCam = sqrt(distx * distx + disty * disty);
         if(distToCam > component->GetMinDistToCamera())
         {
           // std::cout << "MinDistCam\n";
            continue;
         }

         osg::Vec3d lastpos = component->GetLastClampedPosition();
        
         double distMovedX = fabs(translation[0] - lastpos[0]);
         double distMovedY = fabs(translation[1] - lastpos[1]);

         // if only moved a little: Set height to last clamp height to override other
         // height modifiers
         if(distMovedX < MINIMUM_MOVEMENT_DISTANCE && distMovedY < MINIMUM_MOVEMENT_DISTANCE)
         {
            transformcomp->SetTranslation(osg::Vec3d(translation[0], translation[1], lastpos[2]));
            //std::cout << "MinDistMove\n";
            continue;
         }

         osgUtil::LineSegmentIntersector* intersector = component->GetIntersector();
         intersector->setStart(osg::Vec3d(translation[0], translation[1], translation[2] + 10000));
         intersector->setEnd(osg::Vec3d(translation[0], translation[1], translation[2] - 10000));
         mIntersectorGroup->addIntersector(intersector);
         iSectToCompMap.push_back(std::make_pair(intersector, component));
         static int i = 0;
         ++i;
         //std::cout << "Intersect" << i << "\n";
      }

      mIntersectionVisitor.reset();
      mIntersectionVisitor.setIntersector(mIntersectorGroup.get());
      mRootNode->accept(mIntersectionVisitor);
      
      for(ISectToCompMap::iterator i = iSectToCompMap.begin(); i != iSectToCompMap.end(); ++i)
      {
         osgUtil::LineSegmentIntersector* isector = i->first;
         GroundClampingComponent* component = i->second;
         HandleIntersections(component, isector->getIntersections());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::HandleIntersections(GroundClampingComponent* component,
      const osgUtil::LineSegmentIntersector::Intersections& intersections)
   {
      osgUtil::LineSegmentIntersector::Intersections::const_iterator isect;
      for(isect = intersections.begin(); isect != intersections.end(); ++isect)
      {
         for(osg::NodePath::const_reverse_iterator j = isect->nodePath.rbegin(); j != isect->nodePath.rend(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();

            if(referenced == NULL) continue;
            const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL)
            {
               osg::Vec3d isectpos = isect->getWorldIntersectPoint();         
         
               dtEntity::StringId mode = component->GetClampingMode();

               dtEntity::TransformComponent* transformcomp = component->GetTransformComponent();
               osg::Vec3d translation = transformcomp->GetTranslation();

               float voffset = component->GetVerticalOffset();

               if(mode != GroundClampingComponent::ClampingMode_KeepAboveTerrainId || 
                  translation[2] - voffset < isectpos[2])
               {
                  translation[2] = isectpos[2] + voffset;
                  transformcomp->SetTranslation(translation);
                  component->SetLastClampedPosition(translation);
                  
               }

               if(mode == GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId)
               {
                  osg::Quat rot = transformcomp->GetRotation();

                  // get current up vector of entity in global coords
                  osg::Vec3 up(0, 0, 1);
                  up = rot * up;

                  // make rotation from global up vec to surface normal
                  osg::Quat addrot;
                  addrot.makeRotate(up, isect->getWorldIntersectNormal());
                  rot = rot * addrot;
                  transformcomp->SetRotation(rot);                  
               }               
               return;
            }
         }
      }

      // no intersection found, set last clamped pos to current position
      // so that there is no intersection test until translation changes
      component->SetLastClampedPosition(component->GetTransformComponent()->GetTranslation());
   }
}
