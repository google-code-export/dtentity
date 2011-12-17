include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/selectionmanager.js");

var positionAttitudeTransformSystem = getEntitySystem("PositionAttitudeTransform");
var layerSystem = getEntitySystem("Layer");
var ddm = new DebugDrawManager(EntityManager);

function closestPointOnXAxis(starta, dira) {
  var u = osg.Vec3.cross(dira, [1,0,0]);
  osg.Vec3.normalize(u, u);
  var g = osg.Vec3.dot(starta, u);
  var ug = osg.Vec3.mult(u, g);
  var t = (-ug[1] - starta[1]) / dira[1] * dira[0] + ug[0] + starta[0];
  return [t, 0, 0];
}

function closestPointOnYAxis(starta, dira) {
  var u = osg.Vec3.cross(dira, [0,1,0]);
  osg.Vec3.normalize(u, u);
  var g = osg.Vec3.dot(starta, u);
  var ug = osg.Vec3.mult(u, g);
  var t = (-ug[0] - starta[0]) / dira[0] * dira[1] + ug[1] + starta[1];
  return [0, t, 0];
}

function closestPointOnZAxis(starta, dira) {
  var u = osg.Vec3.cross(dira, [0,0,1]);
  osg.Vec3.normalize(u, u);
  var g = osg.Vec3.dot(starta, u);
  var ug = osg.Vec3.mult(u, g);
  var t = (-ug[1] - starta[1]) / dira[1] * dira[2] + ug[2] + starta[2];
  return [0, 0, t];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Prototype for tools ////////////////////////////

var camid = EntityManager.getEntitySystem("Map").getEntityIdByUniqueId("cam_0");
var cameraSystem = EntityManager.getEntitySystem("Camera");
var mainCamera = null;
if(camid !== null) {
  mainCamera = EntityManager.getEntitySystem("Camera").getComponent(camid);
}

// create camera motion system when camera is created
function onCameraAdded(name, params) {

   if(params.ContextId === 0) {
     camid = params.AboutEntity;
     var cam = cameraSystem.getComponent(camid);
     if(cam && cam.IsMainCamera) {
       mainCamera = cam;
     }
   }
}
EntityManager.registerForMessages("CameraAddedMessage", onCameraAdded);


var Tool = {
  activate: function () {},
  deactivate: function () {},
  update: function () {},
  selectionUpdated: function () {},
  disableMotionSystem: function () {
   
    var c = getEntitySystem("Motion").getComponent(camid);
    if(c) {
		c.Enabled = false;
	}
   
  },
  enableMotionSystem: function () {
    var c = getEntitySystem("Motion").getComponent(camid);
    if(c) {
		c.Enabled = true;
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
  },
  scaleToCameraPlane: function (referenceVector, targetVector) { /** scales targetVector to be on the plane defined by the camera normal and the referenceVector. */

    osg.Vec3.normalize(targetVector, targetVector);
    var campos = this.getCameraPosition();
    var pickrayCenter = this.getCameraDirection();

    var camToObj = [0, 0, 0];
    osg.Vec3.sub(referenceVector, campos, camToObj);
    var distend = osg.Vec3.dot(camToObj, pickrayCenter) / osg.Vec3.dot(targetVector, pickrayCenter);
    osg.Vec3.mult(targetVector, distend, targetVector);
  },
  scaleToXPlane: function (targetx, targetVector) { /** scales targetVector to be on X plane. */
    var campos = this.getCameraPosition();
    osg.Vec3.normalize(targetVector, targetVector);
    var dist = (targetx - campos[0]) / targetVector[0];
    targetVector[0] = targetx - campos[0];
    targetVector[1] = dist * targetVector[1];
    targetVector[2] = dist * targetVector[2];
  },
  scaleToYPlane: function (targety, targetVector) { /** scales targetVector to be on Y plane. */
    var campos = this.getCameraPosition();
    osg.Vec3.normalize(targetVector, targetVector);
    var dist = (targety - campos[1]) / targetVector[1];
    targetVector[0] = dist * targetVector[0];
    targetVector[1] = targety - campos[1];
    targetVector[2] = dist * targetVector[2];
  },
  scaleToZPlane: function (targetz, targetVector) { /** scales targetVector to be on Z plane. */
    var campos = this.getCameraPosition();
    osg.Vec3.normalize(targetVector, targetVector);
    var dist = (targetz - campos[2]) / targetVector[2];
    targetVector[0] = dist * targetVector[0];
    targetVector[1] = dist * targetVector[1];
    targetVector[2] = targetz - campos[2];
  },
  projectToXAxis : function (pivot, ray) {
    var campos = this.getCameraPosition();
    var closest = closestPointOnXAxis(campos, ray);
    osg.Vec3.sub(closest, campos, ray);
    return ray;
  },
  projectToYAxis : function (pivot, ray) {
    var campos = this.getCameraPosition();
    var closest = closestPointOnYAxis(osg.Vec3.sub(campos, pivot), ray);
    osg.Vec3.add(closest, pivot, closest);
    osg.Vec3.sub(closest, campos, ray);
    return ray;
  },
  projectToZAxis : function (pivot, ray) {
    var campos = this.getCameraPosition();
    var closest = closestPointOnZAxis(osg.Vec3.sub(campos, pivot), ray);
    osg.Vec3.add(closest, pivot, closest);
    osg.Vec3.sub(closest, campos, ray);
    return ray;
  },
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


//////////////////////////////// Navigate tool /////////////////////////////
Tool.name = "Navigate";
Tool.iconPath = ":icons/draw-spiral.png";
Tool.shortCut = "Ctrl+n";
ToolHolder.addTool(Tool);

//////////////////////////////// Select tool /////////////////////////////


function SelectTool() {
  this.name = "Select";
  this.iconPath = ":icons/edit-select.png";
  this.shortCut = "Ctrl+i";

  this.activate = function () { this.disableMotionSystem(); };
  this.deactivate = function () { this.enableMotionSystem(); };
  
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

      var mouseX = Input.getAxis(Axis.MouseXRaw);
      var mouseY = Input.getAxis(Axis.MouseYRaw);
      var pick = Screen.pickEntity(mouseX, mouseY);

      if (pick === null  || !positionAttitudeTransformSystem.hasComponent(pick.Id, true)) {
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

    } else {
      this.handleToolKeys();
    }
  };
}
SelectTool.prototype = Tool;
var selectTool = new SelectTool;

ToolHolder.addTool(selectTool);
ToolHolder.useTool("Navigate");

//////////////////////////////// Translate tool /////////////////////////////
var tmpvec = [0, 0, 0];

function TranslateTool() {
  this.name = "Translate";
  this.iconPath = ":icons/transform-move.png";
  this.shortCut = "Ctrl+t";

  this.axis = "all";
  this.pivot = [0, 0, 0];
  this.translateStartX = 0;
  this.translateStartY = 0;
  this.undoOp = null;
  this.first = true;
  this.doPlaceOnTerrain = false;
  var lastCamPos = [0,0,0];

  this.activate = function () { this.disableMotionSystem(); };
  this.deactivate = function () { this.enableMotionSystem(); };
  
  this.onClick = function () {

    
    initialCamPos = this.getCameraPosition();
    this.translations = [];
    this.translateStartX = Input.getAxis(Axis.MouseX);
    this.translateStartY = Input.getAxis(Axis.MouseY);

    var doclone = Input.getKey("Shift_L");
    var clones = [];
    if (doclone ) {
      clones = Selection.clone();
    }

    this.undoOp = {
      initialTranslations: [],
      endTranslations: [],
      undo: function () {
        if(doclone) {
          for(var k in clones) {
            EntityManager.removeFromScene(clones[k]);
            EntityManager.killEntity(clones[k]);
          }
        }

        for (var k in this.initialTranslations) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.initialTranslations[k][0], true);
          if (transcomp !== null) {
            var pos = this.initialTranslations[k][1];
            transcomp.Position = pos;
          }
        }

      },
      redo: function () {
        if(doclone) {
          Selection.clone();
        }

        for (var k in this.endTranslations) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.endTranslations[k][0], true);
          if (transcomp !== null) {
            var pos = this.endTranslations[k][1];
            transcomp.Position = pos;
          }
        }
      }
    };

    Selection.calcCenter(this.pivot);

    for (var k in Selection.ids) {
      var eid = Selection.ids[k];

      var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
      if(transcomp !== null) {
        this.translations.push([eid, transcomp, transcomp.Position]);
        this.undoOp.initialTranslations.push([eid, transcomp.Position]);
      }
    }

  };

  this.onRelease = function () {
    if(this.undoOp !== null) {
       for (var k in Selection.ids) {
         var eid = Selection.ids[k];
         var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
         if(transcomp !== null) {
           this.undoOp.endTranslations.push([eid, transcomp.Position]);
         }
       }
       UndoStack.pushOperation(this.undoOp);
    }
  };


  this.onDrag = function () {
    var campos = this.getCameraPosition();
    var cammovement = osg.Vec3.sub(campos, initialCamPos);

    var movement = [0,0,0];

    var pickrayInitial = Screen.getPickRay(this.translateStartX, this.translateStartY);

    var pickrayEnd = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));


    if (this.axis === "x") {

      this.projectToXAxis(this.pivot, pickrayInitial);
      this.projectToXAxis(this.pivot, pickrayEnd);
      osg.Vec3.sub(pickrayEnd, pickrayInitial, movement);

    } else if (this.axis === "y") {

      this.projectToYAxis(this.pivot, pickrayInitial);
      this.projectToYAxis(this.pivot, pickrayEnd);
      osg.Vec3.sub(pickrayEnd, pickrayInitial, movement);

    } else if (this.axis === "z") {

      this.projectToZAxis(this.pivot, pickrayInitial);
      this.projectToZAxis(this.pivot, pickrayEnd);
      osg.Vec3.sub(pickrayEnd, pickrayInitial, movement);

    } else {
      this.scaleToCameraPlane(this.pivot, pickrayInitial);
      this.scaleToCameraPlane(this.pivot, pickrayEnd);
      osg.Vec3.sub(pickrayEnd, pickrayInitial, movement);
    }

    var movementsum = osg.Vec3.add(movement, cammovement);
    if (movementsum[0] !== 0 || movementsum[1] !== 0 || movementsum[2] !== 0) {

      for (var k in this.translations) {
        var eid = this.translations[k][0];
        var transcomp = this.translations[k][1];
        var objPos = this.translations[k][2];
        transcomp.Position = osg.Vec3.add(movementsum, objPos);
        if(this.doPlaceOnTerrain) {
          this.placeOnTerrain(eid, transcomp);
        }
      }
      //Selection.calcCenter(this.pivot);
    }
  }

  this.update = function () {

    if (Selection.ids.length === 0) {
      return;
    }

    if (Input.getKeyDown("x") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "x") ? "all" : "x";
    }
    if (Input.getKeyDown("y") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "y") ? "all" : "y";
    }
    if (Input.getKeyDown("z") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "z") ? "all" : "z";
    }
    if (Input.getMouseButtonDown(0)) {
      this.onClick();
    } else if (Input.getMouseButtonUp(0)) {
      this.onRelease();
    } else if (Input.getMouseButton(0)) {
      this.onDrag();
    }

    this.handleToolKeys();

    if (this.axis === "x") {
      ddm.addLine(osg.Vec3.add(this.pivot, [-1000, 0, 0]), osg.Vec3.add(this.pivot, [1000, 0, 0]), [0, 1, 0, 1], 1, 0);
    }
    if (this.axis === "y") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, -1000, 0]), osg.Vec3.add(this.pivot, [0, 1000, 0]), [0, 1, 0, 1], 1, 0);
    }
    if (this.axis === "z") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, 0, -1000]), osg.Vec3.add(this.pivot, [0, 0, 1000]), [0, 1, 0, 1], 1, 0);
    }
  };
};

TranslateTool.prototype = Tool;
var translateTool = new TranslateTool();
ToolHolder.addTool(translateTool);


//////////////////////////////// Rotate tool /////////////////////////////

function RotateTool() {

  this.name = "Rotate";
  this.iconPath = ":icons/transform-rotate.png";
  this.shortCut = "Ctrl+r";

  this.axis = "all";
  this.rotateStartX = 0;
  this.rotateStartY = 0;
  this.rotations = [];
  this.pivot = [0, 0, 0];
  this.rotquat = [0, 0, 0, 1];
  this.temprotmat = osg.Matrix.makeIdentity();
  this.tempmat = osg.Matrix.makeIdentity();
  this.undoOp = null;

  this.activate = function () { this.disableMotionSystem(); };
  this.deactivate = function () { this.enableMotionSystem(); };
  
  this.onClick = function () {

    var doclone = Input.getKey("Shift_L");
    var clones = [];
    if (doclone ) {
      clones = Selection.clone();
    }

    this.rotateStartX = Input.getAxis(Axis.MouseX);
    this.rotateStartY = Input.getAxis(Axis.MouseY);

    this.undoOp = {
      initialTransforms: [],
      endTransforms: [],
      undo: function () {
        if(doclone) {
          for(var k in clones) {
            EntityManager.removeFromScene(clones[k]);
            EntityManager.killEntity(clones[k]);
          }
        }
        for (var k in this.initialTransforms) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.initialTransforms[k][0], true);
          if (transcomp != null) {
            var pos = this.initialTransforms[k][1];
            var att = this.initialTransforms[k][2];
            transcomp.Position = pos;
            transcomp.Attitude = att;
          }
        }
      },
      redo: function () {
        if(doclone) {
          Selection.clone();
        }

        for (var k in this.endTransforms) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.endTransforms[k][0], true);
          if (transcomp != null) {
            transcomp.Position = this.endTransforms[k][1];
            transcomp.Attitude = this.endTransforms[k][2];
          }
        }
      }
    };

    this.rotations = [];
    Selection.calcCenter(this.pivot);
    for (var k in Selection.ids) {

      var eid = Selection.ids[k];
      var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
      this.rotations.push([transcomp, transcomp.Attitude, transcomp.Position]);

      this.undoOp.initialTransforms.push([eid, transcomp.Position, transcomp.Attitude]);
    }
  };

  this.onRelease = function () {
    Selection.calcCenter(this.pivot);

    if(this.undoOp !== null) {
       for (var k in Selection.ids) {
         var eid = Selection.ids[k];
         var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);

         this.undoOp.endTransforms.push([eid, transcomp.Position, transcomp.Attitude]);
       }
       UndoStack.pushOperation(this.undoOp);
    }
  };

  this.onDrag = function () {
    var campos = this.getCameraPosition();

    var pickrayEnd = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));

    var pickrayInitial = Screen.getPickRay(this.rotateStartX, this.rotateStartY);

    ddm.addLine(this.pivot, osg.Vec3.add(campos, pickrayEnd), [1, 0, 0, 1], 1, 0);

    var end = [0, 0, 0];
    var start = [0, 0, 0];

    if (this.axis === "x") {
      this.scaleToXPlane(this.pivot[0], pickrayEnd);
      this.scaleToXPlane(this.pivot[0], pickrayInitial);
    } else if (this.axis === "y") {
      this.scaleToYPlane(this.pivot[1], pickrayEnd);
      this.scaleToYPlane(this.pivot[1], pickrayInitial);
    } else if (this.axis === "z") {
      this.scaleToZPlane(this.pivot[2], pickrayEnd);
      this.scaleToZPlane(this.pivot[2], pickrayInitial);
    } else {
      this.scaleToCameraPlane(this.pivot, pickrayEnd);
      this.scaleToCameraPlane(this.pivot, pickrayInitial);
    }
    osg.Vec3.sub(osg.Vec3.add(campos, pickrayEnd), this.pivot, end);
    osg.Vec3.sub(osg.Vec3.add(campos, pickrayInitial), this.pivot, start);

    osg.Vec3.normalize(start);
    osg.Vec3.normalize(end);
    osg.Quat.makeRotateFromVecs(start, end, this.rotquat);
    osg.Matrix.makeRotateFromQuat(this.rotquat, this.temprotmat);

    for (var k in this.rotations) {
      var transcomp = this.rotations[k][0];
      var attitude = this.rotations[k][1];
      var position = this.rotations[k][2];

      osg.Vec3.sub(position, this.pivot, tmpvec);
      osg.Matrix.makeIdentity(this.tempmat);
      osg.Matrix.setTrans(this.tempmat, tmpvec[0], tmpvec[1], tmpvec[2]);
      osg.Matrix.setRotateFromQuat(this.tempmat, attitude);

      osg.Matrix.mult(this.tempmat, this.temprotmat, this.tempmat);

      osg.Matrix.getTrans(this.tempmat, tmpvec);
      osg.Vec3.add(tmpvec, this.pivot, tmpvec);
      var newrot = osg.Matrix.getRotate(this.tempmat);
      transcomp.Position = tmpvec;
      transcomp.Attitude = newrot;
    }
  };

  this.update = function () {

    if (Selection.ids.length === 0) {
      return;
    }

    if (Input.getKeyDown("x") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "x") ? "all" : "x";
    }
    if (Input.getKeyDown("y") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "y") ? "all" : "y";
    }
    if (Input.getKeyDown("z") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "z") ? "all" : "z";
    }
    var movementX = Input.getAxis(Axis.MouseDeltaX);
    var movementY = Input.getAxis(Axis.MouseDeltaY);
    if (Input.getMouseButtonDown(0)) {
      this.onClick();
    } else if (Input.getMouseButtonUp(0)) {
      this.onRelease();
    } else if (Input.getMouseButton(0) && (Input.getAxis(Axis.MouseDeltaX) !== 0 || Input.getAxis(Axis.MouseDeltaY) !== 0)) {
      this.onDrag();
    } else {
      this.handleToolKeys();
    }

    if (this.axis === "x") {
      ddm.addLine(osg.Vec3.add(this.pivot, [-1000, 0, 0]), osg.Vec3.add(this.pivot, [1000, 0, 0]), [0, 1, 0, 1], 1, 0);
      ddm.addCircle(this.pivot, [1, 0, 0], 1, [0, 1, 0, 1], 0);
    }
    if (this.axis === "y") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, -1000, 0]), osg.Vec3.add(this.pivot, [0, 1000, 0]), [0, 1, 0, 1], 1, 0);
      ddm.addCircle(this.pivot, [0, 1, 0], 1, [0, 1, 0, 1], 0);
    }
    if (this.axis === "z") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, 0, -1000]), osg.Vec3.add(this.pivot, [0, 0, 1000]), [0, 1, 0, 1], 1, 0);
      ddm.addCircle(this.pivot, [0, 0, 1], 1, [0, 1, 0, 1], 0);
    }
  };
};

RotateTool.prototype = Tool;
var rotateTool = new RotateTool();
ToolHolder.addTool(rotateTool);


//////////////////////////////// Scale tool /////////////////////////////

function ScaleTool() {
  this.name = "Scale";
  this.iconPath = ":icons/transform-scale.png";
  this.shortCut = "Ctrl+s";

  this.axis = "all";
  this.pivot = [0, 0, 0];
  this.translateStartX = 0;
  this.translateStartY = 0;
  this.undoOp = null;

  this.activate = function () { this.disableMotionSystem(); };
  this.deactivate = function () { this.enableMotionSystem(); };
  
  this.onClick = function () {

    if (Input.getKey("Shift_L")) {
      Selection.clone();
    }
    this.translations = [];
    this.translateStartX = Input.getAxis(Axis.MouseX);
    this.translateStartY = Input.getAxis(Axis.MouseY);

    this.undoOp = {
      initialTranslations: [],
      endTranslations: [],
      undo: function () {
        for (var k in this.initialTranslations) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.initialTranslations[k][0], true);
          if (transcomp != null) {
            transcomp.Scale = this.initialTranslations[k][1];;
          }
        }
      },
      redo: function () {
        for (var k in this.endTranslations) {
          var transcomp = positionAttitudeTransformSystem.getComponent(this.endTranslations[k][0], true);
          if (transcomp != null) {
            transcomp.Position = this.endTranslations[k][1];;
          }
        }
      }
    };

    Selection.calcCenter(this.pivot);

    for (var k in Selection.ids) {
      var eid = Selection.ids[k];

      var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
      this.translations.push([transcomp, transcomp.Scale]);
      this.undoOp.initialTranslations.push([eid, transcomp.Scale]);
    }
  };

  this.onRelease = function () {
    if(this.undoOp !== null) {
       for (var k in Selection.ids) {
         var eid = Selection.ids[k];
         var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
         this.undoOp.endTranslations.push([eid, transcomp.Scale]);
       }
       UndoStack.pushOperation(this.undoOp);
     }
  };

  this.onDrag = function () {

    var campos = this.getCameraPosition();

    var pickrayInitial = Screen.getPickRay(this.translateStartX, this.translateStartY);
    var pickrayEnd = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));


    var scalevec = [1, 1, 1];

    if (this.axis === "x") {

      this.projectToXAxis(this.pivot, pickrayInitial);
      this.projectToXAxis(this.pivot, pickrayEnd);

      scalevec[0] =  1 + pickrayEnd[0] - pickrayInitial[0];

    } else if (this.axis === "y") {

      this.projectToYAxis(this.pivot, pickrayInitial);
      this.projectToYAxis(this.pivot, pickrayEnd);

      scalevec[1] =  1 + pickrayEnd[1] - pickrayInitial[1];

    } else if (this.axis === "z") {

      this.projectToZAxis(this.pivot, pickrayInitial);
      this.projectToZAxis(this.pivot, pickrayEnd);

      scalevec[2] =  1 + pickrayEnd[2] - pickrayInitial[2];

    } else {
      this.scaleToCameraPlane(this.pivot, pickrayInitial);
      this.scaleToCameraPlane(this.pivot, pickrayEnd);
      osg.Vec3.add(campos, pickrayInitial, pickrayInitial)
      osg.Vec3.add(campos, pickrayEnd, pickrayEnd)
      ddm.addLine(this.pivot, pickrayEnd, [1, 0, 0, 1], 1, 0);

      var distInitial = osg.Vec3.length(osg.Vec3.sub(pickrayInitial, this.pivot));
      var distNow = osg.Vec3.length(osg.Vec3.sub(pickrayEnd, this.pivot));
      scalevec[0] = scalevec[1] = scalevec[2] = distNow / distInitial;

    }


    for (var k in this.translations) {
      var transcomp = this.translations[k][0];
      var scale = this.translations[k][1];
      transcomp.Scale = [scale[0] * scalevec[0], scale[1] * scalevec[1], scale[2] * scalevec[2]];
    }
  }

  this.update = function () {

    if (Selection.ids.length === 0) {
      return;
    }

    if (Input.getKeyDown("x") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "x") ? "all" : "x";
    }
    if (Input.getKeyDown("y") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "y") ? "all" : "y";
    }
    if (Input.getKeyDown("z") && !Input.getKey("Control_L")) {
      this.axis = (this.axis == "z") ? "all" : "z";
    }

    if (Input.getMouseButtonDown(0)) {
      this.onClick();
    } else if (Input.getMouseButtonUp(0)) {
      this.onRelease();
    } else if (Input.getMouseButton(0) && (Input.getAxis(Axis.MouseDeltaX) !== 0 || Input.getAxis(Axis.MouseDeltaY) !== 0)) {
      this.onDrag();
    }

    this.handleToolKeys();
    if (this.axis === "x") {
      ddm.addLine(osg.Vec3.add(this.pivot, [-1000, 0, 0]), osg.Vec3.add(this.pivot, [1000, 0, 0]), [0, 1, 0, 1], 1, 0);
    }
    if (this.axis === "y") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, -1000, 0]), osg.Vec3.add(this.pivot, [0, 1000, 0]), [0, 1, 0, 1], 1, 0);
    }
    if (this.axis === "z") {
      ddm.addLine(osg.Vec3.add(this.pivot, [0, 0, -1000]), osg.Vec3.add(this.pivot, [0, 0, 1000]), [0, 1, 0, 1], 1, 0);
    }

  };
};

ScaleTool.prototype = Tool;
var scaleTool = new ScaleTool();
ToolHolder.addTool(scaleTool);
