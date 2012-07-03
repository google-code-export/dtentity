#pragma once

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

#include <osgManipulator/Dragger>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/Scale1DDragger>
#include <osgManipulator/ScaleAxisDragger>

namespace dtEntitySimulation
{

   class TerrainTranslateDragger : public osgManipulator::CompositeDragger
   {
      typedef osgManipulator::CompositeDragger BaseClass;

   public:

      TerrainTranslateDragger();
      virtual void setupDefaultGeometry();
      virtual bool handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

   private:
      osg::ref_ptr<osgManipulator::Translate1DDragger> _xDragger;
      osg::ref_ptr<osgManipulator::Translate1DDragger> _yDragger;
      osg::ref_ptr<osgManipulator::Translate1DDragger> _zDragger;
      osg::ref_ptr<osgManipulator::Translate2DDragger > _translate2DDragger;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ScaleAllDragger : public osgManipulator::Dragger
   {
    public:
        virtual bool handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

    protected:
        osg::Vec2 _startPoint;
        osg::Matrix _localToWorld;
        osg::Matrix _worldToLocal;
   };


   ////////////////////////////////////////////////////////////////////////////////
   class ScaleDragger : public osgManipulator::CompositeDragger
   {
      typedef osgManipulator::CompositeDragger BaseClass;

   public:

      ScaleDragger();
      virtual void setupDefaultGeometry();
      virtual bool handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

   private:
      void setupGeometry(osgManipulator::Scale1DDragger*, const osg::Vec4& color);
      osg::ref_ptr<osgManipulator::Scale1DDragger> _xDragger;
      osg::ref_ptr<osgManipulator::Scale1DDragger> _yDragger;
      osg::ref_ptr<osgManipulator::Scale1DDragger> _zDragger;
      osg::ref_ptr<ScaleAllDragger> _scaleAllDragger;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class TrackballDragger : public osgManipulator::TrackballDragger
   {
      typedef osgManipulator::TrackballDragger BaseClass;
   public:
      virtual bool handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

   };
}
