#pragma once

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

