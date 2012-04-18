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

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <dtEntityQtWidgets/export.h>
#include <osg/observer_ptr>
#include <osgViewer/GraphicsWindow>

/// @endcond

namespace dtEntityQtWidgets
{

   class ENTITYQTWIDGETS_EXPORT OSGAdapterWidget : public QGLWidget
   {
      Q_OBJECT

   public:
      OSGAdapterWidget(QGLContext* context, QWidget* parent = NULL,
         const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL);

      virtual ~OSGAdapterWidget();

      osgViewer::GraphicsWindow& GetGraphicsWindow();
      const osgViewer::GraphicsWindow& GetGraphicsWindow() const;

      void SetGraphicsWindow(osgViewer::GraphicsWindow& newWindow);

      virtual void DoResize(int w, int h)
      {
         //Don't, send resize over signal/slot connection
      }

      void dragEnterEvent(QDragEnterEvent *event);
      void dropEvent(QDropEvent *event);

      bool event(QEvent* e);

   signals:

      void TextDropped(const QPointF& pos, const QString&);
      void UrlDropped(const QPointF& pos, const QUrl& url);

      // when escape key is pressed
      void EscapePressed();

   protected:

      virtual void resizeEvent(QResizeEvent* evt);
      
      virtual void resizeGL(int width, int height);
      void resizeGLImpl(int width, int height);

      virtual void keyPressEvent(QKeyEvent* event);
      virtual void keyReleaseEvent(QKeyEvent* event);
      virtual void mousePressEvent(QMouseEvent* event);
      virtual void mouseReleaseEvent(QMouseEvent* event);
      virtual void mouseMoveEvent(QMouseEvent* event);
      virtual void mouseDoubleClickEvent(QMouseEvent* event);
      virtual void wheelEvent(QWheelEvent* event);
      virtual void enterEvent(QEvent* event);
      virtual void leaveEvent(QEvent* event);

      osg::observer_ptr<osgViewer::GraphicsWindow> mGraphicsWindow;

      std::set<std::pair<int, int> > mPressedKeys;

   };

}
