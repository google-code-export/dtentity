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


#include <dtEntityQtWidgets/qtguiwindowsystemwrapper.h>
#include <dtEntityQtWidgets/osggraphicswindowqt.h>
#include <dtEntity/log.h>
#include <QtGui/QApplication>

namespace dtEntityQtWidgets
{
   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::EnableQtGUIWrapper(GLWidgetFactory* factory)
   {
      osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

      if (winSys != NULL)
      {
         QtGuiWindowSystemWrapper* wrapper = new QtGuiWindowSystemWrapper(*winSys);
         osg::GraphicsContext::setWindowingSystemInterface(wrapper);

         if(factory != NULL)
         {
            wrapper->SetGLWidgetFactory(factory);
         }
      }
      else
      {
         LOG_ERROR("Unable to initialize. OSG reported not having any windowing system interface.");
      }

   }

   /////////////////////////////////////////////////////////////
   QtGuiWindowSystemWrapper::QtGuiWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface)
   : mInterface(&oldInterface)
   , mWidgetFactory(NULL)
   {
   }

   /////////////////////////////////////////////////////////////
   unsigned int QtGuiWindowSystemWrapper::getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier)
   {
      return mInterface->getNumScreens(screenIdentifier);
   }

   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            unsigned int& width, unsigned int& height)
   {
      mInterface->getScreenResolution(screenIdentifier, width, height);
   }

   /////////////////////////////////////////////////////////////
   bool QtGuiWindowSystemWrapper::setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            unsigned int width, unsigned int height)
   {
      return mInterface->setScreenResolution(screenIdentifier, width, height);
   }

   /////////////////////////////////////////////////////////////
   bool QtGuiWindowSystemWrapper::setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            double refreshRate)
   {
      return mInterface->setScreenRefreshRate(screenIdentifier, refreshRate);
   }


   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings & resolution)
   {
      mInterface->getScreenSettings(si, resolution);
   }

   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl)
   {
      mInterface->enumerateScreenSettings(si, rl);
   }

   /////////////////////////////////////////////////////////////
   osg::GraphicsContext* QtGuiWindowSystemWrapper::createGraphicsContext(osg::GraphicsContext::Traits* traits)
   {
      if (traits->pbuffer)
      {
         return mInterface->createGraphicsContext(traits);
      }
      else
      {
         OSGGraphicsWindowQt* w = new OSGGraphicsWindowQt(traits, mWidgetFactory);
         w->moveToThread(QCoreApplication::instance()->thread());
         return w;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::SetGLWidgetFactory(GLWidgetFactory* factory)
   {
      mWidgetFactory = factory;
   }
}
