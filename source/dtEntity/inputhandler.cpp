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

#include <dtEntity/inputhandler.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/log.h>
#include <osg/Version>
#include <osgViewer/CompositeViewer>
#include <iostream>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   std::string TouchPhase::ToString(e v)
   {
      switch(v)
      {
         case BEGAN:       return "BEGAN";
         case MOVED:       return "MOVED";
         case STATIONARY:  return "STATIONARY";
         case ENDED:       return "ENDED";
         default:          return "UNKNOWN";
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   const StringId InputHandler::MouseXId(SID("MouseX"));  
   const StringId InputHandler::MouseYId(SID("MouseY"));  
   const StringId InputHandler::MouseXRawId(SID("MouseXRaw"));
   const StringId InputHandler::MouseYRawId(SID("MouseYRaw"));
   const StringId InputHandler::MouseDeltaXId(SID("MouseDeltaX"));
   const StringId InputHandler::MouseDeltaYId(SID("MouseDeltaY"));
   const StringId InputHandler::MouseDeltaXRawId(SID("MouseDeltaXRaw"));
   const StringId InputHandler::MouseDeltaYRawId(SID("MouseDeltaYRaw"));

   ////////////////////////////////////////////////////////////////////////////////
   InputHandler::InputHandler(dtEntity::EntityManager& em)
      : mMultiTouchEnabled(false)
      , mLockCursor(false)
      , mNumTouches(0)
      , mFrameNumber(0)
   {
      //setNumChildrenRequiringEventTraversal(getNumChildrenRequiringEventTraversal() + 1);

      mKeyNames["0"] = osgGA::GUIEventAdapter::KEY_0;
      mKeyNames["1"] = osgGA::GUIEventAdapter::KEY_1;
      mKeyNames["2"] = osgGA::GUIEventAdapter::KEY_2;
      mKeyNames["3"] = osgGA::GUIEventAdapter::KEY_3;
      mKeyNames["4"] = osgGA::GUIEventAdapter::KEY_4;
      mKeyNames["5"] = osgGA::GUIEventAdapter::KEY_5;
      mKeyNames["6"] = osgGA::GUIEventAdapter::KEY_6;
      mKeyNames["7"] = osgGA::GUIEventAdapter::KEY_7;
      mKeyNames["8"] = osgGA::GUIEventAdapter::KEY_8;
      mKeyNames["9"] = osgGA::GUIEventAdapter::KEY_9;

      mKeyNames["a"] = osgGA::GUIEventAdapter::KEY_A;
      mKeyNames["b"] = osgGA::GUIEventAdapter::KEY_B;
      mKeyNames["c"] = osgGA::GUIEventAdapter::KEY_C;
      mKeyNames["d"] = osgGA::GUIEventAdapter::KEY_D;
      mKeyNames["e"] = osgGA::GUIEventAdapter::KEY_E;
      mKeyNames["f"] = osgGA::GUIEventAdapter::KEY_F;
      mKeyNames["g"] = osgGA::GUIEventAdapter::KEY_G;
      mKeyNames["h"] = osgGA::GUIEventAdapter::KEY_H;
      mKeyNames["i"] = osgGA::GUIEventAdapter::KEY_I;
      mKeyNames["j"] = osgGA::GUIEventAdapter::KEY_J;
      mKeyNames["k"] = osgGA::GUIEventAdapter::KEY_K;
      mKeyNames["l"] = osgGA::GUIEventAdapter::KEY_L;
      mKeyNames["m"] = osgGA::GUIEventAdapter::KEY_M;
      mKeyNames["n"] = osgGA::GUIEventAdapter::KEY_N;
      mKeyNames["o"] = osgGA::GUIEventAdapter::KEY_O;
      mKeyNames["p"] = osgGA::GUIEventAdapter::KEY_P;
      mKeyNames["q"] = osgGA::GUIEventAdapter::KEY_Q;
      mKeyNames["r"] = osgGA::GUIEventAdapter::KEY_R;
      mKeyNames["s"] = osgGA::GUIEventAdapter::KEY_S;
      mKeyNames["t"] = osgGA::GUIEventAdapter::KEY_T;
      mKeyNames["u"] = osgGA::GUIEventAdapter::KEY_U;
      mKeyNames["v"] = osgGA::GUIEventAdapter::KEY_V;
      mKeyNames["w"] = osgGA::GUIEventAdapter::KEY_W;
      mKeyNames["x"] = osgGA::GUIEventAdapter::KEY_X;
      mKeyNames["y"] = osgGA::GUIEventAdapter::KEY_Y;
      mKeyNames["z"] = osgGA::GUIEventAdapter::KEY_Z;
      mKeyNames["Exclaim"] = osgGA::GUIEventAdapter::KEY_Exclaim;
      mKeyNames["Quotedbl"] = osgGA::GUIEventAdapter::KEY_Quotedbl;
      mKeyNames["Hash"] = osgGA::GUIEventAdapter::KEY_Hash;
      mKeyNames["Dollar"] = osgGA::GUIEventAdapter::KEY_Dollar;
      mKeyNames["Ampersand"] = osgGA::GUIEventAdapter::KEY_Ampersand;
      mKeyNames["Quote"] = osgGA::GUIEventAdapter::KEY_Quote;
      mKeyNames["Leftparen"] = osgGA::GUIEventAdapter::KEY_Leftparen;
      mKeyNames["Rightparen"] = osgGA::GUIEventAdapter::KEY_Rightparen;
      mKeyNames["Asterisk"] = osgGA::GUIEventAdapter::KEY_Asterisk;
      mKeyNames["Plus"] = osgGA::GUIEventAdapter::KEY_Plus;
      mKeyNames["Comma"] = osgGA::GUIEventAdapter::KEY_Comma;
      mKeyNames["Minus"] = osgGA::GUIEventAdapter::KEY_Minus;
      mKeyNames["Period"] = osgGA::GUIEventAdapter::KEY_Period;
      mKeyNames["Slash"] = osgGA::GUIEventAdapter::KEY_Slash;
      mKeyNames["Colon"] = osgGA::GUIEventAdapter::KEY_Colon;
      mKeyNames["Semicolon"] = osgGA::GUIEventAdapter::KEY_Semicolon;
      mKeyNames["Less"] = osgGA::GUIEventAdapter::KEY_Less;
      mKeyNames["Equals"] = osgGA::GUIEventAdapter::KEY_Equals;
      mKeyNames["Greater"] = osgGA::GUIEventAdapter::KEY_Greater;
      mKeyNames["Question"] = osgGA::GUIEventAdapter::KEY_Question;
      mKeyNames["At"] = osgGA::GUIEventAdapter::KEY_At;
      mKeyNames["Leftbracket"] = osgGA::GUIEventAdapter::KEY_Leftbracket;
      mKeyNames["Backslash"] = osgGA::GUIEventAdapter::KEY_Backslash;
      mKeyNames["Rightbracket"] = osgGA::GUIEventAdapter::KEY_Rightbracket;
      mKeyNames["Caret"] = osgGA::GUIEventAdapter::KEY_Caret;
      mKeyNames["Underscore"] = osgGA::GUIEventAdapter::KEY_Underscore;
      mKeyNames["Backquote"] = osgGA::GUIEventAdapter::KEY_Backquote;

      mKeyNames["Space"] = osgGA::GUIEventAdapter::KEY_Space;    
      mKeyNames["BackSpace"] = osgGA::GUIEventAdapter::KEY_BackSpace;    
      mKeyNames["Tab"] = osgGA::GUIEventAdapter::KEY_Tab;          
      mKeyNames["Linefeed"] = osgGA::GUIEventAdapter::KEY_Linefeed;     
      mKeyNames["Clear"] = osgGA::GUIEventAdapter::KEY_Clear;        
      mKeyNames["Return"] = osgGA::GUIEventAdapter::KEY_Return;       
      mKeyNames["Pause"] = osgGA::GUIEventAdapter::KEY_Pause;        
      mKeyNames["Scroll_Lock"] = osgGA::GUIEventAdapter::KEY_Scroll_Lock;  
      mKeyNames["Sys_Req"] = osgGA::GUIEventAdapter::KEY_Sys_Req;      
      mKeyNames["Escape"] = osgGA::GUIEventAdapter::KEY_Escape;       
      mKeyNames["Delete"] = osgGA::GUIEventAdapter::KEY_Delete;       

      mKeyNames["Home"] = osgGA::GUIEventAdapter::KEY_Home;  
      mKeyNames["Left"] = osgGA::GUIEventAdapter::KEY_Left;     
      mKeyNames["Up"] = osgGA::GUIEventAdapter::KEY_Up;       
      mKeyNames["Right"] = osgGA::GUIEventAdapter::KEY_Right;    
      mKeyNames["Down"] = osgGA::GUIEventAdapter::KEY_Down;     
      mKeyNames["Prior"] = osgGA::GUIEventAdapter::KEY_Prior;    
      mKeyNames["Page_Up"] = osgGA::GUIEventAdapter::KEY_Page_Up;  
      mKeyNames["Next"] = osgGA::GUIEventAdapter::KEY_Next;     
      mKeyNames["Page_Down"] = osgGA::GUIEventAdapter::KEY_Page_Down;
      mKeyNames["End"] = osgGA::GUIEventAdapter::KEY_End;      
      mKeyNames["Begin"] = osgGA::GUIEventAdapter::KEY_Begin;   

      mKeyNames["Select"] = osgGA::GUIEventAdapter::KEY_Select;       
      mKeyNames["Print"] = osgGA::GUIEventAdapter::KEY_Print;        
      mKeyNames["Execute"] = osgGA::GUIEventAdapter::KEY_Execute;      
      mKeyNames["Insert"] = osgGA::GUIEventAdapter::KEY_Insert;       
      mKeyNames["Undo"] = osgGA::GUIEventAdapter::KEY_Undo;         
      mKeyNames["Redo"] = osgGA::GUIEventAdapter::KEY_Redo;         
      mKeyNames["Menu"] = osgGA::GUIEventAdapter::KEY_Menu;         
      mKeyNames["Find"] = osgGA::GUIEventAdapter::KEY_Find;         
      mKeyNames["Cancel"] = osgGA::GUIEventAdapter::KEY_Cancel;       
      mKeyNames["Help"] = osgGA::GUIEventAdapter::KEY_Help;         
      mKeyNames["Break"] = osgGA::GUIEventAdapter::KEY_Break;        
      mKeyNames["Mode_switch"] = osgGA::GUIEventAdapter::KEY_Mode_switch;  
      mKeyNames["Script_switch"] = osgGA::GUIEventAdapter::KEY_Script_switch;
      mKeyNames["Num_Lock"] = osgGA::GUIEventAdapter::KEY_Num_Lock;     

      mKeyNames["KP_Space"] = osgGA::GUIEventAdapter::KEY_KP_Space;    
      mKeyNames["KP_Tab"] = osgGA::GUIEventAdapter::KEY_KP_Tab;      
      mKeyNames["KP_Enter"] = osgGA::GUIEventAdapter::KEY_KP_Enter;    
      mKeyNames["KP_F1"] = osgGA::GUIEventAdapter::KEY_KP_F1;       
      mKeyNames["KP_F2"] = osgGA::GUIEventAdapter::KEY_KP_F2;       
      mKeyNames["KP_F3"] = osgGA::GUIEventAdapter::KEY_KP_F3;       
      mKeyNames["KP_F4"] = osgGA::GUIEventAdapter::KEY_KP_F4;       
      mKeyNames["KP_Home"] = osgGA::GUIEventAdapter::KEY_KP_Home;     
      mKeyNames["KP_Left"] = osgGA::GUIEventAdapter::KEY_KP_Left;     
      mKeyNames["KP_Up"] = osgGA::GUIEventAdapter::KEY_KP_Up;       
      mKeyNames["KP_Right"] = osgGA::GUIEventAdapter::KEY_KP_Right;    
      mKeyNames["KP_Down"] = osgGA::GUIEventAdapter::KEY_KP_Down;     
      mKeyNames["KP_Prior"] = osgGA::GUIEventAdapter::KEY_KP_Prior;    
      mKeyNames["KP_Page_Up"] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;  
      mKeyNames["KP_Next"] = osgGA::GUIEventAdapter::KEY_KP_Next;     
      mKeyNames["KP_Page_Down"] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
      mKeyNames["KP_End"] = osgGA::GUIEventAdapter::KEY_KP_End;      
      mKeyNames["KP_Begin"] = osgGA::GUIEventAdapter::KEY_KP_Begin;    
      mKeyNames["KP_Insert"] = osgGA::GUIEventAdapter::KEY_KP_Insert;   
      mKeyNames["KP_Delete"] = osgGA::GUIEventAdapter::KEY_KP_Delete;   
      mKeyNames["KP_Equal"] = osgGA::GUIEventAdapter::KEY_KP_Equal;    
      mKeyNames["KP_Multiply"] = osgGA::GUIEventAdapter::KEY_KP_Multiply; 
      mKeyNames["KP_Add"] = osgGA::GUIEventAdapter::KEY_KP_Add;      
      mKeyNames["KP_Separator"] = osgGA::GUIEventAdapter::KEY_KP_Separator;
      mKeyNames["KP_Subtract"] = osgGA::GUIEventAdapter::KEY_KP_Subtract; 
      mKeyNames["KP_Decimal"] = osgGA::GUIEventAdapter::KEY_KP_Decimal;  
      mKeyNames["KP_Divide"] = osgGA::GUIEventAdapter::KEY_KP_Divide;   

      mKeyNames["KP_0"] = osgGA::GUIEventAdapter::KEY_KP_0;
      mKeyNames["KP_1"] = osgGA::GUIEventAdapter::KEY_KP_1;
      mKeyNames["KP_2"] = osgGA::GUIEventAdapter::KEY_KP_2;
      mKeyNames["KP_3"] = osgGA::GUIEventAdapter::KEY_KP_3;
      mKeyNames["KP_4"] = osgGA::GUIEventAdapter::KEY_KP_4;
      mKeyNames["KP_5"] = osgGA::GUIEventAdapter::KEY_KP_5;
      mKeyNames["KP_6"] = osgGA::GUIEventAdapter::KEY_KP_6;
      mKeyNames["KP_7"] = osgGA::GUIEventAdapter::KEY_KP_7;
      mKeyNames["KP_8"] = osgGA::GUIEventAdapter::KEY_KP_8;
      mKeyNames["KP_9"] = osgGA::GUIEventAdapter::KEY_KP_9;

      mKeyNames["F1"] = osgGA::GUIEventAdapter::KEY_F1;
      mKeyNames["F2"] = osgGA::GUIEventAdapter::KEY_F2;
      mKeyNames["F3"] = osgGA::GUIEventAdapter::KEY_F3;
      mKeyNames["F4"] = osgGA::GUIEventAdapter::KEY_F4;
      mKeyNames["F5"] = osgGA::GUIEventAdapter::KEY_F5;
      mKeyNames["F6"] = osgGA::GUIEventAdapter::KEY_F6;
      mKeyNames["F7"] = osgGA::GUIEventAdapter::KEY_F7;
      mKeyNames["F8"] = osgGA::GUIEventAdapter::KEY_F8;
      mKeyNames["F9"] = osgGA::GUIEventAdapter::KEY_F9;
      mKeyNames["F10"] = osgGA::GUIEventAdapter::KEY_F10;
      mKeyNames["F11"] = osgGA::GUIEventAdapter::KEY_F11;
      mKeyNames["F12"] = osgGA::GUIEventAdapter::KEY_F12;
      mKeyNames["F13"] = osgGA::GUIEventAdapter::KEY_F13;
      mKeyNames["F14"] = osgGA::GUIEventAdapter::KEY_F14;
      mKeyNames["F15"] = osgGA::GUIEventAdapter::KEY_F15;
      mKeyNames["F16"] = osgGA::GUIEventAdapter::KEY_F16;
      mKeyNames["F17"] = osgGA::GUIEventAdapter::KEY_F17;
      mKeyNames["F18"] = osgGA::GUIEventAdapter::KEY_F18;
      mKeyNames["F19"] = osgGA::GUIEventAdapter::KEY_F19;
      mKeyNames["F20"] = osgGA::GUIEventAdapter::KEY_F20;
      mKeyNames["F21"] = osgGA::GUIEventAdapter::KEY_F21;
      mKeyNames["F22"] = osgGA::GUIEventAdapter::KEY_F22;
      mKeyNames["F23"] = osgGA::GUIEventAdapter::KEY_F23;
      mKeyNames["F24"] = osgGA::GUIEventAdapter::KEY_F24;
      mKeyNames["F25"] = osgGA::GUIEventAdapter::KEY_F25;
      mKeyNames["F26"] = osgGA::GUIEventAdapter::KEY_F26;
      mKeyNames["F27"] = osgGA::GUIEventAdapter::KEY_F27;
      mKeyNames["F28"] = osgGA::GUIEventAdapter::KEY_F28;
      mKeyNames["F29"] = osgGA::GUIEventAdapter::KEY_F29;
      mKeyNames["F30"] = osgGA::GUIEventAdapter::KEY_F30;
      mKeyNames["F31"] = osgGA::GUIEventAdapter::KEY_F31;
      mKeyNames["F32"] = osgGA::GUIEventAdapter::KEY_F32;
      mKeyNames["F33"] = osgGA::GUIEventAdapter::KEY_F33;
      mKeyNames["F34"] = osgGA::GUIEventAdapter::KEY_F34;
      mKeyNames["F35"] = osgGA::GUIEventAdapter::KEY_F35;

      mKeyNames["Shift_L"] = osgGA::GUIEventAdapter::KEY_Shift_L;   
      mKeyNames["Shift_R"] = osgGA::GUIEventAdapter::KEY_Shift_R;   
      mKeyNames["Control_L"] = osgGA::GUIEventAdapter::KEY_Control_L;
      mKeyNames["Control_R"] = osgGA::GUIEventAdapter::KEY_Control_R; 
      mKeyNames["Caps_Lock"] = osgGA::GUIEventAdapter::KEY_Caps_Lock; 
      mKeyNames["Shift_Lock"] = osgGA::GUIEventAdapter::KEY_Shift_Lock;

      mKeyNames["Meta_L"] = osgGA::GUIEventAdapter::KEY_Meta_L;    
      mKeyNames["Meta_R"] = osgGA::GUIEventAdapter::KEY_Meta_R;    
      mKeyNames["Alt_L"] = osgGA::GUIEventAdapter::KEY_Alt_L;     
      mKeyNames["Alt_R"] = osgGA::GUIEventAdapter::KEY_Alt_R;     
      mKeyNames["Super_L"] = osgGA::GUIEventAdapter::KEY_Super_L;   
      mKeyNames["Super_R"] = osgGA::GUIEventAdapter::KEY_Super_R;   
      mKeyNames["Hyper_L"] = osgGA::GUIEventAdapter::KEY_Hyper_L;   
      mKeyNames["Hyper_R"] = osgGA::GUIEventAdapter::KEY_Hyper_R;       

      for(KeyNames::const_iterator i = mKeyNames.begin(); i != mKeyNames.end(); ++i)
      {
         mKeyNamesReverse[i->second] = i->first;
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   InputHandler::~InputHandler()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
       // note, callback is responsible for scenegraph traversal so
       // they must call traverse(node,nv) to ensure that the
       // scene graph subtree (and associated callbacks) are traversed.
       traverse(node,nv);
       if(nv->getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
       {
          osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(nv);
          for(osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
              itr != ev->getEvents().end();
              ++itr)
          {
             osgGA::GUIEventAdapter* ea = itr->get();
             if (handle(*ea, *(ev->getActionAdapter()), nv))
             {
                ea->setHandled(true);
             }
          }
       }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::AddInputCallback(InputCallbackInterface* cb)
   {
      mCallbacks.push_back(cb);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::RemoveInputCallback(InputCallbackInterface* cb)
   {
      for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
      {
         if(*i == cb)
         {
            mCallbacks.erase(i);
            return true;
         }         
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::handle(const osgGA::GUIEventAdapter& ea, 
                             osgGA::GUIActionAdapter& aa, 
                             osg::NodeVisitor* nv)
   {

      unsigned int fn = nv->getFrameStamp()->getFrameNumber();
      if(fn != mFrameNumber)
      {
         Reset();
         mFrameNumber = fn;
      }

      switch (ea.getEventType() )
      {
         case osgGA::GUIEventAdapter::PUSH:
         {
            HandleMouseDown(ea);

            if(ea.isMultiTouchEvent())
            {
               HandleMultiTouch(ea);
            }

            break;
         }
         case osgGA::GUIEventAdapter::RELEASE:
         {
            HandleMouseUp(ea);
            if(ea.isMultiTouchEvent())
            {
               HandleMultiTouch(ea);
            }

            break;
         }
         case osgGA::GUIEventAdapter::DRAG:
         case osgGA::GUIEventAdapter::MOVE:
         {
            HandleMouseMove(ea);
            if(ea.isMultiTouchEvent())
            {
               HandleMultiTouch(ea);
            }

            break;
         }
         case osgGA::GUIEventAdapter::KEYDOWN:
         {
            HandleKeyDown(ea);
            break;
         }
         case osgGA::GUIEventAdapter::KEYUP:
         {
            HandleKeyUp(ea);
            break;
         }
         case osgGA::GUIEventAdapter::SCROLL:
         {
            HandleMouseWheel(ea);
            break;
         }
         case osgGA::GUIEventAdapter::USER:
         {
            const MouseEnterLeaveEvent* e = dynamic_cast<const MouseEnterLeaveEvent*>(ea.getUserData());
            if(e)
            {
               HandleMouseEnterLeave(ea);
            }
            break;
         }
         default: break;
      }
      return false;
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::Reset()
   {
      mKeyUpSet.clear();
      mKeyDownSet.clear();
      mMouseButtonUp.clear();
      mMouseButtonDown.clear();
      mMouseDeltaX = 0;
      mMouseDeltaY = 0;
      mMouseDeltaXRaw = 0;
      mMouseDeltaYRaw = 0;
      mMouseScroll = osgGA::GUIEventAdapter::SCROLL_NONE;
      mInputString.str("");
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleKeyUp(const osgGA::GUIEventAdapter& ea)
   {
      int key = ea.getUnmodifiedKey();
      unsigned int contextId = ea.getGraphicsContext()->getState()->getContextID();

      std::pair<unsigned int, int> pair(contextId, key);
      mKeyUpSet.insert(pair);
      mKeyPressedSet.erase(pair);

     if(!mCallbacks.empty())
      {
         std::string v = mKeyNamesReverse[key];
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->KeyUp(v, handled, contextId);
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleKeyDown(const osgGA::GUIEventAdapter& ea)
   {
      int key = ea.getUnmodifiedKey();
      unsigned int contextId = ea.getGraphicsContext()->getState()->getContextID();

      std::pair<unsigned int, int> pair(contextId, key);

      mKeyDownSet.insert(pair);
      mKeyPressedSet.insert(pair);
      mInputString << mKeyNamesReverse[key];
      if(!mCallbacks.empty())
      {
         std::string v = mKeyNamesReverse[key];
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->KeyDown(v, handled, contextId);
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMouseUp(const osgGA::GUIEventAdapter& ea)
   {
      unsigned int contextId = ea.getGraphicsContext()->getState()->getContextID();

      int index;
      switch(ea.getButton())
      {
         case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON: index = 0; break;
         case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON: index = 2; break;
         case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON: index = 1; break;
      }

      std::pair<unsigned int, int> pair(contextId, index);
      mMouseButtonPressed.erase(pair);
      mMouseButtonUp.insert(pair);

      if(!mCallbacks.empty())
      {
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->MouseButtonUp(index, handled, contextId);
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMouseDown(const osgGA::GUIEventAdapter& ea)
   {
      unsigned int contextId = ea.getGraphicsContext()->getState()->getContextID();
      int index;
      switch(ea.getButton())
      {
         case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON: index = 0; break;
         case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON: index = 2; break;
         case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON: index = 1; break;
      }

      std::pair<unsigned int, int> pair(contextId, index);

      mMouseButtonPressed.insert(pair);
      mMouseButtonDown.insert(pair);
      if(!mCallbacks.empty())
      {
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->MouseButtonDown(index, handled, contextId);
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMouseMove(const osgGA::GUIEventAdapter& ea)
   {
      float nx = ea.getXnormalized();
      float ny = ea.getYnormalized();

      mMouseDeltaX = nx - mMouseX;
      mMouseDeltaY = ny - mMouseY;

      mMouseX = nx;
      mMouseY = ny;

      int nxr = ea.getX();
      int nyr = ea.getY();

      if(mLockCursor)
      {
         bool xmodified = true;
         if(nx > 0.99) mMouseX = -0.98f;
         else if(nx < -0.99) mMouseX = 0.98f;
         else xmodified = false;

         bool ymodified = true;
         if(ny > 0.99) mMouseY = -0.98f;
         else if(ny < -0.99) mMouseY = 0.98f;
         else ymodified = false;

         if(xmodified || ymodified) {
            const osgViewer::GraphicsWindow* win =
                  dynamic_cast<const osgViewer::GraphicsWindow*>(ea.getGraphicsContext());
            if(win)
            {
               mMouseXRaw = ea.getXmin() + (ea.getXmax() - ea.getXmin()) * (1 + mMouseX) / 2;
               mMouseYRaw = ea.getYmin() + (ea.getYmax() - ea.getYmin()) * (1 + mMouseY) / 2;
               int jump = ea.getYmax() - (ea.getYmax() - ea.getYmin()) * (1 + mMouseY) / 2;

               const_cast<osgViewer::GraphicsWindow*>(win)->requestWarpPointer(mMouseXRaw,jump);
               mMouseDeltaXRaw = 0;
               mMouseDeltaYRaw = 0;

               return;
            }
         }
      }

      mMouseDeltaXRaw = nxr - mMouseXRaw;
      mMouseDeltaYRaw = nyr - mMouseYRaw;
      mMouseXRaw = nxr;
      mMouseYRaw = nyr;

      if(!mCallbacks.empty())
      {
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->MouseMove(nx, ny, handled, ea.getGraphicsContext()->getState()->getContextID());
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMouseWheel(const osgGA::GUIEventAdapter& ea)
   {

      unsigned int  contextId = ea.getGraphicsContext()->getState()->getContextID();
      mMouseScroll = ea.getScrollingMotion();
      mMouseScrollContext = contextId;
      
      if(!mCallbacks.empty())
      {
         int dir = 0;
         switch(mMouseScroll)
         {
         case osgGA::GUIEventAdapter::SCROLL_UP:
            dir = 1; break;
         case osgGA::GUIEventAdapter::SCROLL_DOWN:
            dir = -1; break;
         default:
            return;
         }
         bool handled = ea.getHandled();
         for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
         {
            handled |= (*i)->MouseWheel(dir, handled, ea.getGraphicsContext()->getState()->getContextID());
         }
         if(handled && !ea.getHandled())
         {
            ea.setHandled(true);
         }
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMultiTouch(const osgGA::GUIEventAdapter& ea)
   {     
      osgGA::GUIEventAdapter::TouchData* data = ea.getTouchData();
      mNumTouches = 0;
      mTouches.resize(data->getNumTouchPoints());

      for(unsigned int i = 0; i < data->getNumTouchPoints(); ++i)
      {
         osgGA::GUIEventAdapter::TouchData::TouchPoint tp = data->get(i);
         TouchPoint mypt;
         mypt.mId = tp.id;
         mypt.mX = tp.x;
         mypt.mY = tp.y;
         mypt.mTapCount = tp.tapCount;

         switch(tp.phase)
         {
         case osgGA::GUIEventAdapter::TOUCH_UNKNOWN: 
            mypt.mTouchPhase = TouchPhase::UNKNOWN; 
            break;
         case osgGA::GUIEventAdapter::TOUCH_BEGAN: 
            mypt.mTouchPhase = TouchPhase::BEGAN;
            ++mNumTouches;
            break;
         case osgGA::GUIEventAdapter::TOUCH_MOVED: 
            mypt.mTouchPhase = TouchPhase::MOVED; 
            ++mNumTouches;
            break;
         case osgGA::GUIEventAdapter::TOUCH_STATIONERY: 
            mypt.mTouchPhase = TouchPhase::STATIONARY; 
            ++mNumTouches;
            break;
         case osgGA::GUIEventAdapter::TOUCH_ENDED: 
            mypt.mTouchPhase = TouchPhase::ENDED; 
            break;
         }
         mTouches[i] = mypt;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputHandler::HandleMouseEnterLeave(const osgGA::GUIEventAdapter& ea)
   {
      const MouseEnterLeaveEvent* e = static_cast<const MouseEnterLeaveEvent*>(ea.getUserData());
      bool focused = e->mFocused;

      for(Callbacks::iterator i = mCallbacks.begin(); i != mCallbacks.end(); ++i)
      {
         (*i)->MouseEnterLeave(focused, ea.getGraphicsContext()->getState()->getContextID());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgGA::GUIEventAdapter::KeySymbol InputHandler::GetKeySymbol(const std::string& name)  const
   {
      KeyNames::const_iterator found = mKeyNames.find(name);
      if(found == mKeyNames.end())
      { 
         return (osgGA::GUIEventAdapter::KeySymbol)0;
      }
      return found->second;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetKey(const std::string& v, unsigned int contextId)  const
   {  
      std::pair<unsigned int, int> pair(contextId, GetKeySymbol(v));
      return (mKeyPressedSet.find(pair) != mKeyPressedSet.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetKeyUp(const std::string& v, unsigned int contextId)  const
   {
      std::pair<unsigned int, int> pair(contextId, GetKeySymbol(v));
      return (mKeyUpSet.find(pair) != mKeyUpSet.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetKeyDown(const std::string& v, unsigned int contextId)  const
   {
      std::pair<unsigned int, int> pair(contextId, GetKeySymbol(v));
      return (mKeyDownSet.find(pair) != mKeyDownSet.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::AnyKeyDown() const
   {
      return (!mKeyDownSet.empty());
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string InputHandler::GetInputString() const
   {
      return mInputString.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetMouseButton(int button, unsigned int contextId) const
   {
      std::pair<unsigned int, int> pair(contextId, button);
      return mMouseButtonPressed.find(pair) != mMouseButtonPressed.end();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetMouseButtonDown(int button, unsigned int contextId) const
   {
      std::pair<unsigned int, int> pair(contextId, button);
      return mMouseButtonDown.find(pair) != mMouseButtonDown.end();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool InputHandler::GetMouseButtonUp(int button, unsigned int contextId) const
   {
      std::pair<unsigned int, int> pair(contextId, button);
      return mMouseButtonUp.find(pair) != mMouseButtonUp.end();
   }

   ////////////////////////////////////////////////////////////////////////////////
   double InputHandler::GetAxis(StringId axisname) const
   {
      if(axisname == MouseXId)
      {
         return mMouseX;
      }
      else if(axisname == MouseYId)
      {
         return mMouseY;
      }
      else if(axisname == MouseXRawId)
      {
         return mMouseXRaw;
      }
      else if(axisname == MouseYRawId)
      {
         return mMouseYRaw;
      }
      else if(axisname == MouseDeltaXId)
      {
         return mMouseDeltaX;
      }
      else if(axisname == MouseDeltaYId)
      {
         return mMouseDeltaY;
      }
      else if(axisname == MouseDeltaXRawId)
      {
         return mMouseDeltaXRaw;
      }
      else if(axisname == MouseDeltaYRawId)
      {
         return mMouseDeltaYRaw;
      }
      LOG_ERROR("Unknown axis:" + GetStringFromSID(axisname));
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osgGA::GUIEventAdapter::ScrollingMotion InputHandler::GetMouseWheelState(unsigned int contextId) const
   {
      if(contextId == mMouseScrollContext)
      {
         return mMouseScroll;
      }
      else
      {
         return osgGA::GUIEventAdapter::SCROLL_NONE;
      }
   }
}
