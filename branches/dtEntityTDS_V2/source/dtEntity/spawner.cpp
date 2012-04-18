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

#include <dtEntity/spawner.h>

#include <dtEntity/component.h>
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
         PropertyContainer::ConstPropertyMap props;
         component->GetProperties(props);
         
         DynamicPropertyContainer ph;         
         PropertyContainer::ConstPropertyMap::iterator j;
         for(j = props.begin(); j != props.end(); ++j)
         {
            ph.AddProperty(j->first, *j->second);
         }
         mComponentProperties[component->GetType()] = ph;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Spawner::SetValue(ComponentType ctype, StringId propname, const Property& newval)
   {
      ComponentProperties::iterator i = mComponentProperties.find(ctype);
      if(i != mComponentProperties.end()) 
      {
         DynamicPropertyContainer props = i->second;
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
         DynamicPropertyContainer propertyHolder = i->second;
         Component* newcomp;
         bool success = entity.GetComponent(ctype, newcomp);
         if(!success)
         {
            LOG_WARNING("Cannot set property of component " + GetStringFromSID(ctype));
            continue;
         }

         PropertyContainer::ConstPropertyMap props;
         propertyHolder.GetProperties(props);

         PropertyContainer::ConstPropertyMap::iterator j;         
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
               newcomp->OnPropertyChanged(propname, *toSet);
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
   void Spawner::AddComponent(ComponentType ctype, const DynamicPropertyContainer& props)
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
   DynamicPropertyContainer Spawner::GetComponentValues(ComponentType ctype) const
   {
      ComponentProperties::const_iterator i = mComponentProperties.find(ctype);
      if(i == mComponentProperties.end())
         return DynamicPropertyContainer();
      else
         return i->second;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Spawner::SetComponentValues(ComponentType ctype, const DynamicPropertyContainer& props)
   {
      mComponentProperties[ctype] = props;
   }
}
