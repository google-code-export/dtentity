
include_once("Scripts/tools.js");
include_once("Scripts/motionmodel.js");

////////////////////////////// Variables ////////////////////////////////
var obj1;
var objEntityIds = [];
var toollist = null;
var terrainid = 0;


// for rocket option handling
function changeTool(value) {
   if(value !=="") {
      EntityManager.emitMessage("ToolActivatedMessage", {ToolName : value});
   }
}

////////////////////////////////////////////////////////////////////////////////
function startToolsDemo() {
  
  var camid = mapSystem.getEntityIdByUniqueId("defaultCam");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [0, -10, 2];
  cameraComponent.EyeDirection = osg.Vec3.normalize([0.2, 1, -0.2]);
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 100;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  

  // pure data object that will act as prototype for our entity
  var terrainProto = {
    StaticMesh : {
      Mesh: "StaticMeshes/terrain_simple.ive"
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh"],
      Position: [0, 0, -10]
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    }
  };

  terrainid = createEntity(terrainProto);
  EntityManager.addToScene(terrainid);

  // pure data object that will act as prototype for our entity
  var entityProto = {
    StaticMesh : {
      Mesh: "StaticMeshes/physics_crate.ive"          
    },
    PositionAttitudeTransform : {
      Children : ["StaticMesh"],
      Position: [0, 0, 0.5]
    },
    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    }
  };
  
  for(var i = 0; i < 5; ++i) {
     
     // create entity and setup components from prototype
     var entityId = createEntity(entityProto);
     objEntityIds.push(entityId);
     EntityManager.addToScene(entityId);
     transformSystem.getComponent(entityId).Position = [(i - 2) * 2, 0, 0];
  }
  
  var toolwidgets = [];
  
  if(rocketSystem) {
     var toolbox = window.getElementById("toolbox");
     toolbox.style = "visibility:visible;"
     changeTool("Select");

  } else {
	  var rootWidget = GUI.getWidget("Root");
	  toollist = GUI.createWidget(rootWidget, "WindowsLook/ItemListbox", "toollist");
	  toollist.UnifiedAreaRect = "{{0,20},{0,160},{0,150},{0,300}}";
	  toollist.Visible = true;
	  toollist.onSelectionChanged = function() {

		for(var i = 0; i < toolwidgets.length; ++i) {
		  var item = toolwidgets[i];

		  if(item.Selected == "True") {
			EntityManager.emitMessage("ToolActivatedMessage", {ToolName : item.Text});
		  }
		}
	  }
	  
	  function addTool(name, selected) {
		var w = GUI.createWidget(toollist, "WindowsLook/ListboxItem", name);
		w.Text = name;
		w.Visible = true;
		if(selected) w.Selected = "True";
		toolwidgets.push(w);
	  }
	  
	  addTool("Select", true);
	  addTool("Translate");
	  addTool("Rotate");
	  addTool("Scale");
   }	  
	  

  var text = "Hold down ctrl to select multiple entities.\n";
  text += "x,y and z restrict tools to axes.\n";
  text += "Hold shift while clicking to clone selection";
  showHelp(text);
  
  // draw a grid
  for(i = -10; i <= 10; ++i) {
    ddm.addLine([i, -10, 0], [i, 10, 0], [1,1,1,1], 1, 100000);
    ddm.addLine([-10, i, 0], [10, i, 0], [1,1,1,1], 1, 100000);
  }

  motionComp.Enabled = true;
}

////////////////////////////////////////////////////////////////////////////////
function stopToolsDemo() {
  EntityManager.removeFromScene(terrainid);
  EntityManager.killEntity(terrainid);

  for(k in objEntityIds) {
    var eid = objEntityIds[k];
    EntityManager.removeFromScene(eid);  
    EntityManager.killEntity(eid); 
    if(rocketSystem) {
       var toolbox = window.getElementById("toolbox");
       toolbox.style = "visibility:hidden;"


	} else {
	  GUI.destroyWidget(toollist);
	}

  }
  hideHelp();
  ddm.clear();
  motionComp.Enabled = false;
}

addDemo("Tools", startToolsDemo, stopToolsDemo);
