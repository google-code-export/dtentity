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

#include <dtEntityWrappers/export.h>
#include <v8.h>

namespace dtEntity
{
   class EntityManager;
   class MessageFactory;
   class Message;
}

namespace dtEntityWrappers
{
   class ScriptSystem;

   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Object> WrapEntityManager
      (ScriptSystem*, dtEntity::EntityManager* v, dtEntity::MessageFactory* mf);
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Object> WrapPriorities();
   DTENTITY_WRAPPERS_EXPORT dtEntity::EntityManager* UnwrapEntityManager(v8::Handle<v8::Value>);
   DTENTITY_WRAPPERS_EXPORT dtEntity::MessageFactory* UnwrapMessageFactory(v8::Handle<v8::Value>);
   DTENTITY_WRAPPERS_EXPORT void UnregisterJavaScriptFromMessages(ScriptSystem*);
   DTENTITY_WRAPPERS_EXPORT void ConvertJSToMessage(v8::Handle<v8::Value> val, dtEntity::Message* msg);
}
