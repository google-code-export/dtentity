/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#pragma once

#include <dtEntityWrappers/export.h>
#include <dtEntityWrappers/wrapper.h>
#include <osgGA/GUIEventHandler>
#include <dtEntity/entitymanager.h>

namespace dtEntityWrappers
{   

   ////////////////////////////////////////////////////////////////////////////////
   namespace TouchPhase
   {
      enum e 
      {
         UNKNOWN,
         BEGAN,
         MOVED,
         STATIONERY,
         ENDED
      };

      std::string ToString(e v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct TouchPoint
   {
      unsigned int mId;
      float mX;
      float mY;
      unsigned int mTapCount;
      TouchPhase::e mTouchPhase;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_WRAPPERS_EXPORT InputHandler : public osgGA::GUIEventHandler
   {
      public:
   
         InputHandler(dtEntity::EntityManager& em);
   
         virtual bool handle(const osgGA::GUIEventAdapter& ea, 
                             osgGA::GUIActionAdapter& aa, osg::Object*,
                             osg::NodeVisitor *);
   
         bool GetMultiTouchEnabled() const { return mMultiTouchEnabled; }
         void SetMultiTouchEnabled(bool v) { mMultiTouchEnabled = v; }

         unsigned int GetNumTouches() const { return mNumTouches; } 
         const std::vector<TouchPoint>& GetTouches() const { return mTouches; }

      protected:

         virtual ~InputHandler();
         
   private:

      dtEntity::EntityManager* mEntityManager;
      bool mMultiTouchEnabled;
      unsigned int mNumTouches;
      std::vector<TouchPoint> mTouches;

   };

   class DTENTITY_WRAPPERS_EXPORT InputHandlerWrapper
      : public WrapperBase
      , public dtEntity::Singleton<InputHandlerWrapper>
   {
   public:

      InputHandlerWrapper();
      
   };

   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Object> WrapInputHandler(InputHandler* v);
   DTENTITY_WRAPPERS_EXPORT InputHandler* UnwrapInputHandler(v8::Handle<v8::Value>);
   DTENTITY_WRAPPERS_EXPORT bool IsInputHandler(v8::Handle<v8::Value>);
}
