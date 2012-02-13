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

#include <list>
#include <dtEntity/entityid.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osgDB/DynamicLibrary>

namespace dtEntity
{
   class EntityManager;
   class EntitySystem;


   /**
    * PluginFactory is a pure virtual interface for the values
    * returned by an entity system plugin entry point.
    * The plugin factories are stored in the plugin manager and are used to list available
    * entity systems, start and stop them.
    */
   class ComponentPluginFactory : public osg::Referenced
   {
   public:

      ComponentPluginFactory()
      {
      }

      /** get the name of the plugin */
      virtual std::string GetName() const = 0;

      /**
       * Implementations should construct their entity system now and assign it to es variable
       * Return true if success, else false
       */
      virtual bool Create(dtEntity::EntityManager* em, EntitySystem*& es) = 0;

      /** delete the entity system */
      virtual void Destroy() {}
      
      /**
       * return type of entity system
       */
      virtual ComponentType GetType() const = 0;

      /**
       * return a textual description of the entity system functionality
       */
      virtual std::string GetDescription() const = 0;

      /** 
       * fill list with types of all entity systems this system depends on.
       * WARNING: circular dependencies are not handled and
       * will cause a crash!
      */
      virtual void GetDependencies(std::list<ComponentType>&) {}

      osgDB::DynamicLibrary* GetLibrary() const { return mLibrary.get(); }
      void SetLibrary(osgDB::DynamicLibrary* l) { mLibrary = l; }

   protected:

      virtual ~ComponentPluginFactory()
      {
      }

      osg::ref_ptr<osgDB::DynamicLibrary> mLibrary;
   };

}
