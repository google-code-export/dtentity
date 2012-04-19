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

#include <dtEntitySimulation/pathcomponent.h>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osgManipulator/Dragger>
#include <osgUtil/LineSegmentIntersector>
#include <dtEntity/nodemasks.h>

namespace dtEntitySimulation
{

   class VertexIndexHolder : public osg::Referenced
   {
   public:
     VertexIndexHolder(unsigned int i) : mIndex(i) {}
     unsigned int mIndex;
   };


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId PathComponent::TYPE(dtEntity::SID("Path"));

   const dtEntity::StringId PathComponent::VertsId(dtEntity::SID("Verts"));
   const dtEntity::StringId PathComponent::PathVisibleId(dtEntity::SID("PathVisible"));
   const dtEntity::StringId PathComponent::VertsVisibleId(dtEntity::SID("VertsVisible"));

   ////////////////////////////////////////////////////////////////////////////
   PathComponent::PathComponent()
      : BaseClass(new osg::Geode())
   {
      Register(VertsId, &mVerts);
      Register(PathVisibleId, &mPathsVisible);
      Register(VertsVisibleId, &mVertsVisible);

      osg::StateSet* ss = GetNode()->getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   PathComponent::~PathComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PathComponent::Finished()
   {
      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());
      geode->setNodeMask(dtEntity::NodeMasks::VISIBLE);

      geode->removeDrawables(0, geode->getNumDrawables());

      if(GetPathsVisible())
      {

         dtEntity::PropertyArray props = mVerts.Get();

         osg::Geometry* geometry = new osg::Geometry();
         osg::Vec3Array* arr =  new osg::Vec3Array(props.size());

         for(unsigned int i = 0; i != props.size(); ++i)
         {
            (*arr)[i] = props[i]->Vec3Value();
         }

         geometry->setVertexArray(arr);

         osg::Vec4 c[] = { osg::Vec4(1,0,0,1) };
         geometry->setColorArray(new osg::Vec4Array(1, c));
         geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
         geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, props.size()));
         geode->addDrawable(geometry);
      }

      if(GetVertsVisible())
      {
         dtEntity::PropertyArray props = mVerts.Get();

         osg::TessellationHints* hints = new osg::TessellationHints;
         hints->setDetailRatio(0.2f);

         for(unsigned int i = 0; i != props.size(); ++i)
         {
            osg::Vec3 v = props[i]->Vec3Value();

            osg::Sphere* sphere = new osg::Sphere(v, 0.3);

            osg::ShapeDrawable* drawable = new osg::ShapeDrawable(sphere);
            drawable->setUserData(new VertexIndexHolder(i));
            drawable->setColor(osg::Vec4(0,0,1,1));

            drawable->setTessellationHints(hints);

            geode->addDrawable(drawable);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 PathComponent::GetVertex(unsigned int index) const
   {
      dtEntity::PropertyArray props = mVerts.Get();
      if(props.size() <= index)
      {
         LOG_ERROR("Index out of bounds!");
         return osg::Vec3();
      }
      return props[index]->Vec3Value();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PathComponent::SetVertex(unsigned int index, const osg::Vec3& v)
   {
     dtEntity::PropertyArray props = mVerts.Get();
     if(props.size() <= index)
     {
        LOG_ERROR("Index out of bounds!");
        return;
     }
     props[index]->SetVec3(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned int PathComponent::GetNumVertices() const
   {
      dtEntity::PropertyArray props = mVerts.Get();
      return props.size();
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId PathSystem::TYPE(dtEntity::SID("Path"));

   ////////////////////////////////////////////////////////////////////////////
   PathSystem::PathSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
   {
      AddScriptedMethod("pickVertex", dtEntity::ScriptMethodFunctor(this, &PathSystem::ScriptPickVertex));
      AddScriptedMethod("getVertexWorldPosition", dtEntity::ScriptMethodFunctor(this, &PathSystem::ScriptGetVertexWorldPosition));
      AddScriptedMethod("setVertexWorldPosition", dtEntity::ScriptMethodFunctor(this, &PathSystem::ScriptSetVertexWorldPosition));
      AddScriptedMethod("duplicateVertex", dtEntity::ScriptMethodFunctor(this, &PathSystem::ScriptDuplicateVertex));
   }

   ////////////////////////////////////////////////////////////////////////////
   PathSystem::~PathSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* PathSystem::ScriptDuplicateVertex(const dtEntity::PropertyArgs& args)
   {
     if(args.size() != 3)
     {
       LOG_ERROR("Usage: duplicateVertex(entityid, uint index, uint targetIndex");
       return NULL;
     }

     dtEntity::EntityId eid = args[0]->UIntValue();
     unsigned int idx = args[1]->UIntValue();
     unsigned int targetidx = args[2]->UIntValue();

     PathComponent* pathcomp = GetComponent(eid);

     if(pathcomp == NULL)
     {
       return NULL;
     }

     osg::Vec3 vert = pathcomp->GetVertex(idx);

     dtEntity::Property* prop = pathcomp->Get(PathComponent::VertsId);
     dtEntity::ArrayProperty* aprop = static_cast<dtEntity::ArrayProperty*>(prop);
     aprop->Insert(targetidx, new dtEntity::Vec3Property(vert));
     return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* PathSystem::ScriptGetVertexWorldPosition(const dtEntity::PropertyArgs& args)
   {
     if(args.size() != 2)
     {
       LOG_ERROR("Usage: getVertexWorldPosition(entityid, uint index");
       return NULL;
     }

     dtEntity::EntityId eid = args[0]->UIntValue();
     unsigned int idx = args[1]->UIntValue();

     PathComponent* pathcomp = GetComponent(eid);

     if(pathcomp == NULL)
     {
       return NULL;
     }

     osg::Node* node = pathcomp->GetNode();
     osg::NodePath nodePathToRoot;
     osgManipulator::computeNodePathToRoot(*node, nodePathToRoot);
     osg::Matrix ltw = osg::computeLocalToWorld(nodePathToRoot);
     osg::Vec3 vert = pathcomp->GetVertex(idx);
     return new dtEntity::Vec3Property(vert * ltw);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* PathSystem::ScriptSetVertexWorldPosition(const dtEntity::PropertyArgs& args)
   {
     if(args.size() != 3)
     {
       LOG_ERROR("Usage: setVertexWorldPosition(entityid, uint index, vec3");
       return NULL;
     }

     dtEntity::EntityId eid = args[0]->UIntValue();
     unsigned int idx = args[1]->UIntValue();

     PathComponent* pathcomp = GetComponent(eid);

     if(pathcomp == NULL)
     {
       return NULL;
     }

     osg::Node* node = pathcomp->GetNode();
     osg::NodePath nodePathToRoot;
     osgManipulator::computeNodePathToRoot(*node, nodePathToRoot);
     osg::Matrix wtl = osg::computeWorldToLocal(nodePathToRoot);
     osg::Vec3 vert = args[2]->Vec3Value() * wtl;
     pathcomp->SetVertex(idx, vert);
     return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* PathSystem::ScriptPickVertex(const dtEntity::PropertyArgs& args)
   {
     if(args.size() < 3)
     {
       LOG_ERROR("Usage: pickVertex(entityid, vec3 start, vec3 dir)");
       return NULL;
     }

     dtEntity::EntityId eid = args[0]->UIntValue();
     osg::Vec3 start = args[1]->Vec3Value();
     osg::Vec4 dir(args[2]->Vec3Value(), 0);

     PathComponent* pathcomp = GetComponent(eid);

     if(pathcomp == NULL)
     {
       return NULL;
     }

     osg::Node* node = pathcomp->GetNode();
     osg::NodePath nodePathToRoot;
     osgManipulator::computeNodePathToRoot(*node, nodePathToRoot);
     osg::Matrix wtl = osg::computeWorldToLocal(nodePathToRoot);
     start = start * wtl;
     dir = dir * wtl;

     osg::Vec3 dirv(dir[0] * 10000, dir[1] * 10000, dir[2] * 10000);
     osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start, start + dirv);

     osgUtil::IntersectionVisitor iv(lsi.get());

     node->accept(iv);

     if(!lsi->containsIntersections())
     {
        return NULL;
     }
     osgUtil::LineSegmentIntersector::Intersections::iterator i;
     for(i = lsi->getIntersections().begin(); i != lsi->getIntersections().end(); ++i)
     {
        osgUtil::LineSegmentIntersector::Intersection isect = *i;
        VertexIndexHolder* idx = dynamic_cast<VertexIndexHolder*>(isect.drawable->getUserData());
        if(idx != NULL)
        {
          return new dtEntity::UIntProperty(idx->mIndex);
        }
     }

     return NULL;
   }

}
