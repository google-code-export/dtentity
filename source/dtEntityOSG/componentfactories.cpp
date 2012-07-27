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

#include <dtEntityOSG/componentfactories.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntity/dtentity_config.h>

#include <dtEntityOSG/lightcomponent.h>
#include <dtEntityOSG/pickshapecomponent.h>
#include <dtEntityOSG/shadercomponent.h>
#include <dtEntityOSG/shadowcomponent.h>
#include <dtEntityOSG/skyboxcomponent.h>
#include <dtEntityOSG/staticmeshcomponent.h>
#include <dtEntityOSG/textlabelcomponent.h>
#include <dtEntityOSG/texturelabelcomponent.h>
#include <dtEntityOSG/osganimationcomponent.h>

namespace dtEntityOSG
{
   void RegisterStandardFactories(dtEntity::ComponentPluginManager& pluginManager)
   {
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<LightSystem>("Light"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<OSGAnimationSystem>("OSGAnimation"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<PickShapeSystem>("PickShape"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ShaderSystem>("Shader"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ShadowSystem>("Shadow"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<SkyBoxSystem>("SkyBox"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<StaticMeshSystem>("StaticMesh"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<TextLabelSystem>("TextLabel"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<TextureLabelSystem>("TextureLabel"));

   }
}
