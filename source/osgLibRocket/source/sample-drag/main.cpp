/* osgLibRocket, an interface for OpenSceneGraph to use LibRocket
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/
//
// This code is copyright (c) 2011 Martin Scheffler martin.scheffler@googlemail.com
//

#include <osg/PositionAttitudeTransform>
#include <osgDB/FileUtils>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgLibRocket/FileInterface>
#include <osgLibRocket/GuiNode>
#include <osgLibRocket/RenderInterface>
#include <osgLibRocket/SystemInterface>

void addWidgets(osgLibRocket::GuiNode* gui)
{
  // create some GUI widgets
  Rocket::Core::ElementDocument* window1 = gui->getContext()->LoadDocument("demo.rml");
  window1->Show();
  window1->RemoveReference();

  Rocket::Core::ElementDocument* window2 = gui->getContext()->LoadDocument("demo.rml");
  window2->Show();
  window2->RemoveReference();
  window2->SetBox(Rocket::Core::Box(Rocket::Core::Vector2f(100, 100)));

  // load mouse cursor so that we can test if system cursor and gui cursor
  // are overlapping
  gui->getContext()->LoadMouseCursor("cursor.rml");

}

int main(int argc, char** argv)
{
	// add rocket samples to data path
	if(osgDB::fileExists("./assets"))
	{
		osgDB::setDataFilePathList("./assets");
	}

   int curArg = 0;
   while (curArg < argc)
   {
      std::string curArgv = argv[curArg];

      if (curArgv == "--projectAssets")
      {
         ++curArg;
         if (curArg < argc)
         {            
            osgDB::setDataFilePathList(argv[curArg] + std::string("/LibRocket"));
         }
      }
      ++curArg;
   }

   if(osgDB::findDataFile("window.rml") == "")
   {
      std::cout << "Cannot find data files. Please add parameter --projectAssets <assetsdir>\n";
      return -1;
   }


   osgLibRocket::FileInterface* file_interface = new osgLibRocket::FileInterface();
   Rocket::Core::SetFileInterface(file_interface);

  // create and set libRocket to OSG interfaces
   osgLibRocket::RenderInterface* renderer = new osgLibRocket::RenderInterface();
   Rocket::Core::SetRenderInterface(renderer);

   osgLibRocket::SystemInterface* system_interface = new osgLibRocket::SystemInterface();
   Rocket::Core::SetSystemInterface(system_interface);


   Rocket::Core::Initialise();

   // load some fonts
   Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");
   Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
   Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
   Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");

  // create the OSG viewer
  osg::ArgumentParser arguments(&argc,argv);
  osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer(arguments);
  //viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
  
  // scene graph root
  osg::Group* sceneNode = new osg::Group();
  viewer->setSceneData(sceneNode);
  viewer->addEventHandler(new osgViewer::StatsHandler);

  {
    // GuiNode is responsible for rendering the LibRocket GUI to OSG
    osgLibRocket::GuiNode* gui = new osgLibRocket::GuiNode("fullscreen",  true);

    // create a camera that will be rendered after the main OSG scene.
    // This is adapted from osghud example
    osg::ref_ptr<osg::Camera> cam = new osg::Camera();
    cam->setClearMask(GL_DEPTH_BUFFER_BIT);
    cam->setRenderOrder(osg::Camera::POST_RENDER, 100);
    cam->setAllowEventFocus(false);

    // set the view matrix
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    osgViewer::ViewerBase::Views views;
    viewer->getViews(views);

    osg::GraphicsContext* gc = viewer->getCamera()->getGraphicsContext();
    // same graphics context as main camera
    cam->setGraphicsContext(gc);

    // passing the camera to the gui node makes it render to that cam
    // and adapt the camera settings in accord to the window size
    gui->setCamera(cam);

    // add gui as child to cam
    cam->addChild(gui);

    // gui cam is slave cam of viewer cam
    sceneNode->addChild(cam);
    addWidgets(gui);

    viewer->addEventHandler(gui->GetGUIEventHandler());

  }

  {
    // GuiNode is responsible for rendering the LibRocket GUI to OSG
    osgLibRocket::GuiNode* gui = new osgLibRocket::GuiNode("inscene", false);

    // render in-scene

    // position in front of camera
    osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
    pat->setPosition(osg::Vec3(0, 10, 0));

    // rotate so that gui is upright
    pat->setAttitude(osg::Quat(-osg::PI_2, osg::Vec3(1,0,0)));

    // add to scene
    sceneNode->addChild(pat);
    pat->addChild(gui);
    addWidgets(gui);
    viewer->addEventHandler(gui->GetGUIEventHandler());
  }



  viewer->run();


  // shutdown makes some problems. Shutdown is clean if all gui nodes are removed
  // first and then the rocket system is shut down.
  // Not cleaning up at all seems to work without a crash, too.
  //viewer = NULL;
  //Rocket::Core::Shutdown();

}
