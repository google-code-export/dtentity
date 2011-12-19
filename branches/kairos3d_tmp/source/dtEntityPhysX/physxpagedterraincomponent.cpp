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

#include <dtEntityPhysX/physxpagedterraincomponent.h>

#include <dtEntity/applicationcomponent.h>
#include <dtEntity/entity.h>
#include <dtEntityPhysX/physxcomponent.h>
#include <dtEntityPhysX/pagedterraincullvisitor.h>
#include <dtEntityPhysX/utils.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/stringid.h>
#include <osgDB/Registry>
#include <osgViewer/CompositeViewer>
#include <osgViewer/Renderer>
#include <osgViewer/View>
#include <sstream>

namespace dtEntityPhysX
{

   const dtEntity::StringId PhysXPagedTerrainComponent::TYPE(dtEntity::SID("PhysXPagedTerrain"));   
   
   ////////////////////////////////////////////////////////////////////////////
   PhysXPagedTerrainComponent::PhysXPagedTerrainComponent()
   {
   }
    
   ////////////////////////////////////////////////////////////////////////////
   PhysXPagedTerrainComponent::~PhysXPagedTerrainComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////

   TerrainNode::TerrainNode(osg::Geode* toSet) 
      : mGeodePTR(toSet)           // the group pointer of what was loaded
       , mFilledBL(false)           // filled for physics use yet?
       , mFlags(TILE_TODO_DISABLE)  // for flag system
       , mEntityId(0)
   {
      
   }

   //////////////////////////////////////////////////////////////////////
   TerrainNode::~TerrainNode()
   {
      mGeodePTR = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId PhysXPagedTerrainSystem::CookingRadiusId(dtEntity::SID("CookingRadius"));   
   const dtEntity::StringId PhysXPagedTerrainSystem::CullRadiusId(dtEntity::SID("CullRadius"));   
   const dtEntity::StringId PhysXPagedTerrainSystem::FrameDelayId(dtEntity::SID("FrameDelay"));   
   const dtEntity::StringId PhysXPagedTerrainSystem::DBPagerDoPrecompileId(dtEntity::SID("DBPagerDoPrecompile"));   
   const dtEntity::StringId PhysXPagedTerrainSystem::DBPagerTargetFrameRateId(dtEntity::SID("DBPagerTargetFrameRate"));   
   const dtEntity::StringId PhysXPagedTerrainSystem::DBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrameId(dtEntity::SID("DBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrame"));   
   

   PhysXPagedTerrainSystem::PhysXPagedTerrainSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mTerrainId(0)
   {
      mCookingRadius.Set(30000);
      mCullRadius.Set(75000);
      mFrameDelay.Set(30);
      Register(CookingRadiusId, &mCookingRadius);
      Register(CullRadiusId, &mCullRadius);
      Register(FrameDelayId, &mFrameDelay);
      Register(DBPagerDoPrecompileId, &mDBPagerDoPrecompile);
      //Register(DBPagerTargetFrameRateId, &mDBPagerTargetFrameRate);
     // Register(DBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrameId, &mDBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrame);

      mTickFunctor = dtEntity::MessageFunctor(this, &PhysXPagedTerrainSystem::Tick);

      mEnterWorldFunctor = dtEntity::MessageFunctor(this, &PhysXPagedTerrainSystem::OnEnterWorld);
      GetEntityManager().RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);

      mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &PhysXPagedTerrainSystem::OnLeaveWorld);
      GetEntityManager().RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);

      dtEntity::ApplicationSystem* appsys;
      GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);

      osgViewer::ViewerBase::Views views;
      appsys->GetViewer()->getViews(views);
      osgDB::DatabasePager* dbp = views.front()->getDatabasePager();
      mDBPagerDoPrecompile.Set(dbp->getDoPreCompile());
      //mDBPagerTargetFrameRate.Set(dbp->getTargetFrameRate());
      //mDBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrame.Set(dbp->getMinimumTimeAvailableForGLCompileAndDeletePerFrame());
   }

   ////////////////////////////////////////////////////////////////////////////
   PhysXPagedTerrainSystem::~PhysXPagedTerrainSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::Tick(const dtEntity::Message& msg)
   {      
      UpdateCullVisitor();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::OnEnterWorld(const dtEntity::Message& msg)
   {
      const dtEntity::EntityAddedToSceneMessage& m = static_cast<const dtEntity::EntityAddedToSceneMessage&>(msg);
      PhysXPagedTerrainComponent* pcomp;
      if(GetEntityManager().GetComponent(m.GetAboutEntityId(), pcomp))
      {
         SetTerrain(m.GetAboutEntityId());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::OnLeaveWorld(const dtEntity::Message& msg)
   {
      const dtEntity::EntityRemovedFromSceneMessage& m = static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(msg);
      if(m.GetAboutEntityId() == mTerrainId)
      {
         SetTerrain(0);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == CookingRadiusId)
      {
         SetCookingRadius(prop.FloatValue());
      }
      else if(propname == CullRadiusId)
      {
         SetCullRadius(prop.FloatValue());
      }
      else if(propname == FrameDelayId)
      {
         SetFrameDelay(prop.IntValue());
      }
      else if(propname == DBPagerDoPrecompileId)
      {
         dtEntity::ApplicationSystem* appsys;
         GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
         osgViewer::ViewerBase::Views views;
         appsys->GetViewer()->getViews(views);
         osgDB::DatabasePager* dbp = views.front()->getDatabasePager();
         dbp->setDoPreCompile(prop.BoolValue());
      }
      /*else if(propname == DBPagerTargetFrameRateId)
      {
         dtEntity::ApplicationSystem* appsys;
         GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
         osgDB::DatabasePager* dbp = appsys->GetApplication()->GetView()->GetOsgViewerView()->getDatabasePager();
         dbp->setTargetFrameRate(prop.DoubleValue());
      }
      else if(propname == DBPagerMinimumTimeAvailableForGLCompileAndDeletePerFrameId)
      {
         dtEntity::ApplicationSystem* appsys;
         GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
         osgDB::DatabasePager* dbp = appsys->GetApplication()->GetView()->GetOsgViewerView()->getDatabasePager();
         dbp->setMinimumTimeAvailableForGLCompileAndDeletePerFrame(prop.DoubleValue());
      }*/
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::SetCookingRadius(float v)
   {
      mCookingRadius.Set(v);
      if(mCullVisitor) mCullVisitor->SetCookingRadius(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::SetCullRadius(float v)
   {
      mCullRadius.Set(v);
      if(mCullVisitor) mCullVisitor->SetCullRadius(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::SetFrameDelay(int v)
   {
      mFrameDelay.Set(v);
      if(mCullVisitor) mCullVisitor->SetFrameDelay(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::SetTerrain(dtEntity::EntityId id)
   {      
      if(id == mTerrainId)
      {
         return;
      }
      mTerrainId = id;
      // Get terrain
      if(mComponents.size() > 1)
      {
         LOG_ERROR("Can only add one paged terrain to physics!");
         return;
      }

      while(!mTerrainMap.empty())
      {
         TerrainNode* n = mTerrainMap.begin()->second.get();
         GetEntityManager().RemoveFromScene(n->GetEntityId());
         GetEntityManager().KillEntity(n->GetEntityId());
         mTerrainMap.erase(mTerrainMap.begin());
      }

      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);

      dtEntity::ApplicationSystem* appsys;
      GetEntityManager().GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      mCamera = appsys->GetPrimaryCamera();
      
      dtEntity::PositionAttitudeTransformComponent* mtcomp;
      bool success = GetEntityManager().GetComponent(mTerrainId, mtcomp);
      if(!success) 
      {
         LOG_ERROR("PhysX paged terrain component requires a PositionAttitude transform component!");
         return;
      }      

      osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(mCamera->getRenderer());
      osgUtil::SceneView* sceneView = renderer->getSceneView(0);

      
      mCullVisitor = new dtEntityPhysX::PagedTerrainCullVisitor(GetEntityManager());
      mCullVisitor->SetCookingRadius(mCookingRadius.Get());
      mCullVisitor->SetCullRadius(mCullRadius.Get());
      mCullVisitor->SetFrameDelay(mFrameDelay.Get());
      mCullVisitor->SetTerrainNode(mtcomp->GetNode()->asTransform());

      mCullVisitor->setRenderStage(sceneView->getRenderStage());
      mCullVisitor->setStateGraph(sceneView->getStateGraph());

      int cflags = osgUtil::CullVisitor::ENABLE_ALL_CULLING;

      // probably only need to set the scene veiw, and it
      // auto sets the cull visitor but havent checked yet.

      mCullVisitor->setCullingMode(cflags);
      sceneView->setCullingMode(cflags);
      sceneView->setCullVisitor(mCullVisitor.get());      
   }

   ////////////////////////////////////////////////////////////////////////////
   bool PhysXPagedTerrainSystem::UpdateCullVisitor()
   {
      if(!mCullVisitor.valid())
      {
         //LOG_ALWAYS("mCullVisitor Is not valid in Rendering Component, which means \
         //   it wasnt initialized. ");
         return false;
      }

      PagedTerrainCullVisitor* cv = static_cast<PagedTerrainCullVisitor*>(mCullVisitor.get());

      if (cv->GetTerrainNode() == NULL)
      {
         return false;
      }
      assert(mCamera.valid());

      osg::Vec3f eye;
      osg::Vec3f center;
      osg::Vec3f up;
      mCamera->getViewMatrixAsLookAt(eye, center, up);

      cv->SetCameraTransform(eye);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::ResetTerrainIterator()
   {
      mFinalizeTerrainIter = mTerrainMap.begin();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool PhysXPagedTerrainSystem::FinalizeTerrain(int amountOfFrames)
   {
      osg::Vec3 campos = mCamera->getViewMatrix().getTrans();
     
      std::map<osg::Geode*, osg::ref_ptr<TerrainNode> >::iterator removeIter;
      for(unsigned int i = 0;
         mFinalizeTerrainIter != mTerrainMap.end()
         && i < mTerrainMap.size() / amountOfFrames + 1;
         ++i)
      {
         TerrainNode* currentNode = mFinalizeTerrainIter->second.get();
            
         if(currentNode->GetGeodePointer() == NULL)
         {
            // Remove ageia stuff if its loaded.
            if(currentNode->GetEntityId() != 0)
            {
               GetEntityManager().RemoveFromScene(currentNode->GetEntityId());
               GetEntityManager().KillEntity(currentNode->GetEntityId());
            }
            mTerrainMap.erase(mFinalizeTerrainIter++);
            continue;
         }
         else
         {
            // only regard horizontal distance
            osg::Vec3 dist3d = currentNode->GetGeodePointer()->getBoundingBox().center() - campos;
            dist3d[2] = 0;
            float dist = dist3d.length();
         
            if(currentNode->GetEntityId() != 0 && dist > mCookingRadius.Get())
            {
               
               GetEntityManager().RemoveFromScene(currentNode->GetEntityId());
               GetEntityManager().KillEntity(currentNode->GetEntityId());            
               mTerrainMap.erase(mFinalizeTerrainIter++);
               continue;
            
            }
            else if(currentNode->GetFlags() == TerrainNode::TILE_TODO_DISABLE
                  &&currentNode->IsFilled())
            {
               PhysXComponent* pcomp = NULL;
               bool success = GetEntityManager().GetComponent(currentNode->GetEntityId(), pcomp);
               assert(success);
               pcomp->SetCollisionEnabled(false);            
            }
            else if(currentNode->GetFlags() == TerrainNode::TILE_TODO_KEEP
                  &&currentNode->IsFilled())
            {
               PhysXComponent* pcomp = NULL;
               bool success = GetEntityManager().GetComponent(currentNode->GetEntityId(), pcomp);
               assert(success);
               pcomp->SetCollisionEnabled(true);
            }
            else if(currentNode->GetFlags() == TerrainNode::TILE_TODO_LOAD
                  &&currentNode->GetGeodePointer() != NULL)
            {
               // load the tile into physx
               currentNode->SetEntityId(CreatePhysXObject(*currentNode->GetGeodePointer()));
               currentNode->SetFilled(true);

            }
         }

         if(currentNode->IsFilled() == false)
            currentNode->SetFlagsToLoad();
         else
            currentNode->SetFlagsToDisable();

         ++mFinalizeTerrainIter;
      }

      if(mFinalizeTerrainIter ==  mTerrainMap.end() )
      {
         return false;
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityId PhysXPagedTerrainSystem::CreatePhysXObject(osg::Node& node)
   {
      dtEntity::Entity* entity;
      GetEntityManager().CreateEntity(entity);
      
      dtEntity::StaticMeshComponent* smc;
      entity->CreateComponent(smc);
      smc->SetStaticMesh(&node);

      PhysXComponent* physxcomponent;
      entity->CreateComponent(physxcomponent);
      physxcomponent->SetCollisionType(PhysXComponent::CollisionTypeMeshId);
      physxcomponent->SetIsStatic(true);
      physxcomponent->SetCollisionGroup(2);
      GetEntityManager().AddToScene(entity->GetId());
      
      return entity->GetId();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXPagedTerrainSystem::CheckGeode(osg::Geode& node, bool loadNow, const osg::Matrix& matrixForTransform)
   {
      std::map<osg::Geode*, osg::ref_ptr<TerrainNode> >::iterator iter;
      TerrainNode* currentNode = NULL;
      iter = mTerrainMap.find(&node);
      if(iter != mTerrainMap.end())
      {
         currentNode = iter->second.get();
         if(currentNode->GetFlags() != TerrainNode::TILE_TODO_LOAD)
            currentNode->SetFlagsToKeep();
      }
      else
      {
         osg::ref_ptr<TerrainNode> terrainNodeToAdd = new TerrainNode(&node);
         terrainNodeToAdd->SetFlagsToLoad();

         if(loadNow)
         {
            terrainNodeToAdd->SetEntityId(CreatePhysXObject(node));
           
            PhysXComponent* physxcomponent;
            GetEntityManager().GetComponent(terrainNodeToAdd->GetEntityId(), physxcomponent);
            osg::Quat quaternion = matrixForTransform.getRotate();
            physxcomponent->GetPhysXActor()->setGlobalOrientationQuat(NxQuat(NxVec3(quaternion[0],quaternion[1],quaternion[2]), quaternion[3]));
            physxcomponent->GetPhysXActor()->setGlobalPosition(NxVec3(  matrixForTransform.getTrans()[0],
               matrixForTransform.getTrans()[1],
               matrixForTransform.getTrans()[2]));
            terrainNodeToAdd->SetFlagsToKeep();
           
         }
         mTerrainMap.insert(std::make_pair(&node, terrainNodeToAdd));
      }
   }
}
