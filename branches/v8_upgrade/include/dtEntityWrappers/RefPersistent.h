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
* Martin Scheffler / Riccardo Corsi
*/

#include <dtEntityWrappers/export.h>
#include <osg/Referenced>
#include <v8.h>
#include <string>

namespace dtEntityWrappers
{
   /// Referenced wrapper around Persistent: 
   /// - auto Reset() on destructor
   /// - allow ref_ptr usage in stl container and/or exported dll, which need copyable classes
   template<typename T> class RefPersistent : public osg::Referenced 
   {
   public:
      // default ctor
      RefPersistent() {}
      // named ctor initialize to the given handle
      RefPersistent(v8::Isolate* isolate, v8::Handle<T> handle) { _persistent.Reset(isolate, handle); }

      // get internal persistent
      v8::Persistent<T>& GetPersistent() { return _persistent; }

      // shortcut to get a local handle from the internal persistent - uses Isolate::GetCurrent
      v8::Local<T> GetLocal() const { return v8::Local<T>::New(v8::Isolate::GetCurrent(), _persistent); }
      // same as above but with explicit isolate passed in as param
      v8::Local<T> GetLocal(v8::Isolate* isolate) const { return v8::Local<T>::New(isolate, _persistent); }


   protected:
      // prevent copy ctor
      RefPersistent(const RefPersistent& that);
      // prevent copy operator
      RefPersistent& operator=(const RefPersistent& that);

      // protected dtor when deriving from Refereced, must be heap allocated
      ~RefPersistent(){ _persistent.Reset(); }
      // internal persistent object
      v8::Persistent<T> _persistent;
   };


}
