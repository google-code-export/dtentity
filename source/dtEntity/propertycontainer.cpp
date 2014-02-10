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

#include <dtEntity/propertycontainer.h>

#include <dtEntity/property.h>
#include <assert.h>

namespace dtEntity
{
  
  
   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::InitFrom(const PropertyContainer& other)
   {
      for(PropertyGroup::const_iterator i = other.mValue.begin(); i != other.mValue.end(); ++i)
      {
         Property* own = Get(i->first);
         if(own == NULL)
         {
            LOG_ERROR("Error in InitFrom: PropertyContainer has no property named " << GetStringFromSID(i->first));
         }
         else
         {
            own->SetFrom(*i->second);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::Register(StringId name, Property* prop)
   {
      assert(!Has(name) && "Property already registered!");
      Add(name, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetArray(StringId name, const PropertyArray& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetArray(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetBool(StringId name, bool val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetBool(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetDouble(StringId name, double val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetDouble(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetFloat(StringId name, float val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetFloat(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetGroup(StringId name, const PropertyGroup& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetGroup(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetInt(StringId name, int val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetInt(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetMatrix(StringId name, const Matrix& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetMatrix(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetQuat(StringId name, const Quat& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetQuat(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetString(StringId name, const std::string& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetString(val);      
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetStringId(StringId name, StringId val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetStringId(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetUInt(StringId name, unsigned int val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetUInt(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec2(StringId name, const Vec2f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3(StringId name, const Vec3f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4(StringId name, const Vec4f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec4(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec2d(StringId name, const Vec2d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2D(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3d(StringId name, const Vec3d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3D(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4d(StringId name, const Vec4d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec4D(val);
#if CALL_ONPROPERTYCHANGED_METHOD
      OnPropertyChanged(name, *prop);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyArray PropertyContainer::GetArray(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->ArrayValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::GetBool(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->BoolValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   double PropertyContainer::GetDouble(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->DoubleValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   float PropertyContainer::GetFloat(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->FloatValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyGroup PropertyContainer::GetGroup(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->GroupValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int PropertyContainer::GetInt(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->IntValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Quat PropertyContainer::GetQuat(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->QuatValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Matrix PropertyContainer::GetMatrix(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->MatrixValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string PropertyContainer::GetString(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->StringValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringId PropertyContainer::GetStringId(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->StringIdValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int PropertyContainer::GetUInt(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->UIntValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec2f PropertyContainer::GetVec2(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec2Value();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Vec3f PropertyContainer::GetVec3(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec3Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4f PropertyContainer::GetVec4(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec4Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec2d PropertyContainer::GetVec2d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec2dValue();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Vec3d PropertyContainer::GetVec3d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec3dValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4d PropertyContainer::GetVec4d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec4dValue();
   }
}
