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

#include <v8.h>
#include <dtEntity/property.h>

namespace dtEntityWrappers
{
   class ScriptSystem;
   void RegisterPropertyFunctions(ScriptSystem* ss, v8::Handle<v8::Context> context);


   class PropertyGetterSetter : public dtEntity::Property
   {
   public:

      static const dtEntity::StringId TemplateHandle;

      PropertyGetterSetter(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter)
      : mGetter(v8::Persistent<v8::Function>::New(getter))
      , mSetter(v8::Persistent<v8::Function>::New(setter))
      {
      }

      v8::Persistent<v8::Function> mGetter;
      v8::Persistent<v8::Function> mSetter;
      v8::Persistent<v8::Object> mHolder;


   };

   v8::Handle<v8::Object> WrapProperty(PropertyGetterSetter* v);
   PropertyGetterSetter* UnwrapProperty(v8::Handle<v8::Value>);

   /*
         ARRAY,
         GROUP,
         MATRIX,
         QUAT,
         STRING,
         STRINGID,
         VEC2,
         VEC3,
         VEC4,
         VEC2D,
         VEC3D,
         VEC4D*/



   ////////////////////////////////////////////////////////////////////////////////
   class JSBoolProperty
         : public PropertyGetterSetter
   {
   public:

      JSBoolProperty(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::BOOL; }

      virtual bool BoolValue() const { return Get(); }
      virtual void SetBool(bool v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::BoolProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::BoolProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::BoolProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.BoolValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.BoolValue()); return true; }

      bool Get() const;
      void Set(bool v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSInt32Property
         : public PropertyGetterSetter
   {
   public:

      JSInt32Property(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::INT; }

      virtual int IntValue() const { return Get(); }
      virtual void SetInt(double v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::IntProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::IntProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::IntProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.IntValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.IntValue()); return true; }

      int Get() const;
      void Set(int v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSUInt32Property
         : public PropertyGetterSetter
   {
   public:

      JSUInt32Property(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::UINT; }

      virtual unsigned int UIntValue() const { return Get(); }
      virtual void SetUInt(unsigned int v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::UIntProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::UIntProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::UIntProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.UIntValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.UIntValue()); return true; }

      unsigned int Get() const;
      void Set(unsigned int v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSNumberProperty
         : public PropertyGetterSetter
   {
   public:

      JSNumberProperty(v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::DOUBLE; }

      virtual double DoubleValue() const { return Get(); }
      virtual void SetDouble(double v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::DoubleProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::DoubleProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::DoubleProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.DoubleValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.DoubleValue()); return true; }

      double Get() const;
      void Set(double v);
   };
}
