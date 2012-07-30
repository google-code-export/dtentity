/* -*-c++-*-
* testDebugDrawManager - testDebugDrawManager(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Martin Scheffler
*/
#include <dtEntity/component.h>
#include <dtEntity/core.h>
#include <dtEntity/debugdrawinterface.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/initosgviewer.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systeminterface.h>
#include <osgViewer/Renderer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/FileUtils>
#include <iostream>


using namespace dtEntity;

///////////////////////////////////////////////

int main(int argc, char** argv)
{

   osg::ArgumentParser arguments(&argc,argv);   
   osgViewer::Viewer viewer(arguments);
   dtEntity::EntityManager em;
   
   dtEntityOSG::InitOSGViewer(argc, argv, viewer, em, false);

   dtEntity::DebugDrawInterface* debugdraw = dtEntity::GetDebugDrawInterface();

   debugdraw->SetEnabled(true);

   debugdraw->AddLine(osg::Vec3(0, 0, 0), osg::Vec3(0, 0, 1), osg::Vec4(1,0,0,1), 1, 100, true);
   debugdraw->AddLine(osg::Vec3(0, 0, 0), osg::Vec3(1, 0, 0), osg::Vec4(0,1,0,1), 1, 100, true);
   debugdraw->AddSphere(osg::Vec3(-1, 0, 0), 0.5f, osg::Vec4(1,1,1,1), 100, true);

   std::vector<osg::Vec3> points;
   for(float f = 0; f < osg::PI * 2; f += 0.1f)
   {
      points.push_back(osg::Vec3(-sin(f) * 2, 0, cos(f) * 2));
      debugdraw->AddPoint(osg::Vec3(-sin(f) * 1.5f, 0, cos(f) * 1.5f), osg::Vec4(1,1,0,1), 1, f * 10, true);
   }

   debugdraw->AddPoints(points, osg::Vec4(0,1,1,1), 1, 100, true);

   std::vector<osg::Vec3> tris;
   osg::Vec3 o(2, 0, 0);
   tris.push_back(o + osg::Vec3(0, 0, 0));
   tris.push_back(o + osg::Vec3(0, 0, 1));
   tris.push_back(o + osg::Vec3(1, 0, 1));

   tris.push_back(o + osg::Vec3(0, 0, 0));
   tris.push_back(o + osg::Vec3(-1, 0, -1));
   tris.push_back(o + osg::Vec3(0, 0, -1));

   debugdraw->AddTriangles(tris, osg::Vec4(0,1,1,1), 1, 100, true);

   //mDebugDrawManager->AddAABB(osg::Vec3(-2,-2,-2), osg::Vec3(2,2,2), osg::Vec4(0.5f,1,0.5f, 1), 1, 50);

   osg::Matrix m;
   m.makeRotate(osg::Vec3(1,0,0), osg::Vec3(1,1,0));
   debugdraw->AddOBB(m, osg::Vec3(-2,-2,-2), osg::Vec3(2,2,2), osg::Vec4(0.5f,1,0.5f, 1), 1, 50);
   debugdraw->AddAxes(m, osg::Vec4(1,0,0.5f, 1), 1, 50);


   viewer.setCameraManipulator(new osgGA::TrackballManipulator());
   viewer.getCameraManipulator()->setHomePosition(osg::Vec3(0, -10, 0), osg::Vec3(0, 1, 0), osg::Vec3(0,0,1),false);
   viewer.getCameraManipulator()->home(0);

   float time = 0;

   dtEntity::SystemInterface* iface = dtEntity::GetSystemInterface();

   while (!viewer.done())
   {
      viewer.advance(DBL_MAX);
      viewer.eventTraversal();

      iface->EmitTickMessagesAndQueuedMessages();

      viewer.updateTraversal();
      
      
      time += 0.1f;
      debugdraw->AddLine(osg::Vec3(0, 0, 0), osg::Vec3(-sin(time), 0, cos(time)), osg::Vec4(0,1,1,1), 1, 0, true);

      debugdraw->AddCircle(osg::Vec3(1, 0, 0), osg::Vec3(-sin(time), 0, cos(time)), 0.5f, osg::Vec4(0,1,1,1), 0, true);

      debugdraw->AddString(osg::Vec3(-sin(time), 0, cos(time)), "My TestString", osg::Vec4(1,1,0,1), 0, true);

      viewer.renderingTraversals();
   }
   return 0;
}
