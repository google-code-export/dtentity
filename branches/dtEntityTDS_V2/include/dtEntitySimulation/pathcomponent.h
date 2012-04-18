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

#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>
#include <dtEntity/nodecomponent.h>
#include <dtEntity/scriptaccessor.h>

namespace dtEntitySimulation
{

   class PathComponent : public dtEntity::NodeComponent
   {
      typedef dtEntity::NodeComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId VertsId;
      static const dtEntity::StringId PathVisibleId;
      static const dtEntity::StringId VertsVisibleId;

      PathComponent();
      virtual ~PathComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }

      virtual void Finished();

      void SetPathsVisible(bool v) { mPathsVisible.Set(v); }
      bool GetPathsVisible() const { return mPathsVisible.Get(); }

      void SetVertsVisible(bool v) { mVertsVisible.Set(v); }
      bool GetVertsVisible() const { return mVertsVisible.Get(); }

      osg::Vec3 GetVertex(unsigned int index) const;
      void SetVertex(unsigned int index, const osg::Vec3& v);
      unsigned int GetNumVertices() const;

   private:

      dtEntity::ArrayProperty mVerts;
      dtEntity::BoolProperty mPathsVisible;
      dtEntity::BoolProperty mVertsVisible;
   };


   ////////////////////////////////////////////////////////////////////////////////

   class PathSystem
      : public dtEntity::DefaultEntitySystem<PathComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<PathComponent> BaseClass;

   public:

      static const dtEntity::ComponentType TYPE;

      PathSystem(dtEntity::EntityManager& em);
      ~PathSystem();

      dtEntity::Property* ScriptPickVertex(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptGetVertexWorldPosition(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetVertexWorldPosition(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptDuplicateVertex(const dtEntity::PropertyArgs& args);

   private:

   };
}
