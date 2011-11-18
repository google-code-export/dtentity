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

#include <dtEntity/animationcomponent.h>

#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <assert.h>


namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////
   const StringId AnimationComponent::TYPE(SID("Animation"));
   const StringId AnimationComponent::MeshId(SID("Mesh"));
   const StringId AnimationComponent::TimeScaleId(SID("TimeScale"));
   const StringId AnimationComponent::FlipYAxisId(SID("FlipYAxis"));
   const StringId AnimationComponent::AttachmentsId(SID("Attachments"));
   const StringId AnimationComponent::PlayAnimationsId(SID("PlayAnimations"));

   const StringId AnimationComponent::BoneNameId(SID("BoneName"));
   const StringId AnimationComponent::LocalTranslationId(SID("LocalTranslation"));
   const StringId AnimationComponent::LocalRotationId(SID("LocalRotation"));
   const StringId AnimationComponent::VisibleId(SID("Visible"));
   
   ////////////////////////////////////////////////////////////////////////////
   AnimationComponent::AnimationComponent()
      : BaseClass(new osg::Group())
      , mHelper(new dtAnim::AnimationHelper())
   {
      Register(MeshId, &mMeshPathProperty);
      Register(TimeScaleId, &mTimeScale);
      Register(FlipYAxisId, &mFlipYAxis);
      Register(AttachmentsId, &mAttachments);
      Register(PlayAnimationsId, &mPlayAnimations);

      GetNode()->setName("StaticMeshComponent");
      mPlayAnimations.Set(true);
      mTimeScale.Set(1);
   }

   ////////////////////////////////////////////////////////////////////////////
   AnimationComponent::~AnimationComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::PlayAnimation(const std::string& pAnim) 
   {
      mHelper->PlayAnimation(pAnim); 
   }
   
   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::ClearAnimation(const std::string& pAnim, float fadeOutTime)
   {
      mHelper->ClearAnimation(pAnim, fadeOutTime); 
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::ClearAll(float fadeOutTime) 
   {
      mHelper->ClearAll(fadeOutTime); 
   }      

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::Finished()
   {
      SetTimeScale(mTimeScale.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(propname == MeshId)
      {
         SetMesh(prop.StringValue());
      }
      else if(propname == FlipYAxisId)
      {
         osg::Group* grp = static_cast<osg::Group*>(GetNode());
         if(mFlipTransform.valid())
         {
            mFlipTransform->removeChildren(0, mFlipTransform->getNumChildren());
            grp->removeChild(mFlipTransform);
            mFlipTransform = NULL;
         }
         else
         {
            grp->removeChildren(0, grp->getNumChildren());
         }

         if(mFlipYAxis.Get())
         {
            mFlipTransform = new osg::MatrixTransform();
            mFlipTransform->addChild(mHelper->GetNode());
            osg::Matrix mat;
            mat.setRotate(osg::Quat(osg::PI, osg::Vec3(0, 0, 1)));
            mFlipTransform->setMatrix(mat);
            grp->addChild(mFlipTransform);

            for(unsigned int i = 0; i < mAttachMeshes.size(); ++i)
            {
               if(mAttachVisible[i])
               {
                  mFlipTransform->addChild(mAttachMeshes[i]);
               }
            }
         }
         else
         {
            grp->addChild(mHelper->GetNode());
            AttachmentList::iterator i;
            for(unsigned int i = 0; i < mAttachMeshes.size(); ++i)
            {
               if(mAttachVisible[i])
               {
                  grp->addChild(mAttachMeshes[i]);
               }
            }
         }        
      }
      else if(propname == AttachmentsId)
      {
         SetupAttachments();
      }
      else
      {
         BaseClass::OnPropertyChanged(propname, prop);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::SetMesh(const std::string& path)
   {
      ClearAttachments();
      if(mHelper->GetNode() != NULL)
      {
         mHelper->GetNode()->getParent(0)->removeChild(mHelper->GetNode());
      }

      mMeshPathProperty.Set(path);

      std::string abspath = osgDB::findDataFile(path);
      mHelper->LoadModel(abspath);
      if(mFlipTransform)
      {
         mFlipTransform->addChild(mHelper->GetNode());
      }
      else
      {
         static_cast<osg::Group*>(GetNode())->addChild(mHelper->GetNode());
      }
      SetupAttachments();
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::Update(float dt)
   {
      if(!mPlayAnimations.Get())
      {
         return;
      }

      mHelper->Update(dt);

      dtAnim::Cal3DModelWrapper* model = mHelper->GetModelWrapper();
      for(unsigned int i = 0; i < mAttachMeshes.size(); ++i)
      {
         int boneid = mAttachBoneIds[i];
         if(boneid == dtAnim::Cal3DModelWrapper::NULL_BONE ||
               mAttachMeshes[i] == NULL)
         {
            continue;
         }
         // find the total transformation for the bone
         osg::Quat parentRot = model->GetBoneAbsoluteRotation(boneid);
         osg::Quat bodyRotation = mAttachLocalRotations[i] * parentRot;
         osg::Vec3 boneTrans = model->GetBoneAbsoluteTranslation(boneid);

         // transform the local point by the total transformation
         // and store result in the absolute point
         osg::Vec3 bodyTranslation = boneTrans + (parentRot * mAttachLocalTranslations[i]);

         osg::PositionAttitudeTransform* pat = mAttachMeshes[i];
         pat->setPosition(bodyTranslation);
         pat->setAttitude(bodyRotation);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string AnimationComponent::GetMesh() const
   {
      return mMeshPathProperty.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::GetRegisteredAnimations(std::vector<std::string>& toFill)
   {
      std::vector<const dtAnim::Animatable*> v;
      mHelper->GetSequenceMixer().GetRegisteredAnimations(v);
      for(std::vector<const dtAnim::Animatable*>::iterator i = v.begin(); i != v.end(); ++i)
      {
         toFill.push_back((*i)->GetName());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool AnimationComponent::HasAnimation(const std::string& name) const
   {
      return (mHelper->GetSequenceMixer().GetRegisteredAnimation(name) != NULL);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool AnimationComponent::IsAnimationPlaying(const std::string& name) const
   {
      return mHelper->GetSequenceMixer().IsAnimationPlaying(name);
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::ClearAttachments()
   {
      mAttachBoneIds.clear();
      mAttachLocalTranslations.clear();
      mAttachLocalRotations.clear();

      osg::Group* grp = static_cast<osg::Group*>(GetNode());
      if(mFlipTransform.valid())
      {
         grp = mFlipTransform;
      }

      AttachmentList::iterator i;
      for(i = mAttachMeshes.begin(); i != mAttachMeshes.end(); ++i)
      {
         grp->removeChild(*i);         
      }

      mAttachMeshes.clear();
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::SetupAttachments()
   {
      ClearAttachments();

      if(mMeshPathProperty.Get() == "")
      {
         return;
      }

      osg::Group* grp = static_cast<osg::Group*>(GetNode());

      dtEntity::PropertyArray arr = mAttachments.ArrayValue();

      mAttachBoneIds.resize(arr.size());
      mAttachMeshes.resize(arr.size());
      mAttachVisible.resize(arr.size());
      mAttachLocalRotations.resize(arr.size());
      mAttachLocalTranslations.resize(arr.size());

      dtEntity::PropertyArray::const_iterator i;
      for(unsigned int i = 0; i < arr.size(); ++i)
      {

         dtEntity::PropertyGroup props = arr[i]->GroupValue();
         if(props.find(MeshId) == props.end() ||
            props.find(LocalTranslationId) == props.end() ||
            props.find(LocalRotationId) == props.end() ||
            props.find(VisibleId) == props.end() ||
            props.find(BoneNameId) == props.end()
         )
         {
            LOG_ERROR("Error in attachment component: Required data not found");
            continue;
         }

         std::string path = props[MeshId]->StringValue();
         std::string bonename = props[BoneNameId]->StringValue();
         osg::Vec3 offset = props[LocalTranslationId]->Vec3Value();
         osg::Quat offrot = props[LocalRotationId]->QuatValue();
         bool visible = props[VisibleId]->BoolValue();

         mAttachLocalTranslations[i] = offset;
         mAttachLocalRotations[i] = offrot;
         mAttachVisible[i] = visible;
         int boneid = GetHelper()->GetModelWrapper()->GetCoreBoneID(bonename);

         if(boneid == dtAnim::Cal3DModelWrapper::NULL_BONE)
         {
            LOG_ERROR("Error attaching to bone, bone not found: " + bonename);
            continue;
         }
         mAttachBoneIds[i] = boneid;

         osg::Node* node = osgDB::readNodeFile(path);
         if(node == NULL)
         {
            LOG_ERROR("Could not load attachment from path " + path);
            continue;
         }
         else
         {
            osg::PositionAttitudeTransform* mt = new osg::PositionAttitudeTransform();
            mt->addChild(node);

            if(visible)
            {
               if(mFlipTransform.valid())
               {
                  mFlipTransform->addChild(mt);
               }
               else
               {
                  grp->addChild(mt);
               }
            }
            mt->setPosition(offset);
            mt->setAttitude(offrot);
            mAttachMeshes[i] = mt;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationComponent::SetTimeScale(float v)
   {
      mTimeScale.Set(v);
      if(mHelper->GetModelWrapper())
      {
         mHelper->GetModelWrapper()->GetCalModel()->getMixer()->setTimeFactor(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const StringId AnimationSystem::UseHardwareSkinningId(SID("UseHardwareSkinning"));

   ////////////////////////////////////////////////////////////////////////////

   AnimationSystem::AnimationSystem(EntityManager& em)
      : dtEntity::DefaultEntitySystem<AnimationComponent>(em)
   {
      Register(UseHardwareSkinningId, &mUseHardwareSkinning);

      mTickFunctor = MessageFunctor(this, &AnimationSystem::Tick);
      em.RegisterForMessages(TickMessage::TYPE, mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "AnimationSystem::Tick");

      dtAnim::AnimNodeBuilder& nodeBuilder = dtAnim::Cal3DDatabase::GetInstance().GetNodeBuilder();

      mUseHardwareSkinning.Set(true);
      nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateHardware));

   }

   ////////////////////////////////////////////////////////////////////////////
   AnimationSystem::~AnimationSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::TickMessage::TYPE, mTickFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void AnimationSystem::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(propname == UseHardwareSkinningId)
      {
         dtAnim::AnimNodeBuilder& nodeBuilder = dtAnim::Cal3DDatabase::GetInstance().GetNodeBuilder();
         if(prop.BoolValue())
         {
            nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateHardware));
         }
         else
         {
            nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateSoftware));
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////
   void AnimationSystem::Tick(const Message& m)
   {
      const TickMessage& msg = static_cast<const TickMessage&>(m);
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         AnimationComponent* mcomp = i->second;
         mcomp->Update(msg.GetDeltaSimTime());
      }
   }
}
