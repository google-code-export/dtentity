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

#include <dtEntitySimulation/export.h>
#include <dtEntitySimulation/groundclampingcomponent.h>
#include <dtEntitySimulation/messages.h>
#include <dtEntitySimulation/manipulatorcomponent.h>
#include <dtEntitySimulation/pathcomponent.h>
#include <dtEntity/componentplugin.h>

namespace dtEntitySimulation
{


   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_SIMULATION_EXPORT GroundClampingFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         es = new GroundClampingSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Ground Clamping";
      }

      virtual dtEntity::ComponentType GetType() const
      {
         return GroundClampingComponent::TYPE;
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for Ground Clamping";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_SIMULATION_EXPORT ManipulatorFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         es = new ManipulatorSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Manipulator";
      }

      virtual dtEntity::ComponentType GetType() const
      {
         return ManipulatorComponent::TYPE;
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for osgManipulator";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_SIMULATION_EXPORT PathFactory : public dtEntity::ComponentPluginFactory
   {
   public:


      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         es = new PathSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Path";
      }

      virtual dtEntity::ComponentType GetType() const
      {
         return PathComponent::TYPE;
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for PAth";
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
extern "C" DT_ENTITY_SIMULATION_EXPORT void RegisterMessages(dtEntity::MessageFactory& mf)
{
   dtEntitySimulation::RegisterMessageTypes(mf);
}


extern "C" DT_ENTITY_SIMULATION_EXPORT void CreatePluginFactories(std::list<dtEntity::ComponentPluginFactory*>& list)
{
   list.push_back(new dtEntitySimulation::GroundClampingFactory());
   list.push_back(new dtEntitySimulation::ManipulatorFactory());
   list.push_back(new dtEntitySimulation::PathFactory());
}
