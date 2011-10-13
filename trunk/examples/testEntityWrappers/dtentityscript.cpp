/* -*-c++-*-
* testEntity - testEntity(.h & .cpp) - Using 'The MIT License'
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

#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/osgcomponents.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <iostream>
#include <osgDB/FileUtils>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <dtEntity/profile.h>

int main(int argc, char** argv)
{

    std::string script = "";
    bool profiling_enabled = false;
    int curArg = 1;

    while (curArg < argc)
    {
       std::string curArgv = argv[curArg];
       if (!curArgv.empty())
       {
          if (curArgv == "--script")
          {
             ++curArg;
             if (curArg < argc)
             {
                script = argv[curArg];
             }
          }
          else if (curArgv == "--enable-profiling")
          {
             profiling_enabled = true;
          }
        }
       ++curArg;
    }

    if(script == "")
    {
        std::cout << "Please give argument --script with path to javascript file!";
        return 0;
    }

   osg::ArgumentParser arguments(&argc,argv);   
   osgViewer::CompositeViewer viewer(arguments);
   osgViewer::View* view = new osgViewer::View();
   viewer.addView(view);
  
   // fullscreen mode has some problems with mouse coordinates if multiple
   // screens are present. So use windowed mode by default.
   view->setUpViewInWindow(100,100,800,600);

   dtEntity::EntityManager* em = new dtEntity::EntityManager();
   
   if(!dtEntity::InitOSGViewer(argc, argv, &viewer, em, true, true))
   {
      LOG_ERROR("Error setting up dtEntity!");
      return 0;
   }
   
   //viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded); 
   
   dtEntityWrappers::ScriptSystem* scriptsys = new dtEntityWrappers::ScriptSystem(*em);
   em->AddEntitySystem(*scriptsys);
   
   scriptsys->ExecuteFile(script);
   
   if(profiling_enabled)
   {
      dtEntity::StringId frameId = dtEntity::SID("Frame");
      dtEntity::StringId frameAdvanceId = dtEntity::SID("Frame_Advance");
      dtEntity::StringId frameEvTrId = dtEntity::SID("Frame_EventTraversal");
      dtEntity::StringId frameUpTrId = dtEntity::SID("Frame_UpdateTraversal");
      dtEntity::StringId frameRenderTrId = dtEntity::SID("Frame_RenderingTraversals");

      unsigned int framecount = 0;
      while (!viewer.done()) 
      {
         CProfileManager::Increment_Frame_Counter();
			CProfileManager::Start_Profile(frameId);
         
         CProfileManager::Start_Profile(frameAdvanceId);
         viewer.advance();
         CProfileManager::Stop_Profile();

         CProfileManager::Start_Profile(frameEvTrId);
         viewer.eventTraversal();
         CProfileManager::Stop_Profile();

         CProfileManager::Start_Profile(frameUpTrId);
         viewer.updateTraversal();
         CProfileManager::Stop_Profile();

         CProfileManager::Start_Profile(frameRenderTrId);
         viewer.renderingTraversals();
         CProfileManager::Stop_Profile();

         CProfileManager::Stop_Profile();
         if(++framecount > 999)
         {
            CProfileManager::dumpAll();
			   fflush(stdout);
            framecount = 0;
            CProfileManager::Reset();
         }
      }
   }
   else
   {
      while (!viewer.done()) viewer.frame();
   }
   return 0;
}
