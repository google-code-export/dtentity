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
#include <dtEntity/dynamicscomponent.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/lightcomponent.h>
#include <dtEntity/osganimationcomponent.h>
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

   void RegisterStandardFactories(ComponentPluginManager& pluginManager)
   {
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<DynamicsSystem>("Dynamics"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<SkyBoxSystem>("SkyBox"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<ShadowSystem>("Shadow"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<ShaderSystem>("Shader"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<TextLabelSystem>("TextLabel"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<TextureLabelSystem>("TextureLabel"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<OSGAnimationSystem>("OSGAnimation"));
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<LightSystem>("Light"));

#if BUILD_CAL3D
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<AnimationSystem>("Animation"));
#endif

#if BUILD_OPENAL
      pluginManager.AddFactory(new ComponentPluginFactoryImpl<SoundSystem>("Sound"));
#endif

   }
}
