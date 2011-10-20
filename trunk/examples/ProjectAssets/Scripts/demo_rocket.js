include_once("Scripts/stdlib.js");

var rocketId = 0;

////////////////////////////////////////////////////////////////////////////////
function startRocket() {

  //var df = getDataFilePathList();
  //df.push("D:\\delta3d_optional\\librocket_win32-vc9-source-1.2.1\\libRocket\\Samples\\assets");
  //setDataFilePathList(df);
  
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
	Position : [-10, 100, 10],
	Attitude : osg.Quat.makeRotate(-Math.PI / 2, 1,0,0),
	Scale: [0.1,0.1,0.1],
	Children : ["Rocket"]
	}
  };
  
   // create entity and setup components from prototype
  guiId = createEntity(entityProto);
  
  var rocketSystem = EntityManager.getEntitySystem("Rocket");
  
  
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
}

addDemo("Rocket", startRocket, stopRocket);


