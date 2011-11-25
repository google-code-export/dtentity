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
#include <osg/MatrixTransform>
#include <dtEntity/layerattachpointcomponent.h>

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

   class MyTranslateAxisDragger : public osgManipulator::TranslateAxisDragger
   {
   public:
      virtual void setupDefaultGeometry()
      {
         // Create a line.
         osg::Geode* lineGeode = new osg::Geode;
         {
             osg::Geometry* geometry = new osg::Geometry();

             osg::Vec3Array* vertices = new osg::Vec3Array(2);
             (*vertices)[0] = osg::Vec3(0.0f,0.0f,0.0f);
             (*vertices)[1] = osg::Vec3(0.0f,0.0f,1.0f);

             geometry->setVertexArray(vertices);
             geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

             lineGeode->addDrawable(geometry);
         }

         // Turn of lighting for line and set line width.
         {
             osg::LineWidth* linewidth = new osg::LineWidth();
             linewidth->setWidth(5.0f);
             lineGeode->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
             lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
         }

         // Add line to all the individual 1D draggers.
         _xDragger->addChild(lineGeode);
         _yDragger->addChild(lineGeode);
         _zDragger->addChild(lineGeode);

          osg::Geode* geodex = new osg::Geode();
          osg::Geode* geodey = new osg::Geode();
          osg::Geode* geodez = new osg::Geode();

          osg::ShapeDrawable* conex = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));
          osg::ShapeDrawable* coney = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));
          osg::ShapeDrawable* conez = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));

          conex->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
          coney->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
          conez->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

          conex->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
          coney->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
          conez->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));

          geodex->addDrawable(conex);
          geodey->addDrawable(coney);
          geodez->addDrawable(conez);

         // Create an invisible cylinder for picking the line.
         {
             osg::Cylinder* cylinder = new osg::Cylinder (osg::Vec3(0.0f,0.0f,0.5f), 0.1f, 1.0f);
             osg::Drawable* geometry = new osg::ShapeDrawable(cylinder);
             osgManipulator::setDrawableToAlwaysCull(*geometry);
             geodex->addDrawable(geometry);
             geodey->addDrawable(geometry);
             geodez->addDrawable(geometry);
         }

         // Add geode to all 1D draggers.
         _xDragger->addChild(geodex);
         _yDragger->addChild(geodey);
         _zDragger->addChild(geodez);

         // Rotate X-axis dragger appropriately.
         {
             osg::Quat rotation; rotation.makeRotate(osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(1.0f, 0.0f, 0.0f));
             _xDragger->setMatrix(osg::Matrix(rotation));
         }

         // Rotate Y-axis dragger appropriately.
         {
             osg::Quat rotation; rotation.makeRotate(osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
             _yDragger->setMatrix(osg::Matrix(rotation));
         }

         // Send different colors for each dragger.
         _xDragger->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
         _yDragger->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
         _zDragger->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   class DraggerCallback : public osgManipulator::DraggerCallback
   {
   public:

      DraggerCallback(dtEntity::TransformComponent* tc)
         : mTransform(tc)
      {
      }


      virtual bool receive(const osgManipulator::MotionCommand& command)
      {

         switch (command.getStage())
         {
             case osgManipulator::MotionCommand::START:
             {
                 // Save the current matrix
                 _startMotionMatrix = mTransform->GetMatrix();

                 // Get the LocalToWorld and WorldToLocal matrix for this node.
                 osg::NodePath nodePathToRoot;
                 osgManipulator::computeNodePathToRoot(*mTransform->GetNode(), nodePathToRoot);
                 _localToWorld = osg::computeLocalToWorld(nodePathToRoot);
                 _worldToLocal = osg::Matrix::inverse(_localToWorld);

                 return true;
             }
             case osgManipulator::MotionCommand::MOVE:
             {
                 // Transform the command's motion matrix into local motion matrix.
                 osg::Matrix localMotionMatrix = _localToWorld * command.getWorldToLocal()
                                                 * command.getMotionMatrix()
                                                 * command.getLocalToWorld() * _worldToLocal;

                 // Transform by the localMotionMatrix
                 mTransform->SetMatrix(localMotionMatrix * _startMotionMatrix);

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
   };

   ////////////////////////////////////////////////////////////////////////////
   class DraggerContainer : public osg::Group
   {
   public:
       DraggerContainer(osgManipulator::Dragger* dragger, dtEntity::TransformComponent* transform)
          : _dragger(dragger)
          , mTransform(transform)
          , _draggerSize(140.0f)
          , _active(true)
       {
          addChild(dragger);
          _dragger->setMatrix( osg::Matrix::scale(1, 1, 1) * osg::Matrix::translate(transform->GetTranslation()) );
       }

       void setDraggerSize( float size ) { _draggerSize = size; }
       float getDraggerSize() const { return _draggerSize; }

       void setActive( bool b ) { _active = b; }
       bool getActive() const { return _active; }


       void traverse( osg::NodeVisitor& nv )
       {
           if ( _dragger.valid() )
           {
               if ( _active && nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
               {
                   osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(&nv);

                   float pixelSize = cv->pixelSize(_dragger->getBound().center(), 0.48f);
                   if ( pixelSize!=_draggerSize )
                   {
                       float pixelScale = pixelSize>0.0f ? _draggerSize/pixelSize : 1.0f;
                       osg::Vec3d scaleFactor(pixelScale, pixelScale, pixelScale);

                       osg::BoundingSphere bs = mTransform->GetNode()->getBound();
                       _dragger->setMatrix( osg::Matrix::scale(scaleFactor) * osg::Matrix::translate(bs.center()) );
                   }
               }
           }
           osg::Group::traverse(nv);
       }

   protected:
       osg::ref_ptr<osgManipulator::Dragger> _dragger;
       float _draggerSize;
       bool _active;
       dtEntity::TransformComponent* mTransform;
   };

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ManipulatorComponent::TYPE(dtEntity::SID("Manipulator"));
   const dtEntity::StringId ManipulatorComponent::LayerId(dtEntity::SID("Layer"));
   const dtEntity::StringId ManipulatorComponent::DraggerTypeId(dtEntity::SID("DraggerType"));

   const dtEntity::StringId ManipulatorComponent::TabPlaneDraggerId(dtEntity::SID("TabPlaneDragger"));
   const dtEntity::StringId ManipulatorComponent::TabPlaneTrackballDraggerId(dtEntity::SID("TabPlaneTrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::TabBoxTrackballDraggerId(dtEntity::SID("TabBoxTrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::TrackballDraggerId(dtEntity::SID("TrackballDragger"));
   const dtEntity::StringId ManipulatorComponent::Translate1DDraggerId(dtEntity::SID("Translate1DDragger"));
   const dtEntity::StringId ManipulatorComponent::Translate2DDraggerId(dtEntity::SID("Translate2DDragger"));
   const dtEntity::StringId ManipulatorComponent::TranslateAxisDraggerId(dtEntity::SID("TranslateAxisDragger"));
   const dtEntity::StringId ManipulatorComponent::TabBoxDraggerId(dtEntity::SID("TabBoxDragger"));

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorComponent::ManipulatorComponent()
      : BaseClass()
      , mAttachPoint(dtEntity::StringId())
      , mEntity(NULL)
      , mDraggerContainer(NULL)
   {
      Register(LayerId, &mLayerProperty);
      Register(DraggerTypeId, &mDraggerType);

      mLayerProperty.Set(dtEntity::SID("default"));

      SetDraggerType(TabBoxDraggerId);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   ManipulatorComponent::~ManipulatorComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ManipulatorComponent::OnRemovedFromEntity(dtEntity::Entity& e)
   {
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
         mDraggerContainer = new DraggerContainer(GetDragger(), tcomp);
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
         SetNode(new osgManipulator::TrackballDragger());
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
         SetNode(new MyTranslateAxisDragger());
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
      dragger->setupDefaultGeometry();
      dragger->setHandleEvents(true);
      //dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);
      //dragger->setActivationKeyEvent('a');
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

   ManipulatorSystem::ManipulatorSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
    {

   }

   ////////////////////////////////////////////////////////////////////////////
   ManipulatorSystem::~ManipulatorSystem()
   {
   }
}
