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
   const dtEntity::StringId PropertyGetterSetter::TemplateHandle = dtEntity::SID("PropertyWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyDestructor(v8::Persistent<Value> v, void* scriptsysnull)
   {
      PropertyGetterSetter* prop = UnwrapProperty(v);
      if(prop != NULL)
      {
         delete prop;
      }
      v.Dispose();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> PRGet(const Arguments& args)
   {
      PropertyGetterSetter* prop = UnwrapProperty(args.This());
      HandleScope scope;
      Context::Scope context_scope(args.This()->CreationContext());
      return prop->mGetter->Call(prop->mHolder, 0, NULL);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> PRSet(const Arguments& args)
   {
      PropertyGetterSetter* prop = UnwrapProperty(args.This());
      HandleScope scope;
      Context::Scope context_scope(args.This()->CreationContext());
      Handle<Value> argv[1] = { args[0] };
      return prop->mSetter->Call(prop->mHolder, 1, argv);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<v8::Object> WrapProperty(PropertyGetterSetter* v)
   {
      HandleScope handle_scope;

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(PropertyGetterSetter::TemplateHandle);
      if(templt.IsEmpty())
      {

         templt = FunctionTemplate::New();

         templt->SetClassName(String::New("Property"));
         templt->InstanceTemplate()->SetInternalFieldCount(1);

         Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

         proto->Set("get", FunctionTemplate::New(PRGet));
         proto->Set("set", FunctionTemplate::New(PRSet));

         GetScriptSystem()->SetTemplateBySID(PropertyGetterSetter::TemplateHandle, templt);
      }


      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));
      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyGetterSetter* UnwrapProperty(Handle<Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      PropertyGetterSetter* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSBoolProperty::JSBoolProperty(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyBool(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyBool expects two function arguments");
      }
      JSBoolProperty* prop = new JSBoolProperty(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSInt32Property::JSInt32Property(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyInt32(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyInt32 expects two function arguments");
      }
      JSInt32Property* prop = new JSInt32Property(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSUInt32Property::JSUInt32Property(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyUInt32(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyInt32 expects two function arguments");
      }
      JSUInt32Property* prop = new JSUInt32Property(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSNumberProperty::JSNumberProperty(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyNumber(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyNumber expects two function arguments");
      }
      JSNumberProperty* prop = new JSNumberProperty(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSQuatProperty::JSQuatProperty(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyQuat(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyQuat expects two function arguments");
      }
      JSQuatProperty* prop = new JSQuatProperty(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSStringProperty::JSStringProperty(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyString(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyString expects two function arguments");
      }
      JSStringProperty* prop = new JSStringProperty(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec2Property::JSVec2Property(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyVec2(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyVec2 expects two function arguments");
      }
      JSVec2Property* prop = new JSVec2Property(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec3Property::JSVec3Property(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyVec3(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyVec3 expects two function arguments");
      }
      JSVec3Property* prop = new JSVec3Property(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   JSVec4Property::JSVec4Property(Handle<Function> getter, Handle<Function> setter)
   : PropertyGetterSetter(getter, setter)
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
   Handle<Value> CreatePropertyVec4(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("__createPropertyVec4 expects two function arguments");
      }
      JSVec4Property* prop = new JSVec4Property(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      prop->mHolder = pobj;
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterPropertyFunctions(ScriptSystem* ss, Handle<Context> context)
   {
      Context::Scope context_scope(context);
      context->Global()->Set(String::New("__createPropertyBool"), FunctionTemplate::New(CreatePropertyBool)->GetFunction());
      context->Global()->Set(String::New("__createPropertyInt32"), FunctionTemplate::New(CreatePropertyInt32)->GetFunction());
      context->Global()->Set(String::New("__createPropertyNumber"), FunctionTemplate::New(CreatePropertyNumber)->GetFunction());
      context->Global()->Set(String::New("__createPropertyQuat"), FunctionTemplate::New(CreatePropertyQuat)->GetFunction());
      context->Global()->Set(String::New("__createPropertyUInt32"), FunctionTemplate::New(CreatePropertyUInt32)->GetFunction());
      context->Global()->Set(String::New("__createPropertyString"), FunctionTemplate::New(CreatePropertyString)->GetFunction());
      context->Global()->Set(String::New("__createPropertyVec2"), FunctionTemplate::New(CreatePropertyVec2)->GetFunction());
      context->Global()->Set(String::New("__createPropertyVec3"), FunctionTemplate::New(CreatePropertyVec3)->GetFunction());
      context->Global()->Set(String::New("__createPropertyVec4"), FunctionTemplate::New(CreatePropertyVec4)->GetFunction());
   }
}
