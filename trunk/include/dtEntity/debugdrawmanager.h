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
#include <dtEntity/entitymanager.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <osg/ref_ptr>
#include <osg/Referenced>

namespace osg
{
   class Group;
   class Geode;
   class Geometry;
}

namespace dtEntity
{

   /**
    * Can be used to add simple debug geometry to the scene. Each debug graphics primitive is added
    * together with a lifetime /  duration. 
    * If you want to do per-frame drawing, simply set a duration of 0 and
    * execute the draw each frame.
    * Warning: To use, first do SetEnabled(true), it is off by default!
    */
   class DT_ENTITY_EXPORT DebugDrawManager : public osg::Referenced
   {
   public:      

      /**
       * Constructor.
       * @param layer Add debug draw manager to layer of this name
       */
      DebugDrawManager(dtEntity::EntityManager& em, StringId layer = LayerAttachPointSystem::DefaultLayerId);
      ~DebugDrawManager();

      /**
       * message functors, don't call directly
       */
      void Tick(const Message& msg);
      void OnEnable(const Message& m);

      /**
       * Draw a single point
       * @param position Draw point here
       * @param color Draw with this color
       * @param size Size of points in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddPoint(const Vec3f& position, const Vec4f& color, int size = 1,
                    float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw multiple points
       * @param positions A vector of point positions
       * @param color Draw with this color
       * @param size Size of points in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddPoints(const std::vector<Vec3f>& positions, const Vec4f& color, int size = 1,
                     float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a single line
       * @param start Line starts here
       * @param end Line ends here
       * @param color Draw with this color
       * @param linewidth Width of line in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddLine(const Vec3f& start, const Vec3f& end, const Vec4f& color, int linewidth = 1,
                   float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a number of lines
       * @param lines A vector containing two entries for each line. So to draw two lines you have to have four entries.
       * @param color Draw with this color
       * @param linewidth Width of line in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddLines(const std::vector<Vec3f>& lines, const Vec4f& color, int linewidth = 1,
                    float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a 3d cross
       * @param position Draw point here
       * @param color Draw with this color
       * @param size length of each arm
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddCross(const Vec3f& position, const Vec4f& color, float size,
                    float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a sphere
       * @param position Draw sphere here
       * @param radius radius of sphere
       * @param color Draw with this color
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddSphere(const Vec3f& position, float radius, const Vec4f& color,
                     float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a single triangle
       * @param vert0 First vertex of triangle
       * @param vert1 Second vertex of triangle
       * @param vert2 Third vertex of triangle
       * @param color Draw with this color
       * @param lineWidth with of line to draw in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddTriangle(const Vec3f& vert0, const Vec3f& vert1, const Vec3f& vert2,
                       const Vec4f& color, int lineWidth, float duration = 0,
                       bool depthTestEnabled = true);

      /**
       * Draw multiple triangles
       * @param verts A vector containing triangle verts. The vector should contain three entries for each triangle.
       * @param color Draw with this color
       * @param lineWidth with of line to draw in pixels
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddTriangles(const std::vector<Vec3f>& verts, const Vec4f& color,
                        int lineWidth, float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a mesh
       * @param geometry The OSG geometry that should be drawn
       * @param position Draw geometry here
       * @param lineColor Draw wireframe with this color. If alpha value is 0 then wireframe will not be drawn
       * @param lineWidth Draw wireframe with this line with. If value is 0 then wireframe will not be drawn
       * @param faceColor Draw polygons with this color. Blending is enabled, so feel free to set an alpha value.
       *                  Alpha value 0 means no polygons are drawn
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddMesh(osg::Geometry* geometry, const Vec3f& position, const Vec4f& lineColor,
                   int lineWidth, const Vec4f& faceColor, float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a circle
       * @param position Center of circle is here
       * @param planeNormal A normal vector pointing away from the face of the disk
       * @param radius Radius of disk
       * @param color Draw with this color
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddCircle(const Vec3f& position, const Vec3f& planeNormal, float radius, const Vec4f& color, float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw a text string
       * @param position Draw text here
       * @param str The text to draw
       * @param color Draw with this color
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddString(const Vec3f& position, const std::string& str, const Vec4f& color, float duration = 0, bool depthTestEnabled = true);

      /**
       * Draw an axis aligned bounding box
       * @param minCoords start box from here
       * @param maxCoords end box here
       * @param color Draw with this color
       * @param lineWidth with of line to draw
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddAABB(const Vec3f& minCoords, const Vec3f& maxCoords, const Vec4f& color, int lineWidth, float duration = 0, bool depthTestEnabled = true);

      /**
       * Add an oriented bounding box
       * @param matrix transformation matrix to apply to box
       * @param minCoords start box from here
       * @param maxCoords end box here
       * @param color Draw with this color
       * @param lineWidth with of line to draw
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddOBB(const Matrix& matrix, const Vec3f& minCoords, const Vec3f& maxCoords, const Vec4f& color, int lineWidth, float duration = 0, bool depthTestEnabled = true);

      /**
       * Add a cross showing coordinate orientation
       * @param matrix transformation cross by this matrix
       * @param color Draw with this color
       * @param size length of cross arms
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddAxes(const Matrix& matrix, const Vec4f& color, float size, float duration = 0, bool depthTestEnabled = true);
      
      /**
       * Add an OSG drawable
       * @param drawable Add this drawable
       * @param duration Show for how many seconds. Value 0 means draw once
       * @param depthTestEnabled if false, always draw on top
       */
      void AddDrawable(osg::Drawable* drawable, float duration = 0, bool depthTestEnabled = true);

      /**
       * DebugDrawManager is disabled by default, enable it!
       */
      void SetEnabled(bool enabled);
      bool IsEnabled() const;

      /**
       * Immediately remove all debug geometry
       */
      void Clear();
   
   private:
      bool mEnabled;

      // osg group with stateset depth test enabled
      osg::ref_ptr<osg::Group> mGroupDepthTest;

      // osg group with stateset depth test disabled
      osg::ref_ptr<osg::Group> mGroupNoDepthTest;

      double mCurrentTime;
      dtEntity::EntityManager* mEntityManager;
      MessageFunctor mEnableFunctor;
      MessageFunctor mTickFunctor;
      dtEntity::StringId mLayerName;
   };
}

