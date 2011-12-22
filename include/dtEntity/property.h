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
#include <dtEntity/entityid.h>
#include <dtEntity/stringid.h>
#include <string>
#include <osg/Matrix>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <assert.h>
#include <vector>
#include <map>

namespace dtEntity
{
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
         CHAR,
         DOUBLE,
         FLOAT,         
         GROUP,
         INT,
         MATRIX,
         QUAT,
         STRING,
         STRINGID,
         UCHAR,
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
      DataType::e GetType() const { return mDataType; }
      
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
       * Get value of property interpreted as various types.
       * These default implementations cause an assertion error in debug mode
       * and return a null value in release mode.
       */
      virtual PropertyArray ArrayValue() const { assert(!"Not a bool property!"); return PropertyArray(); }
      virtual bool BoolValue() const { assert(!"Not a bool property!"); return false; }
      virtual char CharValue() const { assert(!"Not a char property!"); return 0; }
      virtual double DoubleValue() const { assert(!"Not a double property!"); return 0.0; }
      virtual float FloatValue() const { assert(!"Not a float property!"); return 0.0f; }
      virtual PropertyGroup GroupValue() const { return PropertyGroup(); }
      virtual int IntValue() const {assert(!"Not an int property!"); return 0; }
      virtual osg::Matrix MatrixValue() const {assert(!"Not a matrix property!"); return osg::Matrix(); }
      virtual osg::Quat QuatValue() const { assert(!"Not a quat property!"); return osg::Quat(); }
      virtual StringId StringIdValue() const { assert(!"Not a string id property!"); return 0; }
      virtual unsigned char UCharValue() const { assert(!"Not an uchar property!"); return 0; }
      virtual unsigned int UIntValue() const {assert(!"Not an unsigned int property!"); return 0; }
      virtual osg::Vec2f Vec2Value() const { assert(!"Not a vec2 property!"); return osg::Vec2f(); }
      virtual osg::Vec3f Vec3Value() const { assert(!"Not a vec3 property!"); return osg::Vec3f(); }
      virtual osg::Vec4f Vec4Value() const { assert(!"Not a vec4 property!"); return osg::Vec4f(); }
      virtual osg::Vec2d Vec2dValue() const { assert(!"Not a vec2d property!"); return osg::Vec2d(); }
      virtual osg::Vec3d Vec3dValue() const { assert(!"Not a vec3d property!"); return osg::Vec3d(); }
      virtual osg::Vec4d Vec4dValue() const { assert(!"Not a vec4d property!"); return osg::Vec4d(); }
      
      /**
       * To and from string have to be supplied for the editor gui and
       * possibly for map saving
       */
      virtual const std::string StringValue() const = 0;
      virtual void SetString(const std::string&) = 0;

      virtual void SetArray(const PropertyArray&) { assert(!"Not an array property!"); }
      virtual void SetBool(bool) { assert(!"Not a bool property!"); }
      virtual void SetChar(char) { assert(!"Not a char property!"); }
      virtual void SetDouble(double) { assert(!"Not a double property!"); }
      virtual void SetFloat(float) { assert(!"Not a float property!"); }
      virtual void SetGroup(const PropertyGroup&) { assert(!"Not a group property!"); }
      virtual void SetInt(int) { assert(!"Not an int property!"); }
      virtual void SetMatrix(const osg::Matrix&) { assert(!"Not a matrix property!"); }
      virtual void SetQuat(const osg::Quat&) { assert(!"Not a quat property!"); }
      virtual void SetStringId(dtEntity::StringId) { assert(!"Not a stringid property!"); }
      virtual void SetUChar(unsigned char) { assert(!"Not a uchar property!"); }
      virtual void SetUInt(unsigned int) { assert(!"Not a uint property!"); }
      virtual void SetVec2(const osg::Vec2&) { assert(!"Not a vec2 property!"); }
      virtual void SetVec3(const osg::Vec3&) { assert(!"Not a vec3 property!"); }
      virtual void SetVec4(const osg::Vec4&) { assert(!"Not a vec4 property!"); }
      virtual void SetVec2D(const osg::Vec2d&) { assert(!"Not a vec2D property!"); }
      virtual void SetVec3D(const osg::Vec3d&) { assert(!"Not a vec3D property!"); }
      virtual void SetVec4D(const osg::Vec4d&) { assert(!"Not a vec4D property!"); }

   protected:

      /**
       * Subclasses have to pass the property type here,
       * otherwise subclassing is not possible
       */
      Property(DataType::e dtype)
         : mDataType(dtype)
      {
      }

   private:
      
      DataType::e mDataType;
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

      virtual PropertyArray ArrayValue() const;
      virtual void SetArray(const PropertyArray& v) { Set(v); }
      virtual const std::string StringValue() const;
      virtual osg::Vec2f Vec2Value() const;
      virtual osg::Vec3f Vec3Value() const;
      virtual osg::Vec4f Vec4Value() const;
      virtual osg::Vec2d Vec2dValue() const;
      virtual osg::Vec3d Vec3dValue() const;
      virtual osg::Vec4d Vec4dValue() const;
      virtual osg::Quat QuatValue() const;
      
   
      PropertyArray Get() const { return mValue; }

      virtual Property* Clone() const ;
      virtual bool operator==(const Property& other) const;

      void Set(const PropertyArray& v);

      // causes an assertion failure when called
      virtual void SetString(const std::string&);
      virtual void SetVec2(const osg::Vec2&);
      virtual void SetVec3(const osg::Vec3&);
      virtual void SetVec4(const osg::Vec4&);
      virtual void SetVec2D(const osg::Vec2d&);
      virtual void SetVec3D(const osg::Vec3d&);
      virtual void SetVec4D(const osg::Vec4d&);

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
      void Insert(Property* prop, unsigned int index);

      /**
       * Remove from array and delete prop
       * Remove true if prop was found in array
       */
      bool Remove(Property* prop);

      /**
       * Delete all held properties
       */
      void Clear();

      unsigned int Size() const { return mValue.size(); }

   private:
      
      PropertyArray mValue;
   };

    //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT BoolProperty : public Property
   {
   public:
      BoolProperty(bool v = false);

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
   class DT_ENTITY_EXPORT CharProperty : public Property
   {
   public:

      CharProperty(char v = '0');

      virtual char CharValue() const;
      virtual void SetChar(char v) { Set(v); }
      virtual const std::string StringValue() const;

      char Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(char v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      char mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT DoubleProperty : public Property
   {
   public:
      DoubleProperty(double v = 0);

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

      virtual float FloatValue() const;
      virtual double DoubleValue() const;
      virtual void SetFloat(float v) { Set(v); }
      virtual void SetDouble(double v) { Set((float)v); }
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
      void Clear();

      virtual PropertyGroup GroupValue() const;
      virtual void SetGroup(const PropertyGroup& v) { Set(v); }
      virtual const std::string StringValue() const;
      PropertyGroup Get() const { return mValue; }

      // add prop and take ownership
      void Add(StringId name, Property* prop);
      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;

      void Set(const PropertyGroup& v);
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
     PropertyGroup mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT IntProperty : public Property
   {
   public:
      IntProperty(int v = 0);

      virtual int IntValue() const;
      virtual void SetInt(int v) { Set(v); }
      virtual unsigned int UIntValue() const;
      virtual void SetUInt(unsigned int v) { Set((int)v); }

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
      MatrixProperty(const osg::Matrix& v = osg::Matrix());

      virtual osg::Matrix MatrixValue() const;
      virtual void SetMatrix(const osg::Matrix& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Matrix Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Matrix& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      osg::Matrix mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT QuatProperty : public Property
   {
   public:
      QuatProperty(const osg::Quat& v = osg::Quat(0, 0, 0, 1));

      virtual osg::Quat QuatValue() const;
      virtual const std::string StringValue() const;
      virtual void SetQuat(const osg::Quat& v) { Set(v); }
      osg::Quat Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Quat& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      osg::Quat mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT StringProperty : public Property
   {
   public:
      StringProperty(const std::string& v = "");

      virtual const std::string StringValue() const;
      virtual StringId StringIdValue() const;
      virtual void SetStringId(dtEntity::StringId v) { Set(GetStringFromSID(v)); }

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
   class DT_ENTITY_EXPORT UCharProperty : public Property
   {
   public:

      UCharProperty(unsigned char v = '0');

      virtual unsigned char UCharValue() const;
      virtual void SetUChar(unsigned char v) { Set(v); }
      virtual const std::string StringValue() const;

      unsigned char Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(unsigned char v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      unsigned char mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT UIntProperty : public Property
   {
   public:
      UIntProperty(unsigned int v = 0);

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
      Vec2Property(const osg::Vec2f& v = osg::Vec2f());

      virtual osg::Vec2f Vec2Value() const;
      virtual void SetVec2(const osg::Vec2& v) { Set(v); }
      virtual osg::Vec2d Vec2dValue() const;
      virtual void SetVec2D(const osg::Vec2d& v) { Set(v); }
      
      virtual const std::string StringValue() const;
      osg::Vec2f Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec2f& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      osg::Vec2f mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec3Property : public Property
   {
   public:
      Vec3Property(const osg::Vec3f& v = osg::Vec3f());
      
      virtual osg::Vec3f Vec3Value() const;
      virtual void SetVec3(const osg::Vec3& v) { Set(v); }
      virtual osg::Vec3d Vec3dValue() const;
      virtual void SetVec3D(const osg::Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Vec3f Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec3f& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      osg::Vec3f mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec4Property : public Property
   {
   public:
      Vec4Property(const osg::Vec4f& v = osg::Vec4f());

      virtual osg::Vec4f Vec4Value() const;
      virtual void SetVec4(const osg::Vec4& v) { Set(v); }
      virtual osg::Vec4d Vec4dValue() const;
      virtual void SetVec4D(const osg::Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Vec4f Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec4f& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      osg::Vec4f mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec2dProperty : public Property
   {
   public:
      Vec2dProperty(const osg::Vec2d& v = osg::Vec2d());

      virtual osg::Vec2f Vec2Value() const;
      virtual void SetVec2(const osg::Vec2& v) { Set(v); }
      virtual osg::Vec2d Vec2dValue() const;
      virtual void SetVec2D(const osg::Vec2d& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Vec2d Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec2d& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:

      osg::Vec2d mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec3dProperty : public Property
   {
   public:
      Vec3dProperty(const osg::Vec3d& v = osg::Vec3d());

      virtual osg::Vec3f Vec3Value() const;
      virtual void SetVec3(const osg::Vec3& v) { Set(v); }
      virtual osg::Vec3d Vec3dValue() const;
      virtual void SetVec3D(const osg::Vec3d& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Vec3d Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec3d& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      osg::Vec3d mValue;
   };

   //////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT Vec4dProperty : public Property
   {
   public:
      Vec4dProperty(const osg::Vec4d& v = osg::Vec4d());

      virtual osg::Vec4f Vec4Value() const;
      virtual void SetVec4(const osg::Vec4& v) { Set(v); }
      virtual osg::Vec4d Vec4dValue() const;
      virtual void SetVec4D(const osg::Vec4d& v) { Set(v); }
      virtual const std::string StringValue() const;
      osg::Vec4d Get() const { return mValue; }

      virtual Property* Clone() const;
      virtual bool operator==(const Property& other) const;
      void Set(const osg::Vec4d& v) { mValue = v; }
      virtual void SetString(const std::string&);
      virtual bool SetFrom(const Property& other);

   private:
      osg::Vec4d mValue;
   };

}
