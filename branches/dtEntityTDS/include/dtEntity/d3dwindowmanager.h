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


#include <dtEntity/windowmanager.h>


#include <dtABC/application.h>



namespace dtEntity
{ 

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT D3DWindowManager : public WindowManager
   {     
   public:
      D3DWindowManager(EntityManager& em, dtABC::Application& application);

      virtual ~D3DWindowManager();

	  /**
	   * Get pointer to delta3d application
	   */
      dtABC::Application* GetApplication() const { return mApplication.get(); }

      virtual unsigned int OpenWindow(const std::string& name,
         dtEntity::StringId layerName, osg::GraphicsContext::Traits& traits);

      virtual void CloseWindow(const std::string& name);

      /**
       * Delta3D has no method to get a view from the scene by name.
       * This iterates through all views and returns the right one
       */
      dtCore::View* GetD3DViewByName(const std::string& name);

      dtCore::DeltaWin* GetD3DWindowByName(const std::string& name);

      /**
       * Get pick ray at given screen position
       */
      osg::Vec3 GetPickRay(const std::string& name, float x, float y, bool usePixels = false);

      void OnTimeChange(const dtEntity::Message& msg);

   protected:

      void OpenWindowInternal(const std::string& name, dtEntity::StringId layername, osg::GraphicsContext::Traits& traits);
      osg::ref_ptr<dtABC::Application> mApplication;
      dtEntity::MessageFunctor mTimeChangedFunctor;
   };	
}
