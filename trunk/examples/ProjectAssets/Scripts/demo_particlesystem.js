include_once("Scripts/stdlib.js");

////////////////////////////////////////////////////////////////////////////////
function startParticleSystem() {
  var text = "Dynamic particle system properties";
  showHelp(text);
    
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -10, 0];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  // pure data object that will act as prototype for our entity
  var entityProto = {
    Particle : {
    },
    PositionAttitudeTransform : {
      Children : ["Particle"],
      Position: [0, 0, 0.5],
      Scale : [0.1, 0.1, 0.1]
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    }
  };
  
  
  entityId = createEntity(entityProto);
  EntityManager.addToScene(entityId);
    
}

////////////////////////////////////////////////////////////////////////////////
function stopParticleSystem() {

  EntityManager.removeFromScene(entityId);  
  EntityManager.killEntity(entityId);  
  
  hideHelp();  
}

addDemo("ParticleSystem", startParticleSystem, stopParticleSystem);
