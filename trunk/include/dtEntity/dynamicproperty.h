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
   class DT_ENTITY_EXPORT DynamicFloatProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<float, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<float> GetValueCB;

      DynamicFloatProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::FLOAT; }

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
   class DT_ENTITY_EXPORT DynamicDoubleProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<double, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<double> GetValueCB;

      DynamicDoubleProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::DOUBLE; }

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
   class DT_ENTITY_EXPORT DynamicUIntProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1<unsigned int, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<unsigned int> GetValueCB;

      DynamicUIntProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::UINT; }

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
   class DT_ENTITY_EXPORT DynamicStringProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const std::string&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<std::string> GetValueCB;

      DynamicStringProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::STRING; }

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
   class DT_ENTITY_EXPORT DynamicVec3dProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const osg::Vec3d&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<osg::Vec3d> GetValueCB;

      DynamicVec3dProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::VEC3D; }

      virtual osg::Vec3f Vec3Value() const { return Get(); }
      virtual void SetVec3(const osg::Vec3& v) { Set(v); }
      virtual osg::Vec3d Vec3dValue() const { return Get(); }
      virtual void SetVec3D(const osg::Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec3dProperty p(Get()); return p.StringValue(); }
      osg::Vec3d Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec3dProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec3dValue() == Get(); }
      void Set(const osg::Vec3d& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { Vec3dProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.Vec3dValue()); return true; }

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DynamicVec4Property : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const osg::Vec4&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<osg::Vec4> GetValueCB;

      DynamicVec4Property(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual DataType::e GetType() const { return DataType::VEC4; }

      virtual osg::Vec4f Vec4Value() const { return Get(); }
      virtual void SetVec4(const osg::Vec4& v) { Set(v); }
      virtual osg::Vec4d Vec4dValue() const { return Get(); }
      virtual void SetVec4D(const osg::Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const { Vec4Property p(Get()); return p.StringValue(); }
      osg::Vec4 Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new Vec4Property(Get()); }
      virtual bool operator==(const Property& other) const { return other.Vec4Value() == Get(); }
      void Set(const osg::Vec4& v) { mSetValueCallback(v); }
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
      typedef fastdelegate::FastDelegate1< const osg::Quat&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<osg::Quat> GetValueCB;

      virtual DataType::e GetType() const { return DataType::QUAT; }

      DynamicQuatProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual osg::Quat QuatValue() const { return Get(); }
      virtual void SetQuat(const osg::Quat& v) { Set(v); }
      virtual const std::string StringValue() const { QuatProperty p(Get()); return p.StringValue(); }
      osg::Quat Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new QuatProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.QuatValue() == Get(); }
      void Set(const osg::Quat& v) { mSetValueCallback(v); }
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
      typedef fastdelegate::FastDelegate1< const osg::Matrix&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<osg::Matrix> GetValueCB;

      virtual DataType::e GetType() const { return DataType::MATRIX; }

      DynamicMatrixProperty(const SetValueCB& s, const GetValueCB& g)
         : mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual osg::Matrix MatrixValue() const { return Get(); }
      virtual void SetMatrix(const osg::Matrix& v) { Set(v); }
      virtual const std::string StringValue() const { MatrixProperty p(Get()); return p.StringValue(); }
      osg::Matrix Get() const { return mGetValueCallback(); }

      virtual Property* Clone() const { return new MatrixProperty(Get()); }
      virtual bool operator==(const Property& other) const { return other.MatrixValue() == Get(); }
      void Set(const osg::Matrix& v) { mSetValueCallback(v); }
      virtual void SetString(const std::string& v) { MatrixProperty p; p.SetString(v); Set(p.Get());}
      virtual bool SetFrom(const Property& other) { Set(other.MatrixValue()); return true;}

   private:

      SetValueCB mSetValueCallback;
      GetValueCB mGetValueCallback;
   };

}
