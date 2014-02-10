#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <dtEntity/nodemasks.h>

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
      virtual Vec3f GetPickRay(unsigned int contextid, float x, float y, bool usePixels = false) const = 0;

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

      /**
       * Holds intersection info
       */
      struct Intersection
      {
         Vec3d mPosition;
         Vec3f mNormal;
         EntityId mEntityId;
      };

      typedef std::vector<Intersection> Intersections;

      /**
       * Get intersections between start and end and add them to isects vector
       * @param start Start of intersection line
       * @param end End of intersection lnie
       * @param isects Vector return value for isect results
       * @param nodemask Bit mask, used for selecting nodes for intersection
       * @param layer Name of layer attach point component from which intersecition test will
       *              be executed downwards (default: layer "default")
       * @return true if an intersection was found
       */
      virtual bool GetIntersections(const Vec3d& start, const Vec3d& end,
         std::vector<Intersection>& isects,
         unsigned int nodemask = NodeMasks::PICKABLE | NodeMasks::TERRAIN,
          dtEntity::StringId layer = dtEntity::StringId()
      ) const = 0;

   };	

}
