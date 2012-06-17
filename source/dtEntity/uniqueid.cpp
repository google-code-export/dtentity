/*
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
