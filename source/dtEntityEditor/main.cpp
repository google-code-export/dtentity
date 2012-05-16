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

#include <dtEntity/entityid.h>


#include <dtEntity/message.h>
#include <dtEntityEditor/editorapplication.h>
#include <dtEntityEditor/editormainwindow.h>
#include <dtEntityQtWidgets/qtguiwindowsystemwrapper.h>
#include <dtEntity/log.h>
#include <dtEntity/property.h>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Timer>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <QtCore/QMetaType>
#include <QtGui/QApplication>
#include <QtCore/QtPlugin>

#ifdef __GNUC__
   #include <X11/X.h>
   #include <X11/Xlib.h>
#endif

////////////////////////////////////////////////////////////////////////////////
class MyQApplication : public QApplication
{
public:
   MyQApplication ( int & argc, char ** argv )
      : QApplication(argc, argv)
   {
   }
   virtual bool notify ( QObject * receiver, QEvent * e )
   {
      try
      {
         return QApplication::notify(receiver, e);
      }
      catch(const std::exception& ex)
      {
         LOG_ERROR("Exception caught: " + std::string(ex.what()));
      }
      catch(...)
      {
      }
      return false;
   }
};


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{

   // enable multithreaded opengl on linux
#ifdef __GNUC__
   XInitThreads();
#endif

   using namespace dtEntityEditor;

   // start up Qt
   MyQApplication qtapp(argc, argv);


   // for QSettings
   QCoreApplication::setOrganizationName("dtEntity");
   QCoreApplication::setOrganizationDomain("dtEntity");
   QCoreApplication::setApplicationName("dtEntity Editor");

   // let OpenSceneGraph window be a Qt widget
   dtEntityQtWidgets::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();

   // register some used classes as Qt meta types so that they can be
   // used in signal slot connections
   qRegisterMetaType<dtEntity::EntityId>("dtEntity::EntityId");
   qRegisterMetaType<dtEntity::ComponentType>("dtEntity::ComponentType");
   qRegisterMetaType<dtEntity::StringId>("dtEntity::StringId");
   qRegisterMetaType<osg::Vec2>("osg::Vec2");
   qRegisterMetaType<osg::Vec3>("osg::Vec3");
   qRegisterMetaType<osg::Vec4>("osg::Vec4");
   qRegisterMetaType<osg::Timer_t>("osg::Timer_t");
   qRegisterMetaType<dtEntity::GroupProperty>("dtEntity::GroupProperty");
   qRegisterMetaType<dtEntityQtWidgets::OSGGraphicsWindowQt*>("dtEntityQtWidgets::OSGGraphicsWindowQt*");

   bool singleThread = false;

   QString pluginPath = "plugins";
   QString scene = "";
   for(int curArg = 1; curArg < argc; ++curArg)
   {
      std::string curArgv = argv[curArg];
      if (curArgv.empty())
      {
         continue;
      }
      else if(curArgv == "--singleThread")
      {
         singleThread = true;
      }
      else if(curArgv == "--scene")
      {
         ++curArg;
         if (curArg < argc)
         {
            scene = argv[curArg];
         }
      }
   }

   osg::ref_ptr<EditorApplication> application = new EditorApplication(argc, argv);

   QThread* viewerThread;

   if(!singleThread)
   {
      // start dtEntity in a background thread. All communications between
      // Qt and dtEntity go over signal slot connections.
      viewerThread = new QThread();
      application->moveToThread(viewerThread);
   }
   
    // create Qt main window
   EditorMainWindow main(application.get());      
   application->SetMainWindow(&main);

   if(!singleThread)
   {
      // Start the Qt event loop in the d3d thread
      viewerThread->start(QThread::NormalPriority);
   }
  
   // show main window
   main.show();

   // send qt event to game to start processing. This gets delivered thread-safe
   // if qt was started in extra thread
   QMetaObject::invokeMethod(application, "StartGame", Qt::QueuedConnection,
                              Q_ARG(QString, scene));

   qtapp.exec();

   if(!singleThread)
   {
      viewerThread->wait();
      delete viewerThread;
   }
   
   application->moveToThread(QThread::currentThread());
   application = NULL;
   
   return 0;
}
