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
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapencoder.h>
#include <dtEntity/property.h>
#include <osg/ref_ptr>
#include <osg/Timer>
#include <rapidxml.hpp>

namespace dtEntity
{

   class Spawner;
   class MapSystem;
   class Property;

   class DT_ENTITY_EXPORT RapidXMLMapEncoder
         : public MapEncoder
   {
      
   public:

      RapidXMLMapEncoder(EntityManager& em);
      virtual ~RapidXMLMapEncoder();

      virtual bool LoadMapFromFile(const std::string& path);

      // empty destination means overwrite mapPath
      virtual bool SaveMapToFile(const std::string& mapPath, const std::string& destination = "");

      virtual bool LoadSceneFromFile(const std::string& path);

      // empty dest means overwrite original file
      virtual bool SaveSceneToFile(const std::string& path);

      static dtEntity::Property* ParseProperty(rapidxml::xml_node<>* element);

      // create XML nodes for properties and add them to parent node
      static void SerializeProperties(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parent,  const std::map<std::string, const Property*>& props);

   private:

      EntityManager* mEntityManager;
      MapSystem* mMapSystem;

   };
}

