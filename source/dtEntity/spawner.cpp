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

#include <dtEntity/spawner.h>

#include <dtEntity/component.h>
#include <dtEntity/dtentity_config.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/log.h>

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   Spawner::Spawner(const std::string& name, const std::string& mapName, Spawner* parent)
      : mName(name)
      , mMapName(mapName)
      , mAddToSpawnerStore(false)
      , mGUICategory("default")
      , mParent(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   Spawner::~Spawner()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::InitFromEntity(const Entity& entity)
   {
      std::vector<const Component*> toFill;
      entity.GetComponents(toFill);

      std::vector<const Component*>::const_iterator i;
      for(i = toFill.begin(); i != toFill.end(); ++i)
      {
         const Component* component = *i;
         mComponentProperties[component->GetType()] = *component;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::SetValue(ComponentType ctype, StringId propname, const Property& newval)
   {
      ComponentProperties::iterator i = mComponentProperties.find(ctype);
      if(i != mComponentProperties.end()) 
      {
         GroupProperty props = i->second;
         Property* toSet = props.Get(propname);
         if(toSet != NULL)
         {
            toSet->SetFrom(newval);
            mComponentProperties[ctype] = props;
            return true;
         }         
      }
      if(!mParent.valid()) 
      {
         return false;
      }
      return mParent->SetValue(ctype, propname, newval);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::GetAllComponentProperties(ComponentProperties& props) const
   {
      ComponentProperties::const_iterator i;
      for(i = mComponentProperties.begin(); i != mComponentProperties.end(); ++i)
      {
         props[i->first] += i->second;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::GetAllComponentPropertiesRecursive(ComponentProperties& props) const
   {
      if(mParent)
      {
         mParent->GetAllComponentPropertiesRecursive(props);
      }
      GetAllComponentProperties(props);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::Spawn(Entity& entity) const
   {
      // combine component values of this spawner and the parent spawners
      ComponentProperties componentValues;
      GetAllComponentPropertiesRecursive(componentValues);

      //first create all components
      ComponentProperties::const_iterator i;
      for(i = componentValues.begin(); i != componentValues.end(); ++i)
      {
         ComponentType ctype = i->first;
         if(!entity.HasComponent(ctype))
         {
            // start entity system if necessary
            if(!entity.GetEntityManager().HasEntitySystem(ctype))
            {
               bool success = ComponentPluginManager::GetInstance().StartEntitySystem(entity.GetEntityManager(), ctype);
               if(!success)
               {
                  LOG_ERROR("Cannot spawn component, no entity system for this"
                     "component type started: " + GetStringFromSID(ctype));
               }
            }
            
            Component* newcomp;
            bool success = entity.CreateComponent(ctype, newcomp);
            if(!success)
            {
               LOG_ERROR("Could not spawn component of type " + GetStringFromSID(ctype));
            }
         }
      }

      // then set their values
      for(i = componentValues.begin(); i != componentValues.end(); ++i)
      {      
         ComponentType ctype = i->first;
         GroupProperty propertyHolder = i->second;
         Component* newcomp;
         bool success = entity.GetComponent(ctype, newcomp);
         if(!success)
         {
            LOG_WARNING("Cannot set property of component " + GetStringFromSID(ctype));
            continue;
         }

         PropertyGroup props = propertyHolder.Get();

         PropertyGroup::iterator j;
         for(j = props.begin(); j != props.end(); ++j)
         {
            StringId propname = j->first;
            const Property* prop = j->second;
            
            Property* toSet = newcomp->Get(propname);
            if(toSet == NULL)
            {
               LOG_WARNING("Error in spawner: Cannot set property " + GetStringFromSID(propname));
               continue;
            }
            
            bool success = toSet->SetFrom(*prop);
            if(success)
            {  
#if CALL_ONPROPERTYCHANGED_METHOD
               newcomp->OnPropertyChanged(propname, *toSet);
#endif
            }
            else
            {
               LOG_WARNING("Could not set property " + GetStringFromSID(propname));
            }
         }         
      }  

      // inform components that they are finished
      for(i = componentValues.begin(); i != componentValues.end(); ++i)
      {      
         ComponentType ctype = i->first;
         Component* newcomp;
         bool success = entity.GetComponent(ctype, newcomp);
         if(success)
         {
            newcomp->Finished();
         }
      }

      MapComponent* mapcomp;
      if(!entity.GetComponent(mapcomp))
      {
         entity.CreateComponent(mapcomp);
      }
      mapcomp->SetSpawnerName(this->GetName());
      mapcomp->SetMapName(this->GetMapName());

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::HasComponent(ComponentType ctype) const
   {
      return (mComponentProperties.find(ctype) != mComponentProperties.end());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::HasComponentRecursive(ComponentType ctype) const
   {
      if(HasComponent(ctype))
      {
         return true;
      }
      if(!mParent.valid())
      {
         return false;
      }
      return mParent->HasComponentRecursive(ctype);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::AddComponent(ComponentType ctype, const GroupProperty &props)
   {
      mComponentProperties[ctype] = props;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::RemoveComponent(ComponentType ctype)
   {
      ComponentProperties::iterator i = mComponentProperties.find(ctype);
      if(i == mComponentProperties.end())
	  {
         return false;
	  }
	  mComponentProperties.erase(i);
	  return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   GroupProperty Spawner::GetComponentValues(ComponentType ctype) const
   {
      ComponentProperties::const_iterator i = mComponentProperties.find(ctype);
      if(i == mComponentProperties.end())
         return PropertyGroup();
      else
         return i->second;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::SetComponentValues(ComponentType ctype, const GroupProperty &props)
   {
      if(mComponentProperties.find(ctype) == mComponentProperties.end())
      {
         LOG_ERROR("SetComponentValues: Spawner does not exist yet!");
      }
      mComponentProperties[ctype] = props;
   }
}
