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

#include <dtEntityOSG/shadercomponent.h>

#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityOSG/staticmeshcomponent.h>
#include <dtEntity/systemmessages.h>
#include <assert.h>
#include <osg/Geode>
#include <osg/Material>
#include <osg/NodeVisitor>
#include <osg/Program>

namespace dtEntityOSG
{
  
   class MaterialVisitor : public osg::NodeVisitor
   {
      ShaderSystem* mShaderSystem;
      std::string mPrefix;

   public:

      MaterialVisitor(ShaderSystem* ss, const std::string& prefix)
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
         , mShaderSystem(ss)
         , mPrefix(prefix)
      {}

      void applyShaders(osg::StateSet* ss)
      {
         osg::Material* material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
         if(material)
         {
            std::ostringstream os;
            os << mPrefix;
            os << material->getName();
            osg::Program* prg = mShaderSystem->GetProgram(os.str());
            if(prg)
            {
               ss->setAttribute(prg, osg::StateAttribute::ON);
            }
         }
      }

      virtual void apply(osg::Node& node)
      {
         osg::StateSet* ss = node.getStateSet();
         if(ss)
         {
            applyShaders(ss);
         }
         traverse(node);
      }

      virtual void apply(osg::Geode& node)
      {
         osg::StateSet* ss = node.getStateSet();
         if(ss)
         {
            applyShaders(ss);
         }

         unsigned int num = node.getNumDrawables();
         for(unsigned int i = 0; i < num; ++i)
         {
            osg::StateSet* ss = node.getDrawable(i)->getStateSet();
            if(ss)
            {
               applyShaders(ss);
            }
         }

         traverse(node);
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ShaderComponent::TYPE(dtEntity::SID("Shader"));
   const dtEntity::StringId ShaderComponent::MaterialNamePrefixId(dtEntity::SID("MaterialNamePrefix"));
   const dtEntity::StringId ShaderComponent::TopLevelMaterialNameId(dtEntity::SID("TopLevelMaterialName"));

   ////////////////////////////////////////////////////////////////////////////
   ShaderComponent::ShaderComponent()
   {
      Register(MaterialNamePrefixId, &mMaterialNamePrefix);
      Register(TopLevelMaterialNameId, &mTopLevelMaterialName);

   }
  
   ////////////////////////////////////////////////////////////////////////////
   ShaderComponent::~ShaderComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId ShaderSystem::TYPE(dtEntity::SID("Shader"));

   ////////////////////////////////////////////////////////////////////////////
   ShaderSystem::ShaderSystem(dtEntity::EntityManager& em)
      : DefaultEntitySystem<ShaderComponent>(em)
   {
      AddScriptedMethod("addProgram", dtEntity::ScriptMethodFunctor(this, &ShaderSystem::ScriptAddProgram));
      AddScriptedMethod("addUniform", dtEntity::ScriptMethodFunctor(this, &ShaderSystem::ScriptAddUniform));
      AddScriptedMethod("removeUniform", dtEntity::ScriptMethodFunctor(this, &ShaderSystem::ScriptRemoveUniform));

      mChangedMeshFunctor = dtEntity::MessageFunctor(this, &ShaderSystem::OnChangedMesh);
      em.RegisterForMessages(dtEntity::MeshChangedMessage::TYPE, mChangedMeshFunctor, "ShaderSystem::OnChangedMesh");
   }

   ////////////////////////////////////////////////////////////////////////////
   ShaderSystem::~ShaderSystem()
   {
      GetEntityManager().UnregisterForMessages(dtEntity::MeshChangedMessage::TYPE, mChangedMeshFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ShaderSystem::CreateComponent(dtEntity::EntityId eid, dtEntity::Component*& component)
   {
      bool success = BaseClass::CreateComponent(eid, component);
      if(success)
      {
         ApplyShader(eid);
      }
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderSystem::OnChangedMesh(const dtEntity::Message& m)
   {
      const dtEntity::MeshChangedMessage& msg = static_cast<const dtEntity::MeshChangedMessage&>(m);
      ApplyShader(msg.GetAboutEntityId());
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderSystem::ApplyShader(dtEntity::EntityId eid)
   {
      ShaderComponent* comp = GetComponent(eid);
      if(!comp) return;

      dtEntityOSG::StaticMeshComponent* smc;
      if(GetEntityManager().GetComponent(eid, smc, true))
      {
         if(comp->GetTopLevelMaterialName() != "")
         {
            osg::Program* p = GetProgram(comp->GetTopLevelMaterialName());
            if(p)
            {
               smc->GetNode()->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);
            }
         }
         MaterialVisitor v(this, comp->GetMaterialNamePrefix());
         smc->GetNode()->accept(v);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* ShaderSystem::ScriptAddProgram(const dtEntity::PropertyArgs& args)
   {
      if(args.size() < 3)
      {
         LOG_ERROR("Usage: addProgram(vertex_source, fragment_source, name1, name2, ....)");
         return NULL;
      }
      std::string vsrc = args[0] == NULL ? "" : args[0]->StringValue();
      std::string fsrc =  args[1] == NULL ? "" : args[1]->StringValue();

      osg::Program* prg = new osg::Program();

      if(!vsrc.empty())
      {
        osg::Shader* shader = new osg::Shader(osg::Shader::VERTEX);
        shader->setShaderSource(vsrc);
        prg->addShader(shader);
      }

      if(!fsrc.empty())
      {
        osg::Shader* shader = new osg::Shader(osg::Shader::FRAGMENT);
        shader->setShaderSource(fsrc);
        prg->addShader(shader);
      }

      for(unsigned int i = 2; i < args.size(); ++i)
      {
         std::string name = args[i]->StringValue();

         if(mPrograms.find("name") != mPrograms.end())
         {
            LOG_ERROR("Shader already exists: " << name);
            continue;
         }

         AddProgram(name, prg);
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* ShaderSystem::ScriptAddUniform(const dtEntity::PropertyArgs& args)
   {
       if(args.size() < 3)
      {
         LOG_ERROR("Usage: addUniform(eid, name, value)");
         return NULL;
      }
      dtEntity::EntityId eid = args[0]->UIntValue();
      const char* name = args[1]->StringValue().c_str();
      LayerComponent* lc;
      if(!GetEntityManager().GetComponent(eid, lc) || lc->GetAttachedComponentNode() == NULL)
      {
         return NULL;
      }
      osg::StateSet* ss = lc->GetAttachedComponentNode()->getOrCreateStateSet();

      {
         osg::Uniform* old = ss->getUniform(name);
         if(old)
         {
            ss->removeUniform(old);
         }
      }

      osg::ref_ptr<osg::Uniform> uniform = NULL;
      const dtEntity::Property* prop = args[2];
      switch(prop->GetDataType())
      {
      case dtEntity::DataType::BOOL: uniform = new osg::Uniform(name, prop->BoolValue()); break;
      case dtEntity::DataType::DOUBLE:
      case dtEntity::DataType::FLOAT: uniform = new osg::Uniform(name, prop->FloatValue()); break;
      case dtEntity::DataType::INT: uniform = new osg::Uniform(name, prop->IntValue()); break;
      case dtEntity::DataType::UINT: uniform = new osg::Uniform(name, prop->UIntValue()); break;
      case dtEntity::DataType::VEC2:
      case dtEntity::DataType::VEC2D: uniform = new osg::Uniform(name, prop->Vec2Value()); break;
      case dtEntity::DataType::VEC3:
      case dtEntity::DataType::VEC3D: uniform = new osg::Uniform(name, prop->Vec3Value()); break;
      case dtEntity::DataType::VEC4:
      case dtEntity::DataType::VEC4D: uniform = new osg::Uniform(name, prop->Vec4Value()); break;
      default:
         {
         LOG_ERROR("Unhandled uniform input value: " << prop->GetDataType());
         }
      }

      if(uniform.valid())
      {
         ss->addUniform(uniform);
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* ShaderSystem::ScriptRemoveUniform(const dtEntity::PropertyArgs& args)
   {
      if(args.size() < 2)
      {
         LOG_ERROR("Usage: removeUniform(eid, name)");
         return NULL;
      }

      dtEntity::EntityId eid = args[0]->UIntValue();
      std::string name = args[1]->StringValue();
      dtEntityOSG::StaticMeshComponent* smc;
      if(!GetEntityManager().GetComponent(eid, smc, true))
      {
         return NULL;
      }
      osg::StateSet* ss = smc->GetNode()->getStateSet();

      if(ss)
      {
         osg::Uniform* old = ss->getUniform(name);
         if(old)
         {
            ss->removeUniform(old);
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderSystem::AddProgram(const std::string& name, osg::Program* prg)
   {
      mPrograms[name] = prg;
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Program* ShaderSystem::GetProgram(const std::string& name) const
   {
      ProgramMap::const_iterator i = mPrograms.find(name);
      if(i == mPrograms.end())
      {
         return NULL;
      }
      return i->second;
   }

}
