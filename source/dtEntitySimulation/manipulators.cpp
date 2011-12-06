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

namespace dtEntitySimulation
{

  /* ////////////////////////////////////////////////////////////////////////////////
   bool TranslateXYPlaneDragger::handle(const osgManipulator::PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
   {
       // Check if the dragger node is in the nodepath.
       if (!pointer.contains(this)) return false;

       bool handled = false;

        if (_translate2DDragger->handle(pointer, ea, aa))
            handled = true;

       return handled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TranslateXYPlaneDragger::setupDefaultGeometry()
   {

   }*/


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

      setIntersectMask(dtEntity::NodeMasks::MANIPULATOR);
      _xDragger->setIntersectMask(dtEntity::NodeMasks::MANIPULATOR);
      _yDragger->setIntersectMask(dtEntity::NodeMasks::MANIPULATOR);
      _zDragger->setIntersectMask(dtEntity::NodeMasks::MANIPULATOR);
      _translate2DDragger->setIntersectMask(dtEntity::NodeMasks::MANIPULATOR);
   }
}
