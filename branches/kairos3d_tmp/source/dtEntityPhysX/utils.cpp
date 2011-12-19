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

#include <dtEntityPhysX/utils.h>
#include <NxPhysics.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <vector>
#include <osg/Geode>
#include <osg/PagedLOD>
#include <osg/TriangleFunctor>

namespace dtEntityPhysX
{

   ////////////////////////////////////////////////////////////////
   NxVec3 ConvertVec3(const osg::Vec3& v)
   {
      return NxVec3(v[0], v[1], v[2]);
   }

   ////////////////////////////////////////////////////////////////
   osg::Vec3 ConvertVec3(const NxVec3& v)
   {
      return osg::Vec3(v.x, v.y, v.z);
   }

   ////////////////////////////////////////////////////////////////
   NxQuat ConvertQuat(const osg::Quat& q)
   {
      NxQuat out;
      out.setXYZW(q[0], q[1], q[2], q[3]);
      return out;
   }

   ////////////////////////////////////////////////////////////////
   osg::Quat ConvertQuat(const NxQuat& q)
   {
      return osg::Quat(q.x, q.y, q.z, q.w);
   }

   ////////////////////////////////////////////////////////////////

   const char* getNxSDKCreateError(const NxSDKCreateError& errorCode) 
   {
	   switch(errorCode) 
	   {
		   case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
		   case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
		   case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
		   case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
		   case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
		   case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
		   case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
		   default: return "Unknown error";
	   }
   };

   ////////////////////////////////////////////////////////////////
   bool Equivalent(const osg::Vec3& v1, const osg::Vec3& v2, double delta)
   {
      return (fabs(v1[0] - v2[0]) < delta && fabs(v1[1] - v2[1]) < delta && fabs(v1[2] - v2[2]) < delta);
   }

   void TriangleRecorder::operator()(const osg::Vec3& v1,
      const osg::Vec3& v2,
      const osg::Vec3& v3,
      bool treatVertexDataAsTemporary)
   {
      //osg::Vec3 tv1 = osg::Matrix::transform3x3(v1, mMatrix);
      //osg::Vec3 tv2 = osg::Matrix::transform3x3(v2, mMatrix);
      //osg::Vec3 tv3 = osg::Matrix::transform3x3(v3, mMatrix);
      osg::Vec3 tv1 = v1*mMatrix,
         tv2 = v2*mMatrix,
         tv3 = v3*mMatrix;

      // Throw out any degenerate triangles where any of the verts are EXACTLY the same.
      if (Equivalent(tv1, tv2, 0.0005f) || Equivalent(tv2, tv3, 0.0005f) || Equivalent(tv1, tv3, 0.0005f))
         return;


      StridedVertex sv1, sv2, sv3;
      StridedTriangle t;

      t.Indices[0] = mVertices.size();
      t.Indices[1] = mVertices.size() + 1;
      t.Indices[2] = mVertices.size() + 2;

      sv1.Vertex[0] = tv1[0];
      sv1.Vertex[1] = tv1[1];
      sv1.Vertex[2] = tv1[2];

      sv2.Vertex[0] = tv2[0];
      sv2.Vertex[1] = tv2[1];
      sv2.Vertex[2] = tv2[2];

      sv3.Vertex[0] = tv3[0];
      sv3.Vertex[1] = tv3[1];
      sv3.Vertex[2] = tv3[2];

      mTriangles.push_back(t);

      mVertices.push_back(sv1);
      mVertices.push_back(sv2);
      mVertices.push_back(sv3);
   }

   ////////////////////////////////////////////////////////////////
   void CollectDrawablesVisitor::apply(osg::Geode& node)
    {
        for(unsigned int i=0;i<node.getNumDrawables();++i)
        {
            mDrawables.push_back(node.getDrawable(i));
        }           
        traverse(node);
    }

   void CollectDrawablesVisitor::apply(osg::PagedLOD& plod)
   {
       if (plod.getNumFileNames()>0)
       {
           // Identify the range value for the highest res child
           float targetRangeValue;
           if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
               targetRangeValue = 1e6; // Init high to find min value
           else
               targetRangeValue = 0; // Init low to find max value
               
           const osg::LOD::RangeList rl = plod.getRangeList();
           osg::LOD::RangeList::const_iterator rit;
           for( rit = rl.begin();
                rit != rl.end();
                ++rit )
           {
               if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
               {
                   if( rit->first < targetRangeValue )
                       targetRangeValue = rit->first;
               }
               else
               {
                   if( rit->first > targetRangeValue )
                       targetRangeValue = rit->first;
               }
           }

           // Perform an intersection test only on children that display
           // at the maximum resolution.
           unsigned int childIndex;
           for( rit = rl.begin(), childIndex = 0;
                rit != rl.end();
                rit++, childIndex++ )
           {
               if( rit->first != targetRangeValue )
                   // This is not one of the highest res children
                   continue;

               osg::ref_ptr<osg::Node> child( NULL );
               if( plod.getNumChildren() > childIndex )
                   child = plod.getChild( childIndex );

               
               if ( !child.valid() && plod.getNumChildren()>0)
               {
                   // Child is still NULL, so just use the one at the end of the list.
                   child = plod.getChild( plod.getNumChildren()-1 );
               }

               if (child.valid())
               {
                   child->accept(*this);
               }
           }

       }

       traverse(plod);
   }

}
