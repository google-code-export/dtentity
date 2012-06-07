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
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace osg
{
   class Program;
}

namespace dtEntity
{

   class DT_ENTITY_EXPORT ShaderComponent : public Component
   {

      typedef Component BaseClass;

   public:
      
      static const ComponentType TYPE;
      static const StringId MaterialNamePrefixId;
      static const StringId TopLevelMaterialNameId;

      ShaderComponent();
      virtual ~ShaderComponent();

      void OnAddedToEntity(Entity& entity);

      virtual ComponentType GetType() const { return TYPE; }

      virtual void Finished();

      std::string GetMaterialNamePrefix() const { return mMaterialNamePrefix.Get(); }
      void SetMaterialNamePrefix(const std::string& s) { mMaterialNamePrefix.Set(s); }

      std::string GetTopLevelMaterialName() const { return mTopLevelMaterialName.Get(); }
      void SetTopLevelMaterialName(const std::string& s) { mTopLevelMaterialName.Set(s); }

   private:
      StringProperty mMaterialNamePrefix;
      StringProperty mTopLevelMaterialName;

   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DT_ENTITY_EXPORT ShaderSystem
      : public DefaultEntitySystem<ShaderComponent>
      , public ScriptAccessor
   {
      typedef DefaultEntitySystem<ShaderComponent> BaseClass;

   public:
      static const ComponentType TYPE;

      ShaderSystem(EntityManager& em);
      ~ShaderSystem();

      virtual void OnPropertyChanged(StringId propname, Property &prop);

      void AddProgram(const std::string& name, osg::Program* prg);
      osg::Program* GetProgram(const std::string& name) const;

      virtual bool CreateComponent(EntityId eid, Component*& component);

   private:

      typedef std::map<std::string, osg::ref_ptr<osg::Program> > ProgramMap;
      ProgramMap mPrograms;
      MessageFunctor mChangedMeshFunctor;

      void OnChangedMesh(const Message& msg);
      void ApplyShader(EntityId);
      Property* ScriptAddProgram(const PropertyArgs& args);
      Property* ScriptAddUniform(const PropertyArgs& args);
      Property* ScriptRemoveUniform(const PropertyArgs& args);

   };

}
