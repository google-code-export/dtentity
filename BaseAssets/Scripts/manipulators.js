include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/selectionmanager.js");

var layerSystem = getEntitySystem("Layer");
var ddm = new DebugDrawManager(EntityManager);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Prototype for tools ////////////////////////////

var camid = EntityManager.getEntitySystem("Map").getEntityIdByUniqueId("defaultCam");
var cameraSystem = EntityManager.getEntitySystem("Camera");
var mainCamera = null;
if(camid !== null) {
  mainCamera = EntityManager.getEntitySystem("Camera").getComponent(camid);
}

// create camera motion system when camera is created
function onCameraAdded(name, params) {

  camid = params.AboutEntity;
  var cam = cameraSystem.getComponent(camid);
  if(cam && cam.IsMainCamera) {
    mainCamera = cam;
  }
}
EntityManager.registerForMessages("CameraAddedMessage", onCameraAdded);


var Tool = {
  activate: function () {},
  deactivate: function () {},
  update: function () {},
  selectionUpdated: function () {},
  placeOnTerrain : function (eid, transcomp) {
    var intersects = Screen.intersect(osg.Vec3.add(transcomp.Position, [0, 0, 5]), osg.Vec3.add(transcomp.Position, [0, 0, -5]));
    for(var k in intersects) {

      if(intersects[k].EntityId != eid) {
        transcomp.Position = intersects[k].Position;
        return;
      }
    }
  },
  handleToolKeys: function () {
    if (Input.getKeyDown("z") && Input.getKey("Control_L")) {
      UndoStack.undo();
    }
    if (Input.getKeyDown("y") && Input.getKey("Control_L")) {
      UndoStack.redo();
    }
    if (Input.getKeyDown("c") && Input.getKey("Control_L")) {
      Clipboard.copy();
    }
    if (Input.getKeyDown("x") && Input.getKey("Control_L")) {
      Clipboard.cut();
    }
    if (Input.getKeyDown("v") && Input.getKey("Control_L")) {
      Clipboard.paste();
    }
  },
   getCameraPosition: function () {
     return mainCamera.Position;
   },
   getCameraDirection: function () {
     return mainCamera.EyeDirection;
   },
   getCameraUp: function () {
     return mainCamera.Up;
   }
}

////////////////////////////// Tool manager ////////////////////////////

var ToolHolder = {

  tools: {},
  activeTool: null,
  addTool: function (tool) {
    this.tools[tool.name] = tool;

    var toolvars = [];
    for(var k in this.tools) {
       var t = this.tools[k];
       toolvars.push({ToolName : t.name, IconPath : t.iconPath, Shortcut: t.shortCut});
    }
    EntityManager.emitMessage("ToolsUpdatedMessage", {"Tools" : toolvars});
  },
  useTool: function (toolname) {
    var nexttool = null;
    if (toolname in this.tools) {
      nexttool = this.tools[toolname];
      if (nexttool === this.activeTool) {
        return;
      }
    } else {
      Log.error("No tool named " + toolname + " registered!");
      return;
    }

    if (this.activeTool !== null) {
      this.activeTool.deactivate();
    }
    this.activeTool = nexttool;

    if (this.activeTool !== null) {
      this.activeTool.activate();
    }
  },
  selectionUpdated: function (id) {
    if (this.activeTool !== null) {
      this.activeTool.selectionUpdated();
    }
  },
  update: function () {
    if (this.activeTool !== null) {
      this.activeTool.update();
    }
  }
};

function updateTools() {
  ToolHolder.update();
}

EntityManager.registerForMessages("EndOfFrameMessage", updateTools, Priority.lowest);

function OnToolActivatedMessage(msgname, params) {
  var toolname = params.ToolName;
  ToolHolder.useTool(toolname);
}
EntityManager.registerForMessages("ToolActivatedMessage", OnToolActivatedMessage);



//////////////////////////////// Select tool /////////////////////////////


function SelectTool() {
  this.name = "Select";
  this.iconPath = ":icons/edit-select.png";
  this.shortCut = "Ctrl+i";

  this.update = function () {
    if (Input.getMouseButtonDown(0)) {
    
      var pickray = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));
      var campos = this.getCameraPosition();

      osg.Vec3.mult(pickray, 100000, pickray);
      var tmp = osg.Vec3.add(campos, pickray);

      var undoOp = {
        oldSelection: [],
        newSelection: [],
        undo: function () {

          // clear selection
          for (var k in Selection.ids) {
            Selection.deselect(Selection.ids[k]);
          }

          for (var k in this.oldSelection) {
            Selection.select(this.oldSelection[k]);
          }
        },
        redo: function () {

          // clear selection
          for (var k in Selection.ids) {
            Selection.deselect(Selection.ids[k]);
          }
          for (var k in this.newSelection) {
            Selection.select(this.newSelection[k]);
          }
        }
      };

      for (var k in Selection.ids) {
        undoOp.oldSelection.push(Selection.ids[k]);
      }

      var pick = Screen.pickEntity(campos, tmp);

      if (pick === null) {
        return;
      }
      println("Picked: " + pick.Id);
      var usemulti = (Input.getKey("Control_L"));
      RequestEntitySelect("", {
        AboutEntity: pick.Id,
        UseMultiSelect: usemulti
      });

      for (var k in Selection.ids) {
        undoOp.newSelection.push(Selection.ids[k]);
      }
      UndoStack.pushOperation(undoOp);

    } else {
      this.handleToolKeys();
    }
  };
}
SelectTool.prototype = Tool;
var selectTool = new SelectTool;

ToolHolder.addTool(selectTool);
ToolHolder.useTool("Select");


//////////////////////////////// Translate tool /////////////////////////////
var tmpvec = [0, 0, 0];

function TranslateTool() {
  this.name = "Translate";
  this.iconPath = ":icons/transform-move.png";
  this.shortCut = "Ctrl+t";

  this.undoOp = null;
  this.doPlaceOnTerrain = false;

  this.activate = function () {

  }

  this.deactivate = function () {
  }

  this.onClick = function () {
  }

  this.onRelease = function () {

  }


  this.onDrag = function () {

  }

  this.update = function () {
  }
};

TranslateTool.prototype = Tool;
var translateTool = new TranslateTool();
ToolHolder.addTool(translateTool);
