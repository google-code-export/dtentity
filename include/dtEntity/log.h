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

#include <dtEntity/logmanager.h>
#include <sstream>

#define DT_LOG_SOURCE __FILE__, __FUNCTION__, __LINE__

#define LOG_DEBUG(msg)\
{\
   std::ostringstream os; os << msg; \
   dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_DEBUG, __FILE__, __FUNCTION__, __LINE__, os.str());\
}\

#define LOG_INFO(msg)\
{\
   std::ostringstream os; os << msg; \
   dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_INFO, __FILE__, __FUNCTION__, __LINE__, os.str());\
}\

#define LOG_WARNING(msg)\
{\
   std::ostringstream os; os << msg; \
   dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_WARNING, __FILE__, __FUNCTION__, __LINE__, os.str());\
}\

#define LOG_ERROR(msg)\
{\
   std::ostringstream os; os << msg; \
   dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_ERROR, __FILE__, __FUNCTION__, __LINE__, os.str());\
}\

#define LOG_ALWAYS(msg)\
{\
   std::ostringstream os; os << msg; \
   dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_ALWAYS, __FILE__, __FUNCTION__, __LINE__, os.str());\
}\

