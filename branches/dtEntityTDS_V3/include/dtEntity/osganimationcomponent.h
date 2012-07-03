#pragma once

/* -*-c++-*-
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

#include <osg/ref_ptr>
#include <osgAnimation/BasicAnimationManager>
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT OSGAnimationComponent : public Component
   {

   public:
      
      static const ComponentType TYPE;
      static const StringId EnabledId;
      
      OSGAnimationComponent();
      virtual ~OSGAnimationComponent();

      virtual ComponentType GetType() const { return TYPE; }
      virtual void OnAddedToEntity(Entity& entity);

      osgAnimation::BasicAnimationManager* GetAnimationManager() const
      {
         return mAnimationManager;
      }

      void SetupMesh(osg::Node*);

      void SetEnabled(bool);
      bool GetEnabled() const;

      bool AddAttachment(const std::string& boneName, osg::Node* node, const osg::Matrix& m);
      bool RemoveAtachment(const std::string& boneName, osg::Node* node);
      void RemoveAtachments(const std::string& boneName);

   private:

      osg::ref_ptr<osgAnimation::BasicAnimationManager> mAnimationManager;
      Entity* mEntity;
      DynamicBoolProperty mEnabled;
      bool mEnabledVal;      
 
   };

  
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT OSGAnimationSystem
      : public DefaultEntitySystem<OSGAnimationComponent>
      , public ScriptAccessor
   {
   public:

      static const ComponentType TYPE;
      static const StringId VertexShaderId;
      static const StringId FragmentShaderId;
      static const StringId EnabledId;

      OSGAnimationSystem(EntityManager& em);
      ~OSGAnimationSystem();

      void OnMeshChanged(const Message&);

      void SetVertexShader(const std::string& v) { mVertexShader.Set(v); }
      std::string GetVertexShader() const { return mVertexShader.Get(); }

      void SetFragmentShader(const std::string& v) { mFragmentShader.Set(v); }
      std::string GetFragmentShader() const { return mFragmentShader.Get(); }

      void SetEnabled(bool);
      bool GetEnabled() const;

   private:

      bool GetAnimationList(EntityId id, const osgAnimation::AnimationList*& list,
                            osgAnimation::BasicAnimationManager*& manager);
      Property* ScriptPlayAnimation(const PropertyArgs& args);
      Property* ScriptStopAnimation(const PropertyArgs& args);
      Property* ScriptGetAnimations(const PropertyArgs& args);
      Property* ScriptGetAnimationLength(const PropertyArgs& args);
      Property* ScriptGetAnimationPlayMode(const PropertyArgs& args);
      Property* ScriptSetAnimationPlayMode(const PropertyArgs& args);
      Property* ScriptAddAttachment(const PropertyArgs& args);
      Property* ScriptRemoveAttachments(const PropertyArgs& args);

      MessageFunctor mMeshChangedFunctor;
      StringProperty mVertexShader;
      StringProperty mFragmentShader;
      bool mEnabledVal;
      dtEntity::DynamicBoolProperty mEnabled;
   };
}
