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
#include <dtEntityWrappers/RefPersistent.h>
#include <dtEntityWrappers/wrappers.h>

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{
 
   // TODO Ricky on 1/18/2014 
   // Using ref pointers here is as wrappers for the Persistent object which is not copyable and cannot be used within a map.
   // Persistent objects will be reset and deleted by the ref_ptr automatically
   typedef std::map<dtEntity::ComponentType, osg::ref_ptr< RefPersistent<FunctionTemplate> > > SubWrapperMap;
   SubWrapperMap s_subWrapperMap;

   dtEntity::StringId s_entitySystemWrapper = dtEntity::SID("EntitySystemWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   void ESToString(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set( String::NewFromUtf8(v8::Isolate::GetCurrent(), "<EntitySystem>"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESHasComponent(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      
      if(!args[0]->IsUint32())
      {
         ThrowError("Usage: hasComponent(int)");
         return;
      }

      bool searchderived = false;
      if(args.Length() > 1)
      {
         searchderived = args[1]->BooleanValue();
      }
      if(es->GetEntityManager().HasComponent(args[0]->Uint32Value(), es->GetComponentType(), searchderived))
      {
         args.GetReturnValue().Set( True(Isolate::GetCurrent()));
      }
      else
      {
         args.GetReturnValue().Set(False(Isolate::GetCurrent()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESGetComponent(const FunctionCallbackInfo<Value>& args)
   {
      ScriptSystem* ss = GetScriptSystem();
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      if(!args[0]->IsUint32())
      {
         ThrowError("Usage: getComponent(int entityid, [bool getDerived])");
         return;
      }

      dtEntity::EntityId eid = args[0]->Uint32Value();
      dtEntity::Component* comp;
      bool found = es->GetEntityManager().GetComponent(eid, es->GetComponentType(), comp, args[1]->BooleanValue());
      if(found)
      {
         args.GetReturnValue().Set( WrapComponent(args.This(), ss, eid, comp) );
      }
      else
      {
         args.GetReturnValue().Set( Null(Isolate::GetCurrent()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESGetAllComponents(const FunctionCallbackInfo<Value>& args)
   {
      ScriptSystem* ss = GetScriptSystem();
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());

      std::list<dtEntity::EntityId> eids;
      es->GetEntitiesInSystem(eids);

      HandleScope scope(Isolate::GetCurrent());

      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());
      unsigned int count = 0;
      for(std::list<dtEntity::EntityId>::const_iterator i = eids.begin(); i != eids.end(); ++i)
      {
         dtEntity::EntityId eid = *i;
         dtEntity::Component* comp;
         if(es->GetComponent(eid, comp))
         {
            arr->Set(Integer::New(Isolate::GetCurrent(), count++), WrapComponent(args.This(), ss, eid, comp));
         }
      }
      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESGetEntitiesInSystem(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());

      std::list<dtEntity::EntityId> eids;
      es->GetEntitiesInSystem(eids);

      HandleScope scope(Isolate::GetCurrent());

      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());
      unsigned int count = 0;
      for(std::list<dtEntity::EntityId>::const_iterator i = eids.begin(); i != eids.end(); ++i)
      {         
         arr->Set(Integer::New(Isolate::GetCurrent(), count++), Integer::New(Isolate::GetCurrent(), *i));         
      }
      args.GetReturnValue().Set(arr);
   }   
   
   ////////////////////////////////////////////////////////////////////////////////
   void ESGetComponentType(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      args.GetReturnValue().Set( ToJSString(dtEntity::GetStringFromSID(es->GetComponentType())));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESCreateComponent(const FunctionCallbackInfo<Value>& args)
   {
      ScriptSystem* ss = GetScriptSystem();

      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      if(!args[0]->IsUint32())
      {
         ThrowError("Usage: createComponent(int)");
         return;
      }
      dtEntity::Component* component;
      
      dtEntity::EntityId eid = args[0]->Uint32Value();
      if(es->CreateComponent(eid, component))
      {
         args.GetReturnValue().Set( WrapComponent(args.This(), ss, eid, component));
      }
      else
      {
         args.GetReturnValue().Set( Null(Isolate::GetCurrent()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESDeleteComponent(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());

      if(!args[0]->IsUint32())
      {
         ThrowError("Usage: deleteComponent(int)");
         return;
      }
      
      if(es->GetEntityManager().DeleteComponent(args[0]->Uint32Value(), es->GetComponentType()))
      {
         args.GetReturnValue().Set(True(Isolate::GetCurrent()));
      }
      else
      {
         args.GetReturnValue().Set(False(Isolate::GetCurrent()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESStoreComponentToMap(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.This());
      args.GetReturnValue().Set(Boolean::New(Isolate::GetCurrent(), sys->StoreComponentToMap((dtEntity::EntityId)args[0]->Uint32Value())));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESAllowComponentCreationBySpawner(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.This());
      args.GetReturnValue().Set(Boolean::New(Isolate::GetCurrent(), sys->AllowComponentCreationBySpawner()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESStorePropertiesToScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(args.This());
      args.GetReturnValue().Set(Boolean::New(Isolate::GetCurrent(), sys->StorePropertiesToScene()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESCopyPropertyValues(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      dtEntity::StringId propname = UnwrapSID(args[0]);
      dtEntity::Property* prop = es->Get(propname);
      if(prop == NULL)
      {
         ThrowError("Entity System has no property named " + dtEntity::GetStringFromSID(UnwrapSID(args[0])));
         return;
      }
      args.GetReturnValue().Set(SetValueFromProperty(prop, args[1]));
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void ESFinished(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());

      es->Finished();
   }
   

   ////////////////////////////////////////////////////////////////////////////////
   void ConstructES(const v8::FunctionCallbackInfo<Value>& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::EntitySystem* es = static_cast<dtEntity::EntitySystem*>(ext->Value());   
      args.This()->SetInternalField(0, External::New(Isolate::GetCurrent(), es));
   }


   ////////////////////////////////////////////////////////////////////////////////
   void ESCallScriptMethodRecursive(const v8::FunctionCallbackInfo<Value>& args, dtEntity::PropertyArgs& pargs, int idx)
   {
      if(idx < args.Length())
      {
         HandleScope scope(Isolate::GetCurrent());
         Handle<Value> val = args[idx];

         if(val->IsArray())
         {
            Handle<Array> arr = Handle<Array>::Cast(val);

            Handle<String> hintstr = String::NewFromUtf8(v8::Isolate::GetCurrent(), "__TYPE_HINT");
            if(arr->Has(hintstr))
            {
               Handle<Value> hint = arr->Get(hintstr);
               std::string h = ToStdString(hint);
               if(h == "V2")
               {
                  dtEntity::Vec2dProperty p(UnwrapVec2(val));
                  pargs.push_back(&p);
                  return ESCallScriptMethodRecursive(args, pargs, idx + 1);
               }
               else if(h == "V3")
               {
                  dtEntity::Vec3dProperty p(UnwrapVec3(val));
                  pargs.push_back(&p);
                  return ESCallScriptMethodRecursive(args, pargs, idx + 1);

               }
               else if(h == "V4")
               {
                  dtEntity::Vec4dProperty p(UnwrapVec4(val));
                  pargs.push_back(&p);
                  return ESCallScriptMethodRecursive(args, pargs, idx + 1);
               }
               else if(h == "QT")
               {
                  dtEntity::QuatProperty p(UnwrapQuat(val));
                  pargs.push_back(&p);
                  return ESCallScriptMethodRecursive(args, pargs, idx + 1);
               }
               else if(h == "MT")
               {
                  dtEntity::MatrixProperty p(UnwrapMatrix(val));
                  pargs.push_back(&p);
                  return ESCallScriptMethodRecursive(args, pargs, idx + 1);
               }
               else
               {
                  assert(false && "Unknown vector type encountered");
                  args.GetReturnValue().Set(Undefined(Isolate::GetCurrent()));
                  return;
               }
            }
            // no hint
            else
            {
               Handle<Array> arr = Handle<Array>::Cast(val);
               dtEntity::ArrayProperty p;
               for(unsigned int i = 0; i < arr->Length(); ++i)
               {
                  p.Add(ConvertValueToProperty(arr->Get(Integer::New(Isolate::GetCurrent(), i))));
               }

               pargs.push_back(&p);
               return ESCallScriptMethodRecursive(args, pargs, idx + 1);
            }
         } // end - if array property

         else if(val->IsBoolean())
         {
            dtEntity::BoolProperty p(val->BooleanValue());
            pargs.push_back(&p);
            return ESCallScriptMethodRecursive(args, pargs, idx + 1);
         }
         else if(val->IsString())
         {
            dtEntity::StringProperty p(ToStdString(val));
            pargs.push_back(&p);
            return ESCallScriptMethodRecursive(args, pargs, idx + 1);
         }
         else if(val->IsUint32())
         {
            dtEntity::UIntProperty p(val->Uint32Value());
            pargs.push_back(&p);
            return ESCallScriptMethodRecursive(args, pargs, idx + 1);
         }
         else if(val->IsInt32())
         {
            dtEntity::IntProperty p(val->Int32Value());
            pargs.push_back(&p);
            return ESCallScriptMethodRecursive(args, pargs, idx + 1);
         }      
         else if(val->IsNumber())
         {
            dtEntity::DoubleProperty p(val->NumberValue());
            pargs.push_back(&p);
            return ESCallScriptMethodRecursive(args, pargs, idx + 1);
         }
         else
         {
            ThrowError("Error converting script arguments: " + ToStdString(val));
            return;
         }

      }

      // finished with converting the arguments, call the actual scripted method
      else
      {
         dtEntity::EntitySystem* es = UnwrapEntitySystem(args.This());
      
         std::string name = ToStdString(args.Data());
      
         dtEntity::Property* ret = dynamic_cast<dtEntity::ScriptAccessor*>(es)->CallScriptedMethod(name, pargs);
      
         HandleScope scope(Isolate::GetCurrent());
         Handle<Value> r = Null(Isolate::GetCurrent());
         if(ret != NULL)
         {
            r = ConvertPropertyToValue(args.This()->CreationContext(), ret);
            delete ret;
         }

         // return the value returned by the method
         args.GetReturnValue().Set(r);
      }
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void ESCallScriptMethod(const v8::FunctionCallbackInfo<Value>& args)
   {
      dtEntity::PropertyArgs pargs;
      return ESCallScriptMethodRecursive(args, pargs, 0);
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void ESPropertyGetter(Local<String> propname, const PropertyCallbackInfo<Value>& info)
   {
      HandleScope scope(Isolate::GetCurrent());
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
               ThrowError("Internal error: Did property change type on the fly?");
               return;
            }
         }
         info.GetReturnValue().Set(v);
         return;
      }
      default:
      {
         info.GetReturnValue().Set( ConvertPropertyToValue(info.Holder()->CreationContext(), prop));
         return;
      }
         
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ESPropertySetter(Local<String> propname,
                         Local<Value> value,
                         const PropertyCallbackInfo<void>& info)
   {

      HandleScope scope(Isolate::GetCurrent());
      Handle<External> ext = Handle<External>::Cast(info.Data());
      dtEntity::Property* prop = static_cast<dtEntity::Property*>(ext->Value());
      if(prop)
      {
         SetPropertyFromValue(value, prop);
      }
#if CALL_ONPROPERTYCHANGED_METHOD
      dtEntity::EntitySystem* sys = UnwrapEntitySystem(info.Holder());
	  if(sys)
	  {
		sys->OnPropertyChanged(dtEntity::SIDHash(ToStdString(propname)), *prop);
	  }
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitEntitySystemWrapper(ScriptSystem* scriptSystem)
   {  
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handle_scope(isolate);
      Handle<Context> context = scriptSystem->GetGlobalContext();
      Context::Scope context_scope(context);

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_entitySystemWrapper);

      if(templt.IsEmpty())
      {

        templt = FunctionTemplate::New(isolate);

        templt->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "EntitySystem"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, ESToString));
        proto->Set(String::NewFromUtf8(isolate, "getAllComponents"), FunctionTemplate::New(isolate, ESGetAllComponents, External::New(isolate, scriptSystem)));
        proto->Set(String::NewFromUtf8(isolate, "getComponent"), FunctionTemplate::New(isolate, ESGetComponent, External::New(isolate, scriptSystem)));
        proto->Set(String::NewFromUtf8(isolate, "getComponentType"), FunctionTemplate::New(isolate, ESGetComponentType));
        proto->Set(String::NewFromUtf8(isolate, "getEntitiesInSystem"), FunctionTemplate::New(isolate, ESGetEntitiesInSystem));
        proto->Set(String::NewFromUtf8(isolate, "hasComponent"), FunctionTemplate::New(isolate, ESHasComponent));
        proto->Set(String::NewFromUtf8(isolate, "createComponent"), FunctionTemplate::New(isolate, ESCreateComponent, External::New(isolate, scriptSystem)));
        proto->Set(String::NewFromUtf8(isolate, "deleteComponent"), FunctionTemplate::New(isolate, ESDeleteComponent));
        proto->Set(String::NewFromUtf8(isolate, "copyPropertyValues"), FunctionTemplate::New(isolate, ESCopyPropertyValues));
        proto->Set(String::NewFromUtf8(isolate, "finished"), FunctionTemplate::New(isolate, ESFinished));

        proto->Set(String::NewFromUtf8(isolate, "storeComponentToMap"), FunctionTemplate::New(isolate, ESStoreComponentToMap));
        proto->Set(String::NewFromUtf8(isolate, "allowComponentCreationBySpawner"), FunctionTemplate::New(isolate, ESAllowComponentCreationBySpawner));
        proto->Set(String::NewFromUtf8(isolate, "storePropertiesToScene"), FunctionTemplate::New(isolate, ESStorePropertiesToScene));
        GetScriptSystem()->SetTemplateBySID(s_entitySystemWrapper, templt);
      }
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Object> WrapEntitySystem(ScriptSystem* scriptSystem, dtEntity::EntitySystem* v)
   {

      InitEntitySystemWrapper(scriptSystem);

      Isolate* isolate = Isolate::GetCurrent();
      EscapableHandleScope handle_scope(isolate);
      Context::Scope context_scope(scriptSystem->GetGlobalContext());

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_entitySystemWrapper);

      assert(!templt.IsEmpty());

      
      SubWrapperMap::iterator i = s_subWrapperMap.find(v->GetComponentType());
      if(i != s_subWrapperMap.end()) 
      {
         templt = i->second->GetLocal();
      }
      Local<Object> instance = templt->GetFunction()->NewInstance();
      instance->SetInternalField(0, External::New(isolate, v));

      const dtEntity::PropertyGroup& props = v->Get();

      dtEntity::PropertyGroup::const_iterator j;
      for(j = props.begin(); j != props.end(); ++j)
      {
         Handle<External> ext = v8::External::New(isolate, static_cast<void*>(j->second));
         std::string propname = dtEntity::GetStringFromSID(j->first);
         instance->SetAccessor(String::NewFromUtf8(v8::Isolate::GetCurrent(), propname.c_str()),
                               ESPropertyGetter, 
                               ESPropertySetter,
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
            instance->Set(namestr, FunctionTemplate::New(isolate, ESCallScriptMethod, namestr)->GetFunction());
         }
      }
      
      return handle_scope.Escape(instance);
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
   bool IsEntitySystem(v8::Handle<v8::Value> obj)
   {
      HandleScope scope(Isolate::GetCurrent());
      Handle<FunctionTemplate> tpl = GetScriptSystem()->GetTemplateBySID(s_entitySystemWrapper);
      if(tpl.IsEmpty())
      {
         return false;
      }
      return tpl->HasInstance(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RegisterEntitySystempWrapper(ScriptSystem* ss, dtEntity::ComponentType ctype, Handle<FunctionTemplate> ftpl)
   {
      InitEntitySystemWrapper(ss);

      Handle<FunctionTemplate> tmplt = GetScriptSystem()->GetTemplateBySID(s_entitySystemWrapper);
      assert(!tmplt.IsEmpty());

      // allocate a RefPersistent, it will be automatically Reset and cleaned up the the ptr in the map gets destroyed
      //Persistent<FunctionTemplate>* pFT = new Persistent<FunctionTemplate>(Isolate::GetCurrent(), ftpl);
      s_subWrapperMap[ctype] = new RefPersistent<FunctionTemplate>(Isolate::GetCurrent(), ftpl);
      ftpl->Inherit(tmplt);
   }
}
