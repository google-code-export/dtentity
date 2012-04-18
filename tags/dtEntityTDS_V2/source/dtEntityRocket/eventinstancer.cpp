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

#include "eventinstancer.h"

#include "rocketcomponent.h"
#include "messages.h"
#include <dtEntity/log.h>

namespace dtEntityRocket
{

   ////////////////////////////////////////////////////////////////////////////////
   EventInstancer::EventInstancer(dtEntity::MessagePump& p)
      : mMessagePump(&p)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventInstancer::~EventInstancer()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   Rocket::Core::Event* EventInstancer::InstanceEvent(Rocket::Core::Element* target, const Rocket::Core::String& name, const Rocket::Core::Dictionary& parameters, bool interruptable)
   {
      RocketEventMessage msg;
      msg.SetName(name.CString());

      dtEntity::GroupProperty& msgparams = msg.GetParameters();

      int index = 0;
      Rocket::Core::String key;
      Rocket::Core::Variant* value;

      while (parameters.Iterate(index, key, value))
      {
         dtEntity::Property* prop = RocketVariantToProperty(*value);
         if(prop)
         {
            msgparams.Add(dtEntity::SID(key.CString()), prop);
         }
      }

      mMessagePump->EmitMessage(msg);

      return new Rocket::Core::Event(target, name, parameters, interruptable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Releases an event instanced by this instancer.
   void EventInstancer::ReleaseEvent(Rocket::Core::Event* event)
   {
      delete event;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EventInstancer::Release()
   {
      delete this;
   }
}
