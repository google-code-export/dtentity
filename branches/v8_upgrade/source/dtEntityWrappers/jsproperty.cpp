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

#include <dtEntityWrappers/jsproperty.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/scriptcomponent.h>

using namespace v8;

namespace dtEntityWrappers
{


   PropertyGetterSetter::PropertyGetterSetter(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter)         
   {
      //mpHolder = new v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object> >(v8::Isolate::GetCurrent(), holder);
      mpHolder = new RefPersistent<v8::Object>(v8::Isolate::GetCurrent(), holder);
      mpGetter = new RefPersistent<v8::Function>(v8::Isolate::GetCurrent(), getter);
      mpSetter = new RefPersistent<v8::Function>(v8::Isolate::GetCurrent(), setter);
   }

   PropertyGetterSetter::~PropertyGetterSetter()
   {
      // nothing to do here, the ref_ptr will do out of scope and the 
      // persistent will be automatically Reset
   }



#if 0
   ////////////////////////////////////////////////////////////////////////////////
   void PropertyDestructor(v8::Persistent<Value> v, void* scriptsysnull)
   {
      Handle<Object> obj = Handle<Object>::Cast(v);
      assert(!obj.IsEmpty());

      PropertyGetterSetter* gs;
      GetInternal(obj, 0, gs);
      delete gs;
      v.Dispose();
   }
#endif

   ////////////////////////////////////////////////////////////////////////////////
   //Handle<Value> JSPropertyGetter(Local<String> propname, const AccessorInfo& info)
   void JSPropertyGetter( Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      PropertyGetterSetter* prop = static_cast<PropertyGetterSetter*>(Handle<External>::Cast(info.Data())->Value());
      std::string propnamestr = ToStdString(propname);
      if(!prop)
      {
         LOG_ERROR("Property not found!");
         return;
      }
      
      EscapableHandleScope scope(Isolate::GetCurrent());

      // create local handle for persistent objects
      Local<Function> getter = prop->mpGetter->GetLocal();
      //Local<Object> holder = Local<Object>::New(Isolate::GetCurrent(), *prop->mpHolder);
      Local<Object> holder = prop->mpHolder->GetLocal();

      // call the function using the local handles and escape with the return value
      Context::Scope context_scope(getter->CreationContext());
      info.GetReturnValue().Set(getter->Call(holder, 0, NULL));

      //return scope.Escape(getter->Call(holder, 0, NULL));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSPropertySetter(Local<String> propname,
                               Local<Value> value,
                               const PropertyCallbackInfo<void>& info)
   {
      PropertyGetterSetter* prop = static_cast<PropertyGetterSetter*>(Handle<External>::Cast(info.Data())->Value());

      if(!prop)
      {
         LOG_ERROR("Property not found!");
         return;
      }

      // create local handle for persistent objects
      Local<Function> setter = prop->mpSetter->GetLocal();
      //Local<Object> holder = Local<Object>::New(Isolate::GetCurrent(), *prop->mpHolder);
      Local<Object> holder = prop->mpHolder->GetLocal();

      HandleScope scope(Isolate::GetCurrent());      
      Context::Scope context_scope(setter->CreationContext());

      Handle<Value> argv[1] = { value };

      TryCatch try_catch;
      Handle<Value> ret = setter->Call(holder, 1, argv);
      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   template<class T>
   void CreateProp(const FunctionCallbackInfo<Value>& args)
   {
      Isolate* isolate = Isolate::GetCurrent(); 
      HandleScope scope(isolate);

      //obj, propname, val, getter, setter
      Handle<Object> obj = Handle<Function>::Cast(args[0]);
      Handle<String> propname = Handle<String>::Cast(args[1]);
      Handle<Function> fg = Handle<Function>::Cast(args[2]);
      Handle<Function> fs = Handle<Function>::Cast(args[3]);

      if(fg.IsEmpty() || fs.IsEmpty())
      {
         ThrowError("__createPropertyInt32 expects two function arguments");
         return;
      }
      
      // TODO this was previously declared as persistent handle. 
      // review te 
      Local<Object> holder = Local<Object>::New(isolate, Object::New(isolate));
      holder->Set(String::NewFromUtf8(isolate, "get"), fg);
      holder->Set(String::NewFromUtf8(isolate, "set"), fs);

      // TODO review make weak approach
      //holder.MakeWeak(NULL, &PropertyDestructor);
      T* prop = new T(holder, fg, fs);

      Handle<External> ext = v8::External::New(isolate, static_cast<void*>(prop));
      holder->Set(String::NewFromUtf8(isolate, "__property"), ext);

      obj->SetAccessor(propname, JSPropertyGetter, JSPropertySetter, ext, PROHIBITS_OVERWRITING, DontDelete);

      // also store property in object as hidden value. This way it can be found
      // when c++ wrappers are created
      obj->SetHiddenValue(propname, ext);
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSBoolProperty::JSBoolProperty(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool JSBoolProperty::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->BooleanValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSBoolProperty::Set(bool v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Boolean::New(isolate, v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = setter->Call(holder, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSInt32Property::JSInt32Property(v8::Handle<v8::Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   int JSInt32Property::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->Int32Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSInt32Property::Set(int v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Int32::New(isolate, v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = setter->Call(holder, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSUInt32Property::JSUInt32Property(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int JSUInt32Property::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->Uint32Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSUInt32Property::Set(unsigned int v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Uint32::New(isolate, v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = setter->Call(holder, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSNumberProperty::JSNumberProperty(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   double JSNumberProperty::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->NumberValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSNumberProperty::Set(double v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Number::New(isolate, v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = setter->Call(holder, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSQuatProperty::JSQuatProperty(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Quat JSQuatProperty::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 4)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return dtEntity::Quat(a->Get(0)->NumberValue(),
                       a->Get(1)->NumberValue(),
                       a->Get(2)->NumberValue(),
                       a->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSQuatProperty::Set(const dtEntity::Quat& v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[4] = { Number::New(isolate, v[0]),
                                Number::New(isolate, v[1]),
                                Number::New(isolate, v[2]),
                                Number::New(isolate, v[3])
                              };

      Handle<Value> ret = setter->Call(holder, 4, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSStringProperty::JSStringProperty(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string JSStringProperty::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ToStdString(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSStringProperty::Set(const std::string& v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = String::NewFromUtf8(isolate, v.c_str());
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = setter->Call(holder, 1, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec2Property::JSVec2Property(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Vec2d JSVec2Property::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 2)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return dtEntity::Vec2d(a->Get(0)->NumberValue(),
                       a->Get(1)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec2Property::Set(const dtEntity::Vec2d& v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();


      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[2] = { Number::New(isolate, v[0]),
                                Number::New(isolate, v[1])
                              };
      Handle<Value> ret = setter->Call(holder, 2, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec3Property::JSVec3Property(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Vec3d JSVec3Property::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 3)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return dtEntity::Vec3d(a->Get(0)->NumberValue(),
                        a->Get(1)->NumberValue(),
                        a->Get(2)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec3Property::Set(const dtEntity::Vec3d& v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[3] = { Number::New(isolate, v[0]),
                                Number::New(isolate, v[1]),
                                Number::New(isolate, v[2])
                              };

      Handle<Value> ret = setter->Call(holder, 3, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec4Property::JSVec4Property(Handle<Object> holder, Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(holder, getter, setter)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Vec4d JSVec4Property::Get() const
   {
      HandleScope scope(Isolate::GetCurrent());
      // create local handle for persistent objects
      Local<Function> getter = mpGetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();
      Context::Scope context_scope(getter->CreationContext());

      TryCatch try_catch;

      Handle<Value> ret = getter->Call(holder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 4)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return dtEntity::Vec4d(a->Get(0)->NumberValue(),
                        a->Get(1)->NumberValue(),
                        a->Get(2)->NumberValue(),
                        a->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec4Property::Set(const dtEntity::Vec4d& v)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      // create local handle for persistent objects
      Local<Function> setter = mpSetter->GetLocal();
      Local<Object> holder = mpHolder->GetLocal();

      Context::Scope context_scope(setter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[4] = { Number::New(isolate, v[0]),
                                Number::New(isolate, v[1]),
                                Number::New(isolate, v[2]),
                                Number::New(isolate, v[3])
                              };
      Handle<Value> ret = setter->Call(holder, 4, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void RegisterPropertyFunctions(ScriptSystem* ss, Handle<Context> context)
   {
      Context::Scope context_scope(context);
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      context->Global()->Set(String::NewFromUtf8(isolate, "__createInt32Prop"), FunctionTemplate::New(isolate, CreateProp<JSInt32Property>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createUint32Prop"), FunctionTemplate::New(isolate, CreateProp<JSUInt32Property>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createBoolProp"), FunctionTemplate::New(isolate, CreateProp<JSBoolProperty>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createNumberProp"), FunctionTemplate::New(isolate, CreateProp<JSNumberProperty>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createStringProp"), FunctionTemplate::New(isolate, CreateProp<JSStringProperty>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createQuatProp"), FunctionTemplate::New(isolate, CreateProp<JSQuatProperty>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createVec2Prop"), FunctionTemplate::New(isolate, CreateProp<JSVec2Property>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createVec3Prop"), FunctionTemplate::New(isolate, CreateProp<JSVec3Property>)->GetFunction());
      context->Global()->Set(String::NewFromUtf8(isolate, "__createVec4Prop"), FunctionTemplate::New(isolate, CreateProp<JSVec4Property>)->GetFunction());
   }
}
