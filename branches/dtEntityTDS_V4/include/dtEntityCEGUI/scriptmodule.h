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

#include <dtEntityCEGUI/export.h>
#include <dtEntity/FastDelegate.h>
#include <dtEntity/FastDelegateBind.h>
#include <CEGUI/CEGUIScriptModule.h>
#include <map>
#include <string>
#include <vector>

namespace dtEntityCEGUI
{
  /** \brief ScriptModule is the binding from CEGUI::Events to specific application callbacks.
    * Create an instance of this class, and provide it as a parameter during construction of a CEUIDrawable instance.
    * Add new handlers with the AddCallback function.
    */
   class DTENTITY_CEGUI_EXPORT ScriptModule : public CEGUI::ScriptModule
   {
      public:
         virtual ~ScriptModule();

         virtual void destroyBindings(void);


         virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* window,
                                                         const CEGUI::String& eventName,
                                                         CEGUI::Event::Group groupName,
                                                         const CEGUI::String& subscriberName);

         virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* window,
                                                         const CEGUI::String& eventName,
                                                         const CEGUI::String& subscriberName);

         virtual bool executeScriptedEventHandler(const CEGUI::String& handlerName,
                                                  const CEGUI::EventArgs& eventArgs);

         virtual void executeScriptFile (const CEGUI::String &fileName,
                                        const CEGUI::String &resourceGroup="");

         virtual int executeScriptGlobal (const CEGUI::String& functionName);

         virtual void executeString(const CEGUI::String &str);
 
         ///Deprecated 2/23/10
         //typedef dtUtil::Functor<bool,TYPELIST_1(const CEGUI::EventArgs&)> HandlerFunctor;
         typedef fastdelegate::FastDelegate1< const CEGUI::EventArgs&, bool> HandlerFunctor;

         /**
         * Add a callback handler.
         * Example:
         * @code
         * class MyClass
         * {
         *   ...
         *   static bool OnClickStatic (const CEGUI::EventArgs &e);
         *   bool OnClick (const CEGUI::EventArgs &e);
         *   ...
         * }
         *
         * mScriptModule->AddCallback("OnDoSomething", &OnClickStatic);
         * mScriptModule->AddCallback("OnDoSomething", CEGUI::SubscriberSlot(&OnClick, this));
         * @endcode
         *
         * @param callbackName is the string representation of the handler function to be executed for the CEGUI::Event.
         * @param subscriberSlot function to be called when the CEGUI::Event is activated.
         */
         bool AddCallback(const std::string& callbackName, const CEGUI::SubscriberSlot& subscriberSlot);
         //bool RemoveCallback(const std::string& callbackName);

         typedef std::map<std::string,CEGUI::SubscriberSlot> CallbackRegistry;
         
         /**
          * Returns the StaticRegistry.
          */
         const CallbackRegistry& GetRegistry() const;

      private:
         bool NotSupported(const std::string& methodName);

         CallbackRegistry mCallbacks;
         std::vector<CEGUI::Event::Connection> mConnections; 
   };
}
