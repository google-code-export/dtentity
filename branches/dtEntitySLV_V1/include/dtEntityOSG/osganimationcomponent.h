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
#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace dtEntityOSG
{

   ///////////////////////////////////////////////////////////////////////////
   class DTENTITY_OSG_EXPORT OSGAnimationComponent : public dtEntity::Component
   {

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId EnabledId;
      
      OSGAnimationComponent();
      virtual ~OSGAnimationComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual void OnAddedToEntity(dtEntity::Entity& entity);

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
      dtEntity::Entity* mEntity;
      dtEntity::DynamicBoolProperty mEnabled;
      bool mEnabledVal;      
 
   };

  
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT OSGAnimationSystem
      : public dtEntity::DefaultEntitySystem<OSGAnimationComponent>
      , public dtEntity::ScriptAccessor
   {
   public:

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId VertexShaderId;
      static const dtEntity::StringId FragmentShaderId;
      static const dtEntity::StringId EnabledId;

      OSGAnimationSystem(dtEntity::EntityManager& em);
      ~OSGAnimationSystem();

      void OnMeshChanged(const dtEntity::Message&);

      void SetVertexShader(const std::string& v) { mVertexShader.Set(v); }
      std::string GetVertexShader() const { return mVertexShader.Get(); }

      void SetFragmentShader(const std::string& v) { mFragmentShader.Set(v); }
      std::string GetFragmentShader() const { return mFragmentShader.Get(); }

      void SetEnabled(bool);
      bool GetEnabled() const;

   private:

      bool GetAnimationList(dtEntity::EntityId id, const osgAnimation::AnimationList*& list,
                            osgAnimation::BasicAnimationManager*& manager);
      dtEntity::Property* ScriptPlayAnimation(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptStopAnimation(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptGetAnimations(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptGetAnimationLength(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptGetAnimationPlayMode(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetAnimationPlayMode(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptAddAttachment(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptRemoveAttachments(const dtEntity::PropertyArgs& args);

      dtEntity::MessageFunctor mMeshChangedFunctor;
      dtEntity::StringProperty mVertexShader;
      dtEntity::StringProperty mFragmentShader;
      bool mEnabledVal;
      dtEntity::DynamicBoolProperty mEnabled;
   };
}
