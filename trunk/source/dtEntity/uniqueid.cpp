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

#include <dtEntity/uniqueid.h>

#include <dtEntity/log.h>
#ifdef WIN32
   #include <Rpc.h>
   #include <Rpcdce.h>
#else
#include <uuid/uuid.h>
   #include <sys/stat.h>
   #include <sys/types.h>
#endif

namespace dtEntity
{
   std::string CreateUniqueIdString()
   {
#ifdef WIN32
   GUID guid;

   if( UuidCreate( &guid ) == RPC_S_OK )
   {
      unsigned char* guidChar;

      if( UuidToString( const_cast<UUID*>(&guid), &guidChar ) == RPC_S_OK )
      {
         std::string str = reinterpret_cast<const char*>(guidChar);
         if(RpcStringFree(&guidChar) != RPC_S_OK)
         {
            LOG_ERROR("Could not free memory.");
         }
         return str;
      }
      else
      {
         LOG_WARNING("Could not convert UniqueId to std::string." );
         return "ERROR";
      }
   }
   else
   {
      LOG_WARNING("Could not generate UniqueId." );
      return "ERROR";
   }
#else
   uuid_t uuid;
   uuid_generate( uuid );

   char buffer[37];
   uuid_unparse(uuid, buffer);

   return buffer;
#endif
   }

}
