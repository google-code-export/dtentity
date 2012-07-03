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

#include <dtEntityQtWidgets/osggraphicswindowqt.h>
#include <dtEntityQtWidgets/glwidgetfactory.h>
#include <dtEntityQtWidgets/osgadapterwidget.h>
#include <dtEntity/log.h>
#include <QtCore/QThread>

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////
   void AdapterCreator::Create()
   {
      Qt::WindowFlags flags = NULL;
      if (!mTraits.windowDecoration)
      {
         flags |= Qt::FramelessWindowHint;
      }

      QGLFormat format;
      format.setAlpha(mTraits.alpha > 0);
      format.setAlphaBufferSize(mTraits.alpha);
      format.setDepth(mTraits.depth > 0);
      format.setDepthBufferSize(mTraits.depth);
      format.setDoubleBuffer(mTraits.doubleBuffer);
      format.setStencil(mTraits.stencil > 0);
      format.setStencilBufferSize(mTraits.stencil);
      format.setSamples(mTraits.samples);
      format.setSampleBuffers(mTraits.samples > 0);
      format.setSwapInterval(mTraits.vsync ? 1 : 0);

      /*
          from Qt 4.7 documentation on QGLWidget:
          Note that under Windows, the QGLContext belonging to a QGLWidget has
          to be recreated when the QGLWidget is reparented. This is necessary 
          due to limitations on the Windows platform. This will most likely cause
          problems for users that have subclassed and installed their own QGLContext
          on a QGLWidget. It is possible to work around this issue by putting the 
          QGLWidget inside a dummy widget and then reparenting the dummy widget,
          instead of the QGLWidget. This will side-step the issue altogether, and
          is what we recommend for users that need this kind of functionality.
      */

      if (mFactory != NULL)
      {         
         mAdapter = mFactory->CreateWidget(format, mSharedContextWidget, flags);
      }
      else
      {
         QGLContext* newContext = new QGLContext(format);
         newContext->create(NULL);
         mAdapter = new OSGAdapterWidget(newContext, NULL, mSharedContextWidget, flags);         
      }

      mAdapter->SetGraphicsWindow(*mWindow);
      mAdapter->setFocusPolicy(Qt::WheelFocus);

   }

   ////////////////////////////////////////////////////////////
   OSGGraphicsWindowQt::OSGGraphicsWindowQt(osg::GraphicsContext::Traits* traits,
                                             GLWidgetFactory* factory,
                                             OSGAdapterWidget* adapter)
   : BaseClass()
   , mValid(false)
   , mRealized(false)
   , mCloseRequested(false)
   , mQWidget(NULL)
   , mCursorShape(Qt::ArrowCursor)
   {

      _traits = traits;

      setWindowRectangle(0, 0, traits->width, traits->height);

      QGLWidget* sharedContextWidget = NULL;
      if (traits->sharedContext != NULL)
      {
         OSGGraphicsWindowQt* sharedWin = dynamic_cast<OSGGraphicsWindowQt*>(traits->sharedContext);
         if (sharedWin != NULL)
         {
            sharedContextWidget = sharedWin->GetQGLWidget();
         }
         else
         {
            LOG_ERROR("A shared context was specified, but it is not a QGLWidget based context, so it can't be shared.");
         }
      }

      if (adapter == NULL)
      {
         AdapterCreator* creator = new AdapterCreator(this, *traits, sharedContextWidget, factory);

         if(QCoreApplication::instance()->thread() != QThread::currentThread())
         {
            creator->moveToThread(QCoreApplication::instance()->thread());
            QMetaObject::invokeMethod(creator, "Create", Qt::BlockingQueuedConnection);
            OSGAdapterWidget* w = creator->GetAdapter();
            SetQGLWidget(w);
         }
         else
         {
            creator->Create();
            SetQGLWidget(creator->GetAdapter());
         }

         delete creator;

      }
      else
      {
         adapter->SetGraphicsWindow(*this);
         adapter->setFocusPolicy(Qt::StrongFocus);
         SetQGLWidget(adapter);
      }

   }

   ////////////////////////////////////////////////////////////
   OSGGraphicsWindowQt::~OSGGraphicsWindowQt()
   {  
      closeImplementation();
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::SetQGLWidget(QGLWidget* qwidget)
   {
      
      if (mQWidget != NULL && getState() != NULL)
      {
         delete mQWidget;
         mQWidget = NULL;
         decrementContextIDUsageCount(getState()->getContextID());
         setState(NULL);
      }

      mQWidget = qwidget;

      mValid = mQWidget != NULL;

      if (valid())
      {
          setState( new osg::State );
          getState()->setGraphicsContext(this);

          if (_traits.valid() && _traits->sharedContext)
          {
              getState()->setContextID( _traits->sharedContext->getState()->getContextID() );
              incrementContextIDUsageCount( getState()->getContextID() );
          }
          else
          {
              getState()->setContextID( osg::GraphicsContext::createNewContextID() );
          }
      }
   }

   ////////////////////////////////////////////////////////////
   QGLWidget* OSGGraphicsWindowQt::GetQGLWidget()
   {
      return mQWidget;
   }

   ////////////////////////////////////////////////////////////
   const QGLWidget* OSGGraphicsWindowQt::GetQGLWidget() const
   {
      return mQWidget;
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::isSameKindAs(const Object* object) const
   {
      return dynamic_cast<const OSGGraphicsWindowQt*>(object)!=0;
   }

   ////////////////////////////////////////////////////////////
   const char* OSGGraphicsWindowQt::libraryName() const
   {
      return "SteathQt";
   }

   ////////////////////////////////////////////////////////////
   const char* OSGGraphicsWindowQt::className() const
   {
      return "OSGGraphicsWindowQt";
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::valid() const
   {
      return mValid;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::ApplyRealize()
   {
      mQWidget->show();
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::realizeImplementation()
   {

      if (mQWidget != NULL)
      {
         if(QCoreApplication::instance()->thread() != QThread::currentThread())
         {
            QMetaObject::invokeMethod(this, "ApplyRealize", Qt::BlockingQueuedConnection);
         }
         else
         {
            ApplyRealize();
         }
         mRealized = true;
      }
      else
      {
         mRealized = false;
      }

      return mRealized;
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::isRealizedImplementation() const
   {
      return mRealized;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::closeImplementation()
   {
     /*
      QMetaObject::invokeMethod(this, "ApplyClose");
      if(this->thread() != QThread::currentThread())
      {
         QMetaObject::invokeMethod(this, "ApplyClose", Qt::BlockingQueuedConnection);
      }
      else
      {
         ApplyClose();
      }*/
   }
   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::ApplyClose()
   {
      if (mQWidget != NULL)
      {
         mQWidget->close();
         delete mQWidget;
         mQWidget = NULL;
      }
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::makeCurrentImplementation()
   {
      if (mQWidget != NULL && mQWidget->isValid())
      {
         mQWidget->makeCurrent();
         return true;
      }
      else
      {
         LOG_ERROR("Cannot make current, does not exist!");
      }
      return false;
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::releaseContextImplementation()
   {
      if (mQWidget != NULL)
      {
         mQWidget->doneCurrent();
         return true;
      }
      else
      {
         LOG_ERROR("Cannot release, does not exist!");
      }
      return false;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::swapBuffersImplementation()
   {
      if (mQWidget != NULL && mQWidget->isValid())
      {
         mQWidget->swapBuffers();
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::checkEvents()
   {
      if (mCloseRequested)
          getEventQueue()->closeWindow();
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::getWindowRectangle(int& x, int& y, int& width, int& height)
   {
      if (mQWidget != NULL)
      {
         // TODO make thread safe!!!
         QRect r = mQWidget->geometry();
         x = r.left();
         y = r.top();
         width = r.width();
         height = r.height();
      }
   }


   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::setWindowRectangleImplementation(int x, int y, int width, int height)
   {
      if(QCoreApplication::instance()->thread() != QThread::currentThread())
      {
         QMetaObject::invokeMethod(this, "ApplySetWindowRectangle", Qt::QueuedConnection,
                                 Q_ARG(int, x), Q_ARG(int, y), Q_ARG(int, width), Q_ARG(int, height));
      }
      else
      {
         ApplySetWindowRectangle(x, y, width, height);
      }      
      
      return true;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::ApplySetWindowRectangle(int x, int y, int w, int h)
   {
      if (mQWidget != NULL)
      {
         mQWidget->move(x, y);
         mQWidget->resize(w, h);
      }
   }

   ////////////////////////////////////////////////////////////
   bool OSGGraphicsWindowQt::setWindowDecorationImplementation(bool flag)
   {
      if (mQWidget != NULL)
      {
      // this cas be done but there are some quirks with it.
         //mQWidget->setWindowFlags();
         //mQWidget->show();
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::grabFocus()
   {
      if (mQWidget != NULL)
      {
         mQWidget->setFocus();
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::ApplySetFocus()
   {
      if (mQWidget != NULL)
      {
         mQWidget->setFocus();
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::grabFocusIfPointerInWindow()
   {
      if(QCoreApplication::instance()->thread() != QThread::currentThread())
      {
         QMetaObject::invokeMethod(this, "ApplySetFocus", Qt::QueuedConnection);
      }
      else
      {
         ApplySetFocus();
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::requestClose()
   {
      mCloseRequested = true;
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::resizedImplementation(int x, int y, int width, int height)
   {
      BaseClass::resizedImplementation(x, y, width, height);      
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::setWindowName (const std::string& name)
   {
      if (mQWidget != NULL)
      {
         mQWidget->setWindowTitle(name.c_str());
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::ApplyUseCursor(bool cursorOn)
   {
      if (mQWidget != NULL)
      {
         if (cursorOn)
         {
            mQWidget->setCursor(QCursor(mCursorShape));
         }
         else
         {
            mQWidget->setCursor(QCursor(Qt::BlankCursor));
         }
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::useCursor(bool cursorOn)
   {
      QMetaObject::invokeMethod(this, "ApplyUseCursor", Qt::QueuedConnection,
                                 Q_ARG(bool, cursorOn));
      
   }


   ////////////////////////////////////////////////////////////
   using osgViewer::GraphicsWindow;
   void OSGGraphicsWindowQt::setCursor(osgViewer::GraphicsWindow::MouseCursor mouseCursor)
   {

      if (mQWidget != NULL)
      {
         switch (mouseCursor)
         {
            case InheritCursor:
               mCursorShape = Qt::LastCursor;
               break;
            case NoCursor:
               mCursorShape = Qt::BlankCursor;
               break;
            case RightArrowCursor:
               mCursorShape = Qt::ArrowCursor;
               break;
            case LeftArrowCursor:
               break;
            case InfoCursor:
               break;
            case DestroyCursor:
               break;
            case HelpCursor:
               mCursorShape = Qt::WhatsThisCursor;
               break;
            case CycleCursor:
               break;
            case SprayCursor:
               break;
            case WaitCursor:
               mCursorShape = Qt::WaitCursor;
               break;
            case TextCursor:
               break;
            case CrosshairCursor:
               mCursorShape = Qt::CrossCursor;
               break;
            case UpDownCursor:
               mCursorShape = Qt::UpArrowCursor;
               break;
            case LeftRightCursor:
               break;
            case TopSideCursor:
               break;
            case BottomSideCursor:
               break;
            case LeftSideCursor:
               break;
            case RightSideCursor:
               break;
            case TopLeftCorner:
               break;
            case TopRightCorner:
               break;
            case BottomRightCorner:
               break;
            case BottomLeftCorner:
               break;
            default:
               ;
         }
         mQWidget->setCursor(mCursorShape);
      }
   }

   ////////////////////////////////////////////////////////////
   void OSGGraphicsWindowQt::requestWarpPointer(float x, float y)
   {
      mQWidget->cursor().setPos(mQWidget->mapToGlobal(QPoint(x,y)));
      getEventQueue()->mouseWarped(x,y);
   }

}
