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
#include <dtEntity/entity.h>
#include <dtEntity/entityid.h>
#include <dtEntity/propertycontainer.h>
#include <dtEntity/stringid.h>
#include <osg/Referenced>
#include <osg/ref_ptr>

namespace dtEntity
{
   
   /** 
     * Spawner: a template for creating entities.
	 * Each spawner contains a number of components which contain properties. 
	 * When Spawner::Spawn is called, components are created for the passed entity and 
	 * their properties are set to specified values.
	 * Spawners can form a sort of inheritance tree. When a child spawner is used to 
	 * spawn an entity, all components defined in the spawner and its parent chain are 
	 * created. Children spawners can overwrite the property values of their parents.
	 */
   class DT_ENTITY_EXPORT Spawner : public osg::Referenced
   {
   public:
      
      typedef std::map<ComponentType, DynamicPropertyContainer> ComponentProperties;

      /** 
       * Constructor 
       */
      Spawner(const std::string& name, const std::string& mapName, Spawner* parent = NULL);

		/**
		*return name of spawner
		*/
		std::string GetName() const { return mName; }

      /**
	   *return name of map this spawner was loaded from 
	   */
		std::string GetMapName() const { return mMapName; }

		/**
		  * Should spawner show up in spawner drag & drop GUI?
		  */
		bool GetAddToSpawnerStore() const { return mAddToSpawnerStore; }
		void SetAddToSpawnerStore(bool v) { mAddToSpawnerStore = v; }

		/**
		  * category for spawner store
		  */
		std::string GetGUICategory() { return mGUICategory; }
		void SetGUICategory(const std::string& v) { mGUICategory = v; }

		/**
		  * icon for representing spawner in GUI
		  */
		std::string GetIconPath() { return mIconPath; }
		void SetIconPath(const std::string& v) { mIconPath = v; }

		/**
		*return name of map this spawner was loaded from
		*/
		Spawner* GetParent() { return mParent; }
		const Spawner* GetParent() const { return mParent; }

      /**
       * Collect all components of entity and all properties of these components
       * and store them
       */
      void InitFromEntity(const Entity& entity);

      /**
       * Add components to entity and set their properties. Calls OnFinishedSettingProperties().
       */
      bool Spawn(Entity& entity) const;

	  /**
	   * Copy value from newval to a component property
	   */
      bool SetValue(ComponentType ctype, StringId propname, const Property& newval);
      
	  /**
	   * Return true if spawner contains a component of this type
	   */
      bool HasComponent(ComponentType ctype) const;

	  /**
	   * Return true if spawner or one of its parents contain a component of this type
	   */
      bool HasComponentRecursive(ComponentType ctype) const;

	  /**
	   * Add a component of type ctype. The props container holds the initial property
	   * values to assign to the component.
	   */
      void AddComponent(ComponentType ctype, const DynamicPropertyContainer& props);

	  /**
	   * Remove component of this type. Return true if component existed
	   */
      bool RemoveComponent(ComponentType ctype);

	  /**
	   * Get property values to assign to component after spawning it
	   */
      DynamicPropertyContainer GetComponentValues(ComponentType ctype) const;

	  /**
	   * Set properties of existing component in spawner.The props container holds the initial property
	   * values to assign to the component. All previous component values are cleared.
	   */
      void SetComponentValues(ComponentType ctype, const DynamicPropertyContainer&);

      /**
       * Get a map of components and their properties set for this spawner
       */
      void GetAllComponentProperties(ComponentProperties& props) const;

       /**
       * Get component properties of spawner hierarchy, children values overwrite
       * parent values
       */
      void GetAllComponentPropertiesRecursive(ComponentProperties& props) const;

   protected:

      virtual ~Spawner();
      
   private:

      // name of spawner
      std::string mName;

	   // name of map this spawner was loaded from
      std::string mMapName;

      // icon for representing spawner in GUI
      std::string mIconPath;

      // should spawner show up in spawner drag & drop GUI?
      bool mAddToSpawnerStore;

      // category for spawner store
      std::string mGUICategory;

      // parent spawner, may be empty
      osg::ref_ptr<Spawner> mParent;

	  // The components and their properties that should be spawned
      ComponentProperties mComponentProperties;   
   };

}
