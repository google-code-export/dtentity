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

#include <osg/Referenced>
#include <dtEntity/export.h>
#include <dtEntity/entitymanager.h>
#include <osgGA/GUIEventHandler>
#include <osgViewer/GraphicsWindow>
#include <map>
#include <sstream>


namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   namespace TouchPhase
   {
      enum e 
      {
         UNKNOWN,
         BEGAN,
         MOVED,
         STATIONARY,
         ENDED
      };

      std::string ToString(e v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct DT_ENTITY_EXPORT TouchPoint
   {
      unsigned int mId;
      float mX;
      float mY;
      unsigned int mTapCount;
      TouchPhase::e mTouchPhase;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class InputCallbackInterface : public osg::Referenced
   {
   public:
      virtual ~InputCallbackInterface() {}

      virtual bool KeyUp(const std::string& name, bool handled) { return false; }
      virtual bool KeyDown(const std::string& name, bool handled) { return false; }
      virtual bool MouseButtonUp(int button, bool handled) { return false; }
      virtual bool MouseButtonDown(int button, bool handled) { return false; }
      virtual bool MouseWheel(int dir, bool handled) { return false; }
      virtual bool MouseMove(float x, float y, bool handled) { return false; }
      
      virtual void MouseEnterLeave(bool focused, int displaynum, int screennum) {}
   };

   ////////////////////////////////////////////////////////////////////////////////
   // for use with GuiEventAdapter user event
   class MouseEnterLeaveEvent : public osg::Referenced
   {
   public:
      bool mFocused;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT InputHandler : public osg::Node
   {
   public:

      static const StringId MouseXId;
      static const StringId MouseYId;
      static const StringId MouseXRawId;
      static const StringId MouseYRawId;
      static const StringId MouseDeltaXId;
      static const StringId MouseDeltaYId;
      static const StringId MouseDeltaXRawId;
      static const StringId MouseDeltaYRawId;

      typedef std::map<std::string, osgGA::GUIEventAdapter::KeySymbol> KeyNames;
      typedef std::map<int, std::string> KeyNamesReverse;
   
      InputHandler(dtEntity::EntityManager& em);

      virtual void traverse(osg::NodeVisitor& nv);

      void AddInputCallback(InputCallbackInterface*);
      bool RemoveInputCallback(InputCallbackInterface*);

      /**
       * Stores if current system is multitouch-capable
       */
      bool GetMultiTouchEnabled() const { return mMultiTouchEnabled; }
      void SetMultiTouchEnabled(bool v) { mMultiTouchEnabled = v; }

      bool GetLockCursor() const { return mLockCursor; }
      void SetLockCursor(bool v) { mLockCursor = v; }

      /**
       * Multitouch interface: return number of touch points
       */
      unsigned int GetNumTouches() const { return mNumTouches; } 
      
      /**
       * Multitouch interface: return touch points
       */
      const std::vector<TouchPoint>& GetTouches() const { return mTouches; }

      /**
       * Return true if key is pressed, else false
       */
      bool GetKey(const std::string& name) const;
      
      /**
       * Return true if key was released this frame, else false
       */
      bool GetKeyUp(const std::string& name) const;
      
      /**
       * Return true if key was pressed down this frame, else false
       */
      bool GetKeyDown(const std::string& name) const;

      /**
       * Returns true if any keyboard key is currently pressed
       */
      bool AnyKeyDown() const;

      /**
       * Return string entered this frame
       */
      std::string GetInputString() const;

      /**
       * Return true if given mouse button is currently pressed.
       * 0 for left button, 1 for right, 2 for center
       */
      bool GetMouseButton(int button) const;
      
      /**
       * Return true if given mouse button was pressed this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      bool GetMouseButtonDown(int button) const;
      
      /**
       * Return true if given mouse button was released this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      bool GetMouseButtonUp(int button) const;

      /**
       * Return axis value from -1 to 1
       * @TODO: Axes are currently hard coded
       */
      double GetAxis(StringId axisname) const;

      /**
       * Return current mouse wheel state
       */
      osgGA::GUIEventAdapter::ScrollingMotion GetMouseWheelState() const { return mMouseScroll; }

      /**
       * Return a list containing names of all registered keys
       */
      const KeyNames& GetKeyNames() const { return mKeyNames; }

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
      virtual bool handle(const osgGA::GUIEventAdapter& ea, 
                          osgGA::GUIActionAdapter& aa, 
                          osg::NodeVisitor *);

   protected:

      virtual ~InputHandler();
         
   private:

      void HandleMultiTouch(const osgGA::GUIEventAdapter& ea);
      void HandleKeyUp(const osgGA::GUIEventAdapter& ea);
      void HandleKeyDown(const osgGA::GUIEventAdapter& ea);
      void HandleMouseUp(const osgGA::GUIEventAdapter& ea);
      void HandleMouseDown(const osgGA::GUIEventAdapter& ea);
      void HandleMouseMove(const osgGA::GUIEventAdapter& ea);
      void HandleMouseWheel(const osgGA::GUIEventAdapter& ea);
      void HandleMouseEnterLeave(const osgGA::GUIEventAdapter& ea);

      dtEntity::EntityManager* mEntityManager;
      bool mMultiTouchEnabled;
      bool mLockCursor;
      unsigned int mNumTouches;
      std::vector<TouchPoint> mTouches;

      typedef std::vector<osg::ref_ptr<InputCallbackInterface> > Callbacks;
      Callbacks mCallbacks;
      
      std::set<int> mKeyPressedSet;
      std::set<int> mKeyUpSet;
      std::set<int> mKeyDownSet;

      KeyNames mKeyNames;
      KeyNamesReverse mKeyNamesReverse;

      std::vector<bool> mMouseButtonPressed;
      std::vector<bool> mMouseButtonUp;
      std::vector<bool> mMouseButtonDown;

      float mMouseX;
      float mMouseY;

      int mMouseXRaw;
      int mMouseYRaw;
      int mMouseDeltaXRaw;
      int mMouseDeltaYRaw;
      float mMouseDeltaX;
      float mMouseDeltaY;

      osgGA::GUIEventAdapter::ScrollingMotion mMouseScroll;
      std::ostringstream mInputString;
      unsigned int mFrameNumber;
   };

}

