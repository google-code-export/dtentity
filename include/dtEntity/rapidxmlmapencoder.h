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
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapencoder.h>
#include <dtEntity/property.h>
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

      virtual bool AcceptsMapExtension(const std::string& extension) const
      {
         return extension == "dtemap";
      }

      virtual bool AcceptsSceneExtension(const std::string& extension) const
      {
         return extension == "dtescene";
      }

      static dtEntity::Property* ParseProperty(rapidxml::xml_node<>* element);

      // create XML nodes for properties and add them to parent node
      static void SerializeProperties(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parent,  const std::map<std::string, const Property*>& props);

   private:

      EntityManager* mEntityManager;
      MapSystem* mMapSystem;

   };
}

