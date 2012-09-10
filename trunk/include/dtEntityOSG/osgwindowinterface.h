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

#include <dtEntityOSG/export.h>
#include <dtEntity/message.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/property.h>
#include <dtEntity/windowinterface.h>
#include <osg/GraphicsContext>

namespace dtEntity
{ 
   class EntityManager;
}

namespace dtEntityOSG
{

   class DTENTITY_OSG_EXPORT OSGWindowInterface : public dtEntity::WindowInterface
   {     
   public:

      struct WindowPos
      {
         int mX;
         int mY;
         int mW;
         int mH;
         bool mWindowDeco;
      };

      OSGWindowInterface(dtEntity::EntityManager& em);
      ~OSGWindowInterface();

      /**
       * Opens a new window if used viewer is a composite viewer.
       * @param name Name of osg nodes for window, view and camera to set
       * @param layerName Name of layer attach point to show
       * @param traits OSG GraphicsWindow traits to use
       * @contextId receives context id of newly created context
       * @return true if success, else false
       */
      virtual bool OpenWindow(const std::string& name, dtEntity::StringId layername, unsigned int& contextId);
      
      virtual void CloseWindow(const std::string& name);

      /**
       * Get pick ray at given screen position
       */
      dtEntity::Vec3f GetPickRay(const std::string& name, float x, float y, bool usePixels = false) const;
      
      virtual bool GetWindowGeometry(unsigned int contextid, int& x, int& y, int& width, int& height) const;
      virtual bool SetWindowGeometry(unsigned int contextid, int x, int y, int width, int height);

      virtual void SetFullscreen(unsigned int contextid, bool fullscreen);
      virtual bool GetFullscreen(unsigned int contextid) const;

      virtual dtEntity::EntityId PickEntity(double x, double y, unsigned int nodemask, unsigned int contextid) const;

      void SetTraits(osg::GraphicsContext::Traits* traits) { mTraits = traits; }
      osg::GraphicsContext::Traits* GetTraits() const { return mTraits; }

      virtual void SetShowCursor(bool);

      virtual dtEntity::Vec3d ConvertWorldToScreenCoords(unsigned int contextid, const dtEntity::Vec3d& coord);

   protected:

      bool OpenWindowInternal(const std::string& name, dtEntity::StringId layername, unsigned int& contextId);

      dtEntity::EntityManager* mEntityManager;

      typedef std::map<unsigned int, WindowPos> WindowPosMap;
      WindowPosMap mWindowPositions;
      osg::ref_ptr<osg::GraphicsContext::Traits> mTraits;
      
   };	


}
