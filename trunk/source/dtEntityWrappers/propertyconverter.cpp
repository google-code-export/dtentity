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
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace dtEntity;
using namespace v8;

namespace dtEntityWrappers
{

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> ConvertPropertyToValue(v8::Handle<v8::Context> context, const dtEntity::Property* prop)
   {
      using namespace v8;

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      {
         dtEntity::PropertyArray arr = prop->ArrayValue();
         HandleScope scope;
         Context::Scope context_scope(context);

         Handle<Array> out = Array::New(arr.size());

         for(unsigned int i = 0; i < arr.size(); ++i)
         {
            out->Set(Integer::New(i), ConvertPropertyToValue(context, arr[i]));
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
         Context::Scope context_scope(context);

         Handle<Object> out = Object::New();

         for(dtEntity::PropertyGroup::iterator i = grp.begin(); i != grp.end(); ++i)
         {
            out->Set(ToJSString(dtEntity::GetStringFromSID(i->first)), ConvertPropertyToValue(context, i->second));
         }
         
         return scope.Close(out);
      }
      case DataType::INT:      return Int32::New(prop->IntValue());
      case DataType::UINT:     return Uint32::New(prop->UIntValue());
      case DataType::QUAT:     return WrapQuat(prop->QuatValue());
      case DataType::MATRIX:   return WrapMatrix(prop->MatrixValue());
      case DataType::STRING:   return ToJSString(prop->StringValue());
      case DataType::STRINGID: return ToJSString(dtEntity::GetStringFromSID(prop->StringIdValue()));
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

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* ConvertValueToProperty(v8::Handle<v8::Value> val)
   {
      if(val->IsArray())
      {

         HandleScope scope;
         Handle<Array> arr = Handle<Array>::Cast(val);
         unsigned int len = arr->Length();

         Handle<Value> hint = arr->Get(String::New("__TYPE_HINT"));
         if(!hint.IsEmpty())
         {
            std::string h = ToStdString(hint);
            if(h == "V2")
            {
               double x = 0, y = 0;
               if(len > 0) x = arr->Get(0)->NumberValue();
               if(len > 1) y = arr->Get(1)->NumberValue();
               return new Vec2dProperty(x, y);
            }
            else if(h == "V3")
            {
               double x = 0, y = 0, z = 0;
               if(len > 0) x = arr->Get(0)->NumberValue();
               if(len > 1) y = arr->Get(1)->NumberValue();
               if(len > 2) z = arr->Get(2)->NumberValue();
               return new Vec3dProperty(x, y, z);

            }
            else if(h == "V4")
            {
               double x = 0, y = 0, z = 0, w = 0;
               if(len > 0) x = arr->Get(0)->NumberValue();
               if(len > 1) y = arr->Get(1)->NumberValue();
               if(len > 2) z = arr->Get(2)->NumberValue();
               if(len > 3) w = arr->Get(3)->NumberValue();
               return new Vec4dProperty(x, y, z, w);
            }
            else if(h == "QT")
            {
               double x = 0, y = 0, z = 0, w = 0;
               if(len > 0) x = arr->Get(0)->NumberValue();
               if(len > 1) y = arr->Get(1)->NumberValue();
               if(len > 2) z = arr->Get(2)->NumberValue();
               if(len > 3) w = arr->Get(3)->NumberValue();
               return new QuatProperty(x, y, z, w);
            }
            else if(h == "MT")
            {
               Handle<Array> a0 = Handle<Array>::Cast(arr->Get(0));
               Handle<Array> a1 = Handle<Array>::Cast(arr->Get(1));
               Handle<Array> a2 = Handle<Array>::Cast(arr->Get(2));
               Handle<Array> a3 = Handle<Array>::Cast(arr->Get(3));

               double p1 = a0->Get(0)->NumberValue();
               double p2 = a0->Get(1)->NumberValue();
               double p3 = a0->Get(2)->NumberValue();
               double p4 = a0->Get(3)->NumberValue();
               double p5 = a1->Get(0)->NumberValue();
               double p6 = a1->Get(1)->NumberValue();
               double p7 = a1->Get(2)->NumberValue();
               double p8 = a1->Get(3)->NumberValue();
               double p9 = a2->Get(0)->NumberValue();
               double p10 = a2->Get(1)->NumberValue();
               double p11 = a2->Get(2)->NumberValue();
               double p12 = a2->Get(3)->NumberValue();
               double p13 = a3->Get(0)->NumberValue();
               double p14 = a3->Get(1)->NumberValue();
               double p15 = a3->Get(2)->NumberValue();
               double p16 = a3->Get(3)->NumberValue();
               osg::Matrix m(p1,p2,p3,p4,p5,p6,p7,p8,
                             p9,p10,p11,p12,p13,p14,p15,p16);
               return new MatrixProperty(m);
            }
         }
         ArrayProperty* prop = new ArrayProperty();
         for(unsigned int i = 0; i < arr->Length(); ++i)
         {
            prop->Add(ConvertValueToProperty(arr->Get(Integer::New(i))));
         }
         return prop;
      }
      else if(val->IsObject())
      {
         GroupProperty* prp = new GroupProperty();

         HandleScope scope;
         Handle<Object> obj = Handle<Object>::Cast(val);
         Handle<Array> keys = obj->GetPropertyNames();

         for(unsigned int i = 0; i < keys->Length(); ++i)
         {
            Handle<Value> key = keys->Get(Integer::New(i));
            std::string keyname = ToStdString(key);
            Handle<Value> val = obj->Get(key);
            prp->Add(dtEntity::SIDHash(keyname), ConvertValueToProperty(val));
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

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> SetPropertyFromValue(v8::Handle<v8::Value> val, dtEntity::Property*& prop)
   {
      using namespace v8;

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      {

         dtEntity::ArrayProperty* arrayprop = static_cast<dtEntity::ArrayProperty*>(prop);
         arrayprop->Clear();
         dtEntity::PropertyArray vals;
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("array property expects an array!");
         }
         for(unsigned int i = 0; i < arr->Length(); ++i)
         {
            Handle<Value> val = arr->Get(Integer::New(i));
            dtEntity::Property* newprop = ConvertValueToProperty(val);
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

         dtEntity::GroupProperty* groupprop = static_cast<dtEntity::GroupProperty*>(prop);
         groupprop->Clear();
         Handle<Object> obj = Handle<Object>::Cast(val);
         if(obj.IsEmpty())
         {
            return ThrowError("group property expects an object!");
         }
         Handle<Array> propnames = obj->GetPropertyNames();
         
         for(unsigned int i = 0; i < propnames->Length(); ++i)
         {
            Handle<Value> key = propnames->Get(Integer::New(i));
            std::string kname = ToStdString(key);
            Handle<Value> val = obj->Get(key);
            dtEntity::Property* newprop = ConvertValueToProperty(val);
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

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> SetValueFromProperty(dtEntity::Property*& prop, v8::Handle<v8::Value> val)
   {
      using namespace v8;

      switch(prop->GetType())
      {
      case dtEntity::DataType::ARRAY:
      {
         dtEntity::ArrayProperty* arrayprop = static_cast<dtEntity::ArrayProperty*>(prop);

         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("array property expects an array!");
         }

         while(arr->Length() > arrayprop->Size())
         {
            arr->Delete(arr->Length() - 1);
         }

         for(unsigned int i = 0; i < arrayprop->Size(); ++i)
         {
            const Property* prp = arrayprop->Get(i);
            arr->Set(i, ConvertPropertyToValue(arr->CreationContext(), prp));
         }
         break;
      }
      case dtEntity::DataType::GROUP:
      {


         dtEntity::GroupProperty* groupprop = static_cast<dtEntity::GroupProperty*>(prop);

         Handle<Object> obj = Handle<Object>::Cast(val);
         if(obj.IsEmpty())
         {
            return ThrowError("group property expects an object!");
         }

         while(obj->Has(0))
         {
            obj->Delete(0);
         }

         dtEntity::PropertyGroup vals = groupprop->Get();
         for(dtEntity::PropertyGroup::const_iterator i = vals.begin(); i != vals.end(); ++i)
         {
            obj->Set(String::New(dtEntity::GetStringFromSID(i->first).c_str()), ConvertPropertyToValue(obj->CreationContext(), i->second));
         }
         break;
      }
      case dtEntity::DataType::MATRIX:
      {
         osg::Matrix mat = prop->MatrixValue();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts matrix values!");
         }
         arr->Set( 0, Number::New(mat(0,0)));
         arr->Set( 1, Number::New(mat(0,1)));
         arr->Set( 2, Number::New(mat(0,2)));
         arr->Set( 3, Number::New(mat(0,3)));
         arr->Set( 4, Number::New(mat(1,0)));
         arr->Set( 5, Number::New(mat(1,1)));
         arr->Set( 6, Number::New(mat(1,2)));
         arr->Set( 7, Number::New(mat(1,3)));
         arr->Set( 8, Number::New(mat(2,0)));
         arr->Set( 9, Number::New(mat(2,1)));
         arr->Set(10, Number::New(mat(2,2)));
         arr->Set(11, Number::New(mat(2,3)));
         arr->Set(12, Number::New(mat(3,0)));
         arr->Set(13, Number::New(mat(3,1)));
         arr->Set(14, Number::New(mat(3,2)));
         arr->Set(15, Number::New(mat(3,3)));
         break;
      }
      case dtEntity::DataType::QUAT:
      {
         osg::Quat v = prop->QuatValue();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts quat values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         arr->Set( 2, Number::New(v[2]));
         arr->Set( 3, Number::New(v[3]));
         break;
      }
      case dtEntity::DataType::VEC2:
      {
         osg::Vec2 v = prop->Vec2Value();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec2 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         break;
      }
      case dtEntity::DataType::VEC3:
      {
         osg::Vec3 v = prop->Vec3Value();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec3 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         arr->Set( 2, Number::New(v[2]));
         break;
      }
      case dtEntity::DataType::VEC4:
      {
         osg::Vec4 v = prop->Vec4Value();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec3 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         arr->Set( 2, Number::New(v[2]));
         arr->Set( 3, Number::New(v[3]));
         break;
      }
      case dtEntity::DataType::VEC2D:
      {
         osg::Vec2d v = prop->Vec2dValue();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec2 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         break;
      }
      case dtEntity::DataType::VEC3D:
      {
         osg::Vec3d v = prop->Vec3dValue();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec3 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         arr->Set( 2, Number::New(v[2]));
         break;
      }
      case dtEntity::DataType::VEC4D:
      {
         osg::Vec4d v = prop->Vec4dValue();
         Handle<Array> arr = Handle<Array>::Cast(val);
         if(arr.IsEmpty())
         {
            return ThrowError("Property only accepts Vec3 values!");
         }
         arr->Set( 0, Number::New(v[0]));
         arr->Set( 1, Number::New(v[1]));
         arr->Set( 2, Number::New(v[2]));
         arr->Set( 3, Number::New(v[3]));
         break;
      }

      default:
         assert(false && "Cannot set non-aray js values from property!");
         return ThrowError("Cannot set non-aray js values from property!");
      }

      return True();
   }
}
