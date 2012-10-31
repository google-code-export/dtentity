/*
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

#include <dtEntityOSG/osgsysteminterface.h>

#include <dtEntity/entity.h>
#include <dtEntity/messagepump.h>
#include <dtEntity/systemmessages.h>
#include <osg/NodeCallback>
#include <osg/Timer>
#include <osg/FrameStamp>
#include <osg/NodeVisitor>
#include <osgDB/FileUtils>
#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <assert.h>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////////
   class DtEntityUpdateCallback : public osg::NodeCallback
   {

   public:

      dtEntity::Timer_t mStartOfFrameTick;
      dtEntity::Timer_t mSimulationClockTime;
      unsigned int mLastFrameNumber;
      double mSimTime;
      float mDeltaSimTime;
      float mDeltaTime;
      float mTimeScale;
      osg::ref_ptr<const osg::FrameStamp> mLastFrameStamp;


      DtEntityUpdateCallback()
         : mStartOfFrameTick(osg::Timer::instance()->tick())
         , mSimulationClockTime(0)
         , mLastFrameNumber(0)
         , mSimTime(0)
         , mDeltaSimTime(0)
         , mDeltaTime(0)
         , mTimeScale(1)
      {
         time_t t;
         time(&t);
         osg::Timer_t time = t;
         mSimulationClockTime = time / osg::Timer::instance()->getSecondsPerTick();
      }

      void SetSimulationClockTime(osg::Timer_t t) { mSimulationClockTime = t; }
      osg::Timer_t GetSimulationClockTime() const { return mSimulationClockTime; }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         const osg::FrameStamp* fs = nv->getFrameStamp();

         if(fs->getFrameNumber() == mLastFrameNumber)
         {
            return;
         }
         mLastFrameNumber = fs->getFrameNumber();

         mSimTime = fs->getSimulationTime();
         osg::Timer_t lastTick = mStartOfFrameTick;
         mStartOfFrameTick = osg::Timer::instance()->tick();

         mDeltaTime = static_cast<float>(osg::Timer::instance()->delta_s(lastTick, mStartOfFrameTick));
         mDeltaSimTime = mDeltaTime * mTimeScale;

         mSimulationClockTime += mDeltaSimTime;

         mLastFrameStamp = fs;
         traverse(node,nv);
      }

   };

   class OSGSystemInterface::Impl
   {
   public:
	   Impl() 
		   : mUpdateCallback(new DtEntityUpdateCallback())
	   {
	   }
		
	   osg::ref_ptr<DtEntityUpdateCallback> mUpdateCallback;
   };

   ////////////////////////////////////////////////////////////////////////////////
   OSGSystemInterface::OSGSystemInterface(dtEntity::MessagePump& mp)
      : mMessagePump(&mp)
      , mImpl(new Impl())
   {
   }


   ////////////////////////////////////////////////////////////////////////////////
   OSGSystemInterface::~OSGSystemInterface()
   {
      delete mImpl;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::InstallUpdateCallback(osg::Node* node)
   {
      node->setUpdateCallback(mImpl->mUpdateCallback);
   }

   //////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::EmitTickMessagesAndQueuedMessages()
   {
      float deltasimtime = GetDeltaSimTime();
      float deltarealtime = GetDeltaRealTime();
      float simtimescale = GetTimeScale();
      double simulationtime = GetSimulationTime();


      {
         dtEntity::PostFrameMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         mMessagePump->EmitMessage(msg);
      }

      {
         dtEntity::TickMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         mMessagePump->EmitMessage(msg);
      }

      mMessagePump->EmitQueuedMessages(simulationtime);

      {
         dtEntity::EndOfFrameMessage msg;
         msg.SetDeltaSimTime(deltasimtime);
         msg.SetDeltaRealTime(deltarealtime);
         msg.SetSimTimeScale(simtimescale);
         msg.SetSimulationTime(simulationtime);
         mMessagePump->EmitMessage(msg);
      }

   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::View* OSGSystemInterface::GetPrimaryView() const
   {
      osgViewer::ViewerBase::Views views;
      GetViewer()->getViews(views);
      assert(!views.empty());
      return views.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   osgViewer::GraphicsWindow* OSGSystemInterface::GetPrimaryWindow() const
   {
      osgViewer::ViewerBase::Windows wins;
      GetViewer()->getWindows(wins, false);
      assert(!wins.empty());
      return wins.front();
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Camera* OSGSystemInterface::GetPrimaryCamera() const
   {
      osgViewer::ViewerBase::Cameras cams;
      GetViewer()->getCameras(cams);
      if(cams.empty())
      {
         return NULL;
      }
      return cams.front();
   }

   ////////////////////////////////////////////////////////////////////////////////
   float OSGSystemInterface::GetDeltaSimTime() const
   {
      return mImpl->mUpdateCallback->mDeltaSimTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float OSGSystemInterface::GetDeltaRealTime() const
   {
      return mImpl->mUpdateCallback->mDeltaTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float OSGSystemInterface::GetTimeScale() const
   {
      return mImpl->mUpdateCallback->mTimeScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::SetTimeScale(float v)
   {
      mImpl->mUpdateCallback->mTimeScale = v;
      dtEntity::TimeChangedMessage msg;
      msg.SetSimulationTime(GetSimulationTime());
      msg.SetSimulationClockTime(GetSimulationClockTime());
      msg.SetTimeScale(GetTimeScale());
      mMessagePump->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   double OSGSystemInterface::GetSimulationTime() const
   {
      return mImpl->mUpdateCallback->mSimTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::SetSimulationTime(double newTime)
   {
      dtEntity::Timer_t newstarttick = osg::Timer::instance()->tick() - newTime / osg::Timer::instance()->getSecondsPerTick();

      osgViewer::CompositeViewer* cv = dynamic_cast<osgViewer::CompositeViewer*>(GetViewer());
      if(cv)
      {
         cv->setStartTick(newstarttick);
         // calendar time is ignored for now
      }
      else
      {
         osgViewer::Viewer* v = dynamic_cast<osgViewer::Viewer*>(GetViewer());
         if(v != NULL)
         {
            v->setStartTick(newstarttick);
         }
      }

      dtEntity::TimeChangedMessage msg;
      msg.SetSimulationTime(GetSimulationTime());
      msg.SetSimulationClockTime(GetSimulationClockTime());
      msg.SetTimeScale(GetTimeScale());
      mMessagePump->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Timer_t OSGSystemInterface::GetSimulationClockTime() const
   {
      return mImpl->mUpdateCallback->mSimulationClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::SetSimulationClockTime(dtEntity::Timer_t t)
   {
      mImpl->mUpdateCallback->SetSimulationClockTime(t);

      dtEntity::TimeChangedMessage msg;
      msg.SetSimulationTime(GetSimulationTime());
      msg.SetSimulationClockTime(GetSimulationClockTime());
      msg.SetTimeScale(GetTimeScale());
      mMessagePump->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Timer_t OSGSystemInterface::GetRealClockTime()
   {
      return osg::Timer::instance()->tick();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGSystemInterface::GetIntersections(const dtEntity::Vec3d& start, const dtEntity::Vec3d& end, 
         std::vector<dtEntity::SystemInterface::Intersection>& isects, unsigned int nodemask) const
   {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi;
      lsi = new osgUtil::LineSegmentIntersector(start, end);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setUseKdTreeWhenAvailable(true);
      iv.setTraversalMask(nodemask);

      GetPrimaryView()->getSceneData()->accept(iv);
      
      if(!lsi->containsIntersections())
      {
         return false;
      }

      osgUtil::LineSegmentIntersector::Intersections::iterator i;
      for(i = lsi->getIntersections().begin(); i != lsi->getIntersections().end(); ++i)
      {
         osgUtil::LineSegmentIntersector::Intersection isect = *i;
         for(osg::NodePath::const_iterator j = isect.nodePath.begin(); j != isect.nodePath.end(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();
            if(referenced == NULL) continue;
            const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL)
            {
               dtEntity::SystemInterface::Intersection entry;
               entry.mEntityId = entity->GetId();
               entry.mNormal = isect.getWorldIntersectNormal();
               entry.mPosition = isect.getWorldIntersectPoint();
               isects.push_back(entry);
            }
         }
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string OSGSystemInterface::FindDataFile(const std::string& filename)
   {
      return osgDB::findDataFile(filename);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGSystemInterface::FileExists(const std::string& filename)
   {
      return osgDB::fileExists(filename);
   }
}
