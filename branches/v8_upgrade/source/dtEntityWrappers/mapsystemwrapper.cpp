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

#include <dtEntityWrappers/mapsystemwrapper.h>
#include <dtEntityWrappers/entitysystemwrapper.h>

#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
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
   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::MapSystem* UnwrapMapSystem(v8::Handle<v8::Value> val)
   {
      dtEntity::MapSystem* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSToString(const FunctionCallbackInfo<Value>& args)
   {
      args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "<MapSystem>"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSAddEmptyMap(const FunctionCallbackInfo<Value>& args)
   {
      if(args.Length() != 2)
       {
          ThrowError("usage: addEmptyMap(datapath, mappath");
          return;
       }
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->AddEmptyMap(ToStdString(args[0]), ToStdString(args[1]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSAddToScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->AddToScene(args[0]->Uint32Value());

      if(!success)
         ThrowError("Could not add to scene: " + ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSGetEntityIdByUniqueId(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      dtEntity::EntityId id = ms->GetEntityIdByUniqueId(ToStdString(args[0]));
      args.GetReturnValue().Set(Integer::New(Isolate::GetCurrent(), id));
   }


   ////////////////////////////////////////////////////////////////////////////////
   void MSIsSpawnOf(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      args.GetReturnValue().Set(Boolean::New(Isolate::GetCurrent(), ms->IsSpawnOf(args[0]->Uint32Value(), ToStdString(args[1]))) );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSLoadMap(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->LoadMap(ToStdString(args[0]));
      args.GetReturnValue().Set(Boolean::New(Isolate::GetCurrent(), success));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSLoadScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->LoadScene(ToStdString(args[0]));

      if(!success)
         ThrowError("Could not load scene " + ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSRemoveFromScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->RemoveFromScene(args[0]->Uint32Value());
      if(!success)
         ThrowError("Could not remove from scene: " + ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSUnloadScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->UnloadScene();
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void MSSaveScene(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->SaveScene(ToStdString(args[0]), args[1]->BooleanValue());

      if(!success)
         ThrowError("Could not save scene " + ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSUnloadMap(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->UnloadMap(ToStdString(args[0]));

      if(!success)
         ThrowError("Could not unload map " + ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSGetLoadedMaps(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::vector<std::string> maps = ms->GetLoadedMaps();

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Array> arr = Array::New(isolate, maps.size());
      int idx = 0;
      for(std::vector<std::string>::iterator i = maps.begin(); i != maps.end(); ++i)
      {
         arr->Set(Integer::New(isolate, idx), ToJSString(*i));
         ++idx;
      }

      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSSpawn(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      
      int eid = args[1]->Int32Value();
      dtEntity::Entity* entity;
      bool success = ms->GetEntityManager().GetEntity(eid, entity);
      if(!success)
      {
         ThrowError("Cannot spawn: not a valid entity! Usage: spawn(string spawner, entityid)");
         return;
      }
      success = ms->Spawn(ToStdString(args[0]), *entity);
      if(!success)
      {
         ThrowError("Cannot spawn: Spawner not found with name " + ToStdString(args[0]));
         return;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSAddSpawner(const FunctionCallbackInfo<Value>& args)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());

      if(args.Length() != 1)
      {
         ThrowError("Usage: addSpawner({components, name, guicategory, mapname, addtospawnerstore, iconpath})");
         return;
      }

      Handle<Object> obj = Handle<Object>::Cast(args[0]);

      Handle<Value> vname = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"));
      Handle<Value> vcomponents = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "components"));

      if(vname.IsEmpty() || vcomponents.IsEmpty())
      {
         ThrowError("Usage: addSpawner({components, name, guicategory, mapname, addtospawnerstore, iconpath, parentname})");
         return;
      }

      Handle<Value> vguicategory = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "guicategory"));
      Handle<Value> vmapname = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "mapname"));
      Handle<Value> vaddtospawnerstore = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "addtospawnerstore"));
      Handle<Value> viconpath = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "iconpath"));
      Handle<Value> vparentname = obj->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "parentname"));

      std::string name = ToStdString(vname);
      std::string mapname = vmapname.IsEmpty() ? "" : ToStdString(vmapname);

      Handle<Object> components = Handle<Object>::Cast(vcomponents);

      dtEntity::Spawner* parent = NULL;

      if(!vparentname.IsEmpty() && !vparentname->IsUndefined())
      {
         ms->GetSpawner(ToStdString(vparentname), parent);
      }

      osg::ref_ptr<dtEntity::Spawner> spawner = new dtEntity::Spawner(name, mapname, parent);

      if(!vguicategory.IsEmpty() && !vguicategory->IsUndefined())
      {
         spawner->SetGUICategory(ToStdString(vguicategory));
      }

      if(!vaddtospawnerstore.IsEmpty() && !vaddtospawnerstore->IsUndefined())
      {
         spawner->SetAddToSpawnerStore(vaddtospawnerstore->BooleanValue());
      }

      if(!viconpath.IsEmpty() && !viconpath->IsUndefined())
      {
         spawner->SetIconPath(ToStdString(viconpath));
      }

      Handle<Array> keys = components->GetPropertyNames();
      
      for(unsigned int i = 0; i < keys->Length(); ++i)
      {
         Handle<Value> key = keys->Get(Integer::New(isolate, i));
         std::string keyname = ToStdString(key);

         dtEntity::StringId ctype = dtEntity::SIDHash(keyname);

         dtEntity::ComponentPluginManager::GetInstance().StartEntitySystem(ms->GetEntityManager(), ctype);

         if(ms->GetEntityManager().HasEntitySystem(ctype))
         {
            Handle<Value> val = components->Get(key);
            if(val->IsObject())
            {
               Handle<Object> compobj = Handle<Object>::Cast(val);
               Handle<Array> compkeys = compobj->GetPropertyNames();

               dtEntity::GroupProperty props;
               for(unsigned int j = 0; j < compkeys->Length(); ++j)
               {
                  Handle<Value> compkey = compkeys->Get(Integer::New(isolate, j));
                  std::string compkeystr = ToStdString(compkey);
                  Handle<Value> compval = compobj->Get(compkey);
                  dtEntity::Property* prop = ConvertValueToProperty(compval);
                  props.Add(dtEntity::SIDHash(compkeystr), prop);
               }
               spawner->AddComponent(ctype, props);
            }
         }
      }
      
      ms->AddSpawner(*spawner);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSDeleteSpawner(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->DeleteSpawner(ToStdString(args[0]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSGetSpawner(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      dtEntity::Spawner* spawner;
      
      if(!ms->GetSpawner(ToStdString(args[0]), spawner))
         return;

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Handle<Object> obj = Object::New(isolate);

      if(spawner->GetParent())
      {
         obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "parent"), String::NewFromUtf8(v8::Isolate::GetCurrent(), spawner->GetParent()->GetName().c_str()));
      }
      else
      {
         obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "parent"), String::NewFromUtf8(v8::Isolate::GetCurrent(), ""));
      }

      obj->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, spawner->GetName().c_str()));
      obj->Set(String::NewFromUtf8(isolate, "guicategory"), String::NewFromUtf8(isolate, spawner->GetGUICategory().c_str()));
      obj->Set(String::NewFromUtf8(isolate, "mapname"), String::NewFromUtf8(isolate, spawner->GetMapName().c_str()));
      obj->Set(String::NewFromUtf8(isolate, "addtospawnerstore"), Boolean::New(isolate, spawner->GetAddToSpawnerStore()));
      obj->Set(String::NewFromUtf8(isolate, "iconpath"), String::NewFromUtf8(isolate, spawner->GetIconPath().c_str()));

      Handle<Object> comps = Object::New(isolate);
      dtEntity::Spawner::ComponentProperties props;
      spawner->GetAllComponentProperties(props);
      dtEntity::Spawner::ComponentProperties::iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string compname = dtEntity::GetStringFromSID(i->first);
         Handle<Object> jscomp = Object::New(isolate);

         const dtEntity::GroupProperty props = i->second;
         dtEntity::PropertyGroup g = props.Get();

         for(dtEntity::PropertyGroup::const_iterator j = g.begin(); j != g.end(); ++j)
         {
            std::string propname = dtEntity::GetStringFromSID(j->first);
            const dtEntity::Property* prop = j->second;
            jscomp->Set(ToJSString(propname), ConvertPropertyToValue(args.This()->CreationContext(), prop));
         }
         
         comps->Set(ToJSString(compname), jscomp);
      }

      obj->Set(String::NewFromUtf8(isolate, "components"), comps);
      args.GetReturnValue().Set(obj);
   }


   ////////////////////////////////////////////////////////////////////////////////
   /**
     Returns JS object with all components of spawner and its spawner parents
   */

   void MSGetSpawnerComponents(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      dtEntity::Spawner* spawner;

      if(!ms->GetSpawner(ToStdString(args[0]), spawner))
      {
         return;
      }

      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);

      Handle<Object> comps = Object::New(isolate);
      dtEntity::Spawner::ComponentProperties props;
      spawner->GetAllComponentPropertiesRecursive(props);
      dtEntity::Spawner::ComponentProperties::iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string compname = dtEntity::GetStringFromSID(i->first);
         Handle<Object> jscomp = Object::New(isolate);

         const dtEntity::GroupProperty props = i->second;
         dtEntity::PropertyGroup g = props.Get();

         for(dtEntity::PropertyGroup::const_iterator j = g.begin(); j != g.end(); ++j)
         {
            std::string propname = dtEntity::GetStringFromSID(j->first);
            const dtEntity::Property* prop = j->second;
            jscomp->Set(ToJSString(propname), ConvertPropertyToValue(args.This()->CreationContext(), prop));
         }

         comps->Set(ToJSString(compname), jscomp);
      }

      args.GetReturnValue().Set(comps);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void MSGetSpawnerCreatedEntities(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::string spawnername = ToStdString(args[0]);
      bool recursive = args[1]->BooleanValue();

      std::vector<dtEntity::EntityId> ids;
      ms->GetSpawnerCreatedEntities(spawnername, ids, recursive);
      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());
      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(i, Integer::New(Isolate::GetCurrent(), ids[i]));
      }

      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSGetAllSpawnerNames(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::vector<std::string> names;
      ms->GetAllSpawnerNames(names);
      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());
      for(unsigned int i = 0; i < names.size(); ++i)
      {
         arr->Set(Integer::New(Isolate::GetCurrent(), i), ToJSString(names[i]));
      }

      args.GetReturnValue().Set(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MSGetEntitiesInMap(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::string mapname = ToStdString(args[0]);
      std::vector<dtEntity::EntityId> ids;
      ms->GetEntitiesInMap(mapname, ids);
      
      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Array::New(v8::Isolate::GetCurrent());
      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(Integer::New(Isolate::GetCurrent(), i), Uint32::New(Isolate::GetCurrent(), ids[i]));
      }
      args.GetReturnValue().Set(arr);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void MSDeleteEntitiesByMap(const FunctionCallbackInfo<Value>& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->DeleteEntitiesByMap(ToStdString(args[0]));
   }   

   ////////////////////////////////////////////////////////////////////////////////
   void ConstructMS(const v8::FunctionCallbackInfo<Value>& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::MapSystem* ls = static_cast<dtEntity::MapSystem*>(ext->Value());   
      args.This()->SetInternalField(0, External::New(Isolate::GetCurrent(), ls));
   }
  
   ////////////////////////////////////////////////////////////////////////////////
   void InitMapSystemWrapper(ScriptSystem* ss)
   {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope scope(isolate);
      Context::Scope context_scope(ss->GetGlobalContext());

      Handle<FunctionTemplate> templt= FunctionTemplate::New(Isolate::GetCurrent());
      templt->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "MapSystem"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set(String::NewFromUtf8(isolate, "addEmptyMap"), FunctionTemplate::New(isolate, MSAddEmptyMap));
      proto->Set(String::NewFromUtf8(isolate, "getEntityIdByUniqueId"), FunctionTemplate::New(isolate, MSGetEntityIdByUniqueId));
      proto->Set(String::NewFromUtf8(isolate, "isSpawnOf"), FunctionTemplate::New(isolate, MSIsSpawnOf));
      proto->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, MSToString));
      proto->Set(String::NewFromUtf8(isolate, "loadMap"), FunctionTemplate::New(isolate, MSLoadMap));
      proto->Set(String::NewFromUtf8(isolate, "loadScene"), FunctionTemplate::New(isolate, MSLoadScene));
      proto->Set(String::NewFromUtf8(isolate, "unloadScene"), FunctionTemplate::New(isolate, MSUnloadScene));
      proto->Set(String::NewFromUtf8(isolate, "saveScene"), FunctionTemplate::New(isolate, MSSaveScene));
      proto->Set(String::NewFromUtf8(isolate, "unloadMap"), FunctionTemplate::New(isolate, MSUnloadMap));
      proto->Set(String::NewFromUtf8(isolate, "getLoadedMaps"), FunctionTemplate::New(isolate, MSGetLoadedMaps));
      proto->Set(String::NewFromUtf8(isolate, "spawn"), FunctionTemplate::New(isolate, MSSpawn));
      proto->Set(String::NewFromUtf8(isolate, "deleteEntitiesByMap"), FunctionTemplate::New(isolate, MSDeleteEntitiesByMap));
      proto->Set(String::NewFromUtf8(isolate, "addSpawner"), FunctionTemplate::New(isolate, MSAddSpawner));
      proto->Set(String::NewFromUtf8(isolate, "addToScene"), FunctionTemplate::New(isolate, MSAddToScene));
      proto->Set(String::NewFromUtf8(isolate, "deleteSpawner"), FunctionTemplate::New(isolate, MSDeleteSpawner));
      proto->Set(String::NewFromUtf8(isolate, "getSpawner"), FunctionTemplate::New(isolate, MSGetSpawner));
      proto->Set(String::NewFromUtf8(isolate, "MSGetSpawnerComponents"), FunctionTemplate::New(isolate, MSGetSpawnerComponents));
      proto->Set(String::NewFromUtf8(isolate, "getSpawnerCreatedEntities"), FunctionTemplate::New(isolate, MSGetSpawnerCreatedEntities));
      proto->Set(String::NewFromUtf8(isolate, "getAllSpawnerNames"), FunctionTemplate::New(isolate, MSGetAllSpawnerNames));
      proto->Set(String::NewFromUtf8(isolate, "getEntitiesInMap"), FunctionTemplate::New(isolate, MSGetEntitiesInMap));
      proto->Set(String::NewFromUtf8(isolate, "removeFromScene"), FunctionTemplate::New(isolate, MSRemoveFromScene));
      
      RegisterEntitySystempWrapper(ss, dtEntity::MapComponent::TYPE, templt);
   }
}
