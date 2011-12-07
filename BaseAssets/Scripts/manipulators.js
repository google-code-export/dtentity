include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/selectionmanager.js");

var layerSystem = getEntitySystem("Layer");
var manipulatorSystem = getEntitySystem("Manipulator");
var ddm = new DebugDrawManager(EntityManager);
var patSystem = getEntitySystem("PositionAttitudeTransform");
////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Prototype for tools ////////////////////////////

var camid = EntityManager.getEntitySystem("Map").getEntityIdByUniqueId("cam_0");
var cameraSystem = getEntitySystem("Camera");
var mainCamera = null;
if(camid === null) {
   var cameras = cameraSystem.getEntitiesInSystem();
   if(cameras.Length !== 0) {
      camid = cameras.front();
      mainCamera = cameraSystem.getComponent(camid);
   }
} else {
  mainCamera = cameraSystem.getComponent(camid);
}

// create camera motion system when camera is created
function onCameraAdded(name, params) {

  camid = params.AboutEntity;
  var cam = cameraSystem.getComponent(camid);
  if(cam && cam.ContextId === 0) {
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
   if(!handled) {
       if(key == "c" && Input.getKey("Control_L")) {
          Clipboard.copy();
       }
       if(key == "x" && Input.getKey("Control_L")) {
         Clipboard.cut();
       }
       if(key == "v" && Input.getKey("Control_L")) {
         Clipboard.paste();
       }
       if(key == "z" && Input.getKey("Control_L")) {
         UndoStack.undo();
       }
       if(key == "y" && Input.getKey("Control_L")) {
         UndoStack.redo();
       }
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

  var heights = {};
  var clamper = getEntitySystem("GroundClamping");

  this.activate = function () {

      for (var k in Selection.ids) {
         var id = Selection.ids[k];
         if(getEntitySystem("Transform").getComponent(id, true) !== null) {
            var manipulator = manipulatorSystem.createComponent(id);
            manipulator.DraggerType = "TerrainTranslateDragger";
            manipulator.PivotAtBottom = true;
            manipulator.finished();
         }
      }
  }

  this.deactivate = function () {
      for (var k in Selection.ids) {
         manipulatorSystem.deleteComponent(Selection.ids[k]);
      }
  }


  this.mouseButtonDown = function(button, handled) {
      if(button === 0) {
         initialCamPos = this.getCameraPosition();

         var doclone = Input.getKey("Shift_L");
         var clones = [];
         if(doclone) {
           clones = Selection.clone();
         }

         this.undoOp = {
           beforePos : [],
           afterPos : [],
           undo: function () {
             if(doclone) {
                for(var k in clones) {
                  EntityManager.removeFromScene(clones[k]);
                  EntityManager.killEntity(clones[k]);
                }
             }
             for(var k in this.beforePos) {
                var id = this.beforePos[k][0];
                var pos = this.beforePos[k][1];
                patSystem.getComponent(id).Position = pos;
             }
           },
           redo: function () {
            if(doclone) {
              Selection.clone();
            }
            for(var k in this.afterPos) {
               var id = this.afterPos[k][0];
               var pos = this.afterPos[k][1];
               patSystem.getComponent(id).Position = pos;
            }

           }
         };

         heights = [];
         for (var k in Selection.ids) {
            var id = Selection.ids[k];
            var manipulator = manipulatorSystem.getComponent(id);
            if(manipulator === null) continue;
            var center = getEntitySystem("Layer").getBoundingSphere(id);

            var height = center[2];
            if(manipulatorSystem.UseGroundClamping)  {
               var h = clamper.getTerrainHeight(center);
               if(h !== null) height = h;
            }

            heights.push([id, height, manipulator]);

            var patcomp = patSystem.getComponent(id);
            if(patcomp !== null) {
               this.undoOp.beforePos.push([id, patcomp.Position]);
            }
         }


      }
  }

   this.mouseButtonUp = function(button, handled) {
     if(this.undoOp !== null) {
        for (var k in Selection.ids) {
           var id = Selection.ids[k];
           var patcomp = patSystem.getComponent(id);
           if(patcomp !== null) {
              this.undoOp.afterPos.push([id, patcomp.Position]);
           }
           UndoStack.pushOperation(this.undoOp);
        }
     }
   }

  this.update = function () {
    if (Input.getMouseButton(0) && !Input.getMouseButtonDown(0)) {
      var campos = this.getCameraPosition();
      var cammovement = osg.Vec3.sub(campos, initialCamPos);
      cammovement[2] = 0;
      var offs = [0,0,0];

      for(var k in heights) {
         var val = heights[k];
         var id = val[0];
         var lastheight = val[1];
         var manipulator = val[2];
         var center = getEntitySystem("Layer").getBoundingSphere(id);
         var newheight = lastheight;
         if(manipulatorSystem.UseGroundClamping) {
           var h = clamper.getTerrainHeight(center);
            if(h !== null) newheight = h;
         }
         var dist = newheight - lastheight;

         osg.Vec3.copy(cammovement, offs);
         offs[2] += dist;
         manipulator.OffsetFromStart = offs;
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
         if(getEntitySystem("Transform").getComponent(Selection.ids[k], true) !== null) {
            var manip = manipulatorSystem.createComponent(Selection.ids[k]);
            manip.DraggerType = "TrackballDragger";
            manip.finished();
            manipulators.push(manip);
         }
      }
  }

  this.deactivate = function () {
      for (var k in Selection.ids) {
         manipulatorSystem.deleteComponent(Selection.ids[k]);
      }
      manipulators = [];
  }

   this.mouseButtonDown = function(button, handled) {
      var doclone = Input.getKey("Shift_L");
      var clones = [];
      if(doclone) {
        clones = Selection.clone();
      }

       if(button === 0) {
          this.undoOp = {

            beforeRot : [],
            afterRot : [],
            undo: function () {
               if(doclone) {
                  for(var k in clones) {
                    EntityManager.removeFromScene(clones[k]);
                    EntityManager.killEntity(clones[k]);
                  }
               }
              for(var k in this.beforeRot) {
                 var id = this.beforeRot[k][0];
                 var rot = this.beforeRot[k][1];
                 var pos = this.beforeRot[k][2];
                 patSystem.getComponent(id).Attitude = rot;
                 patSystem.getComponent(id).Position = pos;
              }
            },
            redo: function () {
             if(doclone) {
               Selection.clone();
             }
             for(var k in this.afterRot) {
                var id = this.afterRot[k][0];
                var rot = this.afterRot[k][1];
                var pos = this.afterRot[k][2];
                patSystem.getComponent(id).Attitude = rot;
                patSystem.getComponent(id).Position = pos;
             }

            }
          };

          for (var k in Selection.ids) {
             var id = Selection.ids[k];

             var patcomp = patSystem.getComponent(id);
             if(patcomp !== null) {
                this.undoOp.beforeRot.push([id, patcomp.Attitude, patcomp.Position]);
             }
          }

          initialCamPos = this.getCameraPosition();

       }
   }

   this.mouseButtonUp = function(button, handled) {
     if(this.undoOp !== null) {
        for (var k in Selection.ids) {
           var id = Selection.ids[k];
           var patcomp = patSystem.getComponent(id);
           if(patcomp !== null) {
              this.undoOp.afterRot.push([id, patcomp.Attitude, patcomp.Position]);
           }
           UndoStack.pushOperation(this.undoOp);
        }
     }
   }

   this.keyDown = function(key, handled) {
      if(!handled && key == "Space") {

         this.undoOp = {

           beforeRot : [],
           afterRot : [],
           undo: function () {
             for(var k in this.beforeRot) {
                var id = this.beforeRot[k][0];
                var rot = this.beforeRot[k][1];
                var pos = this.beforeRot[k][2];
                patSystem.getComponent(id).Attitude = rot;
                patSystem.getComponent(id).Position = pos;
             }
           },
           redo: function () {
            for(var k in this.afterRot) {
               var id = this.afterRot[k][0];
               var rot = this.afterRot[k][1];
               var pos = this.afterRot[k][2];
               patSystem.getComponent(id).Attitude = rot;
               patSystem.getComponent(id).Position = pos;
            }

           }
         };

         for(var k in Selection.ids) {
            var id = Selection.ids[k];
            var patcomp = getEntitySystem("PositionAttitudeTransform").getComponent(id);
            if(patcomp !== null) {
               this.undoOp.beforeRot.push([id, patcomp.Attitude, patcomp.Position]);
               patcomp.Attitude = [0, 0, 0, 1];
               patcomp.finished();
               this.undoOp.afterRot.push([id, patcomp.Attitude, patcomp.Position]);
            }
         }
         UndoStack.pushOperation(this.undoOp);
         return true;
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
         if(getEntitySystem("Transform").getComponent(Selection.ids[k], true) !== null) {
            var manip = manipulatorSystem.createComponent(Selection.ids[k]);
            manip.DraggerType = "ScaleDragger";
            manip.KeepSizeConstant = false;
            manip.PivotAtBottom = true;
            manip.finished();
            manipulators.push(manip);
         }
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

          var doclone = Input.getKey("Shift_L");if(doclone) {
             for(var k in clones) {
               EntityManager.removeFromScene(clones[k]);
               EntityManager.killEntity(clones[k]);
             }
          }
          var clones = [];
          if(doclone) {
            clones = Selection.clone();
          }

          this.undoOp = {

            beforeScale : [],
            afterScale : [],
            undo: function () {
               if(doclone) {
                  for(var k in clones) {
                    EntityManager.removeFromScene(clones[k]);
                    EntityManager.killEntity(clones[k]);
                  }
               }
              for(var k in this.beforeScale) {
                 var id = this.beforeScale[k][0];
                 var scale = this.beforeScale[k][1];
                 var pos = this.beforeScale[k][2];
                 patSystem.getComponent(id).Scale = scale;
                 patSystem.getComponent(id).Position = pos;
              }
            },
            redo: function () {
             if(doclone) {
               Selection.clone();
             }
             for(var k in this.afterScale) {
                var id = this.afterScale[k][0];
                var scale = this.afterScale[k][1];
                var pos = this.afterScale[k][2];
                patSystem.getComponent(id).Scale = scale;
                patSystem.getComponent(id).Position = pos;
             }

            }
          };

          for (var k in Selection.ids) {
             var id = Selection.ids[k];

             var patcomp = patSystem.getComponent(id);
             if(patcomp !== null) {
                this.undoOp.beforeScale.push([id, patcomp.Scale, patcomp.Position]);
             }
          }
       }
   }

   this.mouseButtonUp = function(button, handled) {
     if(this.undoOp !== null) {
        for (var k in Selection.ids) {
           var id = Selection.ids[k];
           var patcomp = patSystem.getComponent(id);
           if(patcomp !== null) {
              this.undoOp.afterScale.push([id, patcomp.Scale, patcomp.Position]);
           }
           UndoStack.pushOperation(this.undoOp);
        }
     }
   }

   this.keyDown = function(key, handled) {
      if(!handled && key == "Space") {

         this.undoOp = {

           beforeScale : [],
           afterScale : [],
           undo: function () {
             for(var k in this.beforeScale) {
                var id = this.beforeScale[k][0];
                var scale = this.beforeScale[k][1];
                var pos = this.beforeScale[k][2];
                patSystem.getComponent(id).Scale = scale;
                patSystem.getComponent(id).Position = pos;
             }
           },
           redo: function () {
            for(var k in this.afterScale) {
               var id = this.afterScale[k][0];
               var scale = this.afterScale[k][1];
               var pos = this.afterScale[k][2];
               patSystem.getComponent(id).Scale = scale;
               patSystem.getComponent(id).Position = pos;
            }

           }
         };
         for(var k in Selection.ids) {
            var id = Selection.ids[k];
            var patcomp = getEntitySystem("PositionAttitudeTransform").getComponent(id);
            if(patcomp !== null) {
               this.undoOp.beforeScale.push([id, patcomp.Scale, patcomp.Position]);
               patcomp.Scale = [1, 1, 1];
               patcomp.finished();
               this.undoOp.afterScale.push([id, patcomp.Scale, patcomp.Position]);
            }
         }
         return true;
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