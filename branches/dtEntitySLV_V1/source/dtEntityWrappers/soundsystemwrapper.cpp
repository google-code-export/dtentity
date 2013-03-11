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

#include <dtEntityWrappers/soundsystemwrapper.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntity/soundcomponent.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>

#include <iostream>
#include <sstream>
#include <assert.h>
#include <v8.h>

using namespace v8;

namespace dtEntityWrappers
{
   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::SoundSystem* UnwrapSoundSystem(v8::Handle<v8::Value> val)
   {
      dtEntity::SoundSystem* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SSToString(const Arguments& args)
   {
      return String::New("<SoundSystem>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> SSPlaySound(const Arguments& args)
   {
      dtEntity::SoundSystem* ss = UnwrapSoundSystem(args.This());
      ss->PlaySound(args[0]->Int32Value());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitSoundSystemWrapper(ScriptSystem* ss)
   {
      HandleScope scope;
      Context::Scope context_scope(ss->GetGlobalContext());

      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      templt->SetClassName(String::New("SoundSystem"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("toString", FunctionTemplate::New(SSToString));
      proto->Set("playSound", FunctionTemplate::New(SSPlaySound));
      
      RegisterEntitySystempWrapper(ss, dtEntity::SoundComponent::TYPE, templt);
   }

}
