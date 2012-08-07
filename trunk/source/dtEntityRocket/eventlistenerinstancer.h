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

#include <dtEntity/messagepump.h>
#include <Rocket/Core/Event.h>
#include <Rocket/Core/EventListenerInstancer.h>
#include <v8.h>

namespace dtEntityRocket
{
   
  class EventListenerInstancer : public Rocket::Core::EventListenerInstancer
  {
  public:
	  EventListenerInstancer(v8::Handle<v8::Context> context, dtEntity::MessagePump& p);
	  virtual ~EventListenerInstancer();

	  
   #ifndef DTENTITY_USE_LIBROCKET_GIT
	 virtual Rocket::Core::EventListener* InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element);
   #else
	 virtual Rocket::Core::EventListener* InstanceEventListener(const Rocket::Core::String& value);
   #endif	    
	 
     virtual void Release();

  private:
    dtEntity::MessagePump* mMessagePump;
    v8::Persistent<v8::Context> mContext;

  };
}

