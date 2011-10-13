/*
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

#include <dtEntity/shadercomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layercomponent.h>
#include <dtCore/shadermanager.h>

namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////////
   const StringId ShaderComponent::TYPE(SID("Shader"));

   const StringId ShaderComponent::ShaderNameId(SID("ShaderName"));
   
   ////////////////////////////////////////////////////////////////////////////
   ShaderComponent::ShaderComponent()
      : mEntity(NULL)
   {
      Register(ShaderNameId, &mShaderName);
   }

   ////////////////////////////////////////////////////////////////////////////
   ShaderComponent::~ShaderComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      mEntity = &entity;
   }   

   ////////////////////////////////////////////////////////////////////////////
   void ShaderComponent::ApplyShader()
   {
      if(mShaderName.Get() == "" || mEntity == NULL)
      {
         return;
      }
      LayerComponent* lc;
      if(mEntity->GetComponent(lc))
      {
         ComponentType ctype = lc->GetAttachedComponent();
         dtEntity::Component* toShade;
         if(mEntity->GetComponent(ctype, toShade))
         {
            NodeComponent* nodeComp = dynamic_cast<NodeComponent*>(toShade);
            if(!nodeComp)
            {
               LOG_ERROR("Cannot apply shader: Target is not a node!");
               return;
            }
            dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

            dtCore::ShaderProgram* sp = shaderManager.FindShaderPrototype(mShaderName.Get());
            if(!sp)
            {
               LOG_ERROR("Shader not found: " + mShaderName.Get());
               return;
            }
            shaderManager.AssignShaderFromPrototype(*sp, *nodeComp->GetNode());
         }
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////
   ShaderSystem::ShaderSystem(EntityManager& em)
      : DefaultEntitySystem<ShaderComponent>(em)
   {     
      mEnterWorldFunctor = MessageFunctor(this, &ShaderSystem::OnEnterWorld);
      em.RegisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor, "ShaderSystem::OnEnterWorld");
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
     GetEntityManager().UnregisterForMessages(EntityAddedToSceneMessage::TYPE, mEnterWorldFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void ShaderSystem::OnEnterWorld(const Message& msg)
   {
      EntityId id = (EntityId) msg.Get(EntityAddedToSceneMessage::AboutEntityId)->UIntValue();
      ShaderComponent* comp;
      if(GetEntityManager().GetComponent(id, comp))
      {
         comp->ApplyShader();
      }
   }
}
