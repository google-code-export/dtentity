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
       * @param layerName Name of layer attach point to show
       * @param traits OSG GraphicsWindow traits to use
       * @contexId will receive context id of newly created context
       * @return true if success
       */
      virtual bool OpenWindow(const std::string& name,
         dtEntity::StringId layerName, unsigned int& contextId) = 0;

      virtual void CloseWindow(const std::string& name) = 0;

      /**
       * Get pick ray at given screen position
	   * @param usePixels if true, use x-y screen coordinates, if false use values from -1 to 1
       */
      virtual Vec3f GetPickRay(const std::string& name, float x, float y, bool usePixels = false) const = 0;

      virtual bool GetWindowGeometry(unsigned int contextid, int& x, int& y, int& width, int& height) const = 0;
      virtual bool SetWindowGeometry(unsigned int contextid, int x, int y, int width, int height) = 0;

      virtual void SetFullscreen(unsigned int contextid, bool fullscreen) = 0;
      virtual bool GetFullscreen(unsigned int contextid) const = 0;

      virtual EntityId PickEntity(double x, double y, unsigned int nodemask, unsigned int contextid) const = 0;

      virtual void SetShowCursor(bool) = 0;
      virtual Vec3d ConvertWorldToScreenCoords(unsigned int contextid, const Vec3d& coord) = 0;

   };	

}
