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

#include <dtEntityWrappers/export.h>
#include <v8.h>

namespace Rocket
{
   namespace Core
   {
      class Element;
   }
}

namespace dtEntityRocket
{
   v8::Handle<v8::Object> WrapElement(v8::Handle<v8::Context> context, Rocket::Core::Element* v);
   Rocket::Core::Element* UnwrapElement(v8::Handle<v8::Value>);   
   v8::Handle<v8::FunctionTemplate> GetElementTemplate();

	v8::Handle<v8::Value> ELNamedPropertyGetter(v8::Local<v8::String> namestr, const v8::AccessorInfo& info);
	v8::Handle<v8::Boolean> ELNamedPropertyDeleter(v8::Local<v8::String> namestr, const v8::AccessorInfo& info);
	v8::Handle<v8::Value> ELNamedPropertySetter(v8::Local<v8::String> namestr, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	v8::Handle<v8::Integer> ELNamedPropertyQuery(v8::Local<v8::String> namestr,const v8::AccessorInfo& info);
	v8::Handle<v8::Array> ELNamedPropertyEnumerator(const v8::AccessorInfo& info);
}
