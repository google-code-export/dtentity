#pragma once

/* -*-c++-*-
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

#include <dtEntity/export.h>
#include <dtEntity/property.h>
#include <dtEntity/FastDelegate.h>

namespace dtEntity
{
   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicArrayProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<const PropertyArray&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<PropertyArray> GetValueCB;

      DynamicArrayProperty() {}

      DynamicArrayProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::ARRAY; }

      virtual PropertyArray ArrayValue() const { return mGetValueCallback(); }
      virtual void SetArray(const PropertyArray& v) { Set(v); }

      virtual const std::string StringValue() const { ArrayProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { ArrayProperty p; p.SetString(v); Set(p.Get());}
      PropertyArray Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new ArrayProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.ArrayValue() == Get(); }
      void Set(const PropertyArray& v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.ArrayValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicBoolProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<bool, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<bool> GetValueCB;

      DynamicBoolProperty() {}

      DynamicBoolProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::BOOL; }

      virtual bool BoolValue() const { return mGetValueCallback(); }
      virtual void SetBool(bool v) { Set(v); }

      virtual const std::string StringValue() const { BoolProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { BoolProperty p; p.SetString(v); Set(p.Get());}
      bool Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new BoolProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.BoolValue() == Get(); }
      void Set(bool v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.BoolValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicDoubleProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<double, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<double> GetValueCB;

      DynamicDoubleProperty() {}

      DynamicDoubleProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::DOUBLE; }

      virtual double DoubleValue() const { return mGetValueCallback(); }
      virtual void SetDouble(double v) { Set(v); }

      virtual const std::string StringValue() const { DoubleProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { DoubleProperty p; p.SetString(v); Set(p.Get());}
      double Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new DoubleProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.DoubleValue() == Get(); }
      void Set(double v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.DoubleValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicFloatProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<float, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<float> GetValueCB;

      DynamicFloatProperty() {}

      DynamicFloatProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::FLOAT; }

      virtual float FloatValue() const { return mGetValueCallback(); }
      virtual void SetFloat(float v) { Set(v); }

      virtual const std::string StringValue() const { FloatProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { FloatProperty p; p.SetString(v); Set(p.Get());}
      float Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new FloatProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.FloatValue() == Get(); }
      void Set(float v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.FloatValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicGroupProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<const PropertyGroup&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<PropertyGroup> GetValueCB;

      DynamicGroupProperty() {}

      DynamicGroupProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::GROUP; }

      virtual PropertyGroup GroupValue() const { return mGetValueCallback(); }
      virtual void SetGroup(const PropertyGroup& v) { Set(v); }

      virtual const std::string StringValue() const { GroupProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { GroupProperty p; p.SetString(v); Set(p.Get());}
      PropertyGroup Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new GroupProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.GroupValue() == Get(); }
      void Set(const PropertyGroup& v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.GroupValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicIntProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<int, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<int> GetValueCB;

      DynamicIntProperty() {}

      DynamicIntProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::INT; }

      virtual int IntValue() const { return mGetValueCallback(); }
      virtual void SetInt(int v) { Set(v); }

      virtual const std::string StringValue() const { IntProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { IntProperty p; p.SetString(v); Set(p.Get());}
      int Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new IntProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.IntValue() == Get(); }
      void Set(int v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.IntValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicStringProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const std::string&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<std::string> GetValueCB;

      DynamicStringProperty() {}

      DynamicStringProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::STRING; }

      virtual const std::string StringValue() const { return mGetValueCallback(); }
      virtual StringId StringIdValue() const { return dtEntity::SIDHash(StringValue()); }
      //virtual void SetStringId(dtEntity::StringId v) { Set(GetStringFromSID(v)); }

      const std::string Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new StringProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.StringValue() == Get(); }
      void Set(const std::string& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Set(v); }
      virtual bool SetFrom(const Property& other) { Set(other.StringValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicStringIdProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<StringId, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<StringId> GetValueCB;

      DynamicStringIdProperty() {}

      DynamicStringIdProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::STRINGID; }

      virtual StringId StringIdValue() const { return mGetValueCallback(); }
      virtual void SetStringId(StringId v) { Set(v); }

      virtual const std::string StringValue() const { StringIdProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { StringIdProperty p; p.SetString(v); Set(p.Get());}
      StringId Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new StringIdProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.StringIdValue() == Get(); }
      void Set(StringId v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.StringIdValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicUIntProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<unsigned int, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<unsigned int> GetValueCB;

      DynamicUIntProperty() {}

      DynamicUIntProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::UINT; }

      virtual unsigned int UIntValue() const { return mGetValueCallback(); }
      virtual void SetUInt(unsigned int v) { Set(v); }

      virtual const std::string StringValue() const { UIntProperty p(Get()); return p.StringValue(); }
      virtual void SetString(const std::string& v) { UIntProperty p; p.SetString(v); Set(p.Get());}
      unsigned int Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new UIntProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.UIntValue() == Get(); }
      void Set(unsigned int v) { mSetValueCallback(v); }
      virtual bool SetFrom(const Property& other) { Set(other.UIntValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec2dProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Vec2d&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec2d> GetValueCB;

      DynamicVec2dProperty() {}

      DynamicVec2dProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC2D; }

      virtual Vec2f Vec2Value() const { return Get(); }
      virtual void SetVec2(const Vec2f& v) { Set(v); }
      virtual Vec2d Vec2dValue() const { return Get(); }
      virtual void SetVec2D(const Vec2d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec2dProperty p(Get()); return p.StringValue(); }
      Vec2d Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec2dProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec2dValue() == Get(); }
      void Set(const Vec2d& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec2dProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec2dValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec3dProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Vec3d&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec3d> GetValueCB;

      DynamicVec3dProperty() {}

      DynamicVec3dProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC3D; }

      virtual Vec3f Vec3Value() const { return Get(); }
      virtual void SetVec3(const Vec3f& v) { Set(v); }
      virtual Vec3d Vec3dValue() const { return Get(); }
      virtual void SetVec3D(const Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec3dProperty p(Get()); return p.StringValue(); }
      Vec3d Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec3dProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec3dValue() == Get(); }
      void Set(const Vec3d& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec3dProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec3dValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec4dProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Vec4d&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec4d> GetValueCB;

      DynamicVec4dProperty() {}

      DynamicVec4dProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC4D; }

      virtual Vec4f Vec4Value() const { return Get(); }
      virtual void SetVec4(const Vec4f& v) { Set(v); }
      virtual Vec4d Vec4dValue() const { return Get(); }
      virtual void SetVec4D(const Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec4dProperty p(Get()); return p.StringValue(); }
      Vec4d Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec4dProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec4dValue() == Get(); }
      void Set(const Vec4d& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec4dProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec4dValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec2Property : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<const Vec2f&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec2f> GetValueCB;

      DynamicVec2Property() {}

      DynamicVec2Property(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC2; }

      virtual Vec2f Vec2Value() const { return Get(); }
      virtual void SetVec2(const Vec2f& v) { Set(v); }
      virtual Vec2d Vec2dValue() const { return Get(); }
      virtual void SetVec2D(const Vec2d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec2Property p(Get()); return p.StringValue(); }
      Vec2f Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec2Property(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec2Value() == Get(); }
      void Set(const Vec2f& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec2Property p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec2Value()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec3Property : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Vec3f&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec3f> GetValueCB;

      DynamicVec3Property() {}

      DynamicVec3Property(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC3; }

      virtual Vec3f Vec3Value() const { return Get(); }
      virtual void SetVec3(const Vec3f& v) { Set(v); }
      virtual Vec3d Vec3dValue() const { return Get(); }
      virtual void SetVec3D(const Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec3Property p(Get()); return p.StringValue(); }
      Vec3f Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec3Property(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec3Value() == Get(); }
      void Set(const Vec3f& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec3Property p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec3Value()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec4Property : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Vec4f&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Vec4f> GetValueCB;

      DynamicVec4Property() {}

      DynamicVec4Property(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetDataType() const { return DataType::VEC4; }

      virtual Vec4f Vec4Value() const { return Get(); }
      virtual void SetVec4(const Vec4f& v) { Set(v); }
      virtual Vec4d Vec4dValue() const { return Get(); }
      virtual void SetVec4D(const Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec4Property p(Get()); return p.StringValue(); }
      Vec4f Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec4Property(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec4Value() == Get(); }
      void Set(const Vec4f& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec4Property p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec4Value()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicQuatProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const Quat&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Quat> GetValueCB;

      virtual DataType::e GetDataType() const { return DataType::QUAT; }

      DynamicQuatProperty() {}

      DynamicQuatProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual Quat QuatValue() const { return Get(); }
      virtual void SetQuat(const Quat& v) { Set(v); }
      virtual const std::string StringValue() const { QuatProperty p(Get()); return p.StringValue(); }
      Quat Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new QuatProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.QuatValue() == Get(); }
      void Set(const Quat& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { QuatProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.QuatValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicMatrixProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<const Matrix&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<Matrix> GetValueCB;

      virtual DataType::e GetDataType() const { return DataType::MATRIX; }

      DynamicMatrixProperty() {}

      DynamicMatrixProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual Matrix MatrixValue() const { return Get(); }
      virtual void SetMatrix(const Matrix& v) { Set(v); }
      virtual const std::string StringValue() const { MatrixProperty p(Get()); return p.StringValue(); }
      Matrix Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new MatrixProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.MatrixValue() == Get(); }
      void Set(const Matrix& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { MatrixProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.MatrixValue()); return true;}

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

}
