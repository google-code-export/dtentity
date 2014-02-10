/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
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
