#pragma once

/* -*-c++-*-
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


#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/inputhandler.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/stringid.h>

namespace osgViewer
{
   class GraphicsWindow;
   class View;
   class CompositeViewer;
   class Window;
   class ViewerBase;
}

namespace dtEntity
{ 

   struct WindowPos
   {
      int mX;
      int mY;
      int mW;
      int mH;
      bool mWindowDeco;
   };

   ////////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* GetWindowByContextId(unsigned int contextId, osgViewer::ViewerBase* v);

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT WindowManager : public osg::Referenced
   {     
   public:

      WindowManager(EntityManager& em);

      /**
       * Opens a new window if used viewer is a composite viewer.
       * @param name Name of osg nodes for window, view and camera to set
       * @param layerName Name of layer attach point to show
       * @param traits OSG GraphicsWindow traits to use
       * @contexId will receive context id of newly created context
       * @return true if success
       */
      virtual bool OpenWindow(const std::string& name,
         dtEntity::StringId layerName, osg::GraphicsContext::Traits& traits, unsigned int& contextId) = 0;

      virtual void CloseWindow(const std::string& name) = 0;

      
      /**
       * Get pick ray at given screen position
	   * @param usePixels if true, use x-y screen coordinates, if false use values from -1 to 1
       */
      virtual osg::Vec3 GetPickRay(const std::string& name, float x, float y, bool usePixels = false) = 0;

      virtual osgViewer::View* GetViewByName(const std::string& name);
      virtual osgViewer::GraphicsWindow* GetWindowByName(const std::string& name);

      InputHandler& GetInputHandler() { return *mInputHandler.get(); }

      virtual void ProcessQueuedMessages() { mMessagePump.EmitQueuedMessages(FLT_MAX); }

      virtual bool GetWindowGeometry(unsigned int contextid, int& x, int& y, int& width, int& height) = 0;
      virtual bool SetWindowGeometry(unsigned int contextid, int x, int y, int width, int height) = 0;

      virtual void SetFullscreen(unsigned int contextid, bool fullscreen) = 0;
      virtual bool GetFullscreen(unsigned int contextid) const = 0;

   protected:

      EntityManager* mEntityManager;
      osg::ref_ptr<InputHandler> mInputHandler;
      MessagePump mMessagePump;
   };	

   ////////////////////////////////////////////////////////////////////////////////
   class EntityManager;

   class DT_ENTITY_EXPORT OSGWindowManager : public WindowManager
   {     
   public:
      OSGWindowManager(EntityManager& em);
      ~OSGWindowManager();

      /**
       * Opens a new window if used viewer is a composite viewer.
       * @param name Name of osg nodes for window, view and camera to set
       * @param layerName Name of layer attach point to show
       * @param traits OSG GraphicsWindow traits to use
       * @contextId receives context id of newly created context
       * @return true if success, else false
       */
      virtual bool OpenWindow(const std::string& name,
         dtEntity::StringId layerName, osg::GraphicsContext::Traits& traits, unsigned int& contextId);

      virtual void CloseWindow(const std::string& name);

      void OnCloseWindow(const Message& msg);

      /**
       * Get pick ray at given screen position
       */
      osg::Vec3 GetPickRay(const std::string& name, float x, float y, bool usePixels = false);
      
      virtual bool GetWindowGeometry(unsigned int contextid, int& x, int& y, int& width, int& height);
      virtual bool SetWindowGeometry(unsigned int contextid, int x, int y, int width, int height);

      virtual void SetFullscreen(unsigned int contextid, bool fullscreen);
      virtual bool GetFullscreen(unsigned int contextid) const;

   protected:

      bool OpenWindowInternal(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits, unsigned int& contextId);

   private:

      MessageFunctor mCloseWindowFunctor;

      typedef std::map<unsigned int, WindowPos> WindowPosMap;
      WindowPosMap mWindowPositions;
      
   };	


}
