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

#include <dtEntityPhysX/physxcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/stringid.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/debugdrawmanager.h>
#include <dtEntity/profile.h>

#include "NxPhysics.h"
#include "NxCooking.h"

#include <dtEntityPhysX/errorstream.h>
#include <dtEntityPhysX/stream.h>
#include <dtEntityPhysX/utils.h>
#include <sstream>
#include <osg/Geometry>

namespace dtEntityPhysX
{

   const dtEntity::StringId PhysXComponent::TYPE(dtEntity::SID("PhysX"));   
   const dtEntity::StringId PhysXComponent::BoxHalfLengthsId(dtEntity::SID("BoxHalfLengths"));
   const dtEntity::StringId PhysXComponent::CollisionEnabledId(dtEntity::SID("CollisionEnabled"));
   const dtEntity::StringId PhysXComponent::CollisionTypeId(dtEntity::SID("CollisionType"));
   const dtEntity::StringId PhysXComponent::IsKinematicId(dtEntity::SID("IsKinematic"));
   const dtEntity::StringId PhysXComponent::IsStaticId(dtEntity::SID("IsStatic"));
   const dtEntity::StringId PhysXComponent::InitialVelocityId(dtEntity::SID("InitialVelocity"));
   const dtEntity::StringId PhysXComponent::OffsetId(dtEntity::SID("Offset"));
   const dtEntity::StringId PhysXComponent::RadiusId(dtEntity::SID("Radius"));
   const dtEntity::StringId PhysXComponent::CollisionTypeCubeId(dtEntity::SID("Cube"));
   const dtEntity::StringId PhysXComponent::CollisionTypeSphereId(dtEntity::SID("Sphere"));
   const dtEntity::StringId PhysXComponent::CollisionGroupId(dtEntity::SID("CollisionGroup"));
   const dtEntity::StringId PhysXComponent::SkinWidthId(dtEntity::SID("SkinWidth"));
   const dtEntity::StringId PhysXComponent::CollisionTypeMeshId(dtEntity::SID("Mesh"));
   const dtEntity::StringId PhysXComponent::AngularDampingId(dtEntity::SID("AngularDamping"));
   const dtEntity::StringId PhysXComponent::DensityId(dtEntity::SID("Density"));

   ////////////////////////////////////////////////////////////////////////////
   PhysXComponent::PhysXComponent()
      : mPhysXActor(NULL)
      , mOwnerId(0)
   {
      Register(BoxHalfLengthsId, &mBoxHalfLengths);
      Register(CollisionEnabledId, &mCollisionEnabled);
      Register(CollisionTypeId, &mCollisionType);
      Register(IsKinematicId, &mIsKinematic);      
      Register(IsStaticId, &mIsStatic);  
      Register(InitialVelocityId, &mInitialVelocity);  
      Register(RadiusId, &mRadius);      
      Register(OffsetId, &mOffset);      
      Register(AngularDampingId, &mAngularDamping);    
      Register(DensityId, &mDensity);  
      Register(SkinWidthId, &mSkinWidth);  
      Register(CollisionGroupId, &mCollisionGroup);  

      mBoxHalfLengths.Set(osg::Vec3(0.5f, 0.5f, 0.5f));
      mRadius.Set(1);
      mCollisionEnabled.Set(true);
      mAngularDamping.Set(0.5f);
      mDensity.Set(10);
      mSkinWidth.Set(0.025f);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   PhysXComponent::~PhysXComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == CollisionEnabledId)
      {
         SetCollisionEnabled(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      mOwnerId = entity.GetId();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXComponent::OnRemovedFromEntity(dtEntity::Entity& entity)
   {
      mOwnerId = 0;
   }
   
   ////////////////////////////////////////////////////////////////////////////
   NxActor* PhysXComponent::GetPhysXActor() const
   {
      return mPhysXActor;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXComponent::AddForce(const osg::Vec3& force)
   {
      GetPhysXActor()->addForce(ConvertVec3(force));
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXComponent::SetCollisionEnabled(bool v)
   {
      mCollisionEnabled.Set(v);
      if(mPhysXActor != NULL)
      {
         if(v)
         {
             mPhysXActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
         }
         else
         {
            mPhysXActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
         }
      }
   }

     
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId PhysXSystem::EnabledId(dtEntity::SID("Enabled"));

   PhysXSystem::PhysXSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mPhysXSDK(NULL)
      , mPhysXScene(NULL)
      , mErrorStream(NULL)
      , mTimeSinceLastDebugDraw(0)
      , mDebugDrawManager(new dtEntity::DebugDrawManager(em))
   {

      Register(EnabledId, &mEnabled);
      
      bool success = SetupPhysX();
      if(success)
      {
         mTickFunctor = dtEntity::MessageFunctor(this, &PhysXSystem::Tick);         

         mEnterWorldFunctor = dtEntity::MessageFunctor(this, &PhysXSystem::OnEnterWorld);
         em.RegisterForMessages(dtEntity::EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);

         mLeaveWorldFunctor = dtEntity::MessageFunctor(this, &PhysXSystem::OnLeaveWorld);
         em.RegisterForMessages(dtEntity::EntityRemovedFromSceneMessage::TYPE, mLeaveWorldFunctor);

         SetEnabled(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   PhysXSystem::~PhysXSystem()
   {
      TearDownPhysX();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::SetEnabled(bool enabled)
   {
      mEnabled.Set(enabled);
      if(enabled)
      {
         GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);
      }
      else
      {
         GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == EnabledId)
      {
         SetEnabled(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool PhysXSystem::SetupPhysX()
   {
      mErrorStream = new ErrorStream();
      NxPhysicsSDKDesc desc;
	   NxSDKCreateError errorCode = NXCE_NO_ERROR;
      mPhysXSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, mErrorStream, desc, &errorCode);
      if(mPhysXSDK == NULL) 
	   {
         std::ostringstream os;
         os << "PhysX initialisation error: " << getNxSDKCreateError(errorCode);
         LOG_ERROR(os.str());
         return false;
	   }

      if (mPhysXSDK->getFoundationSDK().getRemoteDebugger() && 
         !mPhysXSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
      {
		   mPhysXSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425, 0xFFFFFFFF);
      }
      
      NxSceneDesc sceneDesc;
      NxVec3 gDefaultGravity(0.0f, 0.0f, -9.81f);
      sceneDesc.gravity	= gDefaultGravity;

      // for asynchronous picking:
      sceneDesc.backgroundThreadCount	= 1;
      mPhysXScene = mPhysXSDK->createScene(sceneDesc);

      NxMaterial * defaultMaterial = mPhysXScene->getMaterialFromIndex(0); 
	   defaultMaterial->setRestitution(0.0f);
	   defaultMaterial->setStaticFriction(0.5f);
	   defaultMaterial->setDynamicFriction(0.5f);
      
      mPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
      mPhysXSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
      mPhysXSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1); 

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::TearDownPhysX()
   {
      mPhysXSDK->release();
      mPhysXSDK = NULL;
      delete mErrorStream;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::ApplySceneGraphTransformsToPhysics()
   {
      ComponentStore::iterator i = mComponents.begin();
      for(; i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         PhysXComponent* component = i->second;
         
         if(component->IsKinematic())
         {
            dtEntity::PositionAttitudeTransformComponent* mtcomp;
            bool success = GetEntityManager().GetComponent(id, mtcomp);
            assert(success);
            
            NxActor* nxactor = component->GetPhysXActor();
            if(nxactor == NULL)
            {
               continue;
            }
            osg::Vec3 trans = mtcomp->GetPosition();
            osg::Quat rot = mtcomp->GetAttitude();
            NxMat33 rotmat(ConvertQuat(rot));
            nxactor->moveGlobalPose(NxMat34(rotmat, ConvertVec3(trans)));
            
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::ApplyPhysicsTransformsToSceneGraph()
   {
      float glmat[16];
      NxVec3 position;

      ComponentStore::iterator i = mComponents.begin();
      for(; i != mComponents.end(); ++i)
      {
         dtEntity::EntityId id = i->first;
         PhysXComponent* component = i->second;

         // transform of kinematics is not changed by physx
         if(component->IsKinematic() || component->IsStatic())
         {
            continue;
         }

         NxActor* nxactor = component->GetPhysXActor();

         if(nxactor == NULL)
         {
            continue;
         }

         dtEntity::PositionAttitudeTransformComponent* mtcomp;
         bool success = GetEntityManager().GetComponent(id, mtcomp);
         if(!success)
         {
            LOG_ERROR("Cannot apply physics to object: Has no transform!");
            continue;
         }
         
         memset(&glmat, 0, sizeof(float) * 16);
         nxactor->getGlobalOrientation().getColumnMajorStride4(glmat);
         position = nxactor->getGlobalPosition();
         glmat[12] = position.x;
         glmat[13] = position.y;
         glmat[14] = position.z;
         glmat[15] = 1.0f;

         // this keeps the rotation and position from jittering when the physics model should be asleep
         // In the future, might be good to optimize this out somehow
         for(int i = 0; i < 16; ++i)
         {
            float floorVar = float(int(glmat[i])); // whole number
            float decVar = glmat[i] - floorVar; // decimal part
            decVar = ((int)(decVar * 1000.0f)) * 0.001f; // truncates decimal part to 3 digits
            glmat[i] = decVar + floorVar;  // add new decimal plus whole number
         }
         osg::Matrix m(glmat);
         mtcomp->SetPosition(m.getTrans());
         mtcomp->SetAttitude(m.getRotate());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::DebugDrawManager* PhysXSystem::GetDebugDrawManager()
   {
      return mDebugDrawManager.get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::Tick(const dtEntity::Message& msg)
   {
      CProfileManager::Start_Profile(dtEntity::SID("PhysXSystem::Tick"));
      float dt = msg.GetFloat(dtEntity::TickMessage::DeltaSimTimeId);
      
      ApplySceneGraphTransformsToPhysics();

      mPhysXScene->simulate(dt);
		mPhysXScene->flushStream();
		mPhysXScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

      ApplyPhysicsTransformsToSceneGraph();

      CProfileManager::Stop_Profile();
      if(mDebugDrawManager->IsEnabled())
      {
         DoDebugDrawing(dt);      
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::OnEnterWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityAddedToSceneMessage& msg = 
         static_cast<const dtEntity::EntityAddedToSceneMessage&>(m);
      dtEntity::EntityId id = msg.GetAboutEntityId();
      PhysXComponent* comp = GetComponent(id);
      if(comp != NULL)
      {
         dtEntity::StringId ctype = comp->GetCollisionType();
                
         if(ctype == PhysXComponent::CollisionTypeSphereId)
         {
            SetSphereShape(id);
         }
         else if(ctype == PhysXComponent::CollisionTypeCubeId)
         {
            SetCubeShape(id);
         }
         else if(ctype == PhysXComponent::CollisionTypeMeshId)
         {
            SetMeshShape(id);
         }
         else
         {
            LOG_ERROR("Unknown collision type: " + dtEntity::GetStringFromSID(ctype));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::OnLeaveWorld(const dtEntity::Message& m)
   {
      const dtEntity::EntityRemovedFromSceneMessage& msg = 
         static_cast<const dtEntity::EntityRemovedFromSceneMessage&>(m);
      dtEntity::EntityId id = msg.GetAboutEntityId();
      PhysXComponent* comp = GetComponent(id);
      if(comp != NULL)
      {

         NxActor* actor = comp->GetPhysXActor();
         if(actor != NULL)
         {
            mPhysXScene->releaseActor(*actor);
         }
      }
   }

   void ConvertColor(unsigned int rgb, osg::Vec4& out)
   {
      out[0] = float((rgb>>16)&0xff)/255.0f;
      out[1] = float((rgb>>8)&0xff)/255.0f;
      out[2] = float((rgb)&0xff)/255.0f;
      out[3] = 1;
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::DoDebugDrawing(float dt)
   {
      float interval = 0;
      mTimeSinceLastDebugDraw += dt;
      if(mTimeSinceLastDebugDraw < interval)
         return;

      
      mTimeSinceLastDebugDraw = 0;

     
      const NxDebugRenderable* dbgRenderable = mPhysXScene->getDebugRenderable();
      
       // Render points
       {
           NxU32 NbPoints = dbgRenderable->getNbPoints();
           const NxDebugPoint* points = dbgRenderable->getPoints();
           std::vector<osg::Vec3> p;
           while(NbPoints--)
           {
              p.push_back(osg::Vec3(points->p.x, points->p.y, points->p.z));
              points++;
           }
           if(!p.empty())
           {
              osg::Vec4 color; 
              ConvertColor(points->color, color);
            //  color = osg::Vec4(0,1,0,1);
              mDebugDrawManager->AddPoints(p, color, 1, interval, false);
           }
               
       }
       
       // Render lines
       {
           NxU32 NbLines = dbgRenderable->getNbLines();
           const NxDebugLine* lines = dbgRenderable->getLines();
           std::vector<osg::Vec3> l;           
           while(NbLines--)
           {
              l.push_back(osg::Vec3(lines->p0.x, lines->p0.y, lines->p0.z));
              l.push_back(osg::Vec3(lines->p1.x, lines->p1.y, lines->p1.z));
              lines++;
           }
           if(!l.empty())
           {  
              osg::Vec4 color; 
              ConvertColor(lines->color, color);
             // color = osg::Vec4(0,1,0,1);
              mDebugDrawManager->AddLines(l, color, 1, interval, false);
           }
       }
       
       // Render triangles
       {
           NxU32 NbTris = dbgRenderable->getNbTriangles();
           const NxDebugTriangle* triangles = dbgRenderable->getTriangles();
           std::vector<osg::Vec3> t;   
           while(NbTris--)
           {
              t.push_back(osg::Vec3(triangles->p0.x, triangles->p0.y, triangles->p0.z));
              t.push_back(osg::Vec3(triangles->p1.x, triangles->p1.y, triangles->p1.z));
              t.push_back(osg::Vec3(triangles->p2.x, triangles->p2.y, triangles->p2.z));
              
               triangles++;
           }
           if(!t.empty())
           {
              osg::Vec4 color; 
              ConvertColor(triangles->color, color);
              //color = osg::Vec4(0,1,0,1);
              mDebugDrawManager->AddTriangles(t, color, 1, interval, false);
           }
       }    

   }

   ////////////////////////////////////////////////////////////////////////////
   bool PhysXSystem::RaycastClosestEntity(const osg::Vec3& origin, const osg::Vec3& dir, dtEntity::EntityId& result) const
   {
      NxRay ray;
   	ray.dir = ConvertVec3(dir);
      ray.orig = ConvertVec3(origin);

      
      NxRaycastHit hit;
      NxShape* shape = mPhysXScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);
      if(shape == NULL || shape->userData == NULL)
      {
         return false;
      }
      PhysXComponent* component = static_cast<PhysXComponent*>(shape->userData);
      result = component->GetOwnerId();
      return true;
     
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::ClearPhysXActor(dtEntity::EntityId id)
   {
      PhysXComponent* component = GetComponent(id);
      if(component == NULL) return;
      NxActor* nxactor = component->mPhysXActor;
      if (nxactor != NULL)
      {
         NxShape*const* shapes = nxactor->getShapes();
         for(unsigned int i = 0; i < nxactor->getNbShapes(); i++)
         {
            if(shapes[i]->isHeightField())
            {
               mPhysXSDK->releaseHeightField(shapes[i]->isHeightField()->getHeightField() );
            }
         }
         nxactor->userData = NULL;
         mPhysXScene->releaseActor(*nxactor);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::SetupPhysXActor(NxActor* actor, PhysXComponent* component)
   {
      actor->userData = (void*)(component);
      if(component->mIsKinematic.Get() && component->mPhysXActor->isDynamic())
      {
         actor->raiseBodyFlag(NX_BF_KINEMATIC);         
      }    
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::SetSphereShape(dtEntity::EntityId id)
   {
      PhysXComponent* component = GetComponent(id);
      if(component == NULL)
      {
         dtEntity::Component* c;
         CreateComponent(id, c);
         component = static_cast<PhysXComponent*>(c);
      }

      ClearPhysXActor(id);
	   
	   NxSphereShapeDesc sphereDesc;
	   sphereDesc.radius = component->GetRadius();
      sphereDesc.userData = (void*)(component);
      sphereDesc.localPose.t = ConvertVec3(component->mOffset.Get());
      sphereDesc.skinWidth = NxReal(component->mSkinWidth.Get());
      sphereDesc.group = component->mCollisionGroup.Get();

	   NxActorDesc actorDesc;
	   actorDesc.shapes.pushBack(&sphereDesc);
      actorDesc.name = "Sphere";
      dtEntity::PositionAttitudeTransformComponent* mtcomp;
      if(GetEntityManager().GetComponent(component->GetOwnerId(), mtcomp))
      {
         osg::Vec3 trans = mtcomp->GetPosition();
         osg::Quat rot = mtcomp->GetAttitude();
         NxMat33 rotmat(ConvertQuat(rot));
         actorDesc.globalPose = NxMat34(rotmat, ConvertVec3(trans));
      }

      if(!component->IsStatic())
      {
         NxBodyDesc bodyDesc;
	      bodyDesc.angularDamping	= component->mAngularDamping.Get();
         bodyDesc.linearVelocity = ConvertVec3(component->mInitialVelocity.Get());
         actorDesc.body			= &bodyDesc;
	      actorDesc.density		= component->mDensity.Get();
      }
	   
	   component->mPhysXActor = mPhysXScene->createActor(actorDesc);

      SetupPhysXActor(component->mPhysXActor, component);
      
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::SetCubeShape(dtEntity::EntityId id)
   {
      PhysXComponent* component = GetComponent(id);
      if(component == NULL)
      {
         dtEntity::Component* c;
         CreateComponent(id, c);
         component = static_cast<PhysXComponent*>(c);
      }

      ClearPhysXActor(id);
	   
	   NxBoxShapeDesc boxDesc;
	   boxDesc.dimensions = ConvertVec3(component->GetBoxHalfLengths());
      boxDesc.userData = (void*)(component);
      boxDesc.localPose.t = ConvertVec3(component->mOffset.Get());
      boxDesc.skinWidth = NxReal(component->mSkinWidth.Get());
      boxDesc.group = component->mCollisionGroup.Get();

	   NxActorDesc actorDesc;
	   actorDesc.shapes.pushBack(&boxDesc);
      actorDesc.name = "Sphere";
      dtEntity::PositionAttitudeTransformComponent* mtcomp;
      if(GetEntityManager().GetComponent(component->GetOwnerId(), mtcomp))
      {
         osg::Vec3 trans = mtcomp->GetPosition();
         osg::Quat rot = mtcomp->GetAttitude();
         NxMat33 rotmat(ConvertQuat(rot));
         actorDesc.globalPose = NxMat34(rotmat, ConvertVec3(trans));
      }

      if(!component->IsKinematic() && !component->IsStatic())
      {
         NxBodyDesc bodyDesc;
	      bodyDesc.angularDamping	= component->mAngularDamping.Get();
         bodyDesc.linearVelocity = ConvertVec3(component->mInitialVelocity.Get());
         actorDesc.body			= &bodyDesc;
	      actorDesc.density		= component->mDensity.Get();
      }
	   
	   component->mPhysXActor = mPhysXScene->createActor(actorDesc);

      SetupPhysXActor(component->mPhysXActor, component);
      
   }

   ////////////////////////////////////////////////////////////////////////////
   void PhysXSystem::SetMeshShape(dtEntity::EntityId id)
   {
      CProfileManager::Start_Profile(dtEntity::SID("PhysXSystem::SetMeshShape"));
      PhysXComponent* component = GetComponent(id);
      if(component == NULL)
      {
         dtEntity::Component* c;
         CreateComponent(id, c);
         component = static_cast<PhysXComponent*>(c);
      }

      ClearPhysXActor(id);
      
      dtEntity::StaticMeshComponent* smcomp;
      bool success = GetEntityManager().GetComponent(id, smcomp);
      if(!success)
      {
         LOG_ERROR("Cannot set mesh collision shape: Entity has no mesh component!");
         return;
      }

      //The maximum number of polygons for a convex mesh is limited to 256. !!!
      //Did you set the node u sent in back to the origin before sending it in!?
      DrawableVisitor<TriangleRecorder> mv;
      smcomp->GetNode()->accept(mv);

      unsigned int vertSize = mv.mFunctor.mVertices.size();
      unsigned int facesSize = mv.mFunctor.mTriangles.size();

      if (vertSize <= 0 || facesSize <= 0)
      {
         LOG_INFO("Physics will ignore the geode with 0 verts.");
         return;
      }

      // It's odd, but the verts and triangles match the perfect memory
      // footprint of what we need for PhysX. We just need to reinterpret cast it.
      NxVec3* Verts = reinterpret_cast<NxVec3*>(&mv.mFunctor.mVertices.front());
      NxU32* Faces = reinterpret_cast<NxU32*>(&mv.mFunctor.mTriangles.front());
     
     
      NxTriangleMeshDesc meshDesc;

      meshDesc.numVertices         = mv.mFunctor.mVertices.size();
      meshDesc.numTriangles        = mv.mFunctor.mTriangles.size();
      meshDesc.pointStrideBytes    = sizeof(NxVec3);
      meshDesc.triangleStrideBytes = 3*sizeof(NxU32);
      meshDesc.points              = Verts;
      meshDesc.triangles           = Faces;
      meshDesc.flags               = 0;

      NxInitCooking();
      MemoryWriteBuffer buf;
      success = NxCookTriangleMesh(meshDesc, buf);
      NxCloseCooking();
      if(!success)
      {
         std::stringstream ss;
         ss << "Cooking Failed, could not build mesh in SetCollisionStaticMesh() in NxAgeiaPhysicsHelper. "
            "NumVerts[" << vertSize << "], NumFaces[" << facesSize << "].";
         ss << std::endl << "   *** Verts is[";
         for (unsigned int vindex = 0; vindex < vertSize; vindex++)
            ss << "(" << Verts[vindex][0] << " " << Verts[vindex][1] << " " << Verts[vindex][2] << ") ";
         ss << "]." << std::endl << "   *** Faces is[";
         for (unsigned int findex = 0; findex < facesSize; findex++)
            ss << " " << Faces[findex];
         ss << "]." << std::endl;
         LOG_ERROR(ss.str());
         return;
      }

      NxTriangleMesh* triangleMesh = mPhysXSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
     
      NxTriangleMeshShapeDesc shapeDesc;
      shapeDesc.meshData = triangleMesh;

      shapeDesc.localPose.t = ConvertVec3(component->mOffset.Get());
      shapeDesc.skinWidth = NxReal(component->mSkinWidth.Get());
      shapeDesc.group = component->mCollisionGroup.Get();

      NxActorDesc actorDesc;     
	   actorDesc.shapes.pushBack(&shapeDesc);
      actorDesc.name = "Mesh";
      if(!component->IsKinematic() && !component->IsStatic())
      {
         NxBodyDesc bodyDesc;
         bodyDesc.angularDamping	= component->mAngularDamping.Get();
         bodyDesc.linearVelocity = ConvertVec3(component->mInitialVelocity.Get());
         actorDesc.body			= &bodyDesc;
         actorDesc.density		= component->mDensity.Get();
      }      

      component->mPhysXActor = mPhysXScene->createActor(actorDesc);
      SetupPhysXActor(component->mPhysXActor, component);
      CProfileManager::Stop_Profile();
   }
}
