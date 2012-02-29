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

#include "messages.h"
#include "hudcomponent.h"
#include "rocketcomponent.h"
#include "rocketsystemwrapper.h"
#include <dtEntity/componentplugin.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/messagefactory.h>
#include <dtEntityWrappers/scriptcomponent.h>

namespace dtEntityRocket
{


   ////////////////////////////////////////////////////////////////////////////////
   class RocketFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         dtEntityWrappers::ScriptSystem* scriptsys;
         bool success = em->GetEntitySystem(dtEntityWrappers::ScriptSystem::TYPE, scriptsys);
         assert(success);
         InitRocketSystemWrapper(scriptsys);
         es = new RocketSystem(*em);        

         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Rocket";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "LibRocket bindings";
      }

   };

   ////////////////////////////////////////////////////////////////////////////////
   class HUDFactory : public dtEntity::ComponentPluginFactory
   {
   public:


      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
        es = new HUDSystem(*em);

         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "HUD";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "LibRocket HUD";
      }

   };
}

////////////////////////////////////////////////////////////////////////////////
extern "C" DTE_EXPORT_MACRO void dtEntityMessages_dtEntityRocket(dtEntity::MessageFactory& mf)
{
   using namespace dtEntityRocket;
   mf.RegisterMessageType<RocketEventMessage>(RocketEventMessage::TYPE);
}



REGISTER_DTENTITYPLUGIN(dtEntityRocket, 2,
   new dtEntityRocket::RocketFactory(),
   new dtEntityRocket::HUDFactory()
)
