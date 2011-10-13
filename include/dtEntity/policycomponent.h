#pragma once

/* -*-c++-*-
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

#include <dtEntity/export.h>
#include <dtEntity/component.h>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/FastDelegate.h>
#include <dtEntity/FastDelegateBind.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   struct CallbackPolicyNone
   {
      static void NotifyPropertyChanged(Component* component, StringId propname, const Property& prop)
      {
      }

      static void NotifyPropertiesChanged(Component* component)
      {
      }

   protected:

      ~CallbackPolicyNone() {}

   };

   ////////////////////////////////////////////////////////////////////////////////
   typedef fastdelegate::FastDelegate3<const Component*, StringId, const Property*, void> PropertyChangedFunctor;
   typedef fastdelegate::FastDelegate1<const Component*, void> PropertiesChangedFunctor;

   struct CallbackPolicyVector
   {
      typedef std::multimap<StringId, PropertyChangedFunctor> PCBMap;
      typedef std::vector<PropertiesChangedFunctor> ACBMap;
      void NotifyPropertyChanged(Component* component, StringId propname, const Property& prop)
      {
         std::pair<PCBMap::iterator, PCBMap::iterator> keyRange;
         keyRange = mPropertyCallbacks.equal_range(propname);

         for(PCBMap::iterator it = keyRange.first; it != keyRange.second; ++it)
         {
            it->second(component, propname, &prop);
         }
      }

      void NotifyPropertiesChanged(Component* component)
      {
         for(ACBMap::iterator i = mAllChangedCallbacks.begin(); i != mAllChangedCallbacks.end(); ++i)
         {
            (*i)(component);
         }
      }

      void RegisterForPropertyChanged(StringId propname, const PropertyChangedFunctor& ftr)
      {
         mPropertyCallbacks.insert(std::make_pair(propname, ftr));
      }

      void UnregisterForPropertyChanged(StringId propname, const PropertyChangedFunctor& ftr)
      {
         // TODO will probably cause corruption when cause from a
         // property callback
         std::pair<PCBMap::iterator, PCBMap::iterator> keyRange;
         keyRange = mPropertyCallbacks.equal_range(propname);
         PCBMap::iterator it = keyRange.first;
         for(; it != keyRange.second; ++it)
         {
            if(it->second == ftr)
            {
               mPropertyCallbacks.erase(it);
               return;
            }
         }
      }

      void RegisterForPropertiesChanged(const PropertiesChangedFunctor& ftr)
      {
         mAllChangedCallbacks.push_back(ftr);
      }

      void UnregisterForPropertiesChanged(const PropertiesChangedFunctor& ftr)
      {
         for(ACBMap::iterator i = mAllChangedCallbacks.begin(); i != mAllChangedCallbacks.end(); ++i)
         {
            if((*i) == ftr)
            {
               mAllChangedCallbacks.erase(i);
               return;
            }
         }

      }

   protected:
      ~CallbackPolicyVector() {}

      PCBMap mPropertyCallbacks;
      ACBMap mAllChangedCallbacks;

   };

   ////////////////////////////////////////////////////////////////////////////////
   template<class CallbackPolicy = CallbackPolicyNone>
   class PolicyComponent
      : public Component
      , public CallbackPolicy
   {
   public:

      virtual void OnPropertyChanged(StringId propname, Property& prop)
      {
         Component::OnPropertyChanged(propname, prop);
         NotifyPropertyChanged(this, propname, prop);
      }

      virtual void OnFinishedSettingProperties()
      {
         Component::OnFinishedSettingProperties();
         NotifyPropertiesChanged(this);
      }
   };


}
