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

#include "export.h"
#include "testcomponent.h"
#include <dtEntity/componentpluginmanager.h>


////////////////////////////////////////////////////////////////////////////////
class TESTENTTITYSYSTEM_EXPORT TestPluginFactory : public dtEntity::ComponentPluginFactory
{
public:


   virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
   {
      es = new TestSystem(*em);
      return true;
   }

   virtual std::string GetName() const
   {
      return "Test";
   }

   /** get a description of the plugin */
   virtual std::string GetDescription() const
   {
      return "Test System";
   }
};


REGISTER_DTENTITYPLUGIN(testEntitySystemPlugin, 1,
   new TestPluginFactory()
)