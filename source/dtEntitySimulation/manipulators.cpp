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

#include <dtEntitySimulation/manipulators.h>

#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <dtEntity/nodemasks.h>
#include <osg/PolygonMode>
#include <osg/Version>

namespace dtEntitySimulation
{

   ////////////////////////////////////////////////////////////////////////////////
   TerrainTranslateDragger::TerrainTranslateDragger()
   {
      _xDragger = new osgManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), osg::Vec3(0.0,0.0,1.0));
      addChild(_xDragger.get());
      addDragger(_xDragger.get());

      _yDragger = new osgManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), osg::Vec3(0.0,0.0,1.0));
      addChild(_yDragger.get());
      addDragger(_yDragger.get());

      _zDragger = new osgManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), osg::Vec3(0.0,0.0,1.0));
      addChild(_zDragger.get());
      addDragger(_zDragger.get());

      osg::Plane plane(osg::Vec3(0,0,1),0);
      _translate2DDragger = new osgManipulator::Translate2DDragger(plane);
      _translate2DDragger->setColor(osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
      addChild(_translate2DDragger.get());
      addDragger(_translate2DDragger.get());

      setParentDragger(getParentDragger());
      #if (OSG_VERSION_GREATER_OR_EQUAL(3,1,0) && OPENSCENEGRAPH_SOVERSION >= 90)
//         setIntersectionMask(dtEntity::NodeMasks::MANIPULATOR);
      #endif

      getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TerrainTranslateDragger::setupDefaultGeometry()
   {

      // Create a polygon.
      {
          osg::Geode* geode = new osg::Geode;
          osg::Geometry* geometry = new osg::Geometry();

          osg::Vec3Array* vertices = new osg::Vec3Array(4);
          (*vertices)[0] = osg::Vec3(-0.5,0.5, 0.0);
          (*vertices)[1] = osg::Vec3(-0.5,-0.5, 0.0);
          (*vertices)[2] = osg::Vec3(0.5,-0.5, 0.0);
          (*vertices)[3] = osg::Vec3(0.5,0.5, 0.0);

          geometry->setVertexArray(vertices);
          geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,vertices->size()));

          osg::Vec3Array* normals = new osg::Vec3Array;
          normals->push_back(osg::Vec3(0.0,1.0,0.0));
          geometry->setNormalArray(normals);
          geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

          geode->addDrawable(geometry);

          osg::PolygonMode* polymode = new osg::PolygonMode;
          polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
          geode->getOrCreateStateSet()->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);


          _translate2DDragger->addChild(geode);
      }

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

      unsigned int nodemask = dtEntity::NodeMasks::VISIBLE | dtEntity::NodeMasks::MANIPULATOR;
      _xDragger->setNodeMask(nodemask);
      _yDragger->setNodeMask(nodemask);
      _zDragger->setNodeMask(nodemask);
      _translate2DDragger->setNodeMask(nodemask);


       osg::Geode* geodex = new osg::Geode();
       osg::Geode* geodey = new osg::Geode();
       osg::Geode* geodez = new osg::Geode();

       osg::ShapeDrawable* conex = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));
       osg::ShapeDrawable* coney = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));
       osg::ShapeDrawable* conez = new osg::ShapeDrawable(new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.20f));

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

   ////////////////////////////////////////////////////////////////////////////////
   bool TerrainTranslateDragger::handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
   {
      if(ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return false;
      }
      return BaseClass::handle(pi, ea, aa);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ScaleAllDragger::handle(const osgManipulator::PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
   {
      // Check if the dragger node is in the nodepath.
      if (!pointer.contains(this)) return false;

      switch (ea.getEventType())
      {
          // Pick start.
          case (osgGA::GUIEventAdapter::PUSH):
          {
             osg::NodePath nodePathToRoot;
             computeNodePathToRoot(*this,nodePathToRoot);
             _localToWorld = osg::computeLocalToWorld(nodePathToRoot);
             _worldToLocal = osg::Matrix::inverse(_localToWorld);

             _startPoint = osg::Vec2(ea.getXnormalized(), ea.getYnormalized());

             osg::ref_ptr<osgManipulator::ScaleUniformCommand> cmd = new osgManipulator::ScaleUniformCommand();
             cmd->setStage(osgManipulator::MotionCommand::START);
             cmd->setLocalToWorldAndWorldToLocal(_localToWorld,_worldToLocal);
             dispatch(*cmd);
             return true;
          }

          // Pick move.
          case (osgGA::GUIEventAdapter::DRAG):
           {
               osg::Vec2 move = osg::Vec2(ea.getXnormalized(), ea.getYnormalized()) - _startPoint;

               osg::ref_ptr<osgManipulator::ScaleUniformCommand> cmd = new osgManipulator::ScaleUniformCommand();
               cmd->setStage(osgManipulator::MotionCommand::MOVE);
               cmd->setLocalToWorldAndWorldToLocal(_localToWorld,_worldToLocal);
               cmd->setScale(1 + move[0] + move[1]);
               cmd->setScaleCenter(osg::Vec3());
               dispatch(*cmd);
               return true;
           }

          // Pick finish.
          case (osgGA::GUIEventAdapter::RELEASE):
           {
               osg::ref_ptr<osgManipulator::ScaleUniformCommand> cmd = new osgManipulator::ScaleUniformCommand();

               cmd->setStage(osgManipulator::MotionCommand::FINISH);
               cmd->setLocalToWorldAndWorldToLocal(_localToWorld,_worldToLocal);
               dispatch(*cmd);

               return true;
           }
          default:
              return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ScaleDragger::ScaleDragger()
   {
      _xDragger = new osgManipulator::Scale1DDragger();
      addChild(_xDragger.get());
      addDragger(_xDragger.get());

      _yDragger = new osgManipulator::Scale1DDragger();
      addChild(_yDragger.get());
      addDragger(_yDragger.get());

      _zDragger = new osgManipulator::Scale1DDragger();
      addChild(_zDragger.get());
      addDragger(_zDragger.get());

      _scaleAllDragger = new ScaleAllDragger();
      addChild(_scaleAllDragger.get());
      addDragger(_scaleAllDragger.get());

      setParentDragger(getParentDragger());

#if (OSG_VERSION_GREATER_OR_EQUAL(3,1,0) && OPENSCENEGRAPH_SOVERSION >= 90)
//      setIntersectionMask(dtEntity::NodeMasks::MANIPULATOR);
#endif

      getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ScaleDragger::setupGeometry(osgManipulator::Scale1DDragger* dragger, const osg::Vec4& color)
   {
      unsigned int nodemask = dtEntity::NodeMasks::VISIBLE | dtEntity::NodeMasks::MANIPULATOR;
      dragger->setNodeMask(nodemask);
      dragger->setColor(color);
      osg::Geode* lineGeode = new osg::Geode;

      osg::Geometry* geometry = new osg::Geometry();

      osg::Vec3Array* vertices = new osg::Vec3Array(2);
      (*vertices)[0] = osg::Vec3(0.1f,0.0f,0.0f);
      (*vertices)[1] = osg::Vec3(0.5f,0.0f,0.0f);

      geometry->setVertexArray(vertices);
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

      lineGeode->addDrawable(geometry);

      osg::LineWidth* linewidth = new osg::LineWidth();
      linewidth->setWidth(5.0f);
      lineGeode->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
      lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

      dragger->addChild(lineGeode);


      osg::Geode* geode = new osg::Geode();
      dragger->addChild(geode);
      osg::ShapeDrawable* box = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.5f, 0.0f, 0.0f), 0.05f));
      box->setColor(color);

      geode->addDrawable(box);

     // Create an invisible box for picking the line.
     {
         osg::Box* col = new osg::Box (osg::Vec3(0.3f,0.0f,0.0f), 0.4f, 0.05f, 0.05f);
         osg::Drawable* geometry = new osg::ShapeDrawable(col);
         osgManipulator::setDrawableToAlwaysCull(*geometry);
         geode->addDrawable(geometry);
     }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ScaleDragger::setupDefaultGeometry()
   {
      //_scaleAllDragger->setupDefaultGeometry();
      setupGeometry(_xDragger, osg::Vec4(1.0f,0.0f,0.0f,1.0f));
      setupGeometry(_yDragger, osg::Vec4(0.0f,1.0f,0.0f,1.0f));
      setupGeometry(_zDragger, osg::Vec4(0.0f,0.0f,1.0f,1.0f));

      osg::Quat rotation; rotation.makeRotate(osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
      _xDragger->setMatrix(osg::Matrix(rotation));

      osg::Quat rotation2; rotation2.makeRotate(osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f));
      _yDragger->setMatrix(osg::Matrix(rotation2));

      osg::Geode* geode = new osg::Geode();
      _scaleAllDragger->addChild(geode);
      osg::ShapeDrawable* box = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), 0.1f));
      box->setColor(osg::Vec4(1,1,1,1));
      geode->addDrawable(box);

   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ScaleDragger::handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
   {
      if(ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return false;
      }
      return BaseClass::handle(pi, ea, aa);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool TrackballDragger::handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
   {
      if(ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return false;
      }
      return BaseClass::handle(pi, ea, aa);
   }
}
