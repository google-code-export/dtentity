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

#include <dtEntity/componentfactories.h>

#include <dtEntity/dtentity_config.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/componentplugin.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/lightcomponent.h>
#include <dtEntity/osganimationcomponent.h>
#include <dtEntity/pickshapecomponent.h>
#include <dtEntity/shadercomponent.h>
#include <dtEntity/shadowcomponent.h>
#include <dtEntity/skyboxcomponent.h>
#include <dtEntity/textlabelcomponent.h>
#include <dtEntity/texturelabelcomponent.h>


#if BUILD_OPENAL
  #include <dtEntity/soundcomponent.h>
#endif

#if BUILD_CAL3D
  #include <dtEntity/animationcomponent.h>
#endif

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   class OSGAnimationComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new OSGAnimationSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "OSGAnimation";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "OSG Animation System";
      }
   };


   /* This file contains component factories that are registered with the plugin manager.
      They are responsible for constructing and starting the entity systems that
      come with dtEntity.
   */
#if BUILD_CAL3D
   ////////////////////////////////////////////////////////////////////////////////
   class AnimationComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new AnimationSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Animation";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "Cal3D Animation System";
      }
   };
#endif


   ////////////////////////////////////////////////////////////////////////////////
   class SkyBoxComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:
      
      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new SkyBoxSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const 
      {
         return "SkyBox";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const 
      {
         return "Sky box component system";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ShadowComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new ShadowSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Shadow";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "Shadow component system";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ShaderComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new ShaderSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Shader";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "Shader component system";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class LightComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new LightSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "Light";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "Light component system";
      }
   };

#if BUILD_OPENAL
   ////////////////////////////////////////////////////////////////////////////////
   class SoundComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new SoundSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const 
      {
         return "Sound";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const 
      {
         return "Sound component system";
      }
   };
#endif

   ////////////////////////////////////////////////////////////////////////////////
   class PickShapeComponentFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new PickShapeSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "PickShape";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "Pick Shape component system";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class TextLabelFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new TextLabelSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "TextLabel";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "TextLabel component system";
      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class TextureLabelFactory : public dtEntity::ComponentPluginFactory
   {
   public:

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new TextureLabelSystem(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return "TextureLabel";
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return "TextureLabel component system";
      }
   };


	void RegisterStandardFactories(ComponentPluginManager& pluginManager)
   {
      pluginManager.AddFactory(new PickShapeComponentFactory());
      pluginManager.AddFactory(new SkyBoxComponentFactory());
      pluginManager.AddFactory(new ShadowComponentFactory());
      pluginManager.AddFactory(new ShaderComponentFactory());
      pluginManager.AddFactory(new TextLabelFactory());
      pluginManager.AddFactory(new TextureLabelFactory());
      pluginManager.AddFactory(new OSGAnimationComponentFactory());
      pluginManager.AddFactory(new LightComponentFactory());
#if BUILD_CAL3D
       pluginManager.AddFactory(new AnimationComponentFactory());
#endif

#if BUILD_OPENAL
       pluginManager.AddFactory(new SoundComponentFactory());
#endif

   }
}
