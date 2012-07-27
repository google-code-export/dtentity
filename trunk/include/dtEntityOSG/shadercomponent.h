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

#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace osg
{
   class Program;
}

namespace dtEntityOSG
{

   class DTENTITY_OSG_EXPORT ShaderComponent : public dtEntity::Component
   {

      typedef dtEntity::Component BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId MaterialNamePrefixId;
      static const dtEntity::StringId TopLevelMaterialNameId;

      ShaderComponent();
      virtual ~ShaderComponent();

      void OnAddedToEntity(dtEntity::Entity& entity);

      virtual dtEntity::ComponentType GetType() const { return TYPE; }

      std::string GetMaterialNamePrefix() const { return mMaterialNamePrefix.Get(); }
      void SetMaterialNamePrefix(const std::string& s) { mMaterialNamePrefix.Set(s); }

      std::string GetTopLevelMaterialName() const { return mTopLevelMaterialName.Get(); }
      void SetTopLevelMaterialName(const std::string& s) { mTopLevelMaterialName.Set(s); }

   private:
      dtEntity::StringProperty mMaterialNamePrefix;
      dtEntity::StringProperty mTopLevelMaterialName;

   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DTENTITY_OSG_EXPORT ShaderSystem
      : public dtEntity::DefaultEntitySystem<ShaderComponent>
      , public dtEntity::ScriptAccessor
   {
      typedef dtEntity::DefaultEntitySystem<ShaderComponent> BaseClass;

   public:
      static const dtEntity::ComponentType TYPE;

      ShaderSystem(dtEntity::EntityManager& em);
      ~ShaderSystem();

      void AddProgram(const std::string& name, osg::Program* prg);
      osg::Program* GetProgram(const std::string& name) const;

      virtual bool CreateComponent(dtEntity::EntityId eid, dtEntity::Component*& component);

   private:

      typedef std::map<std::string, osg::ref_ptr<osg::Program> > ProgramMap;
      ProgramMap mPrograms;
      dtEntity::MessageFunctor mChangedMeshFunctor;

      void OnChangedMesh(const dtEntity::Message& msg);
      void ApplyShader(dtEntity::EntityId);
      dtEntity::Property* ScriptAddProgram(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptAddUniform(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptRemoveUniform(const dtEntity::PropertyArgs& args);

   };

}
