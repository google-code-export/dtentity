/* -*-c++-*-
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

#ifndef DELTA_ENTITY_ANIMCOMPONENT
#define DELTA_ENTITY_ANIMCOMPONENT

#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

namespace dtAnim
{
   class Animatable;
}

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////

   class AnimationComponentImpl;

   /**
    * holds an animation helper
    */
   class DT_ENTITY_EXPORT AnimationComponent
      : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const ComponentType TYPE;
      static const StringId MeshId;
      static const StringId TimeScaleId;
      static const StringId FlipYAxisId;
      static const StringId AttachmentsId;
      static const StringId PlayAnimationsId;

      static const StringId BoneNameId;
      static const StringId LocalTranslationId;
      static const StringId LocalRotationId;
      static const StringId VisibleId;
    
      AnimationComponent();
      virtual ~AnimationComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void Finished();
      virtual void OnPropertyChanged(StringId propname, Property& prop);

      std::string GetMesh() const;
      void SetMesh(const std::string&);
      void Update(float dt);

      void PlayAnimation(const std::string& pAnim);
      void ClearAnimation(const std::string& pAnim, float fadeOutTime);
      void ClearAll(float fadeOutTime);
      bool HasAnimation(const std::string& name) const;
      bool IsAnimationPlaying(const std::string& name) const;
      
      void GetRegisteredAnimations(std::vector<std::string>& toFill);

      void SetTimeScale(float v);
      float GetTimeScale() const { return mTimeScale.Get(); }

      void SetFlipYAxis(bool v) { mFlipYAxis.Set(v); }
      bool GetFlipYAxis() const { return mFlipYAxis.Get(); }

      void ClearAttachments();

      dtAnim::Animatable* GetActiveAnimation(const std::string& animname) const;

   private:
     
      void SetupAttachments();

      AnimationComponentImpl* mImpl;



      // path to loaded script file
      StringProperty mMeshPathProperty;
      FloatProperty mTimeScale;
      BoolProperty mFlipYAxis;
      BoolProperty mPlayAnimations;
      ArrayProperty mAttachments;

      typedef std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > AttachmentList;
      AttachmentList mAttachMeshes;
      std::vector<unsigned int> mAttachBoneIds;
      std::vector<osg::Vec3> mAttachLocalTranslations;
      std::vector<osg::Quat> mAttachLocalRotations;
      std::vector<bool> mAttachVisible;
   };


   //////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT AnimationSystem
      : public DefaultEntitySystem<AnimationComponent>
   {
   public:
      static const StringId UseHardwareSkinningId;

      AnimationSystem(EntityManager& em);
      ~AnimationSystem();
      virtual void OnPropertyChanged(StringId propname, Property& prop);

      void Tick(const Message& m);

      virtual bool StorePropertiesToScene() const { return true; }

   private:
      dtEntity::MessageFunctor mTickFunctor;
      BoolProperty mUseHardwareSkinning;
   };

}

#endif // DELTA_ENTITY_ANIMCOMPONENT
