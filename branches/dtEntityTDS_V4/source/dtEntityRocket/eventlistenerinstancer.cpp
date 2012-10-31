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

#include "eventlistenerinstancer.h"

#include "eventlistener.h"
#include "rocketcomponent.h"
#include "messages.h"
#include <dtEntity/log.h>

namespace dtEntityRocket
{
   using namespace v8;

   ////////////////////////////////////////////////////////////////////////////////
   EventListenerInstancer::EventListenerInstancer(v8::Handle<v8::Context> context, dtEntity::MessagePump& p)
      : mMessagePump(&p)
      , mContext(Persistent<Context>::New(context))
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   EventListenerInstancer::~EventListenerInstancer()
   {
      mContext.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////

	  
   #ifdef DTENTITY_USE_LIBROCKET_GIT
	Rocket::Core::EventListener* EventListenerInstancer::InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element)
   #else
	Rocket::Core::EventListener* EventListenerInstancer::InstanceEventListener(const Rocket::Core::String& value)
   #endif	    
	{
      return new EventListener(mContext, value.CString());
	}

   ////////////////////////////////////////////////////////////////////////////////
   void EventListenerInstancer::Release()
   {
      delete this;
   }

}
