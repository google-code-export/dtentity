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
   namespace NodeMasks
   {
      enum e
      {
         REFLECTION_SCENE              = (1<<0),    // OSGOcean
         REFRACTION_SCENE              = (1<<1),    // OSGOcean
         NORMAL_SCENE                  = (1<<2),    // OSGOcean
         SURFACE_MASK                  = (1<<3),    // OSGOcean
         SILT_MASK                     = (1<<4),    // OSGOcean
         VISIBLE                       = (1 << 5),
         PICKABLE                      = (1 << 6),
         TERRAIN                       = (1 << 7),
         CASTS_SHADOWS                 = (1 << 8),
         RECEIVES_SHADOWS              = (1 << 9)
      };
   }

}
