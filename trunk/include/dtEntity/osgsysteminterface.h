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
#include <osgViewer/ViewerBase>

namespace dtEntity
{     
   class MessagePump;

   class DT_ENTITY_EXPORT OSGSystemInterface : public SystemInterface
   {
   public:

      OSGSystemInterface(dtEntity::MessagePump&);
      ~OSGSystemInterface();

      void InstallUpdateCallback(osg::Node* node);

      void SetViewer(osgViewer::ViewerBase* v) { mViewer = v; }
      osgViewer::ViewerBase* GetViewer() const { return mViewer.get(); }

      osgViewer::View* GetPrimaryView() const;
      osgViewer::GraphicsWindow* GetPrimaryWindow() const;
      osg::Camera* GetPrimaryCamera() const;

      virtual float GetDeltaSimTime() const;
      virtual float GetDeltaRealTime() const;
      virtual Timer_t GetRealClockTime();
      virtual double GetSimulationTime() const;
      void SetSimulationTime(double);

      virtual float GetTimeScale() const;
      virtual void SetTimeScale(float v);

      virtual Timer_t GetSimulationClockTime() const;
      virtual void SetSimulationClockTime(Timer_t t);

   private:
      osg::observer_ptr<osgViewer::ViewerBase> mViewer;      
      dtEntity::MessagePump* mMessagePump;
      class Impl;
      Impl* mImpl;
   };
  
}
