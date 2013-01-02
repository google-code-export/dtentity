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

#include <dtEntityOSG/manipulatorcomponent.h>

#include <dtEntityOSG/manipulators.h>
#include <osg/MatrixTransform>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityOSG/transformcomponent.h>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   class DraggerCallback : public osgManipulator::DraggerCallback
   {
   public:

      DraggerCallback(dtEntityOSG::TransformComponent* tc)
         : mTransform(tc)
      {
      }

      void SetOffsetFromStart(const osg::Vec3d& v)
      {
         osg::Vec3d diff = v - _offsetFromStart;
         _offsetFromStart = v;
         mTransform->SetMatrix(mTransform->GetMatrix() * osg::Matrix::translate(diff));
      }

      virtual bool receive(const osgManipulator::MotionCommand& command)
      {

         switch (command.getStage())
         {
             case osgManipulator::MotionCommand::START:
             {
                  _offsetFromStart.set(0,0,0);

                 // Save the current matrix
                 _startMotionMatrix = mTransform->GetMatrix();

                 osg::NodePath nodePathToRoot;
                 osgManipulator::computeNodePathToRoot(*mTransform->GetNode(), nodePathToRoot);
                 _localToWorld = osg::computeLocalToWorld(nodePathToRoot);
                 _worldToLocal = osg::Matrix::inverse(_localToWorld);
                 return true;
             }
             case osgManipulator::MotionCommand::MOVE:
             {
                 osg::Matrix offset;
                 offset.makeTranslate(_offsetFromStart);

                 osg::Matrix localMotionMatrix = _localToWorld * command.getWorldToLocal()
                                              * command.getMotionMatrix()
                                              * command.getLocalToWorld() * _worldToLocal;
                 mTransform->SetMatrix(localMotionMatrix * _startMotionMatrix * offset);


                 return true;
             }
             case osgManipulator::MotionCommand::FINISH:
             {
                 return true;
             }
             case osgManipulator::MotionCommand::NONE:
             default:
                 return false;
         }
      }

   private:
      dtEntityOSG::TransformComponent* mTransform;

      osg::Matrix _startMotionMatrix;
      osg::Matrix _localToWorld;
      osg::Matrix _worldToLocal;
      osg::Vec3 _offsetFromStart;

   };

   ////////////////////////////////////////////////////////////////////////////
   class DraggerContainer : public osg::Group
   {
   public:
       DraggerContainer(osgManipulator::Dragger* dragger, dtEntityOSG::TransformComponent* transform)
          : _dragger(dragger)
          , mTransform(transform)
          , _draggerSize(140.0f)
          , _keepSizeConstant(true)
          , _useLocalCoords(false)
          , _pivotAtBottom(false)

       {
          addChild(dragger);


          setInitialBound(osg::BoundingSphere(osg::Vec3(0,0,0), 100));
          osg::BoundingSphere bs = mTransform->GetNode()->getBound();
          _dragger->setMatrix( osg::Matrix::scale(1, 1, 1) * osg::Matrix::translate(bs.center()) );
       }

       void setDraggerSize( float size ) { _draggerSize = size; }
       float getDraggerSize() const { return _draggerSize; }

       void setKeepSizeConstant( bool b ) { _keepSizeConstant = b; }
       bool getKeepSizeConstant() const { return _keepSizeConstant; }

       void setPivotAtBottom( bool b ) { _pivotAtBottom = b; }
       bool getPivotAtBottom() const { return _pivotAtBottom; }

       void SetUseLocalCoords(bool v)
       {
          _useLocalCoords = v;
       }

       void traverse( osg::NodeVisitor& nv )
       {
           if ( _dragger.valid() )
           {
               if (nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
               {
                   osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(&nv);

                   osg::BoundingSphere bs = mTransform->GetNode()->getBound();
                   if(!bs.valid())
                   {
                     bs.set(mTransform->GetTranslation(), 1);
                   }
                   osg::Vec3d pivot = bs.center();
                   if(_pivotAtBottom)
                   {
                     pivot[2] = mTransform->GetTranslation()[2];
                   }

                   if(_keepSizeConstant)
                   {
                      float pixelSize = cv->pixelSize(_dragger->getBound().center(), 0.48f);
                      if ( pixelSize !=_draggerSize )
                      {
                          float pixelScale = pixelSize>0.0f ? _draggerSize/pixelSize : 1.0f;
                          osg::Vec3d scaleFactor(pixelScale, pixelScale, pixelScale);



                         if(_useLocalCoords)
                         {
                            _dragger->setMatrix( osg::Matrix::scale(scaleFactor) *
                                                 osg::Matrix::rotate(mTransform->GetRotation()) *
                                                 osg::Matrix::translate(pivot) );
                         }
                         else
                         {
                             _dragger->setMatrix( osg::Matrix::scale(scaleFactor) * osg::Matrix::translate(pivot) );
                         }
                      }
                   }
                   else
                   {
                      osg::Vec3d scaleFactor(bs.radius(), bs.radius(), bs.radius());
                      if(_useLocalCoords)
                      {

                         _dragger->setMatrix( osg::Matrix::scale(scaleFactor) *
                                              osg::Matrix::rotate(mTransform->GetRotation()) *
                                              osg::Matrix::translate(pivot) );
                      }
                      else
                      {
                          _dragger->setMatrix(osg::Matrix::scale(scaleFactor) *osg::Matrix::translate(pivot) );
                      }
                   }
               }
           }
           osg::Group::traverse(nv);
       }

   protected:
       osg::ref_ptr<osgManipulator::Dragger> _dragger;
       dtEntityOSG::TransformComponent* mTransform;
       float _draggerSize;
       bool _keepSizeConstant;
       bool _useLocalCoords;
       bool _pivotAtBottom;
   };

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ManipulatorComponent::TYPE(dtEntity::SID("Manipulator"));
   const dtEntity::StringId ManipulatorComponent::LayerId(dtEntity::SID("Layer"));
   const dtEntity::StringId ManipulatorComponent::DraggerTypeId(dtEntity::SID("DraggerType"));
   const dtEntity::StringId ManipulatorComponent::OffsetFromStartId(dtEntity::SID("OffsetFromStart"));
   const dtEntity::StringId ManipulatorComponent::UseLocalCoordsId(dtEntity::SID("UseLocalCoords"));
   const dtEntity::StringId ManipulatorComponent::KeepSizeConstantId(dtEntity::SID("KeepSizeConstant"));
   const dtEntity::StringId ManipulatorComponent::PivotAtBottomId(dtEntity::SID("PivotAtBottom"));

   const dtEntity::StringId ManipulatorComponent::TabPlaneDraggerId(dtEntity::SID("TabPlaneDragger"));
   const dtEntity::StringId ManipulatorComponent::TabPlaneTrackballDraggerId(dtEntity::SID("TabPlaneTrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::TabBoxTrackballDraggerId(dtEntity::SID("TabBoxTrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::TrackballDraggerId(dtEntity::SID("TrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::Translate1DDraggerId(dtEntity::SID("Translate1DDragger"));
   const dtEntity::StringId ManipulatorComponent::Translate2DDraggerId(dtEntity::SID("Translate2DDragger"));
   const dtEntity::StringId ManipulatorComponent::TranslateAxisDraggerId(dtEntity::SID("TranslateAxisDragger"));
   const dtEntity::StringId ManipulatorComponent::TabBoxDraggerId(dtEntity::SID("TabBoxDragger"));
   const dtEntity::StringId ManipulatorComponent::TerrainTranslateDraggerId(dtEntity::SID("TerrainTranslateDragger"));
   const dtEntity::StringId ManipulatorComponent::ScaleDraggerId(dtEntity::SID("ScaleDragger"));

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorComponent::ManipulatorComponent()
      : BaseClass()
      , mLayerProperty(
         dtEntity::DynamicStringIdProperty::SetValueCB(this, &ManipulatorComponent::SetLayer),
         dtEntity::DynamicStringIdProperty::GetValueCB(this, &ManipulatorComponent::GetLayer)
      )
      , mLayerVal(dtEntity::SIDHash("root"))
      , mAttachPoint(dtEntity::StringId())
      , mDraggerType(
         dtEntity::DynamicStringIdProperty::SetValueCB(this, &ManipulatorComponent::SetDraggerType),
         dtEntity::DynamicStringIdProperty::GetValueCB(this, &ManipulatorComponent::GetDraggerType)
      )
      , mDraggerTypeVal(dtEntity::StringId())
      , mOffsetFromStart(
         dtEntity::DynamicVec3dProperty::SetValueCB(this, &ManipulatorComponent::SetOffsetFromStart),
         dtEntity::DynamicVec3dProperty::GetValueCB(this, &ManipulatorComponent::GetOffsetFromStart)
      )
      , mKeepSizeConstant(
         dtEntity::DynamicBoolProperty::SetValueCB(this, &ManipulatorComponent::SetKeepSizeConstant),
         dtEntity::DynamicBoolProperty::GetValueCB(this, &ManipulatorComponent::GetKeepSizeConstant)
      )
      , mKeepSizeConstantVal(true)
      , mUseLocalCoords(
         dtEntity::DynamicBoolProperty::SetValueCB(this, &ManipulatorComponent::SetUseLocalCoords),
         dtEntity::DynamicBoolProperty::GetValueCB(this, &ManipulatorComponent::GetUseLocalCoords)
      )
      , mUseLocalCoordsVal(false)
      , mPivotAtBottom(
         dtEntity::DynamicBoolProperty::SetValueCB(this, &ManipulatorComponent::SetPivotAtBottom),
         dtEntity::DynamicBoolProperty::GetValueCB(this, &ManipulatorComponent::GetPivotAtBottom)
      )
      , mPivotAtBottomVal(false)
      , mDraggerContainer(NULL)
   {
      Register(LayerId, &mLayerProperty);
      Register(DraggerTypeId, &mDraggerType);
      Register(OffsetFromStartId, &mOffsetFromStart);
      Register(UseLocalCoordsId, &mUseLocalCoords);
      Register(KeepSizeConstantId, &mKeepSizeConstant);
      Register(PivotAtBottomId, &mPivotAtBottom);

      SetDraggerType(TabBoxDraggerId);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   ManipulatorComponent::~ManipulatorComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      BaseClass::OnAddedToEntity(e);
      ManipulatorSystem* ms;
      e.GetEntityManager().GetEntitySystem(TYPE, ms);
      SetUseLocalCoords(ms->GetUseLocalCoords());
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
      RemoveFromParent();
      BaseClass::OnRemovedFromEntity(e);      
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetUseLocalCoords(bool v)
   {
      mUseLocalCoordsVal = v;

      if(mDraggerContainer)
      {
         static_cast<DraggerContainer*>(mDraggerContainer.get())->SetUseLocalCoords(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetKeepSizeConstant(bool v)
   {
      mKeepSizeConstantVal = v;

      if(mDraggerContainer)
      {
         static_cast<DraggerContainer*>(mDraggerContainer.get())->setKeepSizeConstant(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetPivotAtBottom(bool v)
   {
      mPivotAtBottomVal = v;

      if(mDraggerContainer)
      {
         static_cast<DraggerContainer*>(mDraggerContainer.get())->setPivotAtBottom(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::RemoveFromParent()
   {
      if(mAttachPoint == dtEntity::StringId() || !mDraggerContainer.valid())
      {
         return;
      }
      assert(GetNodeEntity() != NULL);

      dtEntityOSG::LayerAttachPointSystem* layerattsystem;
      GetNodeEntity()->GetEntityManager().GetEntitySystem(dtEntityOSG::LayerAttachPointComponent::TYPE, layerattsystem);

      dtEntityOSG::LayerAttachPointComponent* current;
      if(layerattsystem->GetByName(mAttachPoint, current))
      {
         current->GetGroup()->removeChild(mDraggerContainer);
         mDraggerContainer = NULL;
      }

      mAttachPoint = dtEntity::StringId();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::AddToLayer()
   {

      assert(mAttachPoint == dtEntity::StringId());

      dtEntityOSG::LayerAttachPointSystem* layerattsystem;
      GetNodeEntity()->GetEntityManager().GetEntitySystem(dtEntityOSG::LayerAttachPointComponent::TYPE, layerattsystem);

      dtEntityOSG::LayerAttachPointComponent* next;
      if(layerattsystem->GetByName(mLayerProperty.Get(), next))
      {
         dtEntityOSG::TransformComponent* tcomp;
         if(GetNodeEntity()->GetEntityManager().GetComponent(GetNodeEntity()->GetId(), tcomp, true))
         {

            assert(!mDraggerContainer.valid());
            assert(dynamic_cast<osgManipulator::Dragger*>(GetNode()) != NULL);
            DraggerContainer* dc = new DraggerContainer(GetDragger(), tcomp);
            mDraggerContainer = dc;

            dc->SetUseLocalCoords(mUseLocalCoordsVal);
            dc->setKeepSizeConstant(mKeepSizeConstantVal);
            dc->setPivotAtBottom(mPivotAtBottomVal);

            next->GetAttachmentGroup()->addChild(mDraggerContainer);
            mAttachPoint = mLayerProperty.Get();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::StringId ManipulatorComponent::GetLayer() const
   {
      return mLayerVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetLayer(dtEntity::StringId layername)
   {
      if(mAttachPoint == layername)
      {
         return;
      }

      mLayerVal = layername;
      if(GetNodeEntity() != NULL)
      {
         RemoveFromParent();
         AddToLayer();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetDraggerType(dtEntity::StringId draggerType)
   {
      mDraggerTypeVal = draggerType;
      RemoveFromParent();
      osgManipulator::Dragger* dragger = GetDragger();
      if(dragger != NULL && mDraggerCallback != NULL)
      {
         dragger->removeDraggerCallback(mDraggerCallback);
      }

      if(draggerType == TabBoxDraggerId)
      {
         SetNode(new osgManipulator::TabBoxDragger());
      }
      else if(draggerType == TabPlaneDraggerId)
      {
         SetNode(new osgManipulator::TabPlaneDragger());
      }
      else if(draggerType == TabPlaneTrackballDraggerId)
      {
         SetNode(new osgManipulator::TabPlaneTrackballDragger());
      }
      else if(draggerType == TabBoxTrackballDraggerId)
      {
         SetNode(new osgManipulator::TabBoxTrackballDragger());
      }
      else if(draggerType == TrackballDraggerId)
      {
         SetNode(new TrackballDragger());
      }
      else if(draggerType == Translate1DDraggerId)
      {
         SetNode(new osgManipulator::Translate1DDragger());
      }
      else if(draggerType == Translate2DDraggerId)
      {
         SetNode(new osgManipulator::Translate2DDragger());
      }
      else if(draggerType == TranslateAxisDraggerId)
      {
         SetNode(new osgManipulator::TranslateAxisDragger());
      }
      else if(draggerType == TerrainTranslateDraggerId)
      {
         SetNode(new TerrainTranslateDragger());
      }
      else if(draggerType == ScaleDraggerId)
      {
         SetNode(new ScaleDragger());
      }
      else
      {
         LOG_ERROR("Unknown dragger type: " + dtEntity::GetStringFromSID(draggerType));
         return;
      }


      dragger = GetDragger();
      osg::StateSet* ss = dragger->getOrCreateStateSet();
      ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      ss->setRenderBinDetails(80, "RenderBin");
      ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

      dragger->setupDefaultGeometry();
      dragger->setHandleEvents(true);

      if(GetNodeEntity() != NULL)
      {
         AddToLayer();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetOffsetFromStart(const osg::Vec3d& v)
   {
      mOffsetFromStartVal = v;

      if(mDraggerCallback)
      {
         static_cast<DraggerCallback*>(mDraggerCallback.get())->SetOffsetFromStart(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   osgManipulator::Dragger* ManipulatorComponent::GetDragger() const
   {
      return static_cast<osgManipulator::Dragger*>(GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::Finished()
   {

      BaseClass::Finished();

      osgManipulator::Dragger* dragger = GetDragger();
      if(dragger != NULL && mDraggerCallback != NULL)
      {
         dragger->removeDraggerCallback(mDraggerCallback);
      }

      dtEntityOSG::TransformComponent* tcomp;
      if(GetNodeEntity()->GetEntityManager().GetComponent(GetNodeEntity()->GetId(), tcomp, true))
      {
         mDraggerCallback = new DraggerCallback(tcomp);
         dragger->addDraggerCallback(mDraggerCallback);
      }

      SetLayer(mLayerProperty.Get());
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ManipulatorSystem::TYPE(dtEntity::SID("Manipulator"));
   const dtEntity::StringId ManipulatorSystem::UseLocalCoordsId(dtEntity::SID("UseLocalCoords"));
   const dtEntity::StringId ManipulatorSystem::UseGroundClampingId(dtEntity::SID("UseGroundClamping"));

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorSystem::ManipulatorSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mUseLocalCoords(
         dtEntity::DynamicBoolProperty::SetValueCB(this, &ManipulatorSystem::SetUseLocalCoords),
         dtEntity::DynamicBoolProperty::GetValueCB(this, &ManipulatorSystem::GetUseLocalCoords)
      )
      , mUseLocalCoordsVal(false)
   {
      Register(UseLocalCoordsId, &mUseLocalCoords);
      Register(UseGroundClampingId, &mUseGroundClamping);

   }

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorSystem::~ManipulatorSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorSystem::SetUseLocalCoords(bool v)
   {
      mUseLocalCoordsVal = v;
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->SetUseLocalCoords(v);
      }
   }
}
