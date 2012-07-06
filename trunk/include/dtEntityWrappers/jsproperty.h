#pragma once

#include <v8.h>

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

namespace dtEntityWrappers
{
   class ScriptSystem;
   void RegisterPropertyFunctions(ScriptSystem* ss, v8::Handle<v8::Context> context);


   class PropertyGetterSetter
   {
   public:
      PropertyGetterSetter(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter)
      : mGetter(v8::Persistent<v8::Function>::New(getter))
      , mSetter(v8::Persistent<v8::Function>::New(setter))
      {
      }

      v8::Persistent<v8::Function> mGetter;
      v8::Persistent<v8::Function> mSetter;
   };

   v8::Handle<v8::Object> WrapProperty(PropertyGetterSetter* v);
   PropertyGetterSetter* UnwrapProperty(v8::Handle<v8::Value>);

}
