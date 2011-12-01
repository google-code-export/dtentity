include_once("Scripts/stdlib.js");

var spiralEntityIds = [];
var spiralTransforms = [];
var NUM_SPIRAL_ACTORS = 100;

function updateSpiral(msgname, params) {       
  var time = params.SimulationTime;
  var radius = 5;
  for(var i = 0; i < NUM_SPIRAL_ACTORS; ++i) {
    var val = i / NUM_SPIRAL_ACTORS;
    entityTransform = spiralTransforms[i];
    var x = Math.sin(val * 10 + time) * radius * val + Math.sin(val * 20 + time * 10);
    var z = Math.cos(val * 10  + time) * radius * val + Math.cos(val * 20 + time * 10);
    entityTransform.Position = [x, 0, z];      
  }
}

////////////////////////////////////////////////////////////////////////////////
function startSpiral() {
  var text = "Demonstrates entity creation and setting properties.";
  showHelp(text);
    
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -10, 0];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  // pure data object that will act as prototype for our entity
  var entityProto = {
    StaticMesh : {
      CacheHint : "All",
      Mesh: "StaticMeshes/physics_crate.ive"          
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh"],
      Position: [0, 0, 0.5],
      Scale : [0.1, 0.1, 0.1]
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    }
  };
  
  for(var i = 0; i < NUM_SPIRAL_ACTORS; ++i) {
     
     // create entity and setup components from prototype
     var entityId = createEntity(entityProto);
     spiralEntityIds.push(entityId);
     EntityManager.addToScene(entityId);
     spiralTransforms[i] = transformSystem.getComponent(entityId);
     
  }
  EntityManager.registerForMessages("TickMessage", updateSpiral);
  
  
}

////////////////////////////////////////////////////////////////////////////////
function stopSpiral() {
  EntityManager.unregisterForMessages("TickMessage", updateSpiral);
  for(k in spiralEntityIds) {
    var eid = spiralEntityIds[k];
    EntityManager.removeFromScene(eid);  
    EntityManager.killEntity(eid);  
  }
  spiralTransforms = [];
  spiralEntityIds = [];  
  hideHelp();
  
}

addDemo("Spiral", startSpiral, stopSpiral);
