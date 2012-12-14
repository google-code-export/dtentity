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


namespace dtEntity
{
   /**
    * Pure virtual interface for loading and saving scene data to maps
    */
   class MapEncoder
   {
      
   public:

      virtual ~MapEncoder() {}

      virtual bool LoadMapFromFile(const std::string& path) = 0;

      // empty destination means overwrite mapPath
      virtual bool SaveMapToFile(const std::string& mapPath, const std::string& destination) = 0;

      virtual bool LoadSceneFromFile(const std::string& path) = 0;
      virtual bool SaveSceneToFile(const std::string& path) = 0;

      virtual bool AcceptsMapExtension(const std::string& /*extension*/) const = 0;
      virtual bool AcceptsSceneExtension(const std::string& /*extension*/) const = 0;
   };
}

