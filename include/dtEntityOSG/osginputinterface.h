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

#include <dtEntity/inputinterface.h>
#include <dtEntityOSG/export.h>
#include <osgGA/GUIEventHandler>
#include <sstream>

namespace dtEntity
{
   class MessagePump;
}

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////////
   // for use with GuiEventAdapter user event
   class MouseEnterLeaveEvent : public osg::Referenced
   {
   public:
      bool mFocused;
   };
   
   ////////////////////////////////////////////////////////////////////////////////
   class DTENTITY_OSG_EXPORT OSGInputInterface
      : public osgGA::GUIEventHandler
      , public dtEntity::InputInterface
   {
   public:
      
      typedef std::map<std::string, osgGA::GUIEventAdapter::KeySymbol> KeyNames;
      typedef std::map<int, std::string> KeyNamesReverse;
   
      OSGInputInterface(dtEntity::MessagePump&);
      virtual ~OSGInputInterface();

      //virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
      virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) ;

      virtual void AddInputCallback(dtEntity::InputCallbackInterface*);
      virtual bool RemoveInputCallback(dtEntity::InputCallbackInterface*);

      /**
       * Stores if current system is multitouch-capable
       */
      virtual bool GetMultiTouchEnabled() const { return mMultiTouchEnabled; }
      virtual void SetMultiTouchEnabled(bool v) { mMultiTouchEnabled = v; }

      virtual bool GetLockCursor() const { return mLockCursor; }
      virtual void SetLockCursor(bool v) { mLockCursor = v; }

      /**
       * Multitouch interface: return number of touch points
       */
      virtual size_t GetNumTouches() const { return mNumTouches; } 
      
      /**
       * Multitouch interface: return touch points
       */
      virtual std::vector<dtEntity::TouchPoint> GetTouches() const { return mTouches; }

      /**
       * Return true if key is pressed, else false
       */
      bool GetKey(const std::string& name, unsigned int contextId = 0) const;
      
      /**
       * Return true if key was released this frame, else false
       */
      virtual bool GetKeyUp(const std::string& name, unsigned int contextId = 0) const;
      
      /**
       * Return true if key was pressed down this frame, else false
       */
      virtual bool GetKeyDown(const std::string& name, unsigned int contextId = 0) const;

      /**
       * Returns true if any keyboard key is currently pressed
       */
      virtual bool AnyKeyDown() const;

      /**
       * Return string entered this frame
       */
      virtual std::string GetInputString() const;

      /**
       * Return true if given mouse button is currently pressed.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButton(int button, unsigned int contextId = 0) const;
      
      /**
       * Return true if given mouse button was pressed this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButtonDown(int button, unsigned int contextId = 0) const;
      
      /**
       * Return true if given mouse button was released this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButtonUp(int button, unsigned int contextId = 0) const;

      /**
       * Return axis value from -1 to 1
       * @TODO: Axes are currently hard coded
       */
      virtual double GetAxis(dtEntity::StringId axisname) const;

      /**
       * Return current mouse wheel state. -1 = down, 1 = up, 0 = no motion
       */
      virtual int GetMouseWheelState(unsigned int contextId = 0) const;

      /**
       * Return a list containing names of all registered keys
       */
      void GetKeyNames(std::map<std::string, unsigned int>&) const;

      /**
       * Get OSG key symbol from key name
       */
      osgGA::GUIEventAdapter::KeySymbol GetKeySymbol(const std::string& name) const;
      
      /**
       * Clear stuff at end of frame
       */
      void Reset();

      /**
       * React to OSG input events
       */
      bool handleInternal(const osgGA::GUIEventAdapter& ea);
      
         
   private:

      void HandleMultiTouch(const osgGA::GUIEventAdapter& ea);
      void HandleKeyUp(const osgGA::GUIEventAdapter& ea);
      void HandleKeyDown(const osgGA::GUIEventAdapter& ea);
      void HandleMouseUp(const osgGA::GUIEventAdapter& ea);
      void HandleMouseDown(const osgGA::GUIEventAdapter& ea);
      void HandleMouseMove(const osgGA::GUIEventAdapter& ea);
      void HandleMouseWheel(const osgGA::GUIEventAdapter& ea);
      void HandleMouseEnterLeave(const osgGA::GUIEventAdapter& ea);

      dtEntity::MessagePump* mMessagePump;
      bool mMultiTouchEnabled;
      bool mLockCursor;
      size_t mNumTouches;
      std::vector<dtEntity::TouchPoint> mTouches;

      typedef std::vector<dtEntity::InputCallbackInterface*> Callbacks;
      Callbacks mCallbacks;
      
      // pair of context id, key
      typedef std::set<std::pair<unsigned int, int> > ContextKeyMap;
      ContextKeyMap mKeyPressedSet;
      ContextKeyMap mKeyUpSet;
      ContextKeyMap mKeyDownSet;

      KeyNames mKeyNames;
      KeyNamesReverse mKeyNamesReverse;

      typedef std::set<std::pair<unsigned int, int> > ContextButtonMap;
      ContextButtonMap mMouseButtonPressed;
      ContextButtonMap mMouseButtonUp;
      ContextButtonMap mMouseButtonDown;

      float mMouseX;
      float mMouseY;

      int mMouseXRaw;
      int mMouseYRaw;
      int mMouseDeltaXRaw;
      int mMouseDeltaYRaw;
      float mMouseDeltaX;
      float mMouseDeltaY;

      unsigned int mFrameNumber;
      int mMouseScroll;
      unsigned int mMouseScrollContext;
      std::ostringstream mInputString;

      bool mNeedReset; // internal flag to check when to reset
   };

}

