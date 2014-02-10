#pragma once

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

#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/stringid.h>

namespace dtEntity
{

   
   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicsComponent
      : public Component
   {
   public:

      typedef Component BaseClass;

      static const ComponentType TYPE;
      static const StringId VelocityId;
      static const StringId AngularVelocityId;
      static const StringId AccelerationId;

      DynamicsComponent();
      virtual ~DynamicsComponent();

      virtual ComponentType GetType() const { return TYPE; }

      void OnAddedToEntity(Entity &entity) { mEntity = &entity; }

      Vec3f GetVelocity() const { return mVelocityVal; }
      void SetVelocity(const Vec3f& v);

      const Quat& GetAngularVelocity() const { return mAngularVelocity.GetAsQuat(); }
      void SetAngularVelocity(const Quat& v) { mAngularVelocity.Set(v); }

      const Vec3f& GetAcceleration() const { return mAcceleration.GetAsVec3(); }
      void SetAcceleration(const Vec3f& v) { mAcceleration.Set(v); }

   protected:
      Entity* mEntity;
     
      // path to loaded script file
      DynamicVec3Property mVelocity;
      Vec3f mVelocityVal;
      QuatProperty mAngularVelocity;
      Vec3Property mAcceleration;
      bool mIsMoving;

   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT DynamicsSystem
      : public DefaultEntitySystem<DynamicsComponent>
   {
   public:

      static const ComponentType TYPE;

      DynamicsSystem(EntityManager& em);
      ~DynamicsSystem();

   };
}
