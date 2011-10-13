#pragma once

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

#include <osg/Quat>
#include <osg/Vec3>

namespace dtEntitySimulation
{

   // lifted from Irrlicht. Irrlicht license:
   /* irrlicht.h -- interface of the 'Irrlicht Engine'

     Copyright (C) 2002-2009 Nikolaus Gebhardt

     This software is provided 'as-is', without any express or implied
     warranty.  In no event will the authors be held liable for any damages
     arising from the use of this software.

     Permission is granted to anyone to use this software for any purpose,
     including commercial applications, and to alter it and redistribute it
     freely, subject to the following restrictions:

     1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
     2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
     3. This notice may not be removed or altered from any source distribution.

     Please note that the Irrlicht Engine is based in part on the work of the
     Independent JPEG Group, the zlib and the libPng. This means that if you use
     the Irrlicht Engine in your product, you must acknowledge somewhere in your
     documentation that you've used the IJG code. It would also be nice to mention
     that you use the Irrlicht Engine, the zlib and libPng. See the README files
     in the jpeglib, the zlib and libPng for further informations.
   */

   typedef double	f64;
   typedef float	f32;

   osg::Vec3 QuatToEuler(const osg::Quat& q)
   {
      f64 X = q[0];
      f64 Y = q[1];
      f64 Z = q[2];
      f64 W = q[3];
	   const float sqw = W*W;
	   const float sqx = X*X;
	   const float sqy = Y*Y;
	   const float sqz = Z*Z;

      osg::Vec3 euler;
	   // heading = rotation about z-axis
	   euler[2] = (f32) (atan2(2.0 * (X*Y +Z*W),(double)(sqx - sqy - sqz + sqw)));

	   // bank = rotation about x-axis
	   euler[0] = (f32) (atan2(2.0 * (Y*Z +X*W),(double)(-sqx - sqy + sqz + sqw)));

	   // attitude = rotation about y-axis
      f64 v = -2.0f * (X*Z - Y*W);
      if(v < -1.0) v = -1.0;
      if(v > 1.0) v = 1.0;
      euler[1] = asinf(v);
      return euler;
   }

   osg::Quat EulerToQuat(const osg::Vec3& euler)
   {
      f64 angle;

	   angle = euler[0] * 0.5;
	   const f64 sr = sin(angle);
	   const f64 cr = cos(angle);

	   angle = euler[1] * 0.5;
	   const f64 sp = sin(angle);
	   const f64 cp = cos(angle);

	   angle = euler[2] * 0.5;
	   const f64 sy = sin(angle);
	   const f64 cy = cos(angle);

	   const f64 cpcy = cp * cy;
	   const f64 spcy = sp * cy;
	   const f64 cpsy = cp * sy;
	   const f64 spsy = sp * sy;

      osg::Quat ret;
      
	   ret[0] = (f32)(sr * cpcy - cr * spsy);
	   ret[1] = (f32)(cr * spcy + sr * cpsy);
	   ret[2] = (f32)(cr * cpsy - sr * spcy);
	   ret[3] = (f32)(cr * cpcy + sr * spsy);

      const f32 n = ret[0]*ret[0] + ret[1]*ret[1] + ret[2]*ret[2] + ret[3]*ret[3];

	   if (n != 1)
      {
        ret *= 1.0 / sqrt(n);
      }

	   return ret;
   }

}

