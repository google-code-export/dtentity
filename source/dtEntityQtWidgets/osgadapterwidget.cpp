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

#include <dtEntityQtWidgets/osgadapterwidget.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>

#include <dtEntity/inputinterface.h>
#include <dtEntity/osginputinterface.h>
#include <dtEntity/log.h>
#include <iostream>
#include <osg/Version>
#include <QtCore/QtDebug>

namespace dtEntityQtWidgets
{
   class QtKeyboardMap
   {

      public:
         QtKeyboardMap()
         {
            mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
            mKeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_Delete;
            mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
            mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
            mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
            mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
            mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
            mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
            mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
            mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
            mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
            mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
            mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
            mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
            mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
            mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
            mKeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
            mKeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
            mKeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;

            mKeyMap[Qt::Key_F1             ] = osgGA::GUIEventAdapter::KEY_F1;
            mKeyMap[Qt::Key_F2             ] = osgGA::GUIEventAdapter::KEY_F2;
            mKeyMap[Qt::Key_F3             ] = osgGA::GUIEventAdapter::KEY_F3;
            mKeyMap[Qt::Key_F4             ] = osgGA::GUIEventAdapter::KEY_F4;
            mKeyMap[Qt::Key_F5             ] = osgGA::GUIEventAdapter::KEY_F5;
            mKeyMap[Qt::Key_F6             ] = osgGA::GUIEventAdapter::KEY_F6;
            mKeyMap[Qt::Key_F7             ] = osgGA::GUIEventAdapter::KEY_F7;
            mKeyMap[Qt::Key_F8             ] = osgGA::GUIEventAdapter::KEY_F8;
            mKeyMap[Qt::Key_F9             ] = osgGA::GUIEventAdapter::KEY_F9;
            mKeyMap[Qt::Key_F10            ] = osgGA::GUIEventAdapter::KEY_F10;
            mKeyMap[Qt::Key_F11            ] = osgGA::GUIEventAdapter::KEY_F11;
            mKeyMap[Qt::Key_F12            ] = osgGA::GUIEventAdapter::KEY_F12;
            mKeyMap[Qt::Key_F13            ] = osgGA::GUIEventAdapter::KEY_F13;
            mKeyMap[Qt::Key_F14            ] = osgGA::GUIEventAdapter::KEY_F14;
            mKeyMap[Qt::Key_F15            ] = osgGA::GUIEventAdapter::KEY_F15;
            mKeyMap[Qt::Key_F16            ] = osgGA::GUIEventAdapter::KEY_F16;
            mKeyMap[Qt::Key_F17            ] = osgGA::GUIEventAdapter::KEY_F17;
            mKeyMap[Qt::Key_F18            ] = osgGA::GUIEventAdapter::KEY_F18;
            mKeyMap[Qt::Key_F19            ] = osgGA::GUIEventAdapter::KEY_F19;
            mKeyMap[Qt::Key_F20            ] = osgGA::GUIEventAdapter::KEY_F20;

            mKeyMap[Qt::Key_hyphen         ] = '-';
            mKeyMap[Qt::Key_Equal         ] = '=';

            mKeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
            mKeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
            mKeyMap[Qt::Key_Minus         ] = '-';
            mKeyMap[Qt::Key_Plus          ] = '+';


            //mKeyMap[Qt::Key_H              ] = osgGA::GUIEventAdapter::KEY_KP_Home;
            //mKeyMap[Qt::Key_                    ] = osgGA::GUIEventAdapter::KEY_KP_Up;
            //mKeyMap[92                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
            //mKeyMap[86                    ] = osgGA::GUIEventAdapter::KEY_KP_Left;
            //mKeyMap[87                    ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
            //mKeyMap[88                    ] = osgGA::GUIEventAdapter::KEY_KP_Right;
            //mKeyMap[83                    ] = osgGA::GUIEventAdapter::KEY_KP_End;
            //mKeyMap[84                    ] = osgGA::GUIEventAdapter::KEY_KP_Down;
            //mKeyMap[85                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
            mKeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_Insert;
            //mKeyMap[Qt::Key_Delete        ] = osgGA::GUIEventAdapter::KEY_KP_Delete;

            mKeyMap[Qt::Key_Plus           ] = osgGA::GUIEventAdapter::KEY_KP_Add;
            mKeyMap[Qt::Key_Minus          ] = osgGA::GUIEventAdapter::KEY_KP_Subtract;


            mKeyMap[Qt::Key_A] = osgGA::GUIEventAdapter::KEY_A;
            mKeyMap[Qt::Key_B] = osgGA::GUIEventAdapter::KEY_B;
            mKeyMap[Qt::Key_C] = osgGA::GUIEventAdapter::KEY_C;
            mKeyMap[Qt::Key_D] = osgGA::GUIEventAdapter::KEY_D;
            mKeyMap[Qt::Key_E] = osgGA::GUIEventAdapter::KEY_E;
            mKeyMap[Qt::Key_F] = osgGA::GUIEventAdapter::KEY_F;
            mKeyMap[Qt::Key_G] = osgGA::GUIEventAdapter::KEY_G;
            mKeyMap[Qt::Key_H] = osgGA::GUIEventAdapter::KEY_H;
            mKeyMap[Qt::Key_I] = osgGA::GUIEventAdapter::KEY_I;
            mKeyMap[Qt::Key_J] = osgGA::GUIEventAdapter::KEY_J;
            mKeyMap[Qt::Key_K] = osgGA::GUIEventAdapter::KEY_K;
            mKeyMap[Qt::Key_L] = osgGA::GUIEventAdapter::KEY_L;
            mKeyMap[Qt::Key_M] = osgGA::GUIEventAdapter::KEY_M;
            mKeyMap[Qt::Key_N] = osgGA::GUIEventAdapter::KEY_N;
            mKeyMap[Qt::Key_O] = osgGA::GUIEventAdapter::KEY_O;
            mKeyMap[Qt::Key_P] = osgGA::GUIEventAdapter::KEY_P;
            mKeyMap[Qt::Key_Q] = osgGA::GUIEventAdapter::KEY_Q;
            mKeyMap[Qt::Key_R] = osgGA::GUIEventAdapter::KEY_R;
            mKeyMap[Qt::Key_S] = osgGA::GUIEventAdapter::KEY_S;
            mKeyMap[Qt::Key_T] = osgGA::GUIEventAdapter::KEY_T;
            mKeyMap[Qt::Key_U] = osgGA::GUIEventAdapter::KEY_U;
            mKeyMap[Qt::Key_V] = osgGA::GUIEventAdapter::KEY_V;
            mKeyMap[Qt::Key_W] = osgGA::GUIEventAdapter::KEY_W;
            mKeyMap[Qt::Key_X] = osgGA::GUIEventAdapter::KEY_X;
            mKeyMap[Qt::Key_Y] = osgGA::GUIEventAdapter::KEY_Y;
            mKeyMap[Qt::Key_Z] = osgGA::GUIEventAdapter::KEY_Z;



         }

         ~QtKeyboardMap()
         {
         }

         int remapKey(QKeyEvent* event)
         {
            KeyMap::iterator itr = mKeyMap.find(event->key());
            if (itr == mKeyMap.end())
            {
               return int(*(event->text().toAscii().data()));
            }
            else
               return itr->second;
         }

         int remapUnmodifiedKey(QKeyEvent* event)
         {
            KeyMap::iterator itr = mKeyMap.find(event->key());
            if (itr == mKeyMap.end())
            {
               return int(*(event->text().toLower().toAscii().data()));
            }
            else
               return itr->second;
         }         

      private:
         typedef std::map<unsigned int, int> KeyMap;
         KeyMap mKeyMap;
   };

   static QtKeyboardMap STATIC_KEY_MAP;

   //////////////////////////////////////////////////////////////////////////////////
   OSGAdapterWidget::OSGAdapterWidget(QGLContext* context, QWidget * parent,
            const QGLWidget * shareWidget, Qt::WindowFlags f):
               QGLWidget(context, parent, shareWidget, f)
   {
     
      //don't let Qt perform the swap buffer since OSG will be handling that
      setAutoBufferSwap(false);

      // This enables us to track mouse movement even when
      // no button is pressed.  The motion models depend
      // on tracking the mouse location to work properly.
      setMouseTracking(true);

      // Accept Qt widget drag & drop operations
      setAcceptDrops(true);

      // don't let Qt do OpenGL calls
      setUpdatesEnabled(false);

      // allow multitouch
      this->setAttribute(Qt::WA_AcceptTouchEvents);

   }

   //////////////////////////////////////////////////////////////////////////////////
   OSGAdapterWidget::~OSGAdapterWidget()
   {

   }

   //////////////////////////////////////////////////////////////////////////////////
   osgGA::GUIEventAdapter::TouchPhase ConvertTouchPointState(Qt::TouchPointState in)
   {
      switch(in)
      {
      case Qt::TouchPointPressed:         return osgGA::GUIEventAdapter::TOUCH_BEGAN;
      case Qt::TouchPointMoved:           return osgGA::GUIEventAdapter::TOUCH_MOVED;
      case Qt::TouchPointStationary:      return osgGA::GUIEventAdapter::TOUCH_STATIONERY;
      case Qt::TouchPointReleased:        return osgGA::GUIEventAdapter::TOUCH_ENDED;
      default:;
      }
      return osgGA::GUIEventAdapter::TOUCH_UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////////////
   bool OSGAdapterWidget::event(QEvent* e)
   {

      switch(e->type())
      {

         case QEvent::TouchBegin: 
         case QEvent::TouchUpdate: 
         case QEvent::TouchEnd:
         {

            QTouchEvent* evt = static_cast<QTouchEvent*>(e);
            osgGA::GUIEventAdapter* ev = new osgGA::GUIEventAdapter();

            switch(e->type())
            {
               case QEvent::TouchBegin:  ev->setEventType(osgGA::GUIEventAdapter::PUSH); break;
               case QEvent::TouchUpdate: ev->setEventType(osgGA::GUIEventAdapter::DRAG); break;
               case QEvent::TouchEnd:    ev->setEventType(osgGA::GUIEventAdapter::RELEASE); break;
               default: ;
            }
            ev->setEventType(osgGA::GUIEventAdapter::PUSH);
            ev->setTime(mGraphicsWindow->getEventQueue()->getTime());
             
            QList<QTouchEvent::TouchPoint> pts = evt->touchPoints();
            for(QList<QTouchEvent::TouchPoint>::iterator i = pts.begin(); i != pts.end(); ++i) 
            {
               QTouchEvent::TouchPoint pt = *i;
               QPointF p = pt.normalizedPos();
               ev->addTouchPoint(pt.id(), ConvertTouchPointState(pt.state()), p.x(), p.y());
            }
            mGraphicsWindow->getEventQueue()->addEvent(ev);
            return true;
         }
         default: 
#if defined(Q_WS_X11)
          // acquires context in linux
          if (e->type() == QEvent::Hide) return false;
#endif
          return QGLWidget::event(e);
        }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::dragEnterEvent(QDragEnterEvent* event)
   {
      // accept all drops for now
      event->acceptProposedAction();
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::dropEvent(QDropEvent* evt)
   {

      QSize s = size();
      float x = (float)evt->pos().x() / (float)s.width() * 2 - 1;
      float y = 1.0 - (float)evt->pos().y() / (float)s.height() * 2;

      if(evt->mimeData()->hasText())
      {
         QString text = evt->mimeData()->text();
         emit TextDropped(QPointF(x, y), text);
      }

      if(evt->mimeData()->hasUrls())
      {
         QList<QUrl> urls = evt->mimeData()->urls();
         foreach(QUrl url, urls)
         {
            emit UrlDropped(QPointF(x, y), url);
         }
      }

      evt->setDropAction(Qt::IgnoreAction);
      
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow& OSGAdapterWidget::GetGraphicsWindow()
   {
      return *mGraphicsWindow;
   }

   //////////////////////////////////////////////////////////////////////////////////
   const osgViewer::GraphicsWindow& OSGAdapterWidget::GetGraphicsWindow() const
   {
      return *mGraphicsWindow;
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::SetGraphicsWindow(osgViewer::GraphicsWindow& newWindow)
   {
      mGraphicsWindow = &newWindow;
      QRect r = geometry();
      mGraphicsWindow->resized(r.left(), r.top(), r.width(), r.height());
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::resizeEvent(QResizeEvent* evt)
   {       
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::resizeGLImpl(int width, int height)
   {
      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         mGraphicsWindow->getEventQueue()->windowResize(0, 0, width, height );
         mGraphicsWindow->resized(0,0,width,height);
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::resizeGL( int width, int height )
   {
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::keyPressEvent( QKeyEvent* event )
   {
      if (mGraphicsWindow.valid())
      {
         int value = STATIC_KEY_MAP.remapKey(event);
         int valueunmod = STATIC_KEY_MAP.remapUnmodifiedKey(event);
         mGraphicsWindow->getEventQueue()->keyPress( value, valueunmod );
         mPressedKeys.insert(std::make_pair(value, valueunmod));
      }
      QGLWidget::keyPressEvent(event);

      
      if(event->key() == Qt::Key_Escape)
      {
         emit EscapePressed();
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::keyReleaseEvent( QKeyEvent* event )
   {
      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         int value = STATIC_KEY_MAP.remapKey(event);
         int valueunmod = STATIC_KEY_MAP.remapUnmodifiedKey(event);
         mGraphicsWindow->getEventQueue()->keyRelease( value, valueunmod );
         
         std::set<std::pair<int, int> >::iterator i;
         for(i = mPressedKeys.begin(); i != mPressedKeys.end(); ++i)
         {
            if(i->first == value) 
            {
               mPressedKeys.erase(i);
               break;
            }
         }
      }
      QGLWidget::keyReleaseEvent(event);
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::mousePressEvent( QMouseEvent* event )
   {
      int button = 0;
      switch(event->button())
      {
         case(Qt::LeftButton): button = 1; break;
         case(Qt::MidButton): button = 2; break;
         case(Qt::RightButton): button = 3; break;
         case(Qt::NoButton): button = 0; break;
         default: button = 0; break;
      }
      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         mGraphicsWindow->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::mouseReleaseEvent( QMouseEvent* event )
   {
      int button = 0;
      switch(event->button())
      {
         case(Qt::LeftButton): button = 1; break;
         case(Qt::MidButton): button = 2; break;
         case (Qt::RightButton): button = 3; break;
         case(Qt::NoButton): button = 0; break;
         default: button = 0; break;
      }

      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         mGraphicsWindow->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::mouseMoveEvent( QMouseEvent* event )
   {
      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         mGraphicsWindow->getEventQueue()->mouseMotion(event->x(), event->y());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::mouseDoubleClickEvent( QMouseEvent* event )
   {
      if (!mGraphicsWindow.valid())
      {
         return;
      }
      osg::ref_ptr<osgViewer::GraphicsWindow> l;

      int button = 0;
      switch(event->button())
      {
         case(Qt::LeftButton): button = 1; break;
         case(Qt::MidButton): button = 2; break;
         case(Qt::RightButton): button = 3; break;
         case(Qt::NoButton): button = 0; break;
         default: button = 0; break;
      }

      mGraphicsWindow->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::wheelEvent(QWheelEvent* event)
   {
      if (mGraphicsWindow.valid())
      {
         osg::ref_ptr<osgViewer::GraphicsWindow> l;
         if (event->orientation() == Qt::Horizontal)
         {
            if (event->delta() > 0)
            {
               mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_LEFT);
            }
            else
            {
               mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_RIGHT);
            }
         }
         else
         {
            if (event->delta() > 0)
            {
               mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
            }
            else
            {
               mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::enterEvent(QEvent* event)
   {
      dtEntity::MouseEnterLeaveEvent* evt = new dtEntity::MouseEnterLeaveEvent();
      evt->mFocused = true;
      mGraphicsWindow->getEventQueue()->userEvent(evt);
   }

   //////////////////////////////////////////////////////////////////////////////////
   void OSGAdapterWidget::leaveEvent(QEvent* event)
   {
      std::set<std::pair<int, int> >::iterator i;
      for(i = mPressedKeys.begin(); i != mPressedKeys.end(); ++i)
      {
         mGraphicsWindow->getEventQueue()->keyRelease(i->first, i->second);
      }
      mPressedKeys.clear();

      dtEntity::MouseEnterLeaveEvent* evt = new dtEntity::MouseEnterLeaveEvent();
      evt->mFocused = false;
      mGraphicsWindow->getEventQueue()->userEvent(evt);
   }


}
