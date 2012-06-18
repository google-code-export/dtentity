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

#include "cal3dcomponent.h"
#include <dtEntity/componentpluginmanager.h>

namespace dtEntityCal3d
{
   ////////////////////////////////////////////////////////////////////////////////
   class Cal3dFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         es = new Cal3dSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Cal3d";
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
extern "C" DTE_EXPORT_MACRO void dtEntityMessages_dtEntityCal3d(dtEntity::MessageFactory& mf)
{
}

REGISTER_DTENTITYPLUGIN(dtEntityCal3d, 1,
   new dtEntityCal3d::Cal3dFactory()
)
