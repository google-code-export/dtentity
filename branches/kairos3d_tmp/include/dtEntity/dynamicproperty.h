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
   class DT_ENTITY_EXPORT DynamicStringProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const std::string&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<std::string> GetValueCB;

      DynamicStringProperty(const SetValueCB& s, const GetValueCB& g)
         : Property(DataType::STRING)
         , mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

      virtual const std::string StringValue() const { return mGetValueCallback(); }
      virtual StringId StringIdValue() const { return dtEntity::SID(StringValue()); }
      virtual void SetStringId(dtEntity::StringId v) { Set(GetStringFromSID(v)); }

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
         : Property(DataType::VEC3D)
         , mSetValueCallback(s)
         , mGetValueCallback(g)
      {
      }

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
   class DT_ENTITY_EXPORT DynamicQuatProperty : public Property
   {
   public:
      typedef fastdelegate::FastDelegate1< const osg::Quat&, void> SetValueCB;
      typedef fastdelegate::FastDelegate0<osg::Quat> GetValueCB;

      DynamicQuatProperty(const SetValueCB& s, const GetValueCB& g)
         : Property(DataType::QUAT)
         , mSetValueCallback(s)
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

      DynamicMatrixProperty(const SetValueCB& s, const GetValueCB& g)
         : Property(DataType::MATRIX)
         , mSetValueCallback(s)
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
