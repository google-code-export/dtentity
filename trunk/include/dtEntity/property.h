#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <dtEntity/export.h>
#include <dtEntity/entityid.h>
#include <dtEntity/log.h>
#include <dtEntity/stringid.h>
#include <string>
#include <osg/Matrix>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec2d>
#include <osg/Vec3d>
#include <osg/Vec4d>
#include <vector>
#include <map>

namespace dtEntity
{

   typedef osg::Vec2f Vec2f;
   typedef osg::Vec3f Vec3f;
   typedef osg::Vec4f Vec4f;
   typedef osg::Vec2d Vec2d;
   typedef osg::Vec3d Vec3d;
   typedef osg::Vec4d Vec4d;
   typedef osg::Quat Quat;
   typedef osg::Matrix Matrix;

   /**
    * dtEntity::Property class is an abstract base class. Each implementation
    * is a wrapper for a (more or less) primitive data type like int, string, bool.
    * The methods ToString and FromString can be used for serialization and deserialization.
    * The property class has a variable mDataType which is used to identify the type
    * of the concrete implementation class.
    */

   /**
    * The data type IDs identifying the various property types
    */
   namespace DataType
   {
      enum e
      {
         UNKNOWN_ID = 0,
         ARRAY,
         BOOL,
         DOUBLE,
         FLOAT,         
         GROUP,
         INT,
         MATRIX,
         QUAT,
         STRING,
         STRINGID,
         UINT,
         VEC2,
         VEC3,
         VEC4,
         VEC2D,
         VEC3D,
         VEC4D
      };

      std::string DT_ENTITY_EXPORT ToString(e);
      e DT_ENTITY_EXPORT FromString(const std::string&);
   }
   
   // helper function to split string by char
   std::vector<std::string> DT_ENTITY_EXPORT split(const std::string &s, char delim);

   // Fwd declaration
   class Property;

   // value type of array property
   typedef std::vector<Property*> PropertyArray;

   // value type of group property
   typedef std::map<StringId, Property*> PropertyGroup;

   /**
    * Base class for properties. Implementations have to set the mDataType
    * variable on construction.
    */
   class DT_ENTITY_EXPORT Property
   {
   public:
      
      virtual ~Property() {}

      /**
       * Get data type of this property
       */
      virtual DataType::e GetDataType() const = 0;
      
      /**
       * Create a copy of this property on the heap
       */
      virtual Property* Clone() const = 0;

      /**
       * Set values of this property from another property.
       * Should always work if both properties have the same type,
       * Casting between property types should be possible case by case.
       * return true if value could be set.
       */
      virtual bool SetFrom(const Property& other) = 0;

      /**
       * Equality operator has to be provided. Is used to check if
       * values were changed from their default at serialization time
       */
      virtual bool operator==(const Property& other) const = 0;

      /**
       * To and from string have to be supplied for the editor gui and
       * possibly for map saving
       */
      virtual const std::string StringValue() const = 0;
      virtual void SetString(const std::string&) = 0;

      /**
       * Get value of property interpreted as various types.
       * These default implementations cause an assertion error in debug mode
       * and return a null value in release mode.
       */
      virtual PropertyArray ArrayValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to an array value!");
         return PropertyArray();
      }

      virtual bool BoolValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a bool value!");
         return false;
      }

      virtual char CharValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a char value!");
         return 0;
      }

      virtual double DoubleValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a double value!");
         return 0.0;
      }

      virtual float FloatValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a float value!");
         return 0.0f;
      }

      virtual PropertyGroup GroupValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a group value!");
         return PropertyGroup();
      }

      virtual int IntValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to an int value!");
         return 0;
      }

      virtual Matrix MatrixValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a matrix value!");
         return Matrix();
      }

      virtual Quat QuatValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a quat value!");
         return Quat();
      }

      virtual StringId StringIdValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a StringId value!");
         return 0;
      }

      virtual unsigned int UIntValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a UInt value!");
         return 0;
      }

      virtual Vec2f Vec2Value() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec2 value!");
         return Vec2f();
      }

      virtual Vec3f Vec3Value() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec3 value!");
         return Vec3f();
      }

      virtual Vec4f Vec4Value() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec4 value!");
         return Vec4f();
      }

      virtual Vec2d Vec2dValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec2d value!");
         return Vec2d();
      }

      virtual Vec3d Vec3dValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec3d value!");
         return Vec3d();
      }

      virtual Vec4d Vec4dValue() const
      {
         LOG_ERROR("Cannot convert " << DataType::ToString(GetDataType()) << " to a vec4d value!");
         return Vec4d();
      }
      

      virtual void SetArray(const PropertyArray&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to an array value!");
      }

      virtual void SetBool(bool)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to an bool value!");
      }

      virtual void SetChar(char)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a char value!");
      }

      virtual void SetDouble(double)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a double value!");
      }

      virtual void SetFloat(float)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a float value!");
      }

      virtual void SetGroup(const PropertyGroup&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a group value!");
      }

      virtual void SetInt(int)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to an int value!");
      }

      virtual void SetMatrix(const Matrix&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a matrix value!");
      }

      virtual void SetQuat(const Quat&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a quat value!");
      }

      virtual void SetStringId(dtEntity::StringId)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a StringId value!");
      }

      virtual void SetUInt(unsigned int)
      {
        LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a uint value!");
      }

      virtual void SetVec2(const Vec2f&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec2 value!");
      }

      virtual void SetVec3(const Vec3f&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec3 value!");
      }

      virtual void SetVec4(const Vec4f&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec4 value!");
      }

      virtual void SetVec2D(const Vec2d&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec2d value!");
      }

      virtual void SetVec3D(const Vec3d&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec3d value!");
      }

      virtual void SetVec4D(const Vec4d&)
      {
         LOG_ERROR("Cannot set " << DataType::ToString(GetDataType()) << " to a vec4d value!");
      }

   };

   ////////////////////////////////////////////////////////////////////////////////

   /**
    * holds an array of properties as pointers.
    * Takes ownership of its properties: all properties are deleted in
    * the destructor.
    */
   class DT_ENTITY_EXPORT ArrayProperty : public Property
   {
   public:
      ArrayProperty(const PropertyArray& v = PropertyArray());
     ~ArrayProperty();

     typedef PropertyArray::size_type size_type;

      virtual DataType::e GetDataType() const { return DataType::ARRAY; }

      virtual PropertyArray ArrayValue() const;
      virtual void SetArray(const PropertyArray& v) { Set(v); }
      virtual const std::string StringValue() const;
      virtual Vec2f Vec2Value() const;
      virtual Vec3f Vec3Value() const;
      virtual Vec4f Vec4Value() const;
      virtual Vec2d Vec2dValue() const;
      virtual Vec3d Vec3dValue() const;
      virtual Vec4d Vec4dValue() const;
      virtual Quat QuatValue() const;
      virtual Matrix MatrixValue() const;

      const PropertyArray& Get() const { return mValue; }

      virtual Property* Clone() const ;
      virtual bool operator==(const Property& other) const;

      void Set(const PropertyArray& v);

      // causes an assertion failure when called
      virtual void SetString(const std::string&);
      virtual void SetVec2(const Vec2f&);
      virtual void SetVec3(const Vec3f&);
      virtual void SetVec4(const Vec4f&);
      virtual void SetVec2D(const Vec2d&);
      virtual void SetVec3D(const Vec3d&);
      virtual void SetVec4D(const Vec4d&);

      virtual bool SetFrom(const Property& other);

      /**
       * Add to array. Ownership is taken by ArrayProperty -
       * property is deleted in destructor
       */
      void Add(Property* prop);

      /**
       * Insert into array at given index. Ownership is taken by ArrayProperty -
       * property is deleted in destructor
       */
      void Insert(size_type index, Property* prop);

      /**
       * @return property at given index
       */
      Property* Get(size_type index);
      const Property* Get(size_type index) const;

      /**
       * Remove from array and delete prop
       * Remove true if prop was found in array
       */
      bool Remove(Property* prop);

      /**
       * Delete all held properties
       */
      void Clear();

      size_type Size() const
      {
         return mValue.size();
      }

   private:
      
      PropertyArray mValue;
   };

    //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT BoolProperty : public Property
   {
   public:
      BoolProperty(bool v = false);

      virtual DataType::e GetDataType() const { return DataType::BOOL; }

      virtual bool BoolValue() const;
      virtual void SetBool(bool v) { Set(v); }

      virtual const std::string StringValue() const;
      const bool Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(bool v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      bool mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DoubleProperty : public Property
   {
   public:
      DoubleProperty(double v = 0);

      virtual DataType::e GetDataType() const { return DataType::DOUBLE; }

      virtual double DoubleValue() const;
      virtual float FloatValue() const;
      virtual unsigned int UIntValue() const;
      virtual void SetDouble(double v) { Set(v); }
      virtual void SetFloat(float v) { Set(v); }
      virtual const std::string StringValue() const;
      double Get() const { return mValue; }
      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(double v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      double mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT FloatProperty : public Property
   {
   public:
      FloatProperty(float v = 0);

      virtual DataType::e GetDataType() const { return DataType::FLOAT; }
      virtual float FloatValue() const;
      virtual double DoubleValue() const;
      virtual void SetFloat(float v) { Set(v); }
      virtual void SetDouble(double v) { Set(static_cast<float>(v)); }
      virtual const std::string StringValue() const;
      float Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(float v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      float mValue;
   };
 
   //////////////////////////////////////////////////////////////////
   /**
    * Takes ownership of added properties, they are deleted in the
    * destructor
    */
   class DT_ENTITY_EXPORT GroupProperty : public Property
   {
   public:
      GroupProperty(const PropertyGroup& v = PropertyGroup());
      ~GroupProperty();
      GroupProperty(const GroupProperty&);

      virtual DataType::e GetDataType() const { return DataType::GROUP; }

      void Clear();

      virtual PropertyGroup GroupValue() const;
      virtual void SetGroup(const PropertyGroup& v) { Set(v); }
      virtual const std::string StringValue() const;

      const PropertyGroup& Get() const { return mValue; }

      // add prop and take ownership
      void Add(StringId name, Property* prop);
      virtual Property* Clone() const;
      void operator=(const GroupProperty&);
      bool operator==(const Property& other) const;
      void operator+=(const GroupProperty& other);
      void Set(const PropertyGroup& v);
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);
      bool Empty() const;
      Property* Get(StringId);
      const Property* Get(StringId) const;
      bool Has(StringId) const;

   protected:
     PropertyGroup mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT IntProperty : public Property
   {
   public:
      IntProperty(int v = 0);

      virtual DataType::e GetDataType() const { return DataType::INT; }

      virtual int IntValue() const;
      virtual void SetInt(int v) { Set(v); }
      virtual unsigned int UIntValue() const;
      virtual void SetUInt(unsigned int v) { Set(static_cast<int>(v)); }

      virtual float FloatValue() const;
      virtual double DoubleValue() const;
      virtual void SetDouble(double);
      virtual void SetFloat(float);
      virtual const std::string StringValue() const;
      int Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(int v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);
   private:
      int mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT MatrixProperty : public Property
   {
   public:
      MatrixProperty(const Matrix& v = Matrix());

      virtual DataType::e GetDataType() const { return DataType::MATRIX; }

      virtual Matrix MatrixValue() const;
      virtual void SetMatrix(const Matrix& v) { Set(v); }
      virtual const std::string StringValue() const;
      const Matrix& Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Matrix& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      Matrix mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT QuatProperty : public Property
   {
   public:
      QuatProperty();
      QuatProperty(const Quat& v);
      QuatProperty(double x, double y, double z, double w);
      QuatProperty(const double* v);

      virtual DataType::e GetDataType() const { return DataType::QUAT; }

      virtual Quat QuatValue() const;
      virtual const std::string StringValue() const;
      virtual void SetQuat(const Quat& v) { Set(v); }

      const Quat Get() const { return Quat(mValues[0], mValues[1], mValues[2], mValues[3]); }

      // warning: not sure if this is safe on all platforms
      const Quat& GetAsQuat() const { return reinterpret_cast<const Quat&>(mValues); }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;

      void Set(const Quat& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      void Set(const double* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      const double* GetValues() const { return mValues; }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      double mValues[4];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT StringProperty : public Property
   {
   public:
      StringProperty(const std::string& v = "");

      virtual DataType::e GetDataType() const { return DataType::STRING; }

      virtual const std::string StringValue() const;
      virtual StringId StringIdValue() const;
      //virtual void SetStringId(dtEntity::StringId v) { Set(GetStringFromSID(v)); }

      const std::string Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const std::string& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      std::string mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT StringIdProperty : public Property
   {
   public:
      StringIdProperty(StringId v = StringId());

      virtual DataType::e GetDataType() const { return DataType::STRINGID; }

      virtual StringId StringIdValue() const;
      virtual void SetStringId(dtEntity::StringId v) { Set(v); }
      virtual const std::string StringValue() const;
      StringId Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(StringId v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      StringId mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT UIntProperty : public Property
   {
   public:
      UIntProperty(unsigned int v = 0);

      virtual DataType::e GetDataType() const { return DataType::UINT; }

      virtual unsigned int UIntValue() const;
      virtual void SetUInt(unsigned int v) { Set(v); }
      virtual int IntValue() const;
      virtual float FloatValue() const;
      virtual double DoubleValue() const;
      virtual const std::string StringValue() const;
      unsigned int Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(unsigned int v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual void SetDouble(double);
      virtual void SetFloat(float);
      virtual bool SetFrom(const Property& other);
   private:
      unsigned int mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec2Property : public Property
   {
   public:

      Vec2Property();
      Vec2Property(const Vec2f& v);
      Vec2Property(float x, float y);
      Vec2Property(const float* vals);

      virtual DataType::e GetDataType() const { return DataType::VEC2; }

      virtual Vec2f Vec2Value() const;
      virtual void SetVec2(const Vec2f& v) { Set(v); }
      virtual Vec2d Vec2dValue() const;
      virtual void SetVec2D(const Vec2d& v) { Set(v); }
      
      virtual const std::string StringValue() const;
      const Vec2f Get() const { return Vec2f(mValues[0], mValues[1]); }

      // warning: not sure if this is safe on all platforms
      const Vec2f& GetAsVec2() const { return reinterpret_cast<const Vec2f&>(mValues); }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;

      void Set(const Vec2f& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
      }

      void Set(const float* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const float* GetValues() const { return mValues; }

   private:

      float mValues[2];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec3Property : public Property
   {
   public:
      Vec3Property();
      Vec3Property(const Vec3f& v);
      Vec3Property(float x, float y, float z);
      Vec3Property(const float* vals);

      virtual DataType::e GetDataType() const { return DataType::VEC3; }
      
      virtual Vec3f Vec3Value() const;
      virtual void SetVec3(const Vec3f& v) { Set(v); }
      virtual Vec3d Vec3dValue() const;
      virtual void SetVec3D(const Vec3d& v) { Set(v); }

      virtual Vec2f Vec2Value() const;
      virtual Vec2d Vec2dValue() const;
      virtual const std::string StringValue() const;
      const Vec3f Get() const { return Vec3f(mValues[0], mValues[1], mValues[2]); }

      // warning: not sure if this is safe on all platforms
      const Vec3f& GetAsVec3() const { return reinterpret_cast<const Vec3f&>(mValues); }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Vec3f& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
      }

      void Set(const float* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const float* GetValues() const { return mValues; }


   private:
      float mValues[3];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec4Property : public Property
   {
   public:
      Vec4Property();
      Vec4Property(const Vec4f& v);
      Vec4Property(float x, float y, float z, float w);
      Vec4Property(const float* vals);

      virtual DataType::e GetDataType() const { return DataType::VEC4; }

      virtual Vec4f Vec4Value() const;
      virtual void SetVec4(const Vec4f& v) { Set(v); }
      virtual Vec4d Vec4dValue() const;
      virtual void SetVec4D(const Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const;

      const Vec4f Get() const { return Vec4f(mValues[0], mValues[1], mValues[2], mValues[3]); }

      // warning: not sure if this is safe on all platforms
      const Vec4f& GetAsVec4() const { return reinterpret_cast<const Vec4f&>(mValues); }

      virtual Vec2f Vec2Value() const;
      virtual Vec2d Vec2dValue() const;
      virtual Vec3f Vec3Value() const;
      virtual Vec3d Vec3dValue() const;

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Vec4f& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      void Set(const float* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const float* GetValues() const { return mValues; }

   private:
      float mValues[4];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec2dProperty : public Property
   {
   public:

      Vec2dProperty();
      Vec2dProperty(const Vec2d& v);
      Vec2dProperty(double x, double y);
      Vec2dProperty(const double* v);

      virtual DataType::e GetDataType() const { return DataType::VEC2D; }

      virtual Vec2f Vec2Value() const;
      virtual void SetVec2(const Vec2f& v) { Set(v); }
      virtual Vec2d Vec2dValue() const;
      virtual void SetVec2D(const Vec2d& v) { Set(v); }
      virtual const std::string StringValue() const;
      const Vec2d Get() const { return Vec2d(mValues[0], mValues[1]); }

      // warning: not sure if this is safe on all platforms
      const Vec2d& GetAsVec2d() const { return reinterpret_cast<const Vec2d&>(mValues); }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Vec2d& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
      }

      void Set(const double* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const double* GetValues() const { return mValues; }

   private:

      double mValues[2];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec3dProperty : public Property
   {
   public:

      Vec3dProperty();
      Vec3dProperty(const Vec3d& v);
      Vec3dProperty(double x, double y, double z);
      Vec3dProperty(const double* v);

      virtual DataType::e GetDataType() const { return DataType::VEC3D; }

      virtual Vec3f Vec3Value() const;
      virtual void SetVec3(const Vec3f& v) { Set(v); }
      virtual Vec3d Vec3dValue() const;
      virtual void SetVec3D(const Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const;
      const Vec3d Get() const { return Vec3d(mValues[0], mValues[1], mValues[2]); }

      // warning: not sure if this is safe on all platforms
      const Vec3d& GetAsVec3d() const { return reinterpret_cast<const Vec3d&>(mValues); }

      virtual Vec2f Vec2Value() const;
      virtual Vec2d Vec2dValue() const;

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Vec3d& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
      }

      void Set(const double* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const double* GetValues() const { return mValues; }

   private:
      double mValues[3];
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec4dProperty : public Property
   {
   public:

      Vec4dProperty();
      Vec4dProperty(const Vec4d& v);
      Vec4dProperty(double x, double y, double z, double w);
      Vec4dProperty(const double* v);

      virtual DataType::e GetDataType() const { return DataType::VEC4D; }

      virtual Vec4f Vec4Value() const;
      virtual void SetVec4(const Vec4f& v) { Set(v); }
      virtual Vec4d Vec4dValue() const;
      virtual void SetVec4D(const Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const;
      const Vec4d Get() const { return Vec4d(mValues[0], mValues[1], mValues[2], mValues[3]); }

      // warning: not sure if this is safe on all platforms
      const Vec4d& GetAsVec2d() const { return reinterpret_cast<const Vec4d&>(mValues); }

      virtual Vec2f Vec2Value() const;
      virtual Vec2d Vec2dValue() const;
      virtual Vec3f Vec3Value() const;
      virtual Vec3d Vec3dValue() const;

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const Vec4d& v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      void Set(const double* v)
      {
         mValues[0] = v[0];
         mValues[1] = v[1];
         mValues[2] = v[2];
         mValues[3] = v[3];
      }

      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

      const double* GetValues() const { return mValues; }

   private:
      double mValues[4];
   };

}
