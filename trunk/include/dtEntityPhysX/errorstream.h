/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#ifndef ERRORSTREAM_H
#define ERRORSTREAM_H

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif

namespace dtEntityPhysX
{
   class ErrorStream : public NxUserOutputStream
	   {
	   public:
	   void reportError(NxErrorCode e, const char* message, const char* file, int line)
		   {
		   printf("%s (%d) :", file, line);
		   switch (e)
			   {
			   case NXE_INVALID_PARAMETER:
				   printf( "invalid parameter");
				   break;
			   case NXE_INVALID_OPERATION:
				   printf( "invalid operation");
				   break;
			   case NXE_OUT_OF_MEMORY:
				   printf( "out of memory");
				   break;
			   case NXE_DB_INFO:
				   printf( "info");
				   break;
			   case NXE_DB_WARNING:
				   printf( "warning");
				   break;
			   default:
				   printf("unknown error");
			   }

		   printf(" : %s\n", message);
		   }

	   NxAssertResponse reportAssertViolation(const char* message, const char* file, int line)
		   {
		   printf("access violation : %s (%s line %d)\n", message, file, line);
   #ifdef WIN32
		   switch (MessageBox(0, message, "AssertViolation, see console for details.", MB_ABORTRETRYIGNORE))
			   {
			   case IDRETRY:
				   return NX_AR_CONTINUE;
			   case IDIGNORE:
				   return NX_AR_IGNORE;
			   case IDABORT:
			   default:
				   return NX_AR_BREAKPOINT;
			   }
   #elif LINUX
		   assert(0);
   #elif _XBOX
		   return NX_AR_BREAKPOINT;
   #elif __CELLOS_LV2__
		   return NX_AR_BREAKPOINT;
   #endif
		   }

	   void print(const char* message)
		   {
		   printf(message);
		   }
	   };
}

#endif
