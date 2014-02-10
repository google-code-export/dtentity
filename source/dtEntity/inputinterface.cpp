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

#include <dtEntity/inputinterface.h>

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
   const StringId InputInterface::MouseXId(dtEntity::SID("MouseX"));  
   const StringId InputInterface::MouseYId(dtEntity::SID("MouseY"));  
   const StringId InputInterface::MouseXRawId(dtEntity::SID("MouseXRaw"));
   const StringId InputInterface::MouseYRawId(dtEntity::SID("MouseYRaw"));
   const StringId InputInterface::MouseDeltaXId(dtEntity::SID("MouseDeltaX"));
   const StringId InputInterface::MouseDeltaYId(dtEntity::SID("MouseDeltaY"));
   const StringId InputInterface::MouseDeltaXRawId(dtEntity::SID("MouseDeltaXRaw"));
   const StringId InputInterface::MouseDeltaYRawId(dtEntity::SID("MouseDeltaYRaw"));

}
