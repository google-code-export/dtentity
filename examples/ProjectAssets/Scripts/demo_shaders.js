include_once("Scripts/stdlib.js");

////////////////////////////////////////////////////////////////////////////////
function startShaders() {
  var text = "Shader component";
  showHelp(text);
    
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -5, 0];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  var vertshdr = "";
  var fragshdr = "";
  fragshdr += "uniform vec4 MyColor;\n";
  fragshdr += "void main(void) { gl_FragColor = MyColor; }";

  var shaderSys = getEntitySystem("Shader");
  shaderSys.addProgram(vertshdr, fragshdr, "Face", "Leg");

  var entityProto = {
    StaticMesh : {
      CacheHint : "All",
      Mesh: "StaticMeshes/nathan.osg"
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh"],
      Position: [0, 0, 0],
      Scale : [1,1,1]
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    },
    Shader :
    {
    }
  };
    
  entityId = createEntity(entityProto);
  shaderSys.addUniform(entityId, "MyColor", makeVec4(1,0,0,1));

  EntityManager.addToScene(entityId);
    
}

////////////////////////////////////////////////////////////////////////////////
function stopShaders() {

  EntityManager.removeFromScene(entityId);  
  EntityManager.killEntity(entityId);  
  
  hideHelp();  
}

addDemo("Shaders", startShaders, stopShaders);
