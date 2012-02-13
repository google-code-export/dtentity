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

#include <dtEntity/scriptmodule.h>
#include <dtEntity/log.h>
#include <CEGUI/CEGUIEventSet.h>
#include <CEGUI/CEGUIEvent.h>

namespace dtEntity
{

   //////////////////////////////////////////////////////////////////////////
   bool ScriptModule::executeScriptedEventHandler(const CEGUI::String& handlerName, const CEGUI::EventArgs& eventArgs)
   {
      const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find( handlerName.c_str());

      if (iter == mCallbacks.end())
      {
         LOG_WARNING(std::string(handlerName.c_str() ) + "is no valid function.");
         return false;
      }

      return iter->second(eventArgs);
   }


   //////////////////////////////////////////////////////////////////////////
   CEGUI::Event::Connection ScriptModule::subscribeEvent(CEGUI::EventSet* window,
                                                         const CEGUI::String& eventName,
                                                         CEGUI::Event::Group groupName,
                                                         const CEGUI::String& subscriberName)
   {

      const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find(subscriberName.c_str());

      if (iter == mCallbacks.end())
      {
         LOG_WARNING(std::string(subscriberName.c_str() ) + "is no valid callback.");
         return NULL;
      }
      CEGUI::SubscriberSlot slot = iter->second;
      CEGUI::Event::Connection c;
      if (groupName)
      {
          c = window->subscribeEvent(eventName, groupName, slot);
      }
      else
      {
         c = window->subscribeEvent(eventName, slot);
      }

      mConnections.push_back(c);
      return c;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ScriptModule::AddCallback(const std::string& callbackName, const CEGUI::SubscriberSlot& subscriberSlot)
   {
      mCallbacks[callbackName.c_str()] = subscriberSlot;
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   //bool ScriptModule::RemoveCallback(const std::string& callbackName)
   //{
   //   const std::map<std::string, CEGUI::SubscriberSlot>::iterator iter = mCallbacks.find(callbackName.c_str());
   //
   //   if (iter == mCallbacks.end())
   //   {
   //      LOG_WARNING(std::string(callbackName.c_str() ) + "is no valid callback.");
   //      return false;
   //   }
   //
   //   CEGUI::SubscriberSlot slot = iter->second;
   //   slot.cleanup();
   //
   //   mCallbacks.erase(iter);
   //
   //   int count = (int)mConnections.size();
   //   for (int index = 0; index < count; ++index)
   //   {
   //      CEGUI::Event::Connection c = mConnections[index];
   //      if (!c.isValid())
   //      {
   //         mConnections.erase(mConnections.begin() + index);
   //         index--;
   //         count--;
   //      }
   //   }
   //
   //   return true;
   //}

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Event::Connection dtEntity::ScriptModule::subscribeEvent(CEGUI::EventSet* window,
                                                                const CEGUI::String& eventName,
                                                                const CEGUI::String& subscriberName)
   {
      return subscribeEvent(window, eventName, (CEGUI::Event::Group) 0, subscriberName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool dtEntity::ScriptModule::NotSupported(const std::string& methodName)
   {
      LOG_WARNING( methodName + " not handled."); return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void dtEntity::ScriptModule::executeScriptFile(const CEGUI::String &fileName, const CEGUI::String &resourceGroup/*="" */)
   {
      NotSupported( __FUNCTION__ );
   }

   ////////////////////////////////////////////////////////////////////////////////
   int dtEntity::ScriptModule::executeScriptGlobal(const CEGUI::String& functionName)
   {
      return NotSupported( __FUNCTION__ );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void dtEntity::ScriptModule::executeString(const CEGUI::String &str)
   {
      NotSupported( __FUNCTION__ );
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::ScriptModule::CallbackRegistry& dtEntity::ScriptModule::GetRegistry() const
   {
      return mCallbacks;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::ScriptModule::~ScriptModule()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   void dtEntity::ScriptModule::destroyBindings(void)
   {
      mCallbacks.clear();

      std::vector<CEGUI::Event::Connection>::iterator itr = mConnections.begin();

      while(itr != mConnections.end())
      {
         if (itr->isValid())
         {
            if ((*itr)->connected())
            {
               (*itr)->disconnect();
            }
            ++itr;
         }
      }

      mConnections.clear();
   }
}
