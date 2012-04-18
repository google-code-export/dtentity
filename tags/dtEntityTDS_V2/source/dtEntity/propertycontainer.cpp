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
   const Property* PropertyContainer::Get(StringId name) const
   {
      PropertyMap::const_iterator it = mProperties.find(name);
      if(it == mProperties.end())
      {
         return NULL;
      }
      else
      {
         return it->second;
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::Empty() const
   {
      return mProperties.empty();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* PropertyContainer::Get(StringId name)
   {
      PropertyMap::iterator it = mProperties.find(name);
      if(it == mProperties.end())
      {
         return NULL;
      }
      else
      {
         return it->second;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::GetProperties(PropertyMap& toFill)
   {
      for(PropertyMap::iterator i = mProperties.begin(); i != mProperties.end(); ++i)
      {
         toFill.insert(*i);
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::GetProperties(ConstPropertyMap& toFill) const
   {
      for(PropertyMap::const_iterator i = mProperties.begin(); i != mProperties.end(); ++i)
      {
         toFill.insert(*i);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::Has(StringId name) const
   {
      return mProperties.find(name) != mProperties.end();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::InitFrom(const PropertyContainer& other)
   {
      for(PropertyMap::const_iterator i = other.mProperties.begin(); i != other.mProperties.end(); ++i)
      {
         Property* own = Get(i->first);
         if(own != NULL)
         {
            own->SetFrom(*i->second);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::Register(StringId name, Property* prop)
   {
      assert(Get(name) == NULL && "Property already registered!");
      mProperties[name] = prop;
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
   void PropertyContainer::SetMatrix(StringId name, const osg::Matrix& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetMatrix(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetQuat(StringId name, const osg::Quat& val)
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
   void PropertyContainer::SetVec2(StringId name, const osg::Vec2f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3(StringId name, const osg::Vec3f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4(StringId name, const osg::Vec4f& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec4(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec2d(StringId name, const osg::Vec2d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec2D(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec3d(StringId name, const osg::Vec3d& val)
   {
      assert(Get(name));
      Property* prop = Get(name);
      prop->SetVec3D(val);
      OnPropertyChanged(name, *prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::SetVec4d(StringId name, const osg::Vec4d& val)
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
   osg::Quat PropertyContainer::GetQuat(StringId name) const
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
   osg::Vec2f PropertyContainer::GetVec2(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec2Value();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3f PropertyContainer::GetVec3(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec3Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4f PropertyContainer::GetVec4(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec4Value();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d PropertyContainer::GetVec2d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec2dValue();
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d PropertyContainer::GetVec3d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec3dValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4d PropertyContainer::GetVec4d(StringId name) const
   {
      const Property* prop = Get(name);
      assert(prop && "Property with that name does not exist!");
      return prop->Vec4dValue();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   DynamicPropertyContainer::DynamicPropertyContainer()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DynamicPropertyContainer::DynamicPropertyContainer(const DynamicPropertyContainer& other)
   {
      PropertyMap::const_iterator i;
      for(i = other.mProperties.begin(); i != other.mProperties.end(); ++i)
      {
         this->AddProperty(i->first, *i->second);
      }
   }
    
   ////////////////////////////////////////////////////////////////////////////////
   DynamicPropertyContainer::~DynamicPropertyContainer()
   {
      for(PropertyMap::iterator i = mProperties.begin(); i != mProperties.end(); ++i)
      {
         delete i->second;
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainer::operator=(const DynamicPropertyContainer& other)
   {
      if(!mProperties.empty())
      {
         for(PropertyMap::iterator i = mProperties.begin(); i != mProperties.end(); ++i)
         {
            delete i->second;
         }
         mProperties.clear();
      }
      PropertyMap::const_iterator j;
      for(j = other.mProperties.begin(); j != other.mProperties.end(); ++j)
      {
         this->AddProperty(j->first, *j->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   DynamicPropertyContainer& DynamicPropertyContainer::operator+=(const DynamicPropertyContainer& other)
   {
      PropertyMap::const_iterator i;
      for(i = other.mProperties.begin(); i != other.mProperties.end(); ++i)
      {
         this->AddProperty(i->first, *i->second);
      }
      return *this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainer::AddProperty(StringId name, const Property& p)
   {
      DeleteProperty(name);
      Register(name, p.Clone());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicPropertyContainer::DeleteProperty(StringId name)
   {
      PropertyMap::iterator i = mProperties.find(name);
      if(i == mProperties.end())
      {
         return false;
      }
      delete i->second;
      mProperties.erase(i);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainer::SetProperties(const ConstPropertyMap& props)
   {
	   while(!mProperties.empty())
	   {
	      this->DeleteProperty(mProperties.begin()->first);
	   }

      PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         this->AddProperty(i->first, *i->second);
      }
   }
}
