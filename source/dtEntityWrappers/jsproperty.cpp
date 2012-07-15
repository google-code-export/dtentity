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

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> JSPropertyGetter(Local<String> propname, const AccessorInfo& info)
   {
      PropertyGetterSetter* prop = static_cast<PropertyGetterSetter*>(Handle<External>::Cast(info.Data())->Value());
      std::string propnamestr = ToStdString(propname);
      if(!prop)
      {
         LOG_ERROR("Property not found!");
         return Undefined();
      }
      HandleScope scope;
      Context::Scope context_scope(prop->mGetter->CreationContext());
      return scope.Close(prop->mGetter->Call(prop->mHolder, 0, NULL));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSPropertySetter(Local<String> propname,
                               Local<Value> value,
                               const AccessorInfo& info)
   {
      PropertyGetterSetter* prop = static_cast<PropertyGetterSetter*>(Handle<External>::Cast(info.Data())->Value());

      if(!prop)
      {
         LOG_ERROR("Property not found!");
         return;
      }

      HandleScope scope;
      Context::Scope context_scope(prop->mSetter->CreationContext());

      Handle<Value> argv[1] = { value };

      TryCatch try_catch;
      Handle<Value> ret = prop->mSetter->Call(prop->mHolder, 1, argv);
      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   template<class T>
   Handle<Value> CreateProp(const Arguments& args)
   {
      HandleScope scope;

      //obj, propname, val, getter, setter
      Handle<Object> obj = Handle<Function>::Cast(args[0]);
      Handle<String> propname = Handle<String>::Cast(args[1]);
      Handle<Function> fg = Handle<Function>::Cast(args[2]);
      Handle<Function> fs = Handle<Function>::Cast(args[3]);

      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyInt32 expects two function arguments");
      }
      Persistent<Object> holder = Persistent<Object>::New(Object::New());
      holder->Set(String::New("get"), fg);
      holder->Set(String::New("set"), fs);

      holder.MakeWeak(NULL, &PropertyDestructor);
      T* prop = new T(holder, fg, fs);

      Handle<External> ext = v8::External::New(static_cast<void*>(prop));
      holder->Set(String::New("__property"), ext);

      obj->SetAccessor(propname, JSPropertyGetter, JSPropertySetter, ext, PROHIBITS_OVERWRITING, DontDelete);

      // also store property in object as hidden value. This way it can be found
      // when c++ wrappers are created
      obj->SetHiddenValue(propname, ext);
      return Undefined();
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
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->BooleanValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSBoolProperty::Set(bool v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Boolean::New(v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = mSetter->Call(mHolder, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->Int32Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSInt32Property::Set(int v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Int32::New(v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = mSetter->Call(mHolder, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->Uint32Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSUInt32Property::Set(unsigned int v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Uint32::New(v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = mSetter->Call(mHolder, 1, argv);

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
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ret->NumberValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSNumberProperty::Set(double v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = Number::New(v);
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = mSetter->Call(mHolder, 1, argv);

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
   osg::Quat JSQuatProperty::Get() const
   {
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 4)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return osg::Quat(a->Get(0)->NumberValue(),
                       a->Get(1)->NumberValue(),
                       a->Get(2)->NumberValue(),
                       a->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSQuatProperty::Set(const osg::Quat& v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[4] = { Number::New(v[0]),
                                Number::New(v[1]),
                                Number::New(v[2]),
                                Number::New(v[3])
                              };
      Handle<Value> ret = mSetter->Call(mHolder, 4, argv);

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
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      return ToStdString(ret);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSStringProperty::Set(const std::string& v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> val = String::New(v.c_str());
      Handle<Value> argv[1] = { val };
      Handle<Value> ret = mSetter->Call(mHolder, 1, argv);

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
   osg::Vec2d JSVec2Property::Get() const
   {
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 2)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return osg::Vec2d(a->Get(0)->NumberValue(),
                       a->Get(1)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec2Property::Set(const osg::Vec2d& v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[2] = { Number::New(v[0]),
                                Number::New(v[1])
                              };
      Handle<Value> ret = mSetter->Call(mHolder, 2, argv);

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
   osg::Vec3d JSVec3Property::Get() const
   {
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 3)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return osg::Vec3d(a->Get(0)->NumberValue(),
                        a->Get(1)->NumberValue(),
                        a->Get(2)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec3Property::Set(const osg::Vec3d& v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[3] = { Number::New(v[0]),
                                Number::New(v[1]),
                                Number::New(v[2])
                              };
      Handle<Value> ret = mSetter->Call(mHolder, 3, argv);

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
   osg::Vec4d JSVec4Property::Get() const
   {
      HandleScope scope;
      Context::Scope context_scope(mGetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> ret = mGetter->Call(mHolder, 0, NULL);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
      Handle<Array> a = Handle<Array>::Cast(ret);
      if(a.IsEmpty() || a->Length() < 4)
      {
         LOG_ERROR("Wrong return value from array property getter!");
      }
      return osg::Vec4d(a->Get(0)->NumberValue(),
                        a->Get(1)->NumberValue(),
                        a->Get(2)->NumberValue(),
                        a->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void JSVec4Property::Set(const osg::Vec4d& v)
   {
      HandleScope scope;
      Context::Scope context_scope(mSetter->CreationContext());
      TryCatch try_catch;

      Handle<Value> argv[4] = { Number::New(v[0]),
                                Number::New(v[1]),
                                Number::New(v[2]),
                                Number::New(v[3])
                              };
      Handle<Value> ret = mSetter->Call(mHolder, 4, argv);

      if(ret.IsEmpty())
      {
         ReportException(&try_catch);
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void RegisterPropertyFunctions(ScriptSystem* ss, Handle<Context> context)
   {
      Context::Scope context_scope(context);

      context->Global()->Set(String::New("__createInt32Prop"), FunctionTemplate::New(CreateProp<JSInt32Property>)->GetFunction());
      context->Global()->Set(String::New("__createUint32Prop"), FunctionTemplate::New(CreateProp<JSUInt32Property>)->GetFunction());
      context->Global()->Set(String::New("__createBoolProp"), FunctionTemplate::New(CreateProp<JSBoolProperty>)->GetFunction());
      context->Global()->Set(String::New("__createNumberProp"), FunctionTemplate::New(CreateProp<JSNumberProperty>)->GetFunction());
      context->Global()->Set(String::New("__createStringProp"), FunctionTemplate::New(CreateProp<JSStringProperty>)->GetFunction());
      context->Global()->Set(String::New("__createQuatProp"), FunctionTemplate::New(CreateProp<JSQuatProperty>)->GetFunction());
      context->Global()->Set(String::New("__createVec2Prop"), FunctionTemplate::New(CreateProp<JSVec2Property>)->GetFunction());
      context->Global()->Set(String::New("__createVec3Prop"), FunctionTemplate::New(CreateProp<JSVec3Property>)->GetFunction());
      context->Global()->Set(String::New("__createVec4Prop"), FunctionTemplate::New(CreateProp<JSVec4Property>)->GetFunction());
   }
}
