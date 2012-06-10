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

#include <dtEntity/property.h>
#include <assert.h>
#include <sstream>
#include <osg/io_utils>
#include <dtEntity/log.h>
#include <climits>

namespace dtEntity
{
  
   ////////////////////////////////////////////////////////////////////////////////
   namespace DataType
   {
      std::string ToString(e dtype)
      {
         switch(dtype)
         {
         case ARRAY:       return "ARRAY";
         case BOOL:        return "BOOL";
         case DOUBLE:      return "DOUBLE";
         case FLOAT:       return "FLOAT";
         case GROUP:       return "GROUP";
         case INT:         return "INT";
         case MATRIX:      return "MATRIX";
         case QUAT:        return "QUAT";
         case STRING:      return "STRING";
         case STRINGID:    return "STRINGID";
         case UINT:        return "UINT";
         case VEC2:        return "VEC2";
         case VEC3:        return "VEC3";
         case VEC4:        return "VEC4";
         case VEC2D:        return "VEC2D";
         case VEC3D:        return "VEC3D";
         case VEC4D:        return "VEC4D";
         default:          return "UNKNOWN";
         }
      }
           

      e FromString(const std::string& s)
      {
         if(s == "ARRAY")        return ARRAY;
         if(s == "BOOL")         return BOOL;
         if(s == "DOUBLE")       return DOUBLE;
         if(s == "FLOAT")        return FLOAT;
         if(s == "GROUP")        return GROUP;
         if(s == "INT")          return INT;
         if(s == "MATRIX")       return MATRIX;
         if(s == "QUAT")         return QUAT;
         if(s == "STRING")       return STRING;
         if(s == "STRINGID")     return STRINGID;
         if(s == "UINT")         return UINT;
         if(s == "VEC2")         return VEC2;
         if(s == "VEC3")         return VEC3;
         if(s == "VEC4")         return VEC4;
         if(s == "VEC2D")        return VEC2D;
         if(s == "VEC3D")        return VEC3D;
         if(s == "VEC4D")        return VEC4D;
         return UNKNOWN_ID;
      }
   }

   // from http://stackoverflow.com/questions/236129/how-to-split-a-string
   std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
       std::stringstream ss(s);
       std::string item;
       while(std::getline(ss, item, delim)) {
           elems.push_back(item);
       }
       return elems;
   }


   std::vector<std::string> split(const std::string &s, char delim) {
       std::vector<std::string> elems;
       return split(s, delim, elems);
   }

   template <class T>
   bool fromString(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
   {
     std::istringstream iss(s);
     return !(iss >> f >> t).fail();
   }


   ////////////////////////////////////////////////////////////////////////////////
   ArrayProperty::ArrayProperty(const PropertyArray& v)
   {
      Set(v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArrayProperty::~ArrayProperty()
   {
      Clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyArray ArrayProperty::ArrayValue() const 
   { 
      return Get(); 
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f ArrayProperty::Vec2Value() const
   {
      if(mValue.size() < 2)
      {
         LOG_ERROR("Not enough entries in array for Vec2Value!");
         return osg::Vec2f();
      }
      return osg::Vec2f(mValue[0]->FloatValue(), mValue[1]->FloatValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3f ArrayProperty::Vec3Value() const
   {
      if(mValue.size() < 3)
      {
         LOG_ERROR("Not enough entries in array for Vec3Value!");
         return osg::Vec3f();
      }
      return osg::Vec3f(mValue[0]->FloatValue(), mValue[1]->FloatValue(), 
         mValue[2]->FloatValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4f ArrayProperty::Vec4Value() const
   {
      if(mValue.size() < 4)
      {
         LOG_ERROR("Not enough entries in array for Vec4Value!");
         return osg::Vec4f();
      }
      return osg::Vec4f(mValue[0]->FloatValue(), mValue[1]->FloatValue(),
         mValue[2]->FloatValue(), mValue[3]->FloatValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d ArrayProperty::Vec2dValue() const
   {
      if(mValue.size() < 2)
      {
         LOG_ERROR("Not enough entries in array for Vec2dValue!");
         return osg::Vec2f();
      }
      return osg::Vec2d(mValue[0]->DoubleValue(), mValue[1]->DoubleValue());
   }
   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d ArrayProperty::Vec3dValue() const
   {
      if(mValue.size() < 3)
      {
         LOG_ERROR("Not enough entries in array for Vec3dValue!");
         return osg::Vec3d();
      }
      return osg::Vec3d(mValue[0]->DoubleValue(), mValue[1]->DoubleValue(), 
         mValue[2]->DoubleValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4d ArrayProperty::Vec4dValue() const
   {
      if(mValue.size() < 4)
      {
         LOG_ERROR("Not enough entries in array for Vec4dValue!");
         return osg::Vec4d();
      }
      return osg::Vec4d(mValue[0]->DoubleValue(), mValue[1]->DoubleValue(),
         mValue[2]->DoubleValue(), mValue[3]->DoubleValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Quat ArrayProperty::QuatValue() const
   {
      if(mValue.size() < 4)
      {
         LOG_ERROR("Not enough entries in array for QuatValue!");
         return osg::Quat(0,0,0,1);
      }
      return osg::Quat(mValue[0]->DoubleValue(), mValue[1]->DoubleValue(),
         mValue[2]->DoubleValue(), mValue[3]->DoubleValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Matrix ArrayProperty::MatrixValue() const
   {
      if(mValue.size() < 16)
      {
         LOG_ERROR("Not enough entries in array for MatrixValue!");
         osg::Matrix m;
         m.makeIdentity();
         return m;
      }
      return osg::Matrix(
               mValue[ 0]->DoubleValue(),
               mValue[ 1]->DoubleValue(),
               mValue[ 2]->DoubleValue(),
               mValue[ 3]->DoubleValue(),
               mValue[ 4]->DoubleValue(),
               mValue[ 5]->DoubleValue(),
               mValue[ 6]->DoubleValue(),
               mValue[ 7]->DoubleValue(),
               mValue[ 8]->DoubleValue(),
               mValue[ 9]->DoubleValue(),
               mValue[10]->DoubleValue(),
               mValue[11]->DoubleValue(),
               mValue[12]->DoubleValue(),
               mValue[13]->DoubleValue(),
               mValue[14]->DoubleValue(),
               mValue[15]->DoubleValue()
               );
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string ArrayProperty::StringValue() const 
   { 
      return "Array"; 
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* ArrayProperty::Clone() const 
   {                 
      return new ArrayProperty(mValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ArrayProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const ArrayProperty& aother = static_cast<const ArrayProperty&>(other);
      return (aother.mValue == mValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::Set(const PropertyArray& v) 
   { 
      Clear();
      for(PropertyArray::const_iterator i = v.begin(); i != v.end(); ++i)
      {
         mValue.push_back((*i)->Clone());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetString(const std::string&)
   {
      assert(false && "Cannot set array from string");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec2(const osg::Vec2& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec3(const osg::Vec3& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
      Add(new DoubleProperty(v[2]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec4(const osg::Vec4& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
      Add(new DoubleProperty(v[2]));
      Add(new DoubleProperty(v[3]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec2D(const osg::Vec2d& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec3D(const osg::Vec3d& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
      Add(new DoubleProperty(v[2]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::SetVec4D(const osg::Vec4d& v)
   {
      Clear();
      Add(new DoubleProperty(v[0]));
      Add(new DoubleProperty(v[1]));
      Add(new DoubleProperty(v[2]));
      Add(new DoubleProperty(v[3]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ArrayProperty::SetFrom(const Property& other)
   {
      this->Set(other.ArrayValue());
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::Clear()
   {
      while(!mValue.empty())
      {
         Property* p = mValue.back();
         mValue.pop_back();
         delete p;
      }
   }  

   /////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::Add(Property* prop)
   {
      mValue.push_back(prop);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ArrayProperty::Insert(unsigned int index, Property* prop)
   {
      PropertyArray::iterator i = mValue.begin();
      i += index;
      mValue.insert(i, prop);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* ArrayProperty::Get(unsigned int index)
   {
      return mValue[index];
   }

   /////////////////////////////////////////////////////////////////////////////////
   const Property* ArrayProperty::Get(unsigned int index) const
   {
      return mValue[index];
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool ArrayProperty::Remove(Property* prop)
   {
      for(PropertyArray::iterator i = mValue.begin(); i != mValue.end(); ++i)
      {
         if((*i) == prop)
         {
            mValue.erase(i);
            delete *i;
            return true;
         }
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   BoolProperty::BoolProperty(bool v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool BoolProperty::BoolValue() const
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string BoolProperty::StringValue() const 
   { 
      return Get() ? "true" : "false"; 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* BoolProperty::Clone() const
   { 
      return new BoolProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool BoolProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const BoolProperty& aother = static_cast<const BoolProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BoolProperty::SetString(const std::string& s)
   {
      if(s.compare("true") == 0 || s.compare("TRUE") == 0)
         mValue = true;
      else
         mValue = false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool BoolProperty::SetFrom(const Property& other)
   {
      this->Set(other.BoolValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   FloatProperty::FloatProperty(float v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   float FloatProperty::FloatValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   double FloatProperty::DoubleValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string FloatProperty::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << Get();
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* FloatProperty::Clone() const 
   { 
      return new FloatProperty(mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool FloatProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const FloatProperty& aother = static_cast<const FloatProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void FloatProperty::SetString(const std::string& s)
   {
      fromString<float>(mValue, s, std::dec);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool FloatProperty::SetFrom(const Property& other)
   {
      this->Set(other.FloatValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   DoubleProperty::DoubleProperty(double v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   double DoubleProperty::DoubleValue() const 
   { 
      return Get(); 
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   float DoubleProperty::FloatValue() const 
   { 
      return (float)Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned int DoubleProperty::UIntValue() const
   {
      return (unsigned int) Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string DoubleProperty::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << Get();
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* DoubleProperty::Clone() const 
   {
      return new DoubleProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DoubleProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const DoubleProperty& aother = static_cast<const DoubleProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DoubleProperty::SetString(const std::string& s)
   {
      fromString<double>(mValue, s, std::dec);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DoubleProperty::SetFrom(const Property& other)
   {
      this->Set(other.DoubleValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   GroupProperty::GroupProperty(const PropertyGroup& v)
   {         
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   GroupProperty::GroupProperty(const GroupProperty& other)
   {
      const PropertyGroup& o = other.Get();
      for(PropertyGroup::const_iterator i = o.begin(); i != o.end(); ++i)
      {
         StringId key = (*i).first;
         Property* val = (*i).second->Clone();
         Add(key, val);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   GroupProperty::~GroupProperty()
   {
      Clear();
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyGroup GroupProperty::GroupValue() const 
   { 
      return mValue;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string GroupProperty::StringValue() const 
   { 
      return "Group"; 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* GroupProperty::Clone() const 
   {
      return new GroupProperty(mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::operator=(const GroupProperty& other)
   {
      operator+=(other);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool GroupProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const GroupProperty& aother = static_cast<const GroupProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::operator+=(const GroupProperty& other)
   {
      const PropertyGroup& o = other.Get();
      for(PropertyGroup::const_iterator i = o.begin(); i != o.end(); ++i)
      {
         StringId key = (*i).first;
         std::string k = GetStringFromSID(key);
         Property* val = (*i).second->Clone();
         Add(key, val);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool GroupProperty::Empty() const
   {
      return mValue.empty();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::Set(const PropertyGroup& v) 
   { 
      Clear();
      for(PropertyGroup::const_iterator i = v.begin(); i != v.end(); ++i)
      {
         StringId key = (*i).first;
         Property* val = (*i).second->Clone();
         mValue[key] = val;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::Add(StringId name, Property* prop)
   {
      PropertyGroup::iterator i = mValue.find(name);
      if(i != mValue.end())
      {
         delete i->second;
      }
      mValue[name] = prop;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::SetString(const std::string& s)
   {
      assert(false && "Cannot set group from string!");
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool GroupProperty::SetFrom(const Property& other)
   {
      this->Set(other.GroupValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void GroupProperty::Clear()
   {
      while(!mValue.empty())
      {
         std::pair<StringId, Property*> pair = *mValue.begin();
         mValue.erase(mValue.begin());
         delete pair.second;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* GroupProperty::Get(StringId id)
   {
      PropertyGroup::iterator i = mValue.find(id);
      if(i == mValue.end())
      {
         return NULL;
      }
      return i->second;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool GroupProperty::Has(StringId id) const
   {
      return mValue.find(id) != mValue.end();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const Property* GroupProperty::Get(StringId id) const
   {
      PropertyGroup::const_iterator i = mValue.find(id);
      if(i == mValue.end())
      {
         return NULL;
      }
      return i->second;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   UIntProperty::UIntProperty(unsigned int v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned int UIntProperty::UIntValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   int UIntProperty::IntValue() const 
   { 
#ifdef _DEBUG
      if(Get() > INT_MAX)
      {
         LOG_ERROR("Error casting uint value to int: Too big!");
      }
#endif
      return (int)Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   float UIntProperty::FloatValue() const
   {
      return (float) Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   double UIntProperty::DoubleValue() const
   {
      return (double) Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string UIntProperty::StringValue() const 
   { 
      std::ostringstream os; os << Get();
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* UIntProperty::Clone() const 
   { 
      return new UIntProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool UIntProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const UIntProperty& aother = static_cast<const UIntProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void UIntProperty::SetString(const std::string& s)
   {
      fromString<unsigned int>(mValue, s, std::dec);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void UIntProperty::SetDouble(double v)
   {
      LOG_WARNING("Setting unsigned int property from double value!");
      Set((unsigned int) v);
   }
   /////////////////////////////////////////////////////////////////////////////////
   void UIntProperty::SetFloat(float v)
   {
      LOG_WARNING("Setting unsigned int property from float value!");
      Set((unsigned int) v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool UIntProperty::SetFrom(const Property& other)
   {
      this->Set(other.UIntValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   MatrixProperty::MatrixProperty(const osg::Matrix& v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Matrix MatrixProperty::MatrixValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string MatrixProperty::StringValue() const 
   { 
      osg::Matrix m = Get();

      std::ostringstream os;
      os.precision(10);
      os << std::fixed;
      os << m(0,0) << " " << m(0,1) << " " << m(0,2) << " " << m(0,3) << " ";
      os << m(1,0) << " " << m(1,1) << " " << m(1,2) << " " << m(1,3) << " ";
      os << m(2,0) << " " << m(2,1) << " " << m(2,2) << " " << m(2,3) << " ";
      os << m(3,0) << " " << m(3,1) << " " << m(3,2) << " " << m(3,3);

      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* MatrixProperty::Clone() const 
   { 
      return new MatrixProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool MatrixProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const MatrixProperty& aother = static_cast<const MatrixProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void MatrixProperty::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 16)
      {
         float v0, v1, v2, v3;
         float v4, v5, v6, v7;
         float v8, v9, v10, v11;
         float v12, v13, v14, v15;
         fromString<float>(v0, l[0], std::dec);
         fromString<float>(v1, l[1], std::dec);
         fromString<float>(v2, l[2], std::dec);
         fromString<float>(v3, l[3], std::dec);
         fromString<float>(v4, l[4], std::dec);
         fromString<float>(v5, l[5], std::dec);
         fromString<float>(v6, l[6], std::dec);
         fromString<float>(v7, l[7], std::dec);
         fromString<float>(v8, l[8], std::dec);
         fromString<float>(v9, l[9], std::dec);
         fromString<float>(v10, l[10], std::dec);
         fromString<float>(v11, l[11], std::dec);
         fromString<float>(v12, l[12], std::dec);
         fromString<float>(v13, l[13], std::dec);
         fromString<float>(v14, l[14], std::dec);
         fromString<float>(v15, l[15], std::dec);

         mValue = osg::Matrix(
            v0, v1, v2, v3,
            v4, v5, v6, v7,
            v8, v9, v10, v11,
            v12, v13, v14, v15
         );
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool MatrixProperty::SetFrom(const Property& other)
   {
      this->Set(other.MatrixValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   StringProperty::StringProperty(const std::string& v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string StringProperty::StringValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   StringId StringProperty::StringIdValue() const 
   { 
      return SID(Get()); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* StringProperty::Clone() const 
   { 
      return new StringProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool StringProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         if(other.GetDataType() == DataType::STRINGID)
         {
            return (SID(mValue) == other.StringIdValue());
         }
         return false;
      }
      const StringProperty& aother = static_cast<const StringProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void StringProperty::SetString(const std::string& v) 
   {
      mValue = v; 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool StringProperty::SetFrom(const Property& other)
   {
      switch(other.GetDataType())
      {
      case DataType::STRING:
      
         this->Set(other.StringValue());
         return true;
      
      case DataType::STRINGID:
         LOG_ERROR("Setting String Property from StringID property. Costly!");
         this->Set(GetStringFromSID(other.StringIdValue()));
         return true;
      default: return false;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   QuatProperty::QuatProperty()
   {
      mValues[0] = 0;
      mValues[1] = 1;
      mValues[2] = 2;
      mValues[3] = 3;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QuatProperty::QuatProperty(const osg::Quat& v)
   {         
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QuatProperty::QuatProperty(double x, double y, double z, double w)
   {
      mValues[0] = x;
      mValues[1] = y;
      mValues[2] = z;
      mValues[3] = w;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QuatProperty::QuatProperty(const double* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Quat QuatProperty::QuatValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string QuatProperty::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1] << " "<< mValues[2] << " "<< mValues[3];
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* QuatProperty::Clone() const 
   { 
      return new QuatProperty(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool QuatProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const QuatProperty& aother = static_cast<const QuatProperty&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1] &&
              aother.mValues[2] == mValues[2] &&
              aother.mValues[3] == mValues[3]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void QuatProperty::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 4)
      {
         fromString<double>(mValues[0], l[0], std::dec);
         fromString<double>(mValues[1], l[1], std::dec);
         fromString<double>(mValues[2], l[2], std::dec);
         fromString<double>(mValues[3], l[3], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool QuatProperty::SetFrom(const Property& other)
   {
      this->Set(other.QuatValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   StringIdProperty::StringIdProperty(StringId v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   StringId StringIdProperty::StringIdValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string StringIdProperty::StringValue() const 
   { 
      return GetStringFromSID(Get()); 
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   Property* StringIdProperty::Clone() const 
   { 
      return new StringIdProperty(mValue); 
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   bool StringIdProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         if(other.GetDataType() == DataType::STRING)
         {
            StringId str = other.StringIdValue();
            return (str == mValue);
         }
         return false;
      }
      const StringIdProperty& aother = static_cast<const StringIdProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void StringIdProperty::SetString(const std::string& s)
   {
      mValue = SID(s);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool StringIdProperty::SetFrom(const Property& other)
   {
      this->Set(other.StringIdValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   IntProperty::IntProperty(int v)
      : mValue(v)
   {         
   }

   /////////////////////////////////////////////////////////////////////////////////
   int IntProperty::IntValue() const 
   { 
      return Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned int IntProperty::UIntValue() const 
   { 
#ifdef _DEBUG
      if(Get() < 0)
      {
         LOG_ERROR("Error casting int value to uint: Value is negative!");
      }
#endif
      return (int)Get(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   float IntProperty::FloatValue() const
   {
      return (float) Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   double IntProperty::DoubleValue() const
   {
      return (double) Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string IntProperty::StringValue() const 
   { 
      std::ostringstream os; os << Get();
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* IntProperty::Clone() const 
   { 
      return new IntProperty(mValue); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool IntProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const IntProperty& aother = static_cast<const IntProperty&>(other);
      return (aother.mValue == mValue);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void IntProperty::SetString(const std::string& s)
   {
      fromString<int>(mValue, s, std::dec);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void IntProperty::SetDouble(double v)
   {
      LOG_WARNING("Setting int property from double value!");
      Set((int) v);
   }
   /////////////////////////////////////////////////////////////////////////////////
   void IntProperty::SetFloat(float v)
   {
      LOG_WARNING("Setting int property from float value!");
      Set((int) v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool IntProperty::SetFrom(const Property& other)
   {
      this->Set(other.IntValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec2Property::Vec2Property()
   {
      mValues[0] = 0;
      mValues[1] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2Property::Vec2Property(const osg::Vec2& v)
   {         
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2Property::Vec2Property(float x, float y)
   {
      mValues[0] = x;
      mValues[1] = y;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2Property::Vec2Property(const float* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 Vec2Property::Vec2Value() const
   { 
      return Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec2Property::Vec2dValue() const
   {
      return osg::Vec2d(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec2Property::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1];
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec2Property::Clone() const 
   { 
      return new Vec2Property(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec2Property::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec2Property& aother = static_cast<const Vec2Property&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec2Property::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 2)
      {
         fromString<float>(mValues[0], l[0], std::dec);
         fromString<float>(mValues[1], l[1], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec2Property::SetFrom(const Property& other)
   {
      this->Set(other.Vec2Value());
      return true;      
   }
   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec3Property::Vec3Property()
   {
      mValues[0] = 0;
      mValues[1] = 0;
      mValues[2] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3Property::Vec3Property(const osg::Vec3& v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3Property::Vec3Property(float x, float y, float z)
   {
      mValues[0] = x;
      mValues[1] = y;
      mValues[2] = z;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3Property::Vec3Property(const float* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Vec3Property::Vec3Value() const
   { 
      return osg::Vec3(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d Vec3Property::Vec3dValue() const
   {
      return osg::Vec3d(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f Vec3Property::Vec2Value() const
   {
      return osg::Vec2f(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec3Property::Vec2dValue() const
   {
      return osg::Vec2d(mValues[0], mValues[1]);
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec3Property::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1] << " "<< mValues[2];
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec3Property::Clone() const 
   { 
      return new Vec3Property(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec3Property::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec3Property& aother = static_cast<const Vec3Property&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1] &&
              aother.mValues[2] == mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec3Property::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 3)
      {
         fromString<float>(mValues[0], l[0], std::dec);
         fromString<float>(mValues[1], l[1], std::dec);
         fromString<float>(mValues[2], l[2], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec3Property::SetFrom(const Property& other)
   {
      this->Set(other.Vec3Value());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec4Property::Vec4Property()
   {
      mValues[0] = 0;
      mValues[1] = 0;
      mValues[2] = 0;
      mValues[3] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4Property::Vec4Property(const osg::Vec4& v)
   {         
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4Property::Vec4Property(float x, float y, float z, float w)
   {
      mValues[0] = x;
      mValues[1] = y;
      mValues[2] = z;
      mValues[3] = w;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4Property::Vec4Property(const float* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Vec4Property::Vec4Value() const
   { 
      return Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec4d Vec4Property::Vec4dValue() const
   {
      return osg::Vec4d(mValues[0], mValues[1], mValues[2], mValues[3]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f Vec4Property::Vec2Value() const
   {
      return osg::Vec2f(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec4Property::Vec2dValue() const
   {
      return osg::Vec2d(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3f Vec4Property::Vec3Value() const
   {
      return osg::Vec3f(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d Vec4Property::Vec3dValue() const
   {
      return osg::Vec3d(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec4Property::StringValue() const 
   { 
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1] << " "<< mValues[2] << " "<< mValues[3];
      return os.str(); 
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec4Property::Clone() const 
   { 
      return new Vec4Property(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec4Property::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec4Property& aother = static_cast<const Vec4Property&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1] &&
              aother.mValues[2] == mValues[2] &&
              aother.mValues[3] == mValues[3]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec4Property::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 4)
      {
         fromString<float>(mValues[0], l[0], std::dec);
         fromString<float>(mValues[1], l[1], std::dec);
         fromString<float>(mValues[2], l[2], std::dec);
         fromString<float>(mValues[3], l[3], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec4Property::SetFrom(const Property& other)
   {
      this->Set(other.Vec4Value());
      return true;      
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty::Vec2dProperty()
   {
      mValues[0] = 0;
      mValues[1] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty::Vec2dProperty(const osg::Vec2d& v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty::Vec2dProperty(double x, double y)
   {
      mValues[0] = x;
      mValues[1] = y;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty::Vec2dProperty(const double* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f Vec2dProperty::Vec2Value() const
   {
      return osg::Vec2f(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec2dProperty::Vec2dValue() const
   {
      return Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec2dProperty::StringValue() const
   {
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1];
      return os.str();
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec2dProperty::Clone() const
   {
      return new Vec2dProperty(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec2dProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec2dProperty& aother = static_cast<const Vec2dProperty&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec2dProperty::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 2)
      {         
         fromString<double>(mValues[0], l[0], std::dec);
         fromString<double>(mValues[1], l[1], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec2dProperty::SetFrom(const Property& other)
   {
      this->Set(other.Vec2dValue());
      return true;
   }
   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty::Vec3dProperty()
   {
      mValues[0] = 0;
      mValues[1] = 0;
      mValues[2] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty::Vec3dProperty(const osg::Vec3d& v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty::Vec3dProperty(double x, double y, double z)
   {
      mValues[0] = x;
      mValues[1] = y;
      mValues[2] = z;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty::Vec3dProperty(const double* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3f Vec3dProperty::Vec3Value() const
   {
      return osg::Vec3f(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d Vec3dProperty::Vec3dValue() const
   {
      return Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f Vec3dProperty::Vec2Value() const
   {
      return osg::Vec2f(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec3dProperty::Vec2dValue() const
   {
      return osg::Vec2d(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec3dProperty::StringValue() const
   {
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1] << " "<< mValues[2];
      return os.str();
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec3dProperty::Clone() const
   {
      return new Vec3dProperty(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec3dProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec3dProperty& aother = static_cast<const Vec3dProperty&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1] &&
              aother.mValues[2] == mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec3dProperty::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 3)
      {
         fromString<double>(mValues[0], l[0], std::dec);
         fromString<double>(mValues[1], l[1], std::dec);
         fromString<double>(mValues[2], l[2], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec3dProperty::SetFrom(const Property& other)
   {
      this->Set(other.Vec3dValue());
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty::Vec4dProperty()
   {
      mValues[0] = 0;
      mValues[1] = 0;
      mValues[2] = 0;
      mValues[3] = 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty::Vec4dProperty(const osg::Vec4d& v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty::Vec4dProperty(double x, double y, double z, double w)
   {
      mValues[0] = x;
      mValues[1] = y;
      mValues[2] = z;
      mValues[3] = w;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty::Vec4dProperty(const double* v)
   {
      Set(v);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec4f Vec4dProperty::Vec4Value() const
   {
      return osg::Vec4f(mValues[0], mValues[1], mValues[2], mValues[3]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec4d Vec4dProperty::Vec4dValue() const
   {
      return Get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2f Vec4dProperty::Vec2Value() const
   {
      return osg::Vec2f(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d Vec4dProperty::Vec2dValue() const
   {
      return osg::Vec2d(mValues[0], mValues[1]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3f Vec4dProperty::Vec3Value() const
   {
      return osg::Vec3f(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d Vec4dProperty::Vec3dValue() const
   {
      return osg::Vec3d(mValues[0], mValues[1], mValues[2]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string Vec4dProperty::StringValue() const
   {
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << mValues[0] << " " << mValues[1] << " "<< mValues[2] << " "<< mValues[3];
      return os.str();
   }

   /////////////////////////////////////////////////////////////////////////////////
   Property* Vec4dProperty::Clone() const
   {
      return new Vec4dProperty(mValues);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec4dProperty::operator==(const Property& other) const
   {
      if(other.GetDataType() != GetDataType())
      {
         return false;
      }
      const Vec4dProperty& aother = static_cast<const Vec4dProperty&>(other);
      return (aother.mValues[0] == mValues[0] &&
              aother.mValues[1] == mValues[1] &&
              aother.mValues[2] == mValues[2] &&
              aother.mValues[3] == mValues[3]);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Vec4dProperty::SetString(const std::string& s)
   {
      std::vector<std::string> l = split(s, ' ');
      if(l.size() >= 4)
      {
         fromString<double>(mValues[0], l[0], std::dec);
         fromString<double>(mValues[1], l[1], std::dec);
         fromString<double>(mValues[2], l[2], std::dec);
         fromString<double>(mValues[3], l[3], std::dec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Vec4dProperty::SetFrom(const Property& other)
   {
      this->Set(other.Vec4dValue());
      return true;
   }
}
