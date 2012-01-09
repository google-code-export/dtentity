include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");

///////////////////////////// Selection Manager /////////////////////////////////////
var Selection = {
  ids : [],
  isEntitySelected : function (id) {
    return this.ids.lastIndexOf(id) != -1;
  },

  select : function (id) {
    if (id === 0 || this.isEntitySelected(id)) {
      return;
    }
    
    this.ids.push(id);

    if(typeof cameraMotionSystem === 'undefined' || cameraMotionSystem.ShowSelection) {
      layerSystem.addVisibleBoundingBox(id);
    }
    EntityManager.emitMessage("EntitySelectedMessage", {
      AboutEntity: id
    });
    ToolHolder.selectionUpdated();
  },

  deselect : function (id) {

    if (id === 0) return false;
    var i = this.ids.lastIndexOf(id);
    if (i == -1) {
      return false;
    } else {
      this.ids.splice(i, 1);
      layerSystem.removeVisibleBoundingBox(id);
      EntityManager.emitMessage("EntityDeselectedMessage", {
        AboutEntity: id
      });
      ToolHolder.selectionUpdated();
      return true;
    }
  },
  deselectAllExcept : function(id) {
   var todeselect = [];
   for (var k in this.ids) {
     var eid = this.ids[k];
     if(id != eid) {
        todeselect.push(eid);
     }
   }
   for(var k in todeselect) {
      this.deselect(todeselect[k]);
   }
  },
  calcCenter : function (center) {
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    var count = 0;
    for (var k in this.ids) {
      var eid = this.ids[k];

      var transcomp = positionAttitudeTransformSystem.getComponent(eid, true);
      if(transcomp !== null) {
        osg.Vec3.add(center, transcomp.Position, center);
        ++count;
      }
    }
    osg.Vec3.mult(center, 1 / count, center);
  },
  clone : function () {
    var ret = [];
    for (var k in this.ids) {
      var eid = this.ids[k];
      var newid = createEntity(createSpawnerFromEntity(eid));
      EntityManager.addToScene(newid);
      ret.push(newid);
    }
    return ret;
  }
}

function RequestEntitySelect(msgname, params) {

  var id = params.AboutEntity;

  if (!params.UseMultiSelect) {
    Selection.deselectAllExcept(id);
  }

  if (!Selection.isEntitySelected(id)) {
    Selection.select(id);
  }
}

EntityManager.registerForMessages("RequestEntitySelectMessage", RequestEntitySelect);

function RequestEntityDeselect(msgname, params) {

  var id = params.AboutEntity;

  if (Selection.isEntitySelected(id)) {
    Selection.deselect(id);
  }
}

EntityManager.registerForMessages("RequestEntityDeselectMessage", RequestEntityDeselect);

function RequestToggleEntitySelection(msgname, params) {

  var id = params.AboutEntity;

  if (Selection.isEntitySelected(id)) {
    Selection.deselect(id);
  } else {
    Selection.select(id);
  }
}

EntityManager.registerForMessages("RequestToggleEntitySelectionMessage", RequestToggleEntitySelection);


////////////////////////////////// Undo / Redo system /////////////////////////////////

var UndoStack = {
  stack: [],
  currentPos: -1,
  undo: function () {

    if (this.currentPos >= 0) {
      this.stack[this.currentPos].undo();
      this.currentPos--;
    }
  },
  redo: function () {
    if (this.stack.length > this.currentPos + 1) {
      this.currentPos++;
      this.stack[this.currentPos].redo();
    }
  },
  clear: function () {
    this.stack = [];
  },
  pushOperation: function (op) {
    this.stack.splice(this.currentPos + 1, this.stack.length - (this.currentPos + 1));
    this.stack.push(op);
    this.currentPos = this.stack.length - 1;
  }
};


///////////////////////////////////// Copy & Paste system //////////////////////////////////////
var Clipboard = {
  spawnerbuffer : [],
  del: function () {
      var todel = [];

      for (var k in Selection.ids) {
         todel.push(Selection.ids[k]);
      }
      for(var k in todel) {
        var id = todel[k];
        Selection.deselect(id);
        EntityManager.removeFromScene(id);
        EntityManager.killEntity(id);
      }
  },
   cut: function () {
       this.copy();
       this.del();
   },
  copy: function () {

    this.spawnerbuffer = [];

    for (var k in Selection.ids) {
      var eid = Selection.ids[k];
      this.spawnerbuffer.push(createSpawnerFromEntity(eid));

    }
  },
  paste: function () {
    for(var k in this.spawnerbuffer) {
      var id = createEntity(this.spawnerbuffer[k]);
      EntityManager.addToScene(id);
    }
  },
  clear: function () {
    this.spawnerbuffer = [];
  }
};
