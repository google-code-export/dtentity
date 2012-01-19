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

#include <dtEntitySimulation/manipulatorcomponent.h>

#include <dtEntitySimulation/manipulators.h>
#include <osg/MatrixTransform>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/nodemasks.h>
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

namespace dtEntitySimulation
{

   ////////////////////////////////////////////////////////////////////////////
   class DraggerCallback : public osgManipulator::DraggerCallback
   {
   public:

      DraggerCallback(dtEntity::TransformComponent* tc)
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
      dtEntity::TransformComponent* mTransform;

      osg::Matrix _startMotionMatrix;
      osg::Matrix _localToWorld;
      osg::Matrix _worldToLocal;
      osg::Vec3 _offsetFromStart;

   };

   ////////////////////////////////////////////////////////////////////////////
   class DraggerContainer : public osg::Group
   {
   public:
       DraggerContainer(osgManipulator::Dragger* dragger, dtEntity::TransformComponent* transform)
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
       dtEntity::TransformComponent* mTransform;
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
      , mAttachPoint(dtEntity::StringId())
      , mDraggerContainer(NULL)
   {
      Register(LayerId, &mLayerProperty);
      Register(DraggerTypeId, &mDraggerType);
      Register(OffsetFromStartId, &mOffsetFromStart);
      Register(UseLocalCoordsId, &mUseLocalCoords);
      Register(KeepSizeConstantId, &mKeepSizeConstant);
      Register(PivotAtBottomId, &mPivotAtBottom);
      mLayerProperty.Set(dtEntity::SIDHash("root"));

      mKeepSizeConstant.Set(true);
      mUseLocalCoords.Set(false);
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
      mEntity = &e;
      ManipulatorSystem* ms;
      mEntity->GetEntityManager().GetEntitySystem(TYPE, ms);
      SetUseLocalCoords(ms->GetUseLocalCoords());
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
      BaseClass:OnRemovedFromEntity(e);
      RemoveFromParent();
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == LayerId)
      {
         SetLayer(prop.StringIdValue());
      }
      else if(propname == DraggerTypeId)
      {
         SetDraggerType(prop.StringIdValue());
      }
      else if(propname == UseLocalCoordsId)
      {
         SetUseLocalCoords(prop.BoolValue());
      }
      else if(propname == KeepSizeConstantId)
      {
         SetKeepSizeConstant(prop.BoolValue());
      }
      else if(propname == PivotAtBottomId)
      {
         SetPivotAtBottom(prop.BoolValue());
      }
      else if(propname == OffsetFromStartId)
      {
         if(mDraggerCallback)
         {
            static_cast<DraggerCallback*>(mDraggerCallback.get())->SetOffsetFromStart(prop.Vec3dValue());
         }
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetUseLocalCoords(bool v)
   {
      mUseLocalCoords.Set(v);

      if(mDraggerContainer)
      {
         static_cast<DraggerContainer*>(mDraggerContainer.get())->SetUseLocalCoords(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetKeepSizeConstant(bool v)
   {
      mKeepSizeConstant.Set(v);

      if(mDraggerContainer)
      {
         static_cast<DraggerContainer*>(mDraggerContainer.get())->setKeepSizeConstant(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetPivotAtBottom(bool v)
   {
      mPivotAtBottom.Set(v);

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
      assert(mEntity != NULL);

      dtEntity::LayerAttachPointSystem* layerattsystem;
      mEntity->GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layerattsystem);

      dtEntity::LayerAttachPointComponent* current;
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

      dtEntity::LayerAttachPointSystem* layerattsystem;
      mEntity->GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layerattsystem);

      dtEntity::LayerAttachPointComponent* next;
      if(layerattsystem->GetByName(mLayerProperty.Get(), next))
      {
         dtEntity::TransformComponent* tcomp;
         mEntity->GetEntityManager().GetComponent(mEntity->GetId(), tcomp, true);

         assert(!mDraggerContainer.valid());
         assert(dynamic_cast<osgManipulator::Dragger*>(GetNode()) != NULL);
         DraggerContainer* dc = new DraggerContainer(GetDragger(), tcomp);
         mDraggerContainer = dc;

         dc->SetUseLocalCoords(mUseLocalCoords.Get());
         dc->setKeepSizeConstant(mKeepSizeConstant.Get());
         dc->setPivotAtBottom(mPivotAtBottom.Get());

         next->GetAttachmentGroup()->addChild(mDraggerContainer);
         mAttachPoint = mLayerProperty.Get();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetLayer(dtEntity::StringId layername)
   {
      if(mAttachPoint == layername)
      {
         return;
      }

      mLayerProperty.Set(layername);
      if(mEntity != NULL)
      {
         RemoveFromParent();
         AddToLayer();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::SetDraggerType(dtEntity::StringId draggerType)
   {
      mDraggerType.Set(draggerType);
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

      if(mEntity != NULL)
      {
         AddToLayer();
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

      osgManipulator::Dragger* dragger = GetDragger();
      if(dragger != NULL && mDraggerCallback != NULL)
      {
         dragger->removeDraggerCallback(mDraggerCallback);
      }

      dtEntity::TransformComponent* tcomp;
      if(mEntity->GetEntityManager().GetComponent(mEntity->GetId(), tcomp, true))
      {
         mDraggerCallback = new DraggerCallback(tcomp);
         dragger->addDraggerCallback(mDraggerCallback);
      }

      SetLayer(mLayerProperty.Get());
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId ManipulatorSystem::UseLocalCoordsId(dtEntity::SID("UseLocalCoords"));
   const dtEntity::StringId ManipulatorSystem::UseGroundClampingId(dtEntity::SID("UseGroundClamping"));

   ManipulatorSystem::ManipulatorSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
      Register(UseLocalCoordsId, &mUseLocalCoords);
      Register(UseGroundClampingId, &mUseGroundClamping);

   }

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorSystem::~ManipulatorSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorSystem::OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop)
   {
      if(propname == UseLocalCoordsId)
      {
         SetUseLocalCoords(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorSystem::SetUseLocalCoords(bool v)
   {
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         i->second->SetUseLocalCoords(v);
      }
   }
}
