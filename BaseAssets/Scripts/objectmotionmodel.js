include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");



////////////////////////////////////////////////////////////////////////////////
function ObjectMotionComponent(eid) {

  this.Enabled = true;
  this.TargetEntityId = 0;

  this.movespeed = 100;
  this.rotatespeed = 0.001;
  this.rotatekeysspeed = 2;

  var rotateOp = [0, 0, 0, 1];
  var toRight = [0,0,0];
  var tempvec = [0,0,0];

  var pivot = [0,0,0];

  var self = this;
  var camera = null;
  var contextId = 0;
  var radius = 1;

  this.finished = function() {
     camera = getEntitySystem("Camera").getComponent(eid);
     if(camera) {
        contextId = camera.ContextId;
     }

     println("TargetId: " + this.TargetEntityId);
     if(this.TargetEntityId !== 0)
     {
        var bounds = getEntitySystem("Layer").getBoundingSphere(this.TargetEntityId);
        osg.Vec3.copy(bounds, pivot);
        radius = bounds[3];
        camera.Position = [bounds[0], bounds[1] - radius * 4, bounds[2]];
        camera.EyeDirection = [0, 1, 0];
        camera.finished();
     }
   }

   this.keyDown = function(key, handled, cid) {

      //println("K: " + key + " cid: " + cid + " this: " + contextId);
      if(!handled && this.Enabled && cid == contextId) {

         switch(key) {
            case "1": self.movespeed = 5; break;
            case "2": self.movespeed = 15; break;
            case "3": self.movespeed = 40; break;
            case "4": self.movespeed = 100; break;
            case "5": self.movespeed = 250; break;
            case "6": self.movespeed = 600; break;
            case "7": self.movespeed = 1500; break;
            case "8": self.movespeed = 4000; break;
            case "9": self.movespeed = 9000; break;
            case "0": self.movespeed = 20000; break;
            case "KP_Add": self.movespeed *= 1.1; break;
            case "KP_Subtract": self.movespeed /= 1.1; break;
         }
      }
   }

   this.mouseButtonDown = function(button, handled, cid) {
      if(!this.Enabled || cid != contextId) return;
      if(button === 0) {
         Screen.lockCursor = true;
         return true;
      }
   }

   this.mouseButtonUp = function(button, handled, cid) {
      if(!this.Enabled || cid != contextId) return;
      if(button === 0) {
         Screen.lockCursor = false;
         return true;
      }
   }

   this.mouseWheel = function(dir, handled, cid) {
      if(!this.Enabled) return;
      if(!handled && camera !== null && cid == contextId) {

         var pos = camera.Position;
         var eyedir = camera.EyeDirection;
         osg.Vec3.mult(eyedir, dir * radius / 4, tempvec);
         osg.Vec3.add(tempvec, pos, pos);
         camera.Position = pos;
         camera.finished();
      }
   }

   this.mouseMove = function(x, y, handled, cid) {

      if(camera === null || !this.Enabled || contextId != cid) return;

      var pos = camera.Position;
      var up = camera.Up;
      var eyedir = camera.EyeDirection;
      var mouseX = Input.getAxis(Axis.MouseDeltaXRaw);
      var mouseY = Input.getAxis(Axis.MouseDeltaYRaw);
      osg.Vec3.cross(eyedir, up, toRight);

      if(Input.getMouseButton(0, contextId)) {


         var pivotToCam = osg.Vec3.sub(pos, pivot);
         osg.Quat.makeRotate(mouseX * -0.001, up[0], up[1], up[2], rotateOp);
         osg.Quat.rotate(rotateOp, pivotToCam, pivotToCam);
         osg.Quat.makeRotate(mouseY * 0.001, toRight[0], toRight[1], toRight[2], rotateOp);
         osg.Quat.rotate(rotateOp, pivotToCam, pivotToCam);
         camera.Position = osg.Vec3.add(pivotToCam, pivot);
         osg.Vec3.mult(pivotToCam, -1, pivotToCam);
         osg.Vec3.normalize(pivotToCam, pivotToCam);
         camera.EyeDirection = pivotToCam;
         camera.finished();
         return true;

      }
   }

   this.update = function() {

      var dt = FRAME_DELTA_TIME;
       if(camera === null || !this.Enabled) {
          return;
       }

      var pos = camera.Position;
      var eyedir = camera.EyeDirection;
      var up = camera.Up;

      osg.Vec3.cross(eyedir, up, toRight);

      var speed = self.movespeed;
      if(Input.getKey("Shift_L", contextId)) {
        speed *= 4;
      }

      var modified = false;
      if(Input.getKey("w", contextId)) {
        osg.Vec3.mult(eyedir, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("s", contextId)) {
        osg.Vec3.mult(eyedir, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("a", contextId)) {
        osg.Vec3.mult(toRight, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("d", contextId)) {
        osg.Vec3.mult(toRight, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("q", contextId)) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("e", contextId)) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }

      if(Input.getKey("Left", contextId)) {
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Right", contextId)) {
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Up", contextId) && eyedir[2] < 0.99) {

        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Down", contextId) && eyedir[2] > -0.99) {
        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }

      if(modified) {
         camera.Position = pos;
         camera.EyeDirection = eyedir;

         camera.finished();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
function ObjectMotionSystem() {

  var self = this;
  // -----------------------------------------
  var components = [];

  this.componentType = "ObjectMotion";

  setInterval(function() {
    for(k in components) {
      components[k].update();
    }
  }, 0);

  // -----------------------------------------
  this.hasComponent = function(eid) {

    return (components[eid]) ? true : false;
  };

  // -----------------------------------------
  this.getComponent = function(eid) {
    return components[eid];
  }

  // ----------------------------
  this.createComponent = function(eid) {
    if(self.hasComponent(eid)) {
      Log.error("ObjectMotion component with id " + eid + " already exists!");
      return self.getComponent(eid);
    }

    var c = new ObjectMotionComponent(eid);
    components[eid] = c;

    Input.addInputCallback(c);
    c.finished();
    return c;
  }

  // -----------------------------------------
  this.deleteComponent = function(eid) {
    if(self.hasComponent(eid)) {
      var comp = components[eid];
      Input.removeInputCallback(comp);
      delete components[eid];
    }
  }

  // -----------------------------------------
  this.getEntitiesInSystem = function() {
    var arr = [];
    for(var key in components) {
       arr.push(parseInt(key));
    }
    return arr;
  }

  // -----------------------------------------
  this.onPropertyChanged = function(propname) {
  }

};

EntityManager.addEntitySystem(new ObjectMotionSystem());
