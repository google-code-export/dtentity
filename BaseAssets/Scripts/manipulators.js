include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/selectionmanager.js");

var layerSystem = getEntitySystem("Layer");
var manipulatorSystem = getEntitySystem("Manipulator");
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
  /*handleToolKeys: function () {
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
  },*/
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

  _tools: {},
  _activeTool: null,
  _focused : false,
  addTool: function (tool) {
    this._tools[tool.name] = tool;

    var toolvars = [];
    for(var k in this._tools) {
       var t = this._tools[k];
       toolvars.push({ToolName : t.name, IconPath : t.iconPath, Shortcut: t.shortCut});
    }
    EntityManager.emitMessage("ToolsUpdatedMessage", {"Tools" : toolvars});
  },
  useTool: function (toolname) {
    var nexttool = null;
    if (toolname in this._tools) {
      nexttool = this._tools[toolname];
      if (nexttool === this._activeTool) {
        return;
      }
    } else {
      Log.error("No tool named " + toolname + " registered!");
      return;
    }
    if (this._activeTool !== null) {
      this._activeTool.deactivate();
    }

    this._activeTool = nexttool;
    if(this._focused && this._activeTool !== null) {
       this._activeTool.activate();
    }
  },
  selectionUpdated: function (id) {
    if (this._activeTool !== null) {
      this._activeTool.selectionUpdated();
    }
  },
  update: function () {
    if (this._focused && this._activeTool !== null) {
      this._activeTool.update();
    }
  },
  keyDown : function(key, handled) {
   if (this._activeTool !== null && typeof this._activeTool.keyDown != 'undefined') {
      this._activeTool.keyDown(key, handled);
    }
  },
  keyUp : function(key, handled) {
   if (this._activeTool !== null && typeof this._activeTool.keyUp != 'undefined') {
      this._activeTool.keyUp(key, handled);
    }
  },
  mouseButtonDown : function(button, handled) {
    if (this._activeTool !== null && typeof this._activeTool.mouseButtonDown != 'undefined') {
      this._activeTool.mouseButtonDown(button, handled);
    }
  },
  mouseButtonUp : function(button, handled) {
    if (this._activeTool !== null && typeof this._activeTool.mouseButtonUp != 'undefined') {
      this._activeTool.mouseButtonUp(button, handled);
    }
  },
  mouseWheel : function(dir, handled) {
     if (this._activeTool !== null && typeof this._activeTool.mouseWheel != 'undefined') {
       this._activeTool.mouseWheel(dir, handled);
     }
  },
  mouseMove : function(x, y, handled) {
      if (this._activeTool !== null && typeof this._activeTool.mouseMove != 'undefined') {
        this._activeTool.mouseMove(x, y, handled);
      }
  },
  mouseEnterLeave : function(focused, displaynum, screennum) {
   if(this._focused == focused) {
      return;
   }

   this._focused = focused;

   if(focused) {
      if (this._activeTool !== null) {
        this._activeTool.activate();
      }
   } else {
      if (this._activeTool !== null) {
        this._activeTool.deactivate();
      }
   }
  }

};

setInterval(function() { ToolHolder.update(); }, 0);
Input.addInputCallback(ToolHolder);

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

   this.mouseButtonDown = function(button, handled) {
      if(button === 0 && !handled) {
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
         var usemulti = (Input.getKey("Control_L"));
         RequestEntitySelect("", {
           AboutEntity: pick.Id,
           UseMultiSelect: usemulti
         });

         for (var k in Selection.ids) {
           undoOp.newSelection.push(Selection.ids[k]);
         }
         UndoStack.pushOperation(undoOp);
      }
   }
}
SelectTool.prototype = Tool;
var selectTool = new SelectTool;

ToolHolder.addTool(selectTool);
ToolHolder.useTool("Select");


//////////////////////////////// Translate tool /////////////////////////////

function TranslateTool() {
  this.name = "Translate";
  this.iconPath = ":icons/transform-move.png";
  this.shortCut = "Ctrl+t";

  this.undoOp = null;
  this.doPlaceOnTerrain = false;

  var initialCamPos = [0,0,0];

  var manipulators = [];

  this.activate = function () {

      for (var k in Selection.ids) {
         var manipulator = manipulatorSystem.createComponent(Selection.ids[k]);
         manipulator.DraggerType = "TranslateAxisDragger";
         manipulator.finished();
         manipulators.push(manipulator);
      }
  }

  this.deactivate = function () {
      for (var k in Selection.ids) {
         manipulatorSystem.deleteComponent(Selection.ids[k]);
      }
      manipulators = [];
  }

  function switchToGlobalCoords() {
      for (var k in Selection.ids) {
         manipulatorSystem.getComponent(Selection.ids[k]).UseLocalCoords = false;
      }
  }

  function switchToObjectCoords() {
      for (var k in Selection.ids) {
         manipulatorSystem.getComponent(Selection.ids[k]).UseLocalCoords = true;
      }
  }

  this.mouseButtonDown = function(button, handled) {

      if(button === 0) {
         initialCamPos = this.getCameraPosition();
         var doclone = Input.getKey("Shift_L");
         if(doclone) {
           Selection.clone();
         }
      }
  }

  this.update = function () {
    if (Input.getMouseButton(0) && !Input.getMouseButtonDown(0)) {
      var campos = this.getCameraPosition();
      var cammovement = osg.Vec3.sub(campos, initialCamPos);
      for(var k in manipulators) {
        var manipulator = manipulators[k];
        manipulator.OffsetFromStart = cammovement;
      }
    }
  }
};

TranslateTool.prototype = Tool;
var translateTool = new TranslateTool();
ToolHolder.addTool(translateTool);

//////////////////////////////// Rotate tool /////////////////////////////

function RotateTool() {
   this.name = "Rotate";
   this.iconPath = ":icons/transform-rotate.png";
   this.shortCut = "Ctrl+r";

  this.undoOp = null;
  this.doPlaceOnTerrain = false;
  var initialCamPos = [0,0,0];
  var manipulators = [];

  this.activate = function () {
      for (var k in Selection.ids) {
         var manip = manipulatorSystem.createComponent(Selection.ids[k]);
         manip.DraggerType = "TrackballDragger";
         manip.finished();
         manipulators.push(manip);
      }
  }

  this.deactivate = function () {
      for (var k in Selection.ids) {
         manipulatorSystem.deleteComponent(Selection.ids[k]);
      }
      manipulators = [];
  }

   this.mouseButtonDown = function(button, handled) {
       if(button === 0) {
          initialCamPos = this.getCameraPosition();
          var doclone = Input.getKey("Shift_L");
          if(doclone) {
            Selection.clone();
          }
       }
   }

  this.update = function () {
      if (Input.getMouseButton(0) && !Input.getMouseButtonDown(0)) {
        var campos = this.getCameraPosition();
        var cammovement = osg.Vec3.sub(campos, initialCamPos);
        for(var k in manipulators) {
          var manipulator = manipulators[k];
          manipulator.OffsetFromStart = cammovement;
        }
      }
   }
};

RotateTool.prototype = Tool;
var rotateTool = new RotateTool();
ToolHolder.addTool(rotateTool);

//////////////////////////////// Scale tool /////////////////////////////

function ScaleTool() {
  this.name = "Scale";
  this.iconPath = ":icons/transform-scale.png";
  this.shortCut = "Ctrl+s";
  var initialCamPos = [0,0,0];
  var manipulators = [];

  this.undoOp = null;
  this.doPlaceOnTerrain = false;

  this.activate = function () {
      for (var k in Selection.ids) {
         var manip = manipulatorSystem.createComponent(Selection.ids[k]);
         manip.DraggerType = "TabBoxDragger";
         manip.finished();
         manipulators.push(manip);
      }
  }

  this.deactivate = function () {
      for (var k in Selection.ids) {
         manipulatorSystem.deleteComponent(Selection.ids[k]);
      }
      manipulators = [];
  }

   this.mouseButtonDown = function(button, handled) {
       if(button === 0) {
          initialCamPos = this.getCameraPosition();
          var doclone = Input.getKey("Shift_L");
          if(doclone) {
            Selection.clone();
          }
       }
   }

  this.update = function () {
      if (Input.getMouseButton(0) && !Input.getMouseButtonDown(0)) {
        var campos = this.getCameraPosition();
        var cammovement = osg.Vec3.sub(campos, initialCamPos);
        for(var k in manipulators) {
          var manipulator = manipulators[k];
          manipulator.OffsetFromStart = cammovement;
        }
      }
  }
};

ScaleTool.prototype = Tool;
var scaleTool = new ScaleTool();
ToolHolder.addTool(scaleTool);
