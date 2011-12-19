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

#include <dtEntityQtWidgets/export.h>
#include <dtEntityQtWidgets/glwidgetfactory.h>
#include <osg/Version>
#include <osg/GraphicsContext>
#include <osg/ref_ptr>

namespace dtEntityQtWidgets
{

   class ENTITYQTWIDGETS_EXPORT QtGuiWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
   {
   public:
      static void EnableQtGUIWrapper(GLWidgetFactory* factory = NULL);

      QtGuiWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface);

      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier =
         osg::GraphicsContext::ScreenIdentifier());

      virtual void getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int& width, unsigned int& height);

      virtual bool setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int width, unsigned int height);

      virtual bool setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               double refreshRate);

      virtual void getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings & resolution);

      virtual void enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl);


      virtual osg::GraphicsContext* createGraphicsContext(osg::GraphicsContext::Traits* traits);

      ///Supply a custom factory to create custom QGLWidgets.
      void SetGLWidgetFactory(GLWidgetFactory* factory);

   protected:
      virtual ~QtGuiWindowSystemWrapper() {}
   private:
      osg::ref_ptr<osg::GraphicsContext::WindowingSystemInterface> mInterface;
      osg::ref_ptr<GLWidgetFactory> mWidgetFactory;
   };

}
