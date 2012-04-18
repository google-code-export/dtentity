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
#include <dtEntity/singleton.h>
#include <dtEntity/entityid.h>
#include <string>

namespace dtEntity
{

   /**
    * Get a unique StringId for given string. 
    * If DTENTITY_USE_STRINGS_AS_STRINGIDS macro is set
    * this returns the string itself. Else a CRC32 hash
    * is returned. 
    * In any case, the crc32 value is added to a reverse lookup table
    * so that it can be resolved back to a string.
    * This function is thread safe.
    * @param str The string to index
    */
   StringId DT_ENTITY_EXPORT SID(const std::string& str);

   /**
    * Get the string that was used to generate the String id.
    * If DTENTITY_USE_STRINGS_AS_STRINGIDS macro is set 
    * the input string is returned unchanged. Else
    * a crc32 value is expected as input.
    */
   std::string DT_ENTITY_EXPORT GetStringFromSID(StringId id);

   /**
    * get string id crc32 hash of string, don't add to reverse lookup
    */
   StringId DT_ENTITY_EXPORT SIDHash(const std::string& str);
   StringId DT_ENTITY_EXPORT SIDHash(StringId str);

   /**
   * get string id crc32 hash of string, add to reverse lookup
   */
   StringId DT_ENTITY_EXPORT SID(unsigned int hash);

}
