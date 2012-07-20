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

#include <dtEntity/systeminterface.h>
#include <dtEntity/windowmanager.h>
#include <osgViewer/ViewerBase>

namespace dtEntity
{     

   class DtEntityUpdateCallback;

   class OSGSystemInterface : public SystemInterface
   {
   public:
      OSGSystemInterface();

      void InstallUpdateCallback(osg::Node* node);

      void SetWindowManager(WindowManager* wm) { mWindowManager = wm; }
      WindowManager* GetWindowManager() { return mWindowManager; }

      void SetViewer(osgViewer::ViewerBase* v) { mViewer = v; }
      osgViewer::ViewerBase* GetViewer() { return mViewer.get(); }

      virtual float GetDeltaSimTime() const;
      virtual float GetDeltaRealTime() const;
      virtual Timer_t GetRealClockTime();
      virtual double GetSimulationTime() const;

      virtual float GetTimeScale() const;
      virtual void SetTimeScale(float v);


      virtual Timer_t GetSimulationClockTime() const;
      virtual void SetSimulationClockTime(Timer_t t);


   private:
      osg::observer_ptr<osgViewer::ViewerBase> mViewer;
      osg::ref_ptr<DtEntityUpdateCallback> mUpdateCallback;
      osg::ref_ptr<WindowManager> mWindowManager;
   };
  
}
