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
#include <dtEntitySimulation/particlecomponent.h>
#include <dtEntitySimulation/pathcomponent.h>
#include <dtEntity/componentplugin.h>
#include <dtEntity/componentpluginmanager.h>

namespace dtEntitySimulation
{


   ////////////////////////////////////////////////////////////////////////////////
   class GroundClampingFactory : public dtEntity::ComponentPluginFactory
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
         return "GroundClamping";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for Ground Clamping";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ManipulatorFactory : public dtEntity::ComponentPluginFactory
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

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for osgManipulator";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class PathFactory : public dtEntity::ComponentPluginFactory
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

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for Path";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ParticleFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
      {
         es = new dtEntitySimulation::ParticleSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Particle";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "System for osgParticle";
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) void dtEntityMessages_dtEntitySimulation(dtEntity::MessageFactory& mf)
{
   dtEntitySimulation::RegisterMessageTypes(mf);
}

REGISTER_DTENTITYPLUGIN(dtEntitySimulation, 4,
   new dtEntitySimulation::GroundClampingFactory(),
   new dtEntitySimulation::ManipulatorFactory(),
   new dtEntitySimulation::ParticleFactory(),
   new dtEntitySimulation::PathFactory()
)

