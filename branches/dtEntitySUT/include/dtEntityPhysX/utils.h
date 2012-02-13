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

#ifndef DTENTITY_PHYSX_UTILS
#define DTENTITY_PHYSX_UTILS

#include <dtEntityPhysX/export.h>
#include <NxPhysics.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Quat>
#include <vector>
#include <osg/Geode>
#include <osg/TriangleFunctor>

namespace dtEntityPhysX
{

   ////////////////////////////////////////////////////////////////
   struct StridedVertex
   {
      NxVec3 Vertex;
   };

   ////////////////////////////////////////////////////////////////
   struct StridedTriangle
   {
      NxU32 Indices[3];
   };

   ////////////////////////////////////////////////////////////////
   NxVec3 ConvertVec3(const osg::Vec3& v);

   ////////////////////////////////////////////////////////////////
   osg::Vec3 ConvertVec3(const NxVec3& v);

   ////////////////////////////////////////////////////////////////
   NxQuat ConvertQuat(const osg::Quat& q);

   ////////////////////////////////////////////////////////////////
   osg::Quat ConvertQuat(const NxQuat& q);

   ////////////////////////////////////////////////////////////////

   const char* getNxSDKCreateError(const NxSDKCreateError& errorCode) ;

   ////////////////////////////////////////////////////////////////
   // copied from dtAgeiaPhysX
   class TriangleRecorder
   {
   public:
      std::vector<StridedVertex> mVertices;
      std::vector<StridedTriangle> mTriangles;
      osg::Matrix mMatrix;

      /**
      * Called once for each visited triangle.
      *
      * @param v1 the triangle's first vertex
      * @param v2 the triangle's second vertex
      * @param v3 the triangle's third vertex
      * @param treatVertexDataAsTemporary whether or not to treat the vertex data
      * as temporary
      */
      void operator()(const osg::Vec3& v1,
         const osg::Vec3& v2,
         const osg::Vec3& v3,
         bool treatVertexDataAsTemporary);
   };

   ////////////////////////////////////////////////////////////////
   template< class T >
   class DrawableVisitor : public osg::NodeVisitor
   {
   public:

      osg::TriangleFunctor<T> mFunctor;

      /**
      * Constructor.
      */
      DrawableVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      {}

      /**
      * Applies this visitor to a geode.
      *
      * @param node the geode to visit
      */
      virtual void apply(osg::Geode& node)
      {
         for(size_t i=0;i<node.getNumDrawables();i++)
         {
            osg::Drawable* d = node.getDrawable(i);

            if(d->supports(mFunctor))
            {
               osg::NodePath nodePath = getNodePath();
               mFunctor.mMatrix = osg::computeLocalToWorld(nodePath);
               d->accept(mFunctor);
            }
         }
      }
   };

   ////////////////////////////////////////////////////////////////
   class CollectDrawablesVisitor : public osg::NodeVisitor
   {
   public:

       CollectDrawablesVisitor():
           osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
       {         
       }
                  
       virtual void apply(osg::Geode& node);
       virtual void apply(osg::PagedLOD& plod);

       std::list<osg::Drawable*> mDrawables;
   };

}

#endif // DTENTITY_PHYSX_UTILS
