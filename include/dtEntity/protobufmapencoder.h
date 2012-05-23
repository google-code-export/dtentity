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
#include <iostream>

namespace dtEntity
{
   class MapSystem;

   class DT_ENTITY_EXPORT ProtoBufMapEncoder
         : public MapEncoder
   {
      
   public:

      ProtoBufMapEncoder(EntityManager& em);
      virtual ~ProtoBufMapEncoder();

      virtual bool LoadMapFromFile(const std::string& path);

      // empty destination means overwrite mapPath
      virtual bool SaveMapToFile(const std::string& mapPath, const std::string& destination = "");

      virtual bool LoadSceneFromFile(const std::string& path);

      // empty dest means overwrite original file
      virtual bool SaveSceneToFile(const std::string& path);

      virtual bool AcceptsMapExtension(const std::string& extension) const
      {
         return extension == "bmap";
      }

      virtual bool AcceptsSceneExtension(const std::string& extension) const
      {
         return extension == "bscene";
      }

      static bool EncodeMessage(const Message& m, std::iostream& stream);
      static Message* DecodeMessage(std::istream& stream);

   private:

      EntityManager* mEntityManager;
      MapSystem* mMapSystem;

   };
}

