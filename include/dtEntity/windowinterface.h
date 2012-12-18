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

#include <dtEntity/property.h>

namespace dtEntity
{ 
   ////////////////////////////////////////////////////////////////////////////////
   class WindowInterface
   {     
   public:

      virtual ~WindowInterface() {}

      /**
       * Opens a new window if used viewer is a composite viewer.
       * @param name Name of osg nodes for window, view and camera to set
       * @param traits OSG GraphicsWindow traits to use
       * @contexId will receive context id of newly created context
       * @return true if success
       */
      virtual bool OpenWindow(const std::string& name, unsigned int& contextId) = 0;

      virtual void CloseWindow(unsigned int contextid) = 0;

      /**
       * Get pick ray at given screen position
	   * @param usePixels if true, use x-y screen coordinates, if false use values from -1 to 1
       */
      virtual Vec3f GetPickRay(unsigned int contextid, float x, float y, bool usePixels = true) const = 0;

      /**
        Get window size and position of window with given context id
        */
      virtual bool GetWindowGeometry(unsigned int contextid, int& x, int& y, int& width, int& height) const = 0;

      /**
        Set window size and position of window with given context id
        */
      virtual bool SetWindowGeometry(unsigned int contextid, int x, int y, int width, int height) = 0;

      /**
        Should window be displayed at full screen size?
        */
      virtual void SetFullscreen(unsigned int contextid, bool fullscreen) = 0;
      virtual bool GetFullscreen(unsigned int contextid) const = 0;

      /**
        * Return entity id of first intersected entity.
        */
      virtual EntityId PickEntity(double x, double y, unsigned int nodemask,
                                  unsigned int contextid, bool usePixels = true) const = 0;

      /**
        * Show or hide cursor when it is above this window?
        */
      virtual void SetShowCursor(unsigned int contextid, bool) = 0;

      /**
        * project world space point into screen coordinates
        */
      virtual Vec3d ConvertWorldToScreenCoords(unsigned int contextid, const Vec3d& coord) = 0;

      /**
        * Should the window with given context id be continuously redrawn
        * or only by request? Default is on
        */
      virtual void SetContinuousRedraw(unsigned int contextid, bool) = 0;

      /**
        * If Continuous Redraw is off, request a redraw in next render traversal
        */
      virtual void RequestRedraw(unsigned int contextid) = 0;

   };	

}
