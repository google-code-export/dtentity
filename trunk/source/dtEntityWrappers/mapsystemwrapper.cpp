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
   Handle<Value> MSToString(const Arguments& args)
   {
      return String::New("<MapSystem>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSAddEmptyMap(const Arguments& args)
   {
      if(args.Length() != 2)
       {
          return ThrowError("usage: addEmptyMap(datapath, mappath");
       }
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->AddEmptyMap(ToStdString(args[0]), ToStdString(args[1]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetEntityIdByUniqueId(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      dtEntity::EntityId id = ms->GetEntityIdByUniqueId(ToStdString(args[0]));
      return Integer::New(id);
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSIsSpawnOf(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      return Boolean::New(ms->IsSpawnOf(args[0]->Uint32Value(), ToStdString(args[1])));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSLoadMap(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->LoadMap(ToStdString(args[0]));
      return Boolean::New(success);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSLoadScene(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->LoadScene(ToStdString(args[0]));
      if(success)
      {
         return Undefined();
      }
      else
      {
         return ThrowError("Could not load scene " + ToStdString(args[0]));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSUnloadScene(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->UnloadScene();
      return Undefined();
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSSaveScene(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->SaveScene(ToStdString(args[0]), args[1]->BooleanValue());
      if(success)
      {
         return Undefined();
      }
      else
      {
         return ThrowError("Could not save scene " + ToStdString(args[0]));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSUnloadMap(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      bool success = ms->UnloadMap(ToStdString(args[0]));
      if(success)
      {
         return Undefined();
      }
      else
      {
         return ThrowError("Could not unload map " + ToStdString(args[0]));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetLoadedMaps(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::vector<std::string> maps = ms->GetLoadedMaps();
      HandleScope scope;
      Handle<Array> arr = Array::New(maps.size());
      int idx = 0;
      for(std::vector<std::string>::iterator i = maps.begin(); i != maps.end(); ++i)
      {
         arr->Set(Integer::New(idx), ToJSString(*i));
         ++idx;
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSSpawn(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      
      int eid = args[1]->Int32Value();
      dtEntity::Entity* entity;
      bool success = ms->GetEntityManager().GetEntity(eid, entity);
      if(!success)
      {
         return ThrowError("Cannot spawn: not a valid entity! Usage: spawn(string spawner, entityid)");
      }
      success = ms->Spawn(ToStdString(args[0]), *entity);
      if(!success)
      {
         return ThrowError("Cannot spawn: Spawner not found with name " + ToStdString(args[0]));
      }
      return True();
   }

    ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSAddSpawner(const Arguments& args)
   {
      HandleScope scope;
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());

      if(args.Length() != 1)
      {
         return ThrowError("Usage: addSpawner({components, name, guicategory, mapname, addtospawnerstore, iconpath})");
      }

      Handle<Object> obj = Handle<Object>::Cast(args[0]);

      Handle<Value> vname = obj->Get(String::New("name"));
      Handle<Value> vcomponents = obj->Get(String::New("components"));

      if(vname.IsEmpty() || vcomponents.IsEmpty())
      {
         return ThrowError("Usage: addSpawner({components, name, guicategory, mapname, addtospawnerstore, iconpath, parentname})");
      }

      Handle<Value> vguicategory = obj->Get(String::New("guicategory"));
      Handle<Value> vmapname = obj->Get(String::New("mapname"));
      Handle<Value> vaddtospawnerstore = obj->Get(String::New("addtospawnerstore"));
      Handle<Value> viconpath = obj->Get(String::New("iconpath"));
      Handle<Value> vparentname = obj->Get(String::New("parentname"));

      std::string name = ToStdString(vname);
      std::string mapname = vmapname.IsEmpty() ? "" : ToStdString(vmapname);

      Handle<Object> components = Handle<Object>::Cast(vcomponents);

      dtEntity::Spawner* parent = NULL;

      if(!vparentname.IsEmpty())
      {
         ms->GetSpawner(ToStdString(vparentname), parent);
      }

      osg::ref_ptr<dtEntity::Spawner> spawner = new dtEntity::Spawner(name, mapname, parent);

      if(!vguicategory.IsEmpty())
      {
         spawner->SetGUICategory(ToStdString(vguicategory));
      }

      if(!vaddtospawnerstore.IsEmpty())
      {
         spawner->SetAddToSpawnerStore(vaddtospawnerstore->BooleanValue());
      }

      if(!viconpath.IsEmpty())
      {
         spawner->SetIconPath(ToStdString(viconpath));
      }

      Handle<Array> keys = components->GetPropertyNames();
      
      for(unsigned int i = 0; i < keys->Length(); ++i)
      {
         Handle<Value> key = keys->Get(Integer::New(i));
         std::string keyname = ToStdString(key);

         dtEntity::StringId ctype = dtEntity::SIDHash(keyname);

         if(ms->GetEntityManager().HasEntitySystem(ctype))
         {
            Handle<Value> val = obj->Get(key);
            if(val->IsObject())
            {
               Handle<Object> compobj = Handle<Object>::Cast(val);
               Handle<Array> compkeys = compobj->GetPropertyNames();

               dtEntity::DynamicPropertyContainer props;  
               for(unsigned int j = 0; j < compkeys->Length(); ++j)
               {
                  Handle<Value> compkey = compkeys->Get(Integer::New(j));
                  std::string compkeystr = ToStdString(compkey);
                  Handle<Value> compval = compobj->Get(compkey);
                  dtEntity::Property* prop = ConvertValueToProperty(compval);
                  props.AddProperty(dtEntity::SIDHash(compkeystr), *prop);
                  delete prop;
               }
               spawner->AddComponent(ctype, props);
            }
         }
      }
      
      ms->AddSpawner(*spawner);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSDeleteSpawner(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->DeleteSpawner(ToStdString(args[0]));
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetSpawner(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      dtEntity::Spawner* spawner;
      
      if(!ms->GetSpawner(ToStdString(args[0]), spawner))
      {
         return Null();
      }
      HandleScope scope;
      Handle<Object> obj = Object::New();

      if(spawner->GetParent())
      {
         obj->Set(String::New("parent"), String::New(spawner->GetParent()->GetName().c_str()));
      }
      else
      {
         obj->Set(String::New("parent"), String::New(""));
      }

      obj->Set(String::New("name"), String::New(spawner->GetName().c_str()));
      obj->Set(String::New("guicategory"), String::New(spawner->GetGUICategory().c_str()));
      obj->Set(String::New("mapname"), String::New(spawner->GetMapName().c_str()));
      obj->Set(String::New("addtospawnerstore"), Boolean::New(spawner->GetAddToSpawnerStore()));
      obj->Set(String::New("iconpath"), Boolean::New(spawner->GetIconPath().c_str()));

      Handle<Object> comps = Object::New();
      dtEntity::Spawner::ComponentProperties props;
      spawner->GetAllComponentProperties(props);
      dtEntity::Spawner::ComponentProperties::iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string compname = dtEntity::GetStringFromSID(i->first);
         Handle<Object> jscomp = Object::New();

         const dtEntity::DynamicPropertyContainer props = i->second;
         dtEntity::PropertyContainer::ConstPropertyMap propmap;
         props.GetProperties(propmap);
         dtEntity::PropertyContainer::ConstPropertyMap::const_iterator j;
         for(j = propmap.begin(); j != propmap.end(); ++j)
         {
            std::string propname = dtEntity::GetStringFromSID(j->first);
            const dtEntity::Property* prop = j->second;
            jscomp->Set(ToJSString(propname), ConvertPropertyToValue(args.Holder()->CreationContext(), prop));
         }
         
         comps->Set(ToJSString(compname), jscomp);
      }
      obj->Set(String::New("components"), comps);
      return scope.Close(obj);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetSpawnerCreatedEntities(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::string spawnername = ToStdString(args[0]);
      bool recursive = args[1]->BooleanValue();

      std::vector<dtEntity::EntityId> ids;
      ms->GetSpawnerCreatedEntities(spawnername, ids, recursive);
      HandleScope scope;
      Handle<Array> arr = Array::New();
      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(i, Integer::New(ids[i]));
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetAllSpawnerNames(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::vector<std::string> names;
      ms->GetAllSpawnerNames(names);
      HandleScope scope;
      Handle<Array> arr = Array::New();
      for(unsigned int i = 0; i < names.size(); ++i)
      {
         arr->Set(Integer::New(i), ToJSString(names[i]));
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSGetEntitiesInMap(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      std::string mapname = ToStdString(args[0]);
      std::vector<dtEntity::EntityId> ids;
      ms->GetEntitiesInMap(mapname, ids);
      
      HandleScope scope;
      Handle<Array> arr = Array::New();
      for(unsigned int i = 0; i < ids.size(); ++i)
      {
         arr->Set(Integer::New(i), Uint32::New(ids[i]));
      }
      return scope.Close(arr);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> MSDeleteEntitiesByMap(const Arguments& args)
   {
      dtEntity::MapSystem* ms = UnwrapMapSystem(args.This());
      ms->DeleteEntitiesByMap(ToStdString(args[0]));
      return Undefined();
   }   

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ConstructMS(const v8::Arguments& args)
   {  
      Handle<External> ext = Handle<External>::Cast(args[0]);
      dtEntity::MapSystem* ls = static_cast<dtEntity::MapSystem*>(ext->Value());   
      args.Holder()->SetInternalField(0, External::New(ls));
    
      return Undefined();
   }
  
   ////////////////////////////////////////////////////////////////////////////////
   void InitMapSystemWrapper(ScriptSystem* ss)
   {
      HandleScope scope;
      Context::Scope context_scope(ss->GetGlobalContext());

      Handle<FunctionTemplate> templt= FunctionTemplate::New();
      templt->SetClassName(String::New("MapSystem"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("addEmptyMap", FunctionTemplate::New(MSAddEmptyMap));
      proto->Set("getEntityIdByUniqueId", FunctionTemplate::New(MSGetEntityIdByUniqueId));
      proto->Set("isSpawnOf", FunctionTemplate::New(MSIsSpawnOf));
      proto->Set("toString", FunctionTemplate::New(MSToString));
      proto->Set("loadMap", FunctionTemplate::New(MSLoadMap));
      proto->Set("loadScene", FunctionTemplate::New(MSLoadScene));
      proto->Set("unloadScene", FunctionTemplate::New(MSUnloadScene));
      proto->Set("saveScene", FunctionTemplate::New(MSSaveScene));
      proto->Set("unloadMap", FunctionTemplate::New(MSUnloadMap));
      proto->Set("getLoadedMaps", FunctionTemplate::New(MSGetLoadedMaps));
      proto->Set("spawn", FunctionTemplate::New(MSSpawn));
      proto->Set("deleteEntitiesByMap", FunctionTemplate::New(MSDeleteEntitiesByMap));
      proto->Set("addSpawner", FunctionTemplate::New(MSAddSpawner));
      proto->Set("deleteSpawner", FunctionTemplate::New(MSDeleteSpawner));
      proto->Set("getSpawner", FunctionTemplate::New(MSGetSpawner));
      proto->Set("getSpawnerCreatedEntities", FunctionTemplate::New(MSGetSpawnerCreatedEntities));
      proto->Set("getAllSpawnerNames", FunctionTemplate::New(MSGetAllSpawnerNames));
      proto->Set("getEntitiesInMap", FunctionTemplate::New(MSGetEntitiesInMap));
      
      RegisterEntitySystempWrapper(ss, dtEntity::MapComponent::TYPE, templt);
   }
}
