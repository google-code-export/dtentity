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

#include <dtEntityEditor/motionmodel.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/core.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/windowinterface.h>
#include <dtEntityOSG/transformcomponent.h>

namespace dtEntityEditor
{
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId MotionModelComponent::TYPE(dtEntity::SID("MotionModel"));

   const dtEntity::StringId MotionModelComponent::ContextIdId(dtEntity::SID("ContextId"));
   const dtEntity::StringId MotionModelComponent::MoveSpeedId(dtEntity::SID("MoveSpeed"));
   const dtEntity::StringId MotionModelComponent::RotateSpeedId(dtEntity::SID("RotateSpeed"));
   const dtEntity::StringId MotionModelComponent::RotateKeySpeedId(dtEntity::SID("RotateKeySpeed"));
   const dtEntity::StringId MotionModelComponent::EnabledId(dtEntity::SID("Enabled"));


   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId MotionModelSystem::TYPE(dtEntity::SID("MotionModel"));

   ////////////////////////////////////////////////////////////////////////////
   MotionModelComponent::MotionModelComponent()
      : mCamera(NULL)
      , mEntity(NULL)
   {
      Register(ContextIdId, &mContextId);
      Register(MoveSpeedId, &mMoveSpeed);
      Register(RotateSpeedId, &mRotateSpeed);
      Register(RotateKeySpeedId, &mRotateKeySpeed);
      Register(EnabledId, &mEnabled);

      mContextId.Set(0);
      mMoveSpeed.Set(100);
      mRotateSpeed.Set(0.001f);
      mRotateKeySpeed.Set(2);
      mEnabled.Set(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      mEntity = &e;
      dtEntity::GetInputInterface()->AddInputCallback(this);
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelComponent::OnRemovedFromEntity(dtEntity::Entity&)
   {
      mEntity = NULL;
      bool success = dtEntity::GetInputInterface()->RemoveInputCallback(this);
      assert(success);
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelComponent::Finished()
   {
      assert(mEntity != NULL);
      bool success = mEntity->GetComponent(mCamera);
      if(success)
      {
         mContextId.Set(mCamera->GetContextId());
      }
      else
      {
         LOG_ERROR("MotionModelComponent on entity without camera component!");
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::KeyUp(const std::string& key, bool handled, unsigned int contextid)
   {
      if(!handled && mEnabled.Get() && (int)contextid == mContextId.Get())
      {
         if(key == "1") mMoveSpeed.Set(5);
         else if(key == "2") mMoveSpeed.Set(15);
         else if(key == "3") mMoveSpeed.Set(40);
         else if(key == "4") mMoveSpeed.Set(100);
         else if(key == "5") mMoveSpeed.Set(250);
         else if(key == "6") mMoveSpeed.Set(600);
         else if(key == "7") mMoveSpeed.Set(1500);
         else if(key == "8") mMoveSpeed.Set(4000);
         else if(key == "9") mMoveSpeed.Set(9000);
         else if(key == "0") mMoveSpeed.Set(20000);
         else if(key == "KP_Add") mMoveSpeed.Set(mMoveSpeed.Get() * 1.1f);
         else if(key == "KP_Subtract") mMoveSpeed.Set(mMoveSpeed.Get() / 1.1f);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::KeyDown(const std::string& name, bool handled, unsigned int contextid)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::MouseButtonUp(int button, bool handled, unsigned int contextid)
   {
      if(!mEnabled.Get() || (int)contextid != mContextId.Get()) return false;
      if(button == 1 || button == 2)
      {
         dtEntity::GetInputInterface()->SetLockCursor(false);
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::MouseButtonDown(int button, bool handled, unsigned int contextid)
   {
      if(handled) return false;
      if(!mEnabled.Get() || (int)contextid != mContextId.Get()) return false;

      dtEntity::InputInterface* inputif = dtEntity::GetInputInterface();
      if(button == 1)
      {
         inputif->SetLockCursor(true);
      }
      else if(button == 2)
      {
         // do intersection test to get pivot point to rotate around
         inputif->SetLockCursor(true);
         double mouseX = inputif->GetAxis(dtEntity::InputInterface::MouseXId);
         double mouseY = inputif->GetAxis(dtEntity::InputInterface::MouseYId);
         osg::Vec3 pickray = dtEntity::GetWindowInterface()->GetPickRay(contextid, mouseX, mouseY);
         osg::Vec3d campos = mCamera->GetPosition();

         std::vector<dtEntity::WindowInterface::Intersection> isects;
         bool found = dtEntity::GetWindowInterface()->GetIntersections(campos, campos + pickray * 10000, isects);
         if(found)
         {
             mPivot = isects.front().mPosition;
         }
         else
         {
            mPivot = campos + pickray * 100;
         }
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::MouseWheel(int dir, bool handled, unsigned int contextid)
   {
      if(!mEnabled.Get())
      {
         return false;
      }

      if(!handled && mCamera != NULL && mContextId.Get() == (int)contextid)
      {
         osg::Vec3d campos = mCamera->GetPosition();
         osg::Vec3d eyedir = mCamera->GetEyeDirection();
         mCamera->SetPosition(campos + eyedir * dir * 20);
         mCamera->UpdateViewMatrix();
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool MotionModelComponent::MouseMove(float x, float y, bool handled, unsigned int contextid)
   {
      if(handled)
      {
         return false;
      }

      if(mCamera == NULL || !mEnabled.Get() || mContextId.Get() != (int)contextid)
      {
         return false;
      }

      dtEntity::InputInterface* inputif = dtEntity::GetInputInterface();

      double mouseX = inputif->GetAxis(dtEntity::InputInterface::MouseDeltaXRawId);
      double mouseY = inputif->GetAxis(dtEntity::InputInterface::MouseDeltaYRawId);

      osg::Vec3d up = mCamera->GetUp();
      osg::Vec3d eyedir = mCamera->GetEyeDirection();
      osg::Vec3d toRight = eyedir ^ up;

      if(inputif->GetMouseButton(1, mContextId.Get()))
      {
         osg::Quat rotateop1; rotateop1.makeRotate(-mouseX * mRotateSpeed.Get(), up);
         osg::Quat rotateop2; rotateop2.makeRotate(mouseY * mRotateSpeed.Get(), toRight);
         eyedir = (rotateop1 * rotateop2) * eyedir;
         eyedir.normalize();
         mCamera->SetEyeDirection(eyedir);
         mCamera->UpdateViewMatrix();
      }
      else if(inputif->GetMouseButton(2, mContextId.Get()))
      {
         osg::Vec3d pivotToCam = mCamera->GetPosition() - mPivot;
         osg::Quat rotateop1; rotateop1.makeRotate(mouseX * -0.001f, up);
         osg::Quat rotateop2; rotateop2.makeRotate(mouseY * 0.001f, toRight);
         pivotToCam = (rotateop1 * rotateop2) * pivotToCam;
         mCamera->SetPosition(pivotToCam + mPivot);

         pivotToCam *= -1;
         pivotToCam.normalize();
         mCamera->SetEyeDirection(pivotToCam);
         mCamera->UpdateViewMatrix();
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelComponent::Tick(float dt)
   {

      if(mCamera == NULL || !mEnabled.Get())
      {
          return;
      }

      dtEntity::InputInterface* inputif = dtEntity::GetInputInterface();

      float speed = mMoveSpeed.Get();
      if(inputif->GetKey("Shift_L", mContextId.Get()))
      {
         speed *= 4;
      }

      bool modified = false;
      if(inputif->GetKey("w", mContextId.Get()))
      {
         mCamera->SetPosition(mCamera->GetPosition() + mCamera->GetEyeDirection() * (dt * speed));
         modified = true;
      }
      if(inputif->GetKey("s", mContextId.Get()))
      {
         mCamera->SetPosition(mCamera->GetPosition() + mCamera->GetEyeDirection() * (dt * -speed));
         modified = true;
      }
      if(inputif->GetKey("a", mContextId.Get()))
      {
         osg::Vec3d toRight = mCamera->GetEyeDirection() ^ mCamera->GetUp();
         mCamera->SetPosition(mCamera->GetPosition() + toRight * (dt * -speed));
         modified = true;
      }
      if(inputif->GetKey("d", mContextId.Get()))
      {
         osg::Vec3d toRight = mCamera->GetEyeDirection() ^ mCamera->GetUp();
         mCamera->SetPosition(mCamera->GetPosition() + toRight * (dt * speed));
         modified = true;
      }
      if(inputif->GetKey("q", mContextId.Get()))
      {
         osg::Vec3d toRight = mCamera->GetEyeDirection() ^ mCamera->GetUp();
         mCamera->SetPosition(mCamera->GetPosition() + (toRight ^ mCamera->GetEyeDirection()) * (dt * -speed));
         modified = true;
      }
      if(inputif->GetKey("e", mContextId.Get()))
      {
         osg::Vec3d toRight = mCamera->GetEyeDirection() ^ mCamera->GetUp();
         mCamera->SetPosition(mCamera->GetPosition() + (toRight ^ mCamera->GetEyeDirection()) * (dt * speed));
         modified = true;
      }

      if(inputif->GetKey("Left", mContextId.Get()))
      {
         osg::Quat q; q.makeRotate(dt * mRotateKeySpeed.Get(), mCamera->GetUp());
         osg::Vec3d eyedir = q * mCamera->GetEyeDirection();
         eyedir.normalize();
         mCamera->SetEyeDirection(eyedir);
         modified = true;
      }
      if(inputif->GetKey("Right", mContextId.Get()))
      {
         osg::Quat q; q.makeRotate(-dt * mRotateKeySpeed.Get(), mCamera->GetUp());
         osg::Vec3d eyedir = q * mCamera->GetEyeDirection();
         eyedir.normalize();
         mCamera->SetEyeDirection(eyedir);
         modified = true;
      }
      if(inputif->GetKey("Up", mContextId.Get()))
      {
         osg::Quat q; q.makeRotate(-dt * mRotateKeySpeed.Get(), mCamera->GetUp() ^ mCamera->GetEyeDirection());
         osg::Vec3d eyedir = q * mCamera->GetEyeDirection();
         eyedir.normalize();
         mCamera->SetEyeDirection(eyedir);
         modified = true;
      }
      if(inputif->GetKey("Down", mContextId.Get()))
      {
         osg::Quat q; q.makeRotate(dt * mRotateKeySpeed.Get(), mCamera->GetUp() ^ mCamera->GetEyeDirection());
         osg::Vec3d eyedir = q * mCamera->GetEyeDirection();
         eyedir.normalize();
         mCamera->SetEyeDirection(eyedir);
         modified = true;
      }

      if(modified)
      {
         mCamera->UpdateViewMatrix();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   MotionModelSystem::MotionModelSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
      mTickFunctor = dtEntity::MessageFunctor(this, &MotionModelSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::TickMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "MotionModelSystem::Tick");

      mMoveToPosFunctor = dtEntity::MessageFunctor(this, &MotionModelSystem::MoveToPos);
      GetEntityManager().RegisterForMessages(dtEntity::MoveCameraToPositionMessage::TYPE,
         mMoveToPosFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "MotionModelSystem::MoveToPos");

      mMoveToEntityFunctor = dtEntity::MessageFunctor(this, &MotionModelSystem::MoveToEntity);
      GetEntityManager().RegisterForMessages(dtEntity::MoveCameraToEntityMessage::TYPE,
         mMoveToEntityFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "MotionModelSystem::MoveToEntity");
   }

   ////////////////////////////////////////////////////////////////////////////
   MotionModelSystem::~MotionModelSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelSystem::Tick(const dtEntity::Message& m)
   {
      const dtEntity::TickMessage& msg = static_cast<const dtEntity::TickMessage&>(m);
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->Tick(msg.GetDeltaRealTime());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelSystem::MoveToPos(const dtEntity::Message& m)
   {
      const dtEntity::MoveCameraToPositionMessage& msg = static_cast<const dtEntity::MoveCameraToPositionMessage&>(m);
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         MotionModelComponent* mmc = i->second;
         if((int)mmc->GetContextId() == msg.GetContextId())
         {
            dtEntityOSG::CameraComponent* cam = mmc->GetCamera();
            if(cam)
            {
               cam->SetPosition(msg.GetPosition());
               osg::Vec3d eyedir = msg.GetLookAt() - msg.GetPosition();
               eyedir.normalize();
               cam->SetEyeDirection(eyedir);
               cam->SetUp(msg.GetUp());
               cam->UpdateViewMatrix();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void MotionModelSystem::MoveToEntity(const dtEntity::Message& m)
   {
      const dtEntity::MoveCameraToEntityMessage& msg = static_cast<const dtEntity::MoveCameraToEntityMessage&>(m);

      dtEntityOSG::TransformComponent* transcomp;
      if(!GetEntityManager().GetComponent(msg.GetAboutEntityId(), transcomp, true))
      {
         LOG_ERROR("Cannot jump to entity: has no transform component!");
         return;
      }
      osg::Vec3d targetpos = transcomp->GetTranslation();

      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         MotionModelComponent* mmc = i->second;
         if((int)mmc->GetContextId() != msg.GetContextId()) continue;

         dtEntityOSG::CameraComponent* cam = mmc->GetCamera();
         if(!cam) continue;

         if(msg.GetKeepCameraDirection())
         {
            osg::Vec3d eyedir = cam->GetEyeDirection();
            eyedir *= msg.GetDistance();
            cam->SetPosition(targetpos - eyedir);
            cam->UpdateViewMatrix();
         }
         else
         {
            osg::Vec3d eyedir = targetpos - cam->GetPosition();
            eyedir.normalize();
            cam->SetEyeDirection(eyedir);
            cam->SetPosition(targetpos + eyedir * -msg.GetDistance());
            cam->UpdateViewMatrix();
         }
      }
   }
}
