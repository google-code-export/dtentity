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

#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/entityid.h>
#include <dtEntity/scriptaccessor.h>
#include <dtEntityWrappers/componentwrapper.h>
#include <dtEntityWrappers/propertyconverter.h>

#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{

   Persistent<FunctionTemplate> s_entitySystemTemplate;
   
   typedef std::map<dtEntity::ComponentType, Persistent<FunctionTemplate> > SubWrapperMap;
   SubWrapperMap s_subWrapperMap;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESToString(const Arguments& args)
   {
      return String::New("<EntitySystem>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESHasComponent(const Arguments& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());
      
      if(!args[0]->IsUint32())
      {
         return ThrowError("Usage: hasComponent(int)");
      }
      bool searchderived = false;
      if(args.Length() > 1)
      {
         searchderived = args[1]->BooleanValue();
      }
      if(es->GetEntityManager().HasComponent(args[0]->Uint32Value(), es->GetComponentType(), searchderived))
      {
         return True();
      }
      else
      {
         return False();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESGetComponent(const Arguments& args)
   {
      ScriptSystem* ss = static_cast<ScriptSystem*>(External::Unwrap(args.Data()));
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());
      if(!args[0]->IsUint32())
      {
         return ThrowError("Usage: getComponent(int entityid, [bool getDerived])");
      }

      dtEntity::EntityId eid = args[0]->Uint32Value();
      dtEntity::Component* comp;
      bool found = es->GetEntityManager().GetComponent(eid, es->GetComponentType(), comp, args[1]->BooleanValue());
      if(found)
      {
         return WrapComponent(args.Holder(), ss, eid, comp);
      }
      else
      {
         return Null();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESGetAllComponents(const Arguments& args)
   {
      ScriptSystem* ss = static_cast<ScriptSystem*>(External::Unwrap(args.Data()));
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());

      std::list<dtEntity::EntityId> eids;
      es->GetEntitiesInSystem(eids);

      HandleScope scope;

      Handle<Array> arr = Array::New();
      unsigned int count = 0;
      for(std::list<dtEntity::EntityId>::const_iterator i = eids.begin(); i != eids.end(); ++i)
      {
         dtEntity::EntityId eid = *i;
         dtEntity::Component* comp;
         if(es->GetComponent(eid, comp))
         {
            arr->Set(Integer::New(count++), WrapComponent(args.Holder(), ss, eid, comp));
         }
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESGetEntitiesInSystem(const Arguments& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());

      std::list<dtEntity::EntityId> eids;
      es->GetEntitiesInSystem(eids);

      HandleScope scope;

      Handle<Array> arr = Array::New();
      unsigned int count = 0;
      for(std::list<dtEntity::EntityId>::const_iterator i = eids.begin(); i != eids.end(); ++i)
      {         
         arr->Set(Integer::New(count++), Integer::New(*i));         
      }
      return scope.Close(arr);
   }   
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESGetComponentType(const Arguments& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());
      return ToJSString(dtEntity::GetStringFromSID(es->GetComponentType()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESCreateComponent(const Arguments& args)
   {
      ScriptSystem* ss = static_cast<ScriptSystem*>(External::Unwrap(args.Data()));

      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());
      if(!args[0]->IsUint32())
      {
         return ThrowError("Usage: createComponent(int)");
      }
      dtEntity::Component* component;
      
      dtEntity::EntityId eid = args[0]->Uint32Value();
      if(es->CreateComponent(eid, component))
      {
         return WrapComponent(args.Holder(), ss, eid, component);
      }
      else
      {
         return Null();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESDeleteComponent(const Arguments& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());

      if(!args[0]->IsUint32())
      {
         return ThrowError("Usage: deleteComponent(int)");
      }
      
      if(es->GetEntityManager().DeleteComponent(args[0]->Uint32Value(), es->GetComponentType()))
      {
         return True();
      }
      else
      {
         return False();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESStoreComponentToMap(const Arguments& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.Holder());
      return Boolean::New(sys->StoreComponentToMap((dtEntity::EntityId)args[0]->Uint32Value()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESAllowComponentCreationBySpawner(const Arguments& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.Holder());
      return Boolean::New(sys->AllowComponentCreationBySpawner());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESStorePropertiesToScene(const Arguments& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.Holder());
      return Boolean::New(sys->StorePropertiesToScene());
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESFinished(const Arguments& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());

      es->Finished();
      return Undefined();
   }
   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConstructES(const v8::Arguments& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::EntitySystem* es = static_cast<dtEntity::EntitySystem*>(ext->Value());   
      args.Holder()->SetInternalField(0, External::New(es));

      return Undefined();
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESCallScriptMethodRecursive(const v8::Arguments& args, dtEntity::PropertyArgs& pargs, int idx)
   {
      if(idx < args.Length())
      {
         HandleScope scope;
         Handle<Value> val = args[idx];

         if(val->IsArray())
         {
            Handle<Array> arr = Handle<Array>::Cast(val);

            Handle<String> hintstr = String::New("__TYPE_HINT");
            if(arr->Has(hintstr))
            {
               Handle<Value> hint = arr->Get(hintstr);
               std::string h = ToStdString(hint);
               if(h == "V2")
               {
                  dtEntity::Vec2dProperty p(UnwrapVec2(val));
                  pargs.push_back(&p);
                  return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
               }
               else if(h == "V3")
               {
                  dtEntity::Vec3dProperty p(UnwrapVec3(val));
                  pargs.push_back(&p);
                  return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));

               }
               else if(h == "V4")
               {
                  dtEntity::Vec4dProperty p(UnwrapVec4(val));
                  pargs.push_back(&p);
                  return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
               }
               else if(h == "QT")
               {
                  dtEntity::QuatProperty p(UnwrapQuat(val));
                  pargs.push_back(&p);
                  return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
               }
               else if(h == "MT")
               {
                  dtEntity::MatrixProperty p(UnwrapMatrix(val));
                  pargs.push_back(&p);
                  return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
               }
               else
               {
                  assert(false && "Unknown vector type encountered");
                  return Undefined();
               }
            }
            else
            {
               Handle<Array> arr = Handle<Array>::Cast(val);
               dtEntity::ArrayProperty p;
               for(unsigned int i = 0; i < arr->Length(); ++i)
               {
                  p.Add(ConvertValueToProperty(arr->Get(Integer::New(i))));
               }

               pargs.push_back(&p);
               return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
            }
            
         }
         else if(val->IsBoolean())
         {
            dtEntity::BoolProperty p(val->BooleanValue());
            pargs.push_back(&p);
            return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
         }
         else if(val->IsString())
         {
            dtEntity::StringProperty p(ToStdString(val));
            pargs.push_back(&p);
            return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
         }
         else if(val->IsUint32())
         {
            dtEntity::UIntProperty p(val->Uint32Value());
            pargs.push_back(&p);
            return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
         }
         else if(val->IsInt32())
         {
            dtEntity::IntProperty p(val->Int32Value());
            pargs.push_back(&p);
            return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
         }      
         else if(val->IsNumber())
         {
            dtEntity::DoubleProperty p(val->NumberValue());
            pargs.push_back(&p);
            return scope.Close(ESCallScriptMethodRecursive(args, pargs, idx + 1));
         }
         else
         {
            return ThrowError("Error converting script arguments: " + ToStdString(val));
         }

      }
      else
      {
         dtEntity::EntitySystem* es = UnwrapEntitySystem(args.Holder());
      
         std::string name = ToStdString(args.Data());
      
         dtEntity::Property* ret = dynamic_cast<dtEntity::ScriptAccessor*>(es)->CallScriptedMethod(name, pargs);
      
         HandleScope scope;
         Handle<Value> r = Null();
         if(ret != NULL)
         {
            r = ConvertPropertyToValue(args.Holder()->CreationContext(), ret);
            delete ret;
         }
         return scope.Close(r);
      }
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESCallScriptMethod(const v8::Arguments& args)
   {
      dtEntity::PropertyArgs pargs;
      return ESCallScriptMethodRecursive(args, pargs, 0);
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ESPropertyGetter(Local<String> propname, const AccessorInfo& info)
   {
      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());

      switch(prop->GetDataType())
      {
      case dtEntity::DataType::ARRAY:
      case dtEntity::DataType::VEC2:
      case dtEntity::DataType::VEC2D:
      case dtEntity::DataType::VEC3:
      case dtEntity::DataType::VEC3D:
      case dtEntity::DataType::VEC4:
      case dtEntity::DataType::VEC4D:
      case dtEntity::DataType::GROUP:
      case dtEntity::DataType::MATRIX:
      case dtEntity::DataType::QUAT:
      {
         Handle<Value> v = info.Holder()->GetHiddenValue(propname);
         if(v.IsEmpty())
         {
            v = ConvertPropertyToValue(info.Holder()->CreationContext(), prop);
            info.Holder()->SetHiddenValue(propname, v);
         }
         else
         {
            Handle<Value> ret = SetPropertyFromValue(v, prop);
            if(ret->BooleanValue() == false) {
               return ThrowError("Internal error: Did property change type on the fly?");
            }
         }
         return scope.Close(v);
      }
      default:
         return scope.Close(ConvertPropertyToValue(info.Holder()->CreationContext(), prop));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESPropertySetter(Local<String> propname,
                               Local<Value> value,
                               const AccessorInfo& info)
   {

      HandleScope scope;
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      if(prop)
      {
         SetPropertyFromValue(value, prop);
      }

      dtEntity::EntitySystem* sys = UnwrapEntitySystem(info.Holder());
	  if(sys)
	  {
		sys->OnPropertyChanged(dtEntity::SIDHash(ToStdString(propname)), *prop);
	  }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitEntitySystemWrapper(ScriptSystem* scriptSystem)
   {
      
      if(s_entitySystemTemplate.IsEmpty())
      {
        HandleScope handle_scope;
        Context::Scope context_scope(scriptSystem->GetGlobalContext());

        Handle<FunctionTemplate> templt = FunctionTemplate::New();
        s_entitySystemTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("EntitySystem"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("toString", FunctionTemplate::New(ESToString));
        proto->Set("getAllComponents", FunctionTemplate::New(ESGetAllComponents, External::New(scriptSystem)));
        proto->Set("getComponent", FunctionTemplate::New(ESGetComponent, External::New(scriptSystem)));
        proto->Set("getComponentType", FunctionTemplate::New(ESGetComponentType));
        proto->Set("getEntitiesInSystem", FunctionTemplate::New(ESGetEntitiesInSystem));
        proto->Set("hasComponent", FunctionTemplate::New(ESHasComponent));
        proto->Set("createComponent", FunctionTemplate::New(ESCreateComponent, External::New(scriptSystem)));
        proto->Set("deleteComponent", FunctionTemplate::New(ESDeleteComponent));
        proto->Set("finished", FunctionTemplate::New(ESFinished));

        proto->Set("storeComponentToMap", FunctionTemplate::New(ESStoreComponentToMap));
        proto->Set("allowComponentCreationBySpawner", FunctionTemplate::New(ESAllowComponentCreationBySpawner));
        proto->Set("storePropertiesToScene", FunctionTemplate::New(ESStorePropertiesToScene));

      }
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Object> WrapEntitySystem(ScriptSystem* scriptSystem, dtEntity::EntitySystem* v)
   {

      InitEntitySystemWrapper(scriptSystem);

      HandleScope handle_scope;
      Context::Scope context_scope(scriptSystem->GetGlobalContext());

      Handle<FunctionTemplate> tpl = s_entitySystemTemplate;
      
      SubWrapperMap::iterator i = s_subWrapperMap.find(v->GetComponentType());
      if(i != s_subWrapperMap.end()) 
      {
         tpl = i->second;
      }
      Local<Object> instance = tpl->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(v));

      const dtEntity::PropertyGroup& props = v->Get();

      dtEntity::PropertyGroup::const_iterator j;
      for(j = props.begin(); j != props.end(); ++j)
      {
         Handle<External> ext = v8::External::New(static_cast<void*>(j->second));
         std::string propname = dtEntity::GetStringFromSID(j->first);
         instance->SetAccessor(String::New(propname.c_str()),
                          ESPropertyGetter, ESPropertySetter,
                          ext);
      }

      dtEntity::ScriptAccessor* scriptaccessor = dynamic_cast<dtEntity::ScriptAccessor*>(v);
      if(scriptaccessor)
      {
         std::vector<std::string> names;
         scriptaccessor->GetMethodNames(names);
         for(std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i)
         {
            std::string name = *i;
            Handle<Value> namestr = ToJSString(name);
            instance->Set(namestr, FunctionTemplate::New(ESCallScriptMethod, namestr)->GetFunction());
         }
      }
      
      return handle_scope.Close(instance);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntitySystem* UnwrapEntitySystem(v8::Handle<v8::Value> val)
   {
      Handle<Object> obj = Handle<Object>::Cast(val);
      dtEntity::EntitySystem* v;
      GetInternal(obj, 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsEntitySystem(v8::Handle<v8::Value> val)
   {
      if(s_entitySystemTemplate.IsEmpty())
     {
       return false;
     }
     return s_entitySystemTemplate->HasInstance(val);
   }      

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterEntitySystempWrapper(ScriptSystem* ss, dtEntity::ComponentType ctype, Handle<FunctionTemplate> ftpl)
   {
      InitEntitySystemWrapper(ss);
      s_subWrapperMap[ctype] = Persistent<FunctionTemplate>::New(ftpl);
      ftpl->Inherit(s_entitySystemTemplate);
   }
}
