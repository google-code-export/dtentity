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
#include <dtEntity/logmanager.h>

namespace dtEntity
{
   class EntityManager;

   class DT_ENTITY_EXPORT ConsoleLogHandler
      : public LogListener
   {
    public:
      virtual void LogMessage(dtEntity::LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg);
   };

   /**
    * adds required entity systems to entity manager, including map, camera, layer, layerattachpoint.
    * Registers a number of factories for entity systems that should be started lazily.
    * Creates application system, passes it the viewer and installs update callback of app system to scene node.
    * @param argc number of command line args
    * @param standard c command line args
    * @param em a valid entity manager
    */
   void DT_ENTITY_EXPORT AddDefaultEntitySystemsAndFactories(int argc, char** argv, dtEntity::EntityManager& em);

   /**
    * sets osg data paths from command line args or environment variables
    */
   bool DT_ENTITY_EXPORT SetupDataPaths(int argc, char** argv, bool checkPaths);
}

