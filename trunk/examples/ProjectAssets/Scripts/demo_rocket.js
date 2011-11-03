include_once("Scripts/stdlib.js");

var hudSystem = EntityManager.getEntitySystem("HUD");
var rocketSystem = EntityManager.getEntitySystem("Rocket");

var guiId = 0;
var sphereId = 0;
var sphereTrans = 0;

function round(x) { return Math.floor(x * 1000) / 1000; }
function moveSphere(name, params) {
  var time = params.SimulationTime;
  sphereTrans.Position = [Math.sin(time / 5) * 10, Math.cos(time / 5) * 10 + 20, 2];
  huds.getElementById("posstring").setInnerRML(round(sphereTrans.Position[0])
  + " "
  + round(sphereTrans.Position[1]));
}
////////////////////////////////////////////////////////////////////////////////
function startRocket() {

  var camid = mapSystem.getEntityIdByUniqueId("defaultCam");
  var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -10, 2];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  
  showHelp("Demo for LibRocket");

  // pure data object that will act as prototype for our entity
  var entityProto = {
    Rocket : {
		FullScreen : false,
		ContextName : "DemoRocket",
		Debug : false
    },
	Layer : {
	  Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
	},
	PositionAttitudeTransform : {
	Position : [0, 10, 2],
	Attitude : osg.Quat.makeRotate(-Math.PI / 2, 1,0,0),
	Scale: [0.01,0.01,0.01],
	Children : ["Rocket"]
	}
  };
  
   // create entity and setup components from prototype
  guiId = createEntity(entityProto);
  
  smileydiv = rocketSystem.instanceElement(null, "*", "div");
  smileydiv.style ="position: absolute;width:200px;height:64px;";
  smileydiv.setId("HUD");
  smileydiv.setInnerRML("This is a libRocket overlay!<br /> Position: <span id='posstring' />"
  + "<br /><br /><img src='bar.png' />");
  huds.appendChild(smileydiv);
  
  var sphereProto = {
    
	Layer : {
	  Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
	},
	PositionAttitudeTransform : {
	Position : [10, 0, 0],
	Children : ["StaticMesh"]
	},
	StaticMesh : {
	  Mesh : "StaticMeshes/physics_happy_sphere.ive"
	},
	HUD : {
		Element : "HUD",
		Offset : [0,0,1],
		PixelOffset : [-30,30]  
	}
  };
  
  
  
  EntityManager.registerForMessages("TickMessage", moveSphere);
  
   // create entity and setup components from prototype
  sphereId = createEntity(sphereProto);
  sphereTrans = transformSystem.getComponent(sphereId);
  EntityManager.addToScene(sphereId);
  
  
  
  
  rocketSystem.loadFontFace("LibRocket/Delicious-Roman.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-Italic.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-Bold.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-BoldItalic.otf");
  
  var guicontext = rocketSystem.getContext(guiId);  
  guicontext.loadMouseCursor("LibRocket/cursor.rml");
  var guiwindow = guicontext.loadDocument("LibRocket/demo.rml");
  guiwindow.addEventListener("resize", function(evtname, params, phase, current, target) {
	println("Event: " + evtname + " Phase: " + phase + " NumParams: " + params.Length); 
	for(var k in params) {
	  println("Key: " + k + " Value: " + params[k]);
	}	  
	
	println("Current:" + current);
	println("Target: " + target);
  });
  guiwindow.show();
  
  EntityManager.addToScene(guiId);
  
  var mapsys = EntityManager.getEntitySystem("Map");
  mapsys.loadMap("maps/fpsdemo.dtemap");
  
}

////////////////////////////////////////////////////////////////////////////////
function stopRocket() {
  var mapsys = EntityManager.getEntitySystem("Map");
  mapsys.unloadMap("maps/fpsdemo.dtemap");
  EntityManager.removeFromScene(guiId);
  EntityManager.killEntity(guiId);
  hideHelp();
  EntityManager.unregisterForMessages("TickMessage", moveSphere);
  huds.removeChild(smileydiv);
}

addDemo("Rocket", startRocket, stopRocket);


