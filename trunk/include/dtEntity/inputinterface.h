#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <dtEntity/export.h>
#include <dtEntity/stringid.h>
#include <map>
#include <vector>

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
   class InputCallbackInterface
   {
   public:
      virtual ~InputCallbackInterface() {}

      virtual bool KeyUp(const std::string& name, bool handled, unsigned int contextid) { return false; }
      virtual bool KeyDown(const std::string& name, bool handled, unsigned int contextid) { return false; }
      virtual bool MouseButtonUp(int button, bool handled, unsigned int contextid) { return false; }
      virtual bool MouseButtonDown(int button, bool handled, unsigned int contextid) { return false; }
      virtual bool MouseWheel(int dir, bool handled, unsigned int contextid) { return false; }
      virtual bool MouseMove(float x, float y, bool handled, unsigned int contextid) { return false; }
      
      virtual void MouseEnterLeave(bool focused, unsigned int contextid) {}
   };

   ////////////////////////////////////////////////////////////////////////////////   
   class DT_ENTITY_EXPORT InputInterface
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

      virtual ~InputInterface() {}

      /**
       * Stores if current system is multitouch-capable
       */
      virtual bool GetMultiTouchEnabled() const = 0;
      virtual void SetMultiTouchEnabled(bool v) = 0;

      /** get or set if mouse cursor should be locked in 
        * center of screen (for FPS style steering)
        */
      virtual bool GetLockCursor() const = 0;
      virtual void SetLockCursor(bool v) = 0;

      /**
       * Multitouch interface: return number of touch points
       */
      virtual size_t GetNumTouches() const = 0;
      
      /**
       * Multitouch interface: return touch points
       */
      virtual std::vector<TouchPoint> GetTouches() const = 0;

      /**
       * Return true if key is pressed, else false
       */
      virtual bool GetKey(const std::string& name, unsigned int contextId = 0) const = 0;
      
      /**
       * Return true if key was released this frame, else false
       */
      virtual bool GetKeyUp(const std::string& name, unsigned int contextId = 0) const = 0;
      
      /**
       * Return true if key was pressed down this frame, else false
       */
      virtual bool GetKeyDown(const std::string& name, unsigned int contextId = 0) const = 0;

      /**
       * Returns true if any keyboard key is currently pressed
       */
      virtual bool AnyKeyDown() const = 0;

      /**
       * Return string entered this frame
       */
      virtual std::string GetInputString() const = 0;

      /**
       * Return true if given mouse button is currently pressed.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButton(int button, unsigned int contextId = 0) const = 0;
      
      /**
       * Return true if given mouse button was pressed this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButtonDown(int button, unsigned int contextId = 0) const = 0;
      
      /**
       * Return true if given mouse button was released this frame.
       * 0 for left button, 1 for right, 2 for center
       */
      virtual bool GetMouseButtonUp(int button, unsigned int contextId = 0) const = 0;

      /**
       * Return axis value from -1 to 1
       * @TODO: Axes are currently hard coded
       */
      virtual double GetAxis(StringId axisname) const = 0;

      /**
       * Return current mouse wheel state. -1 = down, 1 = up, 0 = no motion
       */
      virtual int GetMouseWheelState(unsigned int contextId = 0) const = 0;

      virtual void AddInputCallback(InputCallbackInterface*) = 0;
      virtual bool RemoveInputCallback(InputCallbackInterface*) = 0;

      /**
       * Return a list containing names of all registered keys
       */
      virtual void GetKeyNames(std::map<std::string, unsigned int>&) const = 0;

   };

}

