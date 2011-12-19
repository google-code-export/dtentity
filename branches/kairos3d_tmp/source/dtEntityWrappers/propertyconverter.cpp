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

#include <dtEntityWrappers/propertyconverter.h>

#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappermanager.h>

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace dtEntity;
using namespace v8;

namespace dtEntityWrappers
{
   dtEntity::Property* CreatePropFromType(dtEntity::DataType::e t)
   {
      switch(t)
      {
      case DataType::ARRAY:       return new ArrayProperty();
      case DataType::BOOL:        return new BoolProperty();
      case DataType::CHAR:        return new CharProperty();
      case DataType::DOUBLE:      return new DoubleProperty(); 
      case DataType::FLOAT:       return new FloatProperty(); 
      case DataType::GROUP:       return new GroupProperty(); 
      case DataType::INT:         return new IntProperty(); 
      case DataType::MATRIX:      return new MatrixProperty(); 
      case DataType::QUAT:        return new QuatProperty(); 
      case DataType::STRING:      return new StringProperty(); 
      case DataType::STRINGID:    return new StringIdProperty(); 
      case DataType::UCHAR:       return new UCharProperty();
      case DataType::UINT:        return new UIntProperty();
      case DataType::VEC2:        return new Vec2Property(); 
      case DataType::VEC3:        return new Vec3Property(); 
      case DataType::VEC4:        return new Vec4Property(); 
      case DataType::VEC2D:       return new Vec2dProperty();
      case DataType::VEC3D:       return new Vec3dProperty();
      case DataType::VEC4D:       return new Vec4dProperty();
      default: return NULL;
      }      
   }

   v8::Handle<v8::Value> PropToVal(const dtEntity::Property* prop)
   {
      using namespace v8;

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      {
         dtEntity::PropertyArray arr = prop->ArrayValue();
         HandleScope scope;
         Handle<Context> context = GetGlobalContext();
         Context::Scope context_scope(context);

         Handle<Array> out = Array::New(arr.size());

         for(unsigned int i = 0; i < arr.size(); ++i)
         {
            out->Set(Integer::New(i), PropToVal(arr[i]));
         }
         
         return scope.Close(out);
      }
      case DataType::BOOL:     return Boolean::New(prop->BoolValue());
      case DataType::DOUBLE:   return Number::New(prop->DoubleValue());
      case DataType::FLOAT:    return Number::New(prop->FloatValue());
      case DataType::GROUP:    
      {
         dtEntity::PropertyGroup grp = prop->GroupValue();
         HandleScope scope;
         Handle<Context> context = GetGlobalContext();
         Context::Scope context_scope(context);

         Handle<Object> out = Object::New();

         for(dtEntity::PropertyGroup::iterator i = grp.begin(); i != grp.end(); ++i)
         {
            out->Set(String::New(dtEntity::GetStringFromSID(i->first).c_str()), PropToVal(i->second));
         }
         
         return scope.Close(out);
      }
      case DataType::INT:      return Int32::New(prop->IntValue());
      case DataType::UINT:     return Uint32::New(prop->UIntValue());
      case DataType::QUAT:     return WrapQuat(prop->QuatValue());
      case DataType::MATRIX:   return WrapMatrix(prop->MatrixValue());
      case DataType::STRING:   return String::New(prop->StringValue().c_str());
      case DataType::STRINGID: return String::New(dtEntity::GetStringFromSID(prop->StringIdValue()).c_str());
      case DataType::VEC2:
      case DataType::VEC2D:    return WrapVec2(prop->Vec2dValue());
      case DataType::VEC3:
      case DataType::VEC3D:    return WrapVec3(prop->Vec3dValue());
      case DataType::VEC4:
      case DataType::VEC4D:    return WrapVec4(prop->Vec4dValue());
      default:
         return ThrowError("Type not yet wrapped!");
      }
   }

   dtEntity::Property* Convert(v8::Handle<v8::Value> val)
   {
      if(val->IsArray())
      {
         ArrayProperty* prop = new ArrayProperty();
         Handle<Array> arr = Handle<Array>::Cast(val);
         for(unsigned int i = 0; i < arr->Length(); ++i)
         {
            prop->Add(Convert(arr->Get(Integer::New(i))));
         }
         return prop;
      }
      else if(val->IsObject())
      {
         GroupProperty* prp = new GroupProperty();

         Handle<Object> obj = Handle<Object>::Cast(val);
         Handle<Array> keys = obj->GetPropertyNames();

         for(unsigned int i = 0; i < keys->Length(); ++i)
         {
            Handle<Value> key = keys->Get(Integer::New(i));
            std::string keyname = ToStdString(key);
            Handle<Value> val = obj->Get(key);
            prp->Add(dtEntity::SID(keyname), Convert(val));
         }
         return prp;
      }
      else if(val->IsBoolean())
      {
         return new BoolProperty(val->BooleanValue());
      }
      else if(val->IsString())
      {
         return new StringProperty(ToStdString(val));
      }
      else if(val->IsUint32())
      {
         return new UIntProperty(val->Uint32Value());
      }
      else if(val->IsInt32())
      {
         return new IntProperty(val->Int32Value());
      }      
      else if(val->IsNumber())
      {
         return new DoubleProperty(val->NumberValue());
      }
      
      
      return NULL;
   }

   v8::Handle<v8::Value> ValToProp(v8::Handle<v8::Value> val, dtEntity::Property*& prop)
   {
      using namespace v8;

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      {
         if(!val->IsArray()) 
         {
            return ThrowError("array property expects an array!");
         }
         dtEntity::ArrayProperty* arrayprop = static_cast<dtEntity::ArrayProperty*>(prop);
         arrayprop->Clear();
         dtEntity::PropertyArray vals;
         Handle<Array> arr = Handle<Array>::Cast(val);
         for(unsigned int i = 0; i < arr->Length(); ++i)
         {
            Handle<Value> val = arr->Get(Integer::New(i));
            dtEntity::Property* newprop = Convert(val);
            if(newprop == NULL)
            {
               return ThrowError("Unknown array datatype encountered!");
            }
            arrayprop->Add(newprop);
         }
         break;
      }
      case dtEntity::DataType::BOOL:
         prop->SetBool(val->BooleanValue()); break;
      case dtEntity::DataType::DOUBLE:
         prop->SetDouble(val->NumberValue()); break;
      case dtEntity::DataType::FLOAT:
         prop->SetFloat(val->NumberValue()); break;
      case dtEntity::DataType::GROUP:
      {
         if(!val->IsObject()) 
         {
            return ThrowError("group property expects an object!");
         }
         dtEntity::GroupProperty* groupprop = static_cast<dtEntity::GroupProperty*>(prop);
         groupprop->Clear();
         Handle<Object> obj = Handle<Object>::Cast(val);
         Handle<Array> propnames = obj->GetPropertyNames();
         
         for(unsigned int i = 0; i < propnames->Length(); ++i)
         {
            Handle<Value> key = propnames->Get(Integer::New(i));
            std::string kname = ToStdString(key);
            Handle<Value> val = obj->Get(key);
            dtEntity::Property* newprop = Convert(val);
            if(newprop == NULL)
            {
               return ThrowError("Unknown datatype encountered!");
            }
            groupprop->Add(dtEntity::SID(kname), newprop);
         }

         break;
      }
      case dtEntity::DataType::INT:
         prop->SetInt(val->Int32Value()); break;
      case dtEntity::DataType::UINT:
         prop->SetUInt(val->Uint32Value()); break;
      case dtEntity::DataType::MATRIX:
      {
         if(!IsMatrix(val))
         {
            return ThrowError("Property only accepts matrix values!");
         }
         osg::Matrix mat = UnwrapMatrix(val);
         prop->SetMatrix(mat); break;
      }
      case dtEntity::DataType::QUAT:
      {
        if(!IsQuat(val))
        {
           return ThrowError("Property only accepts quat values!");
        }
         osg::Quat q = UnwrapQuat(val);
         prop->SetQuat(q); break;
      }
      case dtEntity::DataType::STRING:
         prop->SetString(ToStdString(val)); break;
      case dtEntity::DataType::STRINGID:
         prop->SetStringId(dtEntity::SID(ToStdString(val))); break;
      case dtEntity::DataType::VEC2:
      {
        if(!IsVec2(val))
        {
           return ThrowError("Property only accepts vec2 values!");
        }
         osg::Vec2d vec = UnwrapVec2(val);
         prop->SetVec2(vec); break;
      }
      case dtEntity::DataType::VEC3:
      {
         if(!IsVec3(val))
         {
            return ThrowError("Property only accepts vec3 values!" + ToStdString(val));
         }
         osg::Vec3d vec = UnwrapVec3(val);
         prop->SetVec3(vec); break;
      }
      case dtEntity::DataType::VEC4:
      {
         if(!IsVec4(val))
         {
            return ThrowError("Property only accepts vec4 values!");
         }
         osg::Vec4d vec = UnwrapVec4(val);
         prop->SetVec4(vec); break;
      }
      case dtEntity::DataType::VEC2D:
      {
        if(!IsVec2(val))
        {
           return ThrowError("Property only accepts vec2 values!");
        }
         osg::Vec2d vec = UnwrapVec2(val);
         prop->SetVec2D(vec); break;
      }
      case dtEntity::DataType::VEC3D:
      {
         if(!IsVec3(val))
         {
            return ThrowError("Property only accepts vec3 values!" + ToStdString(val));
         }
         osg::Vec3d vec = UnwrapVec3(val);
         prop->SetVec3D(vec); break;
      }
      case dtEntity::DataType::VEC4D:
      {
         if(!IsVec4(val))
         {
            return ThrowError("Property only accepts vec4 values!");
         }
         osg::Vec4d vec = UnwrapVec4(val);
         prop->SetVec4D(vec); break;
      }

      default:
         return ThrowError("Type not yet wrapped!");
      }

      return True();
   }
}
