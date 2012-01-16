#pragma once

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

#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntityWrappers/export.h>

namespace dtEntity
{
   class MessageFactory;
}

namespace dtEntityWrappers
{
   
   void DTENTITY_WRAPPERS_EXPORT RegisterMessageTypes(dtEntity::MessageFactory&);
  
   ////////////////////////////////////////////////////////////////////////////////
   /**
    * send this message to cause a script to be loaded
    */
   class DTENTITY_WRAPPERS_EXPORT ExecuteScriptMessage
      : public dtEntity::Message
   {
   public:

      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId PathId;

      ExecuteScriptMessage()
         : dtEntity::Message(TYPE)
      {
         Register(PathId, &mPath);
      }

      // Create a copy of this message on the heap
      virtual dtEntity::Message* Clone() const { return CloneContainer<ExecuteScriptMessage>(); }

      void SetPath(const std::string& v) { mPath.Set(v); }
      std::string GetPath() const { return mPath.Get(); }

   private:

      dtEntity::StringProperty mPath;
   };
}
