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

#include <dtEntity/osganimationcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/nodemaskvisitor.h>
#include <dtEntity/nodemasks.h>
#include <osg/Geode>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/RigTransformHardware>
#include <osgAnimation/BoneMapVisitor>
#include <osgAnimation/MorphGeometry>
#include <osgDB/FileUtils>
#include <sstream>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   struct AnimationManagerFinder : public osg::NodeVisitor
   {
       osg::ref_ptr<osgAnimation::BasicAnimationManager> _am;
       AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
       void apply(osg::Node& node) {
           if (_am.valid())
               return;
           if (node.getUpdateCallback()) {
               osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
               if (b) {
                   _am = new osgAnimation::BasicAnimationManager(*b);
                   node.removeUpdateCallback(_am);
                   return;
               }
           }
           traverse(node);
       }
   };


   osg::ref_ptr<osg::Program> program;

   ////////////////////////////////////////////////////////////////////////////////
   struct MyRigTransformHardware : public osgAnimation::RigTransformHardware
   {

       void operator()(osgAnimation::RigGeometry& geom)
       {
           if (_needInit)
               if (!init(geom))
                   return;
           computeMatrixPaletteUniform(geom.getMatrixFromSkeletonToGeometry(), geom.getInvMatrixFromSkeletonToGeometry());
       }

       bool init(osgAnimation::RigGeometry& geom)
       {
           osg::Vec3Array* pos = dynamic_cast<osg::Vec3Array*>(geom.getVertexArray());
           if (!pos) {
               osg::notify(osg::WARN) << "RigTransformHardware no vertex array in the geometry " << geom.getName() << std::endl;
               return false;
           }

           if (!geom.getSkeleton()) {
               osg::notify(osg::WARN) << "RigTransformHardware no skeleting set in geometry " << geom.getName() << std::endl;
               return false;
           }

           osgAnimation::BoneMapVisitor mapVisitor;
           geom.getSkeleton()->accept(mapVisitor);
           osgAnimation::BoneMap bm = mapVisitor.getBoneMap();

           if (!createPalette(pos->size(),bm, geom.getVertexInfluenceSet().getVertexToBoneList()))
               return false;

           int attribIndex = 11;
           int nbAttribs = getNumVertexAttrib();

           // use a global program for all avatar
           if (!program.valid()) {
               program = new osg::Program;
               program->setName("HardwareSkinning");
               if (!_shader.valid())
               {
                   std::string shaderPath = osgDB::findDataFile("shaders/osganimationskinning.vert");
                   _shader = osg::Shader::readShaderFile(osg::Shader::VERTEX, shaderPath);

               }

               if (!_shader.valid()) {
                   osg::notify(osg::WARN) << "RigTransformHardware can't load VertexShader" << std::endl;
                   return false;
               }

               // replace max matrix by the value from uniform
               {
                   std::string str = _shader->getShaderSource();
                   std::string toreplace = std::string("MAX_MATRIX");
                   std::size_t start = str.find(toreplace);
                   std::stringstream ss;
                   ss << getMatrixPaletteUniform()->getNumElements();
                   str.replace(start, toreplace.size(), ss.str());
                   _shader->setShaderSource(str);
                   osg::notify(osg::INFO) << "Shader " << str << std::endl;
               }

               program->addShader(_shader.get());

               std::string shaderPath2 = osgDB::findDataFile("shaders/osganimationskinning.frag");
               osg::Shader* shader2 = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, shaderPath2);
               if(shader2)
               {
                  program->addShader(shader2);
               }

               for (int i = 0; i < nbAttribs; i++)
               {
                   std::stringstream ss;
                   ss << "boneWeight" << i;
                   program->addBindAttribLocation(ss.str(), attribIndex + i);

                   osg::notify(osg::INFO) << "set vertex attrib " << ss.str() << std::endl;
               }
           }
           for (int i = 0; i < nbAttribs; i++)
           {
               std::stringstream ss;
               ss << "boneWeight" << i;
               geom.setVertexAttribData(attribIndex + i, osg::Geometry::ArrayData(getVertexAttrib(i),osg::Geometry::BIND_PER_VERTEX));
           }

           osg::ref_ptr<osg::StateSet> ss = geom.getOrCreateStateSet();
           ss->addUniform(getMatrixPaletteUniform());
           ss->addUniform(new osg::Uniform("nbBonesPerVertex", getNumBonesPerVertex()));
           ss->setAttributeAndModes(program.get());

          // geom.setStateSet(ss.get());
           _needInit = false;
           return true;
       }

   };

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   struct SetupRigGeometry : public osg::NodeVisitor
   {
      
       SetupRigGeometry()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
       {
       }

       void apply(osg::Geode& geode)
       {
           for (unsigned int i = 0; i < geode.getNumDrawables(); i++)
           {
              apply(*geode.getDrawable(i));
           }
       }

       void apply(osg::Drawable& geom)
       {          
           osgAnimation::RigGeometry* rig = dynamic_cast<osgAnimation::RigGeometry*>(&geom);
           if (rig)
           {
              rig->setRigTransformImplementation(new MyRigTransformHardware);
           }           
       }
   };


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   const StringId OSGAnimationComponent::TYPE(SID("OSGAnimation"));
   const StringId OSGAnimationComponent::EnabledId(SID("Enabled"));
   
   ////////////////////////////////////////////////////////////////////////////
   OSGAnimationComponent::OSGAnimationComponent()
      : mEntity(NULL)
   {
      Register(EnabledId, &mEnabled);
      mEnabled.Set(false);
   }


   ////////////////////////////////////////////////////////////////////////////
   OSGAnimationComponent::~OSGAnimationComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGAnimationComponent::OnAddedToEntity(Entity& entity)
   {
      mEntity = &entity;
      StaticMeshComponent* smc;
      if(entity.GetComponent(smc))
      {
         SetupMesh(smc->GetNode());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGAnimationComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      if(propname == EnabledId)
      {
         SetEnabled(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGAnimationComponent::SetupMesh(osg::Node* node)
   {
      AnimationManagerFinder finder;
      node->accept(finder);
      mAnimationManager = finder._am;
      
      if(mEnabled.Get())
      {
         SetEnabled(true);
      }

      if(mAnimationManager == NULL)
      {
         return;
      }

      
      SetupRigGeometry switcher;
      node->accept(switcher);     
      SetEnabled(true);
   }


   class EmptyCB : public osg::NodeCallback
   {
   public:
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {}
   };

   ////////////////////////////////////////////////////////////////////////////
   void OSGAnimationComponent::SetEnabled(bool v)
   {
      mEnabled.Set(v);

      if(mAnimationManager)
      {
         StaticMeshComponent* smc;
         if(mEntity->GetComponent(smc))
         {
            if(v)
            {
               smc->GetNode()->setUpdateCallback(mAnimationManager);
            }
            else
            {
               smc->GetNode()->setUpdateCallback(new EmptyCB());
            }
         }         
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool OSGAnimationComponent::GetEnabled() const
   {
      return mEnabled.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   OSGAnimationSystem::OSGAnimationSystem(EntityManager& em)
     : DefaultEntitySystem<OSGAnimationComponent>(em)
   {
      mMeshChangedFunctor = MessageFunctor(this, &OSGAnimationSystem::OnMeshChanged);
      em.RegisterForMessages(MeshChangedMessage::TYPE, mMeshChangedFunctor,
                            FilterOptions::PRIORITY_DEFAULT, "OSGAnimationSystem::OnMeshChanged");
      AddScriptedMethod("playAnimation", ScriptMethodFunctor(this, &OSGAnimationSystem::ScriptPlayAnimation));
      AddScriptedMethod("stopAnimation", ScriptMethodFunctor(this, &OSGAnimationSystem::ScriptStopAnimation));
      AddScriptedMethod("getAnimations", ScriptMethodFunctor(this, &OSGAnimationSystem::ScriptGetAnimations));
      AddScriptedMethod("getAnimationLength", ScriptMethodFunctor(this, &OSGAnimationSystem::ScriptGetAnimationLength));
      
   }


   ////////////////////////////////////////////////////////////////////////////
   bool OSGAnimationSystem::GetAnimationList(EntityId id, const osgAnimation::AnimationList*& list, osgAnimation::BasicAnimationManager*& manager)
   {

      OSGAnimationComponent* comp;
      if(!GetEntityManager().GetComponent(id, comp))
      {
         LOG_ERROR("No OSGAnimationComponent found for this entity id");
         return false;
      }
      manager = comp->GetAnimationManager();
      if(manager == NULL)
      {
         return false;
      }

      list = &manager->getAnimationList();
      return !list->empty();
   }

   ////////////////////////////////////////////////////////////////////////////
   void OSGAnimationSystem::OnMeshChanged(const Message& m)
   {
     const MeshChangedMessage& msg = static_cast<const MeshChangedMessage&>(m);
     EntityId id = msg.GetAboutEntityId();
     OSGAnimationComponent* comp = GetComponent(id);
     if(comp)
     {
       StaticMeshComponent* smc;
       if(GetEntityManager().GetComponent(id, smc))
       {
          comp->SetupMesh(smc->GetNode());
       }
     }
   }

   ////////////////////////////////////////////////////////////////////////////
   Property* OSGAnimationSystem::ScriptPlayAnimation(const PropertyArgs& args)
   {
      if(args.size() < 2)
      {
         LOG_ERROR("Usage: playAnimation(entityid, name,[priority = 0, weight = 1])");
         return NULL;
      }
      float priority = (args.size() > 2) ? args[2]->FloatValue() : 0;
      float weight = (args.size() > 3) ? args[3]->FloatValue() : 1;
      EntityId id = args[0]->UIntValue();
      std::string name = args[1]->StringValue();

      osgAnimation::BasicAnimationManager* manager;
      const osgAnimation::AnimationList* list;
      if(!GetAnimationList(id, list, manager))
      {
         return NULL;
      }

      for(osgAnimation::AnimationList::const_iterator i = list->begin(); i != list->end(); ++i)
      {
         if((*i)->getName() == name)
         {
            (manager)->playAnimation(*i, priority, weight);
             return NULL;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   Property* OSGAnimationSystem::ScriptStopAnimation(const PropertyArgs& args)
   {
      if(args.size() < 2)
      {
         LOG_ERROR("Usage: stopAnimation(entityid, name)");
         return NULL;
      }
      EntityId id = args[0]->UIntValue();
      std::string name = args[1]->StringValue();

      osgAnimation::BasicAnimationManager* manager;
      const osgAnimation::AnimationList* list;
      if(!GetAnimationList(id, list, manager))
      {
         return NULL;
      }

      for(osgAnimation::AnimationList::const_iterator i = list->begin(); i != list->end(); ++i)
      {
         if((*i)->getName() == name)
         {
             manager->stopAnimation(*i);
             return NULL;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   Property* OSGAnimationSystem::ScriptGetAnimations(const PropertyArgs& args)
   {
      if(args.size() != 1)
      {
         LOG_ERROR("Usage: getAnimations(entityid)");
         return NULL;
      }

      EntityId id = args[0]->UIntValue();

      osgAnimation::BasicAnimationManager* manager;
      const osgAnimation::AnimationList* list;
      if(!GetAnimationList(id, list, manager))
      {
         return NULL;
      }

      ArrayProperty* arr = new ArrayProperty();

      for(osgAnimation::AnimationList::const_iterator i = list->begin(); i != list->end(); ++i)
      {
         arr->Add(new StringProperty((*i)->getName()));
      }
      return arr;
   }

   ////////////////////////////////////////////////////////////////////////////
   Property* OSGAnimationSystem::ScriptGetAnimationLength(const PropertyArgs& args)
   {
      if(args.size() < 2)
      {
         LOG_ERROR("Usage: getAnimationLength(entityid, name)");
         return NULL;
      }
      EntityId id = args[0]->UIntValue();
      std::string name = args[1]->StringValue();

      osgAnimation::BasicAnimationManager* manager;
      const osgAnimation::AnimationList* list;
      if(!GetAnimationList(id, list, manager))
      {
         return NULL;
      }

      for(osgAnimation::AnimationList::const_iterator i = list->begin(); i != list->end(); ++i)
      {
         if((*i)->getName() == name)
         {
            (*i)->computeDuration();
            return new dtEntity::DoubleProperty((*i)->getDuration());
         }
      }
      return NULL;
   }


}
