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

      PropertyGetterSetter(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter)
      : mHolder(v8::Persistent<v8::Object>::New(holder))
      , mGetter(v8::Persistent<v8::Function>::New(getter))
      , mSetter(v8::Persistent<v8::Function>::New(setter))
      {
      }

      ~PropertyGetterSetter()
      {
         mHolder.Dispose();
         mGetter.Dispose();
         mSetter.Dispose();         
      }

      v8::Persistent<v8::Object> mHolder;
      v8::Persistent<v8::Function> mGetter;
      v8::Persistent<v8::Function> mSetter;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSBoolProperty
         : public PropertyGetterSetter
   {
   public:

      JSBoolProperty(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

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

      JSInt32Property(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

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

      JSUInt32Property(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

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

      JSNumberProperty(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

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

   ////////////////////////////////////////////////////////////////////////////////
   class JSQuatProperty
         : public PropertyGetterSetter
   {
   public:

      JSQuatProperty(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::QUAT; }

      virtual dtEntity::Quat QuatValue() const { return Get(); }
      virtual void SetQuat(const dtEntity::Quat& v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::QuatProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::QuatProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::QuatProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.QuatValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.QuatValue()); return true; }

      dtEntity::Quat Get() const;
      void Set(const dtEntity::Quat& v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSStringProperty
         : public PropertyGetterSetter
   {
   public:

      JSStringProperty(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::STRING; }

      virtual void SetString(const std::string& v) { Set(v); }
      virtual const std::string StringValue() const { return Get(); }
      virtual Property* Clone() const { return new dtEntity::StringProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.StringValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.StringValue()); return true; }

      std::string Get() const;
      void Set(const std::string& v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSVec2Property
         : public PropertyGetterSetter
   {
   public:

      JSVec2Property(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::VEC2D; }

      virtual dtEntity::Vec2d Vec2dValue() const { return Get(); }
      virtual void SetVec2d(const dtEntity::Vec2d& v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::Vec2dProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::Vec2dProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::Vec2dProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.Vec2dValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.Vec2dValue()); return true; }

      dtEntity::Vec2d Get() const;
      void Set(const dtEntity::Vec2d& v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSVec3Property
         : public PropertyGetterSetter
   {
   public:

      JSVec3Property(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::VEC3D; }

      virtual dtEntity::Vec3d Vec3dValue() const { return Get(); }
      virtual void SetVec3d(const dtEntity::Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::Vec3dProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::Vec3dProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::Vec3dProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.Vec3dValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.Vec3dValue()); return true; }

      dtEntity::Vec3d Get() const;
      void Set(const dtEntity::Vec3d& v);
   };

   ////////////////////////////////////////////////////////////////////////////////
   class JSVec4Property
         : public PropertyGetterSetter
   {
   public:

      JSVec4Property(v8::Handle<v8::Object> holder, v8::Handle<v8::Function> getter, v8::Handle<v8::Function> setter);

      virtual dtEntity::DataType::e GetDataType() const { return dtEntity::DataType::VEC4D; }

      virtual dtEntity::Vec4d Vec4dValue() const { return Get(); }
      virtual void SetVec4d(const dtEntity::Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const { dtEntity::Vec4dProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { dtEntity::Vec4dProperty p; p.SetString(v); Set(p.Get());}
      virtual Property* Clone() const { return new dtEntity::Vec4dProperty(Get()); }
      virtual bool operator==(const dtEntity::Property& other) const { return other.Vec4dValue() == Get(); }
      virtual bool SetFrom(const dtEntity::Property& other) { Set(other.Vec4dValue()); return true; }

      dtEntity::Vec4d Get() const;
      void Set(const dtEntity::Vec4d& v);
   };
}
