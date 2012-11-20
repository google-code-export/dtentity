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

#pragma once

#include <dtEntity/defaultentitysystem.h>
#include <dtEntityOSG/cameracomponent.h>
#include <dtEntity/inputinterface.h>

namespace dtEntityEditor
{   

   class MotionModelComponent
         : public dtEntity::Component
         , public dtEntity::InputCallbackInterface
   {
   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ContextIdId;
      static const dtEntity::StringId MoveSpeedId;
      static const dtEntity::StringId RotateSpeedId;
      static const dtEntity::StringId RotateKeySpeedId;
      static const dtEntity::StringId EnabledId;


      MotionModelComponent();

      dtEntity::ComponentType GetType() const { return TYPE; }

      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      {
         return (id == TYPE);
      }

      void OnAddedToEntity(dtEntity::Entity &entity);
      void OnRemovedFromEntity(dtEntity::Entity &entity);

      virtual void Finished();
      void Tick(float dt);

      unsigned int GetContextId() const { return mContextId.Get(); }
      void SetContextId(unsigned int i) { mContextId.Set(i); }

      virtual bool KeyUp(const std::string& name, bool handled, unsigned int contextid);
      virtual bool KeyDown(const std::string& name, bool handled, unsigned int contextid);
      virtual bool MouseButtonUp(int button, bool handled, unsigned int contextid);
      virtual bool MouseButtonDown(int button, bool handled, unsigned int contextid);
      virtual bool MouseWheel(int dir, bool handled, unsigned int contextid);
      virtual bool MouseMove(float x, float y, bool handled, unsigned int contextid);
      virtual void MouseEnterLeave(bool focused, unsigned int contextid) {}

      dtEntityOSG::CameraComponent* GetCamera() const { return mCamera; }
   private:
      dtEntity::IntProperty mContextId;
      dtEntity::UIntProperty mMoveSpeed;
      dtEntity::FloatProperty mRotateSpeed;
      dtEntity::FloatProperty mRotateKeySpeed;
      dtEntity::BoolProperty mEnabled;
      dtEntityOSG::CameraComponent* mCamera;
      dtEntity::Entity* mEntity;
      osg::Vec3d mPivot;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class MotionModelSystem
      : public dtEntity::DefaultEntitySystem<MotionModelComponent>
   {
      typedef dtEntity::DefaultEntitySystem<MotionModelComponent> BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      MotionModelSystem(dtEntity::EntityManager& em);
      ~MotionModelSystem();

   private:

      void Tick(const dtEntity::Message& m);
      void MoveToPos(const dtEntity::Message& m);
      void MoveToEntity(const dtEntity::Message& m);

      dtEntity::MessageFunctor mTickFunctor;
      dtEntity::MessageFunctor mMoveToPosFunctor;
      dtEntity::MessageFunctor mMoveToEntityFunctor;
   };
}
