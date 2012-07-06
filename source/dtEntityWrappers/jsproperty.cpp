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

   //////////////////////////////////////////////////////////////////
   class JSBoolProperty
         : public dtEntity::Property
         , public PropertyGetterSetter
   {
   public:

      JSBoolProperty(Handle<Function> getter, Handle<Function> setter)
      : PropertyGetterSetter(getter, setter)
      {
      }

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::BOOL; }

      virtual bool BoolValue() const { return Get(); }
      virtual void SetBool(bool v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::BoolProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::BoolProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::BoolProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.BoolValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.BoolValue()); return true; }

      bool Get() const
      {
         HandleScope scope;
         Context::Scope context_scope(mGetter->CreationContext());
         TryCatch try_catch;

         Handle<Value> ret = mGetter->Call(mGetter, 0, NULL);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
         return ret->BooleanValue();
      }

      void Set(bool v)
      {
         HandleScope scope;
         Context::Scope context_scope(mGetter->CreationContext());
         TryCatch try_catch;

         Handle<Value> val = Boolean::New(v);
         Handle<Value> argv[1] = { val };
         Handle<Value> ret = mGetter->Call(mSetter, 1, argv);

         if(ret.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }
   };

   dtEntity::StringId s_propertyWrapper = dtEntity::SID("PropertymWrapper");

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
      return prop->mGetter->Call(prop->mGetter, 0, NULL);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> PRSet(const Arguments& args)
   {
      PropertyGetterSetter* prop = UnwrapProperty(args.This());
      HandleScope scope;
      Context::Scope context_scope(args.This()->CreationContext());
      Handle<Value> argv[1] = { args[0] };
      return prop->mSetter->Call(prop->mSetter, 1, argv);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<v8::Object> WrapProperty(PropertyGetterSetter* v)
   {
      HandleScope handle_scope;

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_propertyWrapper);
      if(templt.IsEmpty())
      {

         templt = FunctionTemplate::New();

         templt->SetClassName(String::New("Property"));
         templt->InstanceTemplate()->SetInternalFieldCount(1);

         Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

         proto->Set("get", FunctionTemplate::New(PRGet));
         proto->Set("set", FunctionTemplate::New(PRSet));

         GetScriptSystem()->SetTemplateBySID(s_propertyWrapper, templt);
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
   Handle<Value> CreatePropertyBool(const Arguments& args)
   {
      HandleScope scope;

      Handle<Function> fg = Handle<Function>::Cast(args[0]);
      Handle<Function> fs = Handle<Function>::Cast(args[1]);
      if(fg.IsEmpty() || fs.IsEmpty())
      {
         return ThrowError("CreateBoolProperty expects two function arguments");
      }
      JSBoolProperty* prop = new JSBoolProperty(fg, fs);
      Persistent<Object> pobj = Persistent<Object>::New(WrapProperty(prop));
      pobj.MakeWeak(NULL, &PropertyDestructor);
      return pobj;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterPropertyFunctions(ScriptSystem* ss, Handle<Context> context)
   {
      //HandleScope handle_scope;
      Context::Scope context_scope(context);
      context->Global()->Set(String::New("createPropertyBool"), FunctionTemplate::New(CreatePropertyBool)->GetFunction());
   }
}
