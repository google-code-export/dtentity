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
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetBool(StringId name, bool val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetBool(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetDouble(StringId name, double val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetDouble(val);
      OnPropertyChanged(name, *prop);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetFloat(StringId name, float val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetFloat(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetGroup(StringId name, const PropertyGroup& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetGroup(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetInt(StringId name, int val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetInt(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetMatrix(StringId name, const Matrix& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetMatrix(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetQuat(StringId name, const Quat& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetQuat(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetString(StringId name, const std::string& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetString(val);      
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetStringId(StringId name, StringId val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetStringId(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetUInt(StringId name, unsigned int val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetUInt(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec2(StringId name, const Vec2f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3(StringId name, const Vec3f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4(StringId name, const Vec4f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec4(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec2d(StringId name, const Vec2d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2D(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3d(StringId name, const Vec3d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3D(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4d(StringId name, const Vec4d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec4D(val);
      OnPropertyChanged(name, *prop);
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
