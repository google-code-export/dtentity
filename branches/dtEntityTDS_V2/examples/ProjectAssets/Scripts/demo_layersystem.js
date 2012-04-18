include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");
var layerobj = null;
var tempQuat = [0, 0, 0, 1];

// change attitude values of entity 1 and 2
function updateLayerObj(msgname, params) {       
  var time = params.SimulationTime;  
  osg.Quat.makeRotate(time * 0.5, 0, 0, 1, tempQuat);
  layerEntity1Trans.Attitude = tempQuat;
  
  osg.Quat.makeRotate(time * 2, 1, 0, 0, tempQuat);
  layerEntity2Trans.Attitude = tempQuat;
}

////////////////////////////////////////////////////////////////////////////////
function startLayerSystem() {
  
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -10, 0];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  // The crate. A LayerAttachPoint component is specified and added as
  // a child to the transform component. All entities attached to this attach point
  // will appear in the scene graph as children of entity 1.
  layerEntity1 = createEntity(
  {
    StaticMesh : {
      Mesh: "StaticMeshes/physics_crate.ive"          
    },
    LayerAttachPoint : {
      Name : "LayerEntity1Children"
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh", "LayerAttachPoint"]      
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    }
  });
  layerEntity1Trans = getEntitySystem("PositionAttitudeTransform").getComponent(layerEntity1);
  EntityManager.addToScene(layerEntity1);
  
  // The smiley face. The name value of the layer component is set to the name of
  // the attachment point on entity 1
  layerEntity2 = createEntity(
  {
    StaticMesh : {
      Mesh: "StaticMeshes/physics_happy_sphere.ive"          
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh"],
      Position : [ -1, 0, 1]
    },
    Layer : {
      Layer: "LayerEntity1Children",
      AttachedComponent : "PositionAttitudeTransform"
    }
  });
  layerEntity2Trans = getEntitySystem("PositionAttitudeTransform").getComponent(layerEntity2);
  EntityManager.addToScene(layerEntity2);

  // start updating the rotation values
  EntityManager.registerForMessages("TickMessage", updateLayerObj);
  
  var text = "Demonstrates the way to attach one entity as a child ";
  text +=    "to another entity in the\nscene graph.\n";
  text +=    "The box entity has an attachment point component with a specified name.\n";
  text +=    "The 'Layer' property on the layer component of the sphere is set to that name.";
  showHelp(text);
}

////////////////////////////////////////////////////////////////////////////////
function stopLayerSystem() {
  EntityManager.removeFromScene(layerEntity1);
  EntityManager.killEntity(layerEntity1);
  EntityManager.unregisterForMessages("TickMessage", updateLayerObj);  
  hideHelp();
}

addDemo("LayerSystem", startLayerSystem, stopLayerSystem);
