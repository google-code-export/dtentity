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

#include <dtEntity/osgsysteminterface.h>

#include <osg/NodeCallback>
#include <osg/Timer>
#include <osg/FrameStamp>
#include <osg/NodeVisitor>
#include <assert.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   class DtEntityUpdateCallback : public osg::NodeCallback
   {

   public:

      Timer_t mStartOfFrameTick;
      Timer_t mSimulationClockTime;
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
   OSGSystemInterface::OSGSystemInterface()
      : mImpl(new Impl())
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
   }

   ////////////////////////////////////////////////////////////////////////////////
   double OSGSystemInterface::GetSimulationTime() const
   {
      return mImpl->mUpdateCallback->mSimTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t OSGSystemInterface::GetSimulationClockTime() const
   {
      return mImpl->mUpdateCallback->mSimulationClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGSystemInterface::SetSimulationClockTime(Timer_t t)
   {
      mImpl->mUpdateCallback->SetSimulationClockTime(t);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t OSGSystemInterface::GetRealClockTime()
   {
      return osg::Timer::instance()->tick();
   }
}
