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


#include <dtEntityNet/export.h>
#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Quat>
#include <string>

namespace dtEntityNet
{

   ////////////////////////////////////////////////////////////////////////////////
   namespace DeadReckoningAlgorithm
   {
      enum e
      {
        DISABLED = 0,
        STATIC = 1,
        FPW = 2/*, NOT YET IMPLEMENTED!
        FPB,
        FVW,
        FVB,
        RPW,
        RPB,
        RVW,
        RVB*/
      };

      e fromString(const std::string& v);
      std::string toString(e v);
   }

   void CalculateDRFPW(const osg::Vec3d& lastpos,
                       const osg::Vec3f& lastrot,
                       const osg::Vec3f& lastvel,
                       const osg::Vec3f& lastangularvel,
                       float timeSinceLastData,
                       osg::Vec3d& newpos,
                       osg::Vec3f& newrot);

   osg::Vec3f QuatToEuler(const osg::Quat& q);
   osg::Quat EulerToQuat(const osg::Vec3f& euler);
}
