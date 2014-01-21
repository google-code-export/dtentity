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
