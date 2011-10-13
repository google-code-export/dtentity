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

#pragma once


#include "export.h"
#include <dtEntity/entitymanager.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>

namespace dtEntityRocket
{

   //////////////////////////////////////////////////////////////////////////////// 
   /**
    * Tick message gets sent each frame by the system.
    * Register for this message if you want to continually update something.
    */
   class DT_ROCKET_EXPORT RocketEventMessage
     : public dtEntity::Message
   {
   public:
      
      static const dtEntity::MessageType TYPE;
      static const dtEntity::StringId NameId;
      static const dtEntity::StringId ParametersId;

      RocketEventMessage()
         : dtEntity::Message(TYPE)
      {
         Register(NameId, &mName);
         Register(ParametersId, &mParameters);
      }
      
      virtual dtEntity::Message* Clone() const { return CloneContainer<RocketEventMessage>(); }
      
      std::string GetName() const { return mName.Get(); }
      void SetName(const std::string& v) { mName.Set(v); }

      dtEntity::GroupProperty& GetParameters() { return mParameters; }
     
   private:

     dtEntity::StringProperty mName;
     dtEntity::GroupProperty mParameters;
    
   };

   

}

