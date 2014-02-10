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
#include <dtEntity/entityid.h>
#include <string>
#include <dtEntity/dtentity_config.h>

namespace dtEntity
{

   // serial ID for strings

   #if DTENTITY_USE_STRINGS_AS_STRINGIDS
   typedef std::string StringId;
   #else
   typedef unsigned int StringId;
   #endif

   // type id for messages
   typedef StringId MessageType;

   // type id for components
   typedef StringId ComponentType;

   /**
    * Get a unique StringId for given string. 
    * If DTENTITY_USE_STRINGS_AS_STRINGIDS macri is set to ON
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

   /**
   * get string id crc32 hash of string, add to reverse lookup
   */
   StringId DT_ENTITY_EXPORT SID(unsigned int hash);

   unsigned int SIDToUInt(StringId);

}
