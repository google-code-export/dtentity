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


#include <UnitTest++.h>
#include <dtEntity/init.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/spawner.h>
#include <dtEntity/entitymanager.h> 
#include <dtEntityOSG/layercomponent.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntityOSG/staticmeshcomponent.h>
#include <osgDB/FileUtils>

using namespace UnitTest;
using namespace dtEntity;


TEST(AttachLayer)
{
   EntityManager em;
   em.AddEntitySystem(*new dtEntityOSG::LayerSystem(em));
   em.AddEntitySystem(*new dtEntityOSG::LayerAttachPointSystem(em));
   MapSystem* mapsys = new MapSystem(em); em.AddEntitySystem(*mapsys);
   em.AddEntitySystem(*new dtEntityOSG::StaticMeshSystem(em));

   dtEntity::StringId testlayer = dtEntity::SID("testlayer");
   
   Entity* mesh;
   em.CreateEntity(mesh);
   
   dtEntityOSG::StaticMeshComponent* smc; mesh->CreateComponent(smc);
   smc->Finished();

   dtEntityOSG::LayerComponent* meshlc; mesh->CreateComponent(meshlc);
   meshlc->SetLayer(testlayer);
   meshlc->SetAttachedComponent(smc->GetType());
   meshlc->Finished();

   mapsys->AddToScene(mesh->GetId());

   Entity* attachpoint;
   em.CreateEntity(attachpoint);
   dtEntityOSG::LayerComponent* attachpointlc; attachpoint->CreateComponent(attachpointlc);
   attachpointlc->SetAttachedComponent(dtEntityOSG::LayerAttachPointComponent::TYPE);
   attachpointlc->Finished();
   dtEntityOSG::LayerAttachPointComponent* lapc; attachpoint->CreateComponent(lapc);   
   lapc->SetName(testlayer);
   lapc->Finished();
   
   mapsys->AddToScene(attachpoint->GetId());
   CHECK(meshlc->GetAttachedComponentNode() == smc->GetNode());
   CHECK(smc->GetNode()->getNumParents() == 1);
   if(smc->GetNode()->getNumParents() == 0)
   {
      CHECK(false && "Static Mesh node has no parents after layer attach");
   }
   else
   {
      CHECK(smc->GetNode()->getParent(0) == lapc->GetNode());
   }
}


/*
<entity>
		<component type="Layer">
			<stringproperty name="AttachedComponent">PositionAttitudeTransform</stringproperty>
			<stringproperty name="Layer">peng</stringproperty>
		</component>
		<component type="Map">
			<stringproperty name="EntityName">two</stringproperty>
			<stringproperty name="MapName">maps/test.dtemap</stringproperty>
			<stringproperty name="UniqueId">2113ab65-726e-499e-b297-55f880b2a90d</stringproperty>
		</component>
		<component type="PositionAttitudeTransform">
			<arrayproperty name="Children">
				<stringproperty name="0">StaticMesh</stringproperty>
			</arrayproperty>
		</component>
		<component type="StaticMesh">
			<stringproperty name="Mesh">StaticMeshes/physics_happy_sphere.ive</stringproperty>
		</component>
	</entity>
	<entity>
		<component type="Layer">
      <stringproperty name="Layer">default</stringproperty>
			<stringproperty name="AttachedComponent">PositionAttitudeTransform</stringproperty>
		</component>
		<component type="LayerAttachPoint">
			<stringproperty name="Name">peng</stringproperty>
		</component>
		<component type="Map">
			<stringproperty name="EntityName">one</stringproperty>
			<stringproperty name="MapName">maps/test.dtemap</stringproperty>
			<stringproperty name="UniqueId">2f54050b-b05f-4291-9e50-1fdc65465e05</stringproperty>
		</component>
		<component type="PositionAttitudeTransform">
			<arrayproperty name="Children">
				<stringproperty name="0">LayerAttachPoint</stringproperty>
			</arrayproperty>
		</component>
	</entity>*/
