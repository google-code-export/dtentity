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

#ifdef BUILD_WITH_DELTA3D
	#include <dtUtil/log.h>
#else

	#include <osg/Notify>

	#define LOG_DEBUG(msg)\
   OSG_DEBUG << msg << std::endl; OSG_DEBUG.flush();

   #define LOG_INFO(msg)\
      OSG_INFO << msg << std::endl; OSG_INFO.flush();

   #define LOG_WARNING(msg)\
      OSG_WARN << msg << std::endl; OSG_WARN.flush();

   #define LOG_ERROR(msg)\
      OSG_FATAL << msg << std::endl; OSG_FATAL.flush();

   #define LOG_ALWAYS(msg)\
      OSG_ALWAYS << msg << std::endl; OSG_ALWAYS.flush();

#endif
