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
#include <dtEntity/osgcomponents.h>
#include <dtEntity/component.h>
#include <dtEntity/inputhandler.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntity/stringid.h>

namespace osgViewer
{
   class View;
   class CompositeViewer;
   class Window;
}

namespace dtCore
{
   class View;
   class DeltaWin;
}

namespace dtEntity
{ 

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT WindowManager : public osg::Referenced
   {     
   public:
      WindowManager(EntityManager& em)
         : mEntityManager(&em)
         , mInputHandler(new InputHandler(em))

      {

      }

      /**
       * Opens a new window if used viewer is a composite viewer.
       * @param name Name of osg nodes for window, view and camera to set
       * @param layerName Name of layer attach point to show
       * @param traits OSG GraphicsWindow traits to use
       * @return context id of newly created context
       */
      virtual unsigned int OpenWindow(const std::string& name,
         dtEntity::StringId layerName, osg::GraphicsContext::Traits& traits) = 0;

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
       * @return context id of newly created context
       */
      virtual unsigned int OpenWindow(const std::string& name,
         dtEntity::StringId layerName, osg::GraphicsContext::Traits& traits);

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

      struct WindowPos
      {
         int mX;
         int mY;
         int mW;
         int mH;
         bool mWindowDeco;
      };

      typedef std::map<unsigned int, WindowPos> WindowPosMap;
      WindowPosMap mWindowPositions;
      
   };	


}
