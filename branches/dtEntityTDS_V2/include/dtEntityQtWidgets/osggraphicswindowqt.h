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
#include <QtCore/Qt>
#include <QtCore/QObject>
#include <osgViewer/GraphicsWindow>
#include <dtEntityQtWidgets/glwidgetfactory.h>

class QGLWidget;

namespace dtEntityQtWidgets
{
   class OSGAdapterWidget;
   class GLWidgetFactory;
   class OSGGraphicsWindowQt;

   class AdapterCreator
         : public QObject
   {
      Q_OBJECT

   public:
      AdapterCreator(OSGGraphicsWindowQt* window, const osg::GraphicsContext::Traits& traits, QGLWidget* sharedContextWidget, GLWidgetFactory* factory = NULL)
         : mWindow(window)
         , mTraits(traits)
         , mFactory(factory)
         , mSharedContextWidget(sharedContextWidget)
         , mAdapter(NULL)
      {

      }

      OSGAdapterWidget* GetAdapter() const { return mAdapter; }
   public slots:
      void Create();

     private:
      OSGGraphicsWindowQt* mWindow;
      osg::GraphicsContext::Traits mTraits;
      GLWidgetFactory* mFactory;
      QGLWidget* mSharedContextWidget;
      OSGAdapterWidget* mAdapter;
   };

   class ENTITYQTWIDGETS_EXPORT OSGGraphicsWindowQt 
      : public QObject
      , public osgViewer::GraphicsWindow
   {
      Q_OBJECT

   public:
      typedef osgViewer::GraphicsWindow BaseClass;

      OSGGraphicsWindowQt(osg::GraphicsContext::Traits* traits,
                          GLWidgetFactory* factory = NULL,
                          OSGAdapterWidget* adapter = NULL);
      virtual ~OSGGraphicsWindowQt();


      QGLWidget* GetQGLWidget();
      const QGLWidget* GetQGLWidget() const;

      virtual bool isSameKindAs(const Object* object) const;
      virtual const char* libraryName() const;
      virtual const char* className() const;

      virtual bool valid() const;

      /** Realise the GraphicsContext.*/
      virtual bool realizeImplementation();

      /** Return true if the graphics context has been realised and is ready to use.*/
      virtual bool isRealizedImplementation() const;

      /** Close the graphics context.*/
      virtual void closeImplementation();

      /** Make this graphics context current.*/
      virtual bool makeCurrentImplementation();

      /** Release the graphics context.*/
      virtual bool releaseContextImplementation();

      /** Swap the front and back buffers.*/
      virtual void swapBuffersImplementation();

      /** Check to see if any events have been generated.*/
      virtual void checkEvents();

      /** Get the window's position and size.*/
      virtual void getWindowRectangle(int& x, int& y, int& width, int& height);

      /** Set the window's position and size.*/
      virtual bool setWindowRectangleImplementation(int x, int y, int width, int height);

      /** Set Window decoration.*/
      virtual bool setWindowDecorationImplementation(bool flag);

      /** Get focus.*/
      virtual void grabFocus();

      /** Get focus on if the pointer is in this window.*/
      virtual void grabFocusIfPointerInWindow();

      void requestClose();
      virtual void resizedImplementation(int x, int y, int width, int height);

      virtual void setWindowName (const std::string & name);
      virtual void useCursor(bool cursorOn);
      virtual void setCursor(osgViewer::GraphicsWindow::MouseCursor mouseCursor);

      virtual void requestWarpPointer(float x, float y);

      void SetQGLWidget(QGLWidget* qwidget);

   protected slots:

      void ApplyUseCursor(bool);
      void ApplySetWindowRectangle(int x, int y, int w, int h);
      void ApplyRealize();
      void ApplyClose();
   private:
      bool mValid;
      bool mRealized;
      bool mCloseRequested;
      bool mPropagateResizeToGlWidget;
      QGLWidget* mQWidget;
      Qt::CursorShape mCursorShape;
   };

}

