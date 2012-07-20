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

namespace dtEntity
{     
   #if defined(_MSC_VER)
       typedef __int64 Timer_t;
   #else
       typedef unsigned long long Timer_t;
   #endif

   class DT_ENTITY_EXPORT SystemInterface
   {
   public:
      virtual float GetDeltaSimTime() const = 0;
      virtual float GetDeltaRealTime() const = 0;
      virtual float GetTimeScale() const = 0;
      virtual void SetTimeScale(float) = 0;
      virtual double GetSimulationTime() const = 0;
      virtual Timer_t GetSimulationClockTime() const = 0;
      virtual void SetSimulationClockTime(Timer_t) = 0;
      virtual Timer_t GetRealClockTime() = 0;

   };
  
}
