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
#include <dtEntityOSG/export.h>
#include <osgViewer/ViewerBase>
#include <vector>

namespace dtEntity
{
   class MessagePump;
}

namespace dtEntityOSG
{     
   
   class DTENTITY_OSG_EXPORT OSGSystemInterface : public dtEntity::SystemInterface
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

      virtual void EmitTickMessagesAndQueuedMessages();

      virtual float GetDeltaSimTime() const;
      virtual float GetDeltaRealTime() const;
      virtual dtEntity::Timer_t GetRealClockTime();
      virtual double GetSimulationTime() const;
      void SetSimulationTime(double);

      virtual float GetTimeScale() const;
      virtual void SetTimeScale(float v);

      virtual dtEntity::Timer_t GetSimulationClockTime() const;
      virtual void SetSimulationClockTime(dtEntity::Timer_t t);

      virtual bool GetIntersections(const dtEntity::Vec3d& start, const dtEntity::Vec3d& end, 
         std::vector<dtEntity::SystemInterface::Intersection>& isects,
         unsigned int nodemask = dtEntity::NodeMasks::PICKABLE | dtEntity::NodeMasks::TERRAIN
         ) const;

   private:
      osg::observer_ptr<osgViewer::ViewerBase> mViewer;      
      dtEntity::MessagePump* mMessagePump;
      class Impl;
      Impl* mImpl;
   };
  
}
