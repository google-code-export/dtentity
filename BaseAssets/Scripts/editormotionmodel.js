include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");



////////////////////////////////////////////////////////////////////////////////
function EditorMotionComponent(eid) {

  this.Projection = "3d";
  this.Enabled = true;
  this.movespeed = 100;
  this.rotatespeed = 0.001;
  this.rotatekeysspeed = 2;

  var rotateOp = [0, 0, 0, 1];
  var toRight = [0,0,0];
  var tempvec = [0,0,0];
  var zoom = 1;
  var pivot = [0,0,0];
  var last3dEyeDirection = [0,1,0];

  var self = this;
  var camera = null;
  var contextId = 0;

  this.finished = function() {
     camera = getEntitySystem("Camera").getComponent(eid);
     contextId = camera.ContextId;
  }

  this.onPropertyChanged = function(propname) {
      if(propname == "Projection") {

         if(this.Projection == "3d") {
            camera.ProjectionMode = "ModePerspective";
            camera.EyeDirection = last3dEyeDirection;
         }
         else
         {
            if(camera.ProjectionMode == "ModePerspective") {
               last3dEyeDirection = camera.EyeDirection;
            }
            camera.ProjectionMode = "ModeOrtho";
            if(this.Projection == "x") {
               camera.EyeDirection = [-1,0,0];
               camera.Up = [0, 0, 1];
            }
            else if(this.Projection == "y") {
               camera.EyeDirection = [0, -1,0];
               camera.Up = [0, 0, 1];
            }
            else if(this.Projection == "z") {
               camera.EyeDirection = [0, 0, -1];
               camera.Up = [0, 1, 0];
            }
         }
         camera.finished();
      }
  }


  this.destruct = function() {
  }

   this.keyDown = function(key, handled, cid) {

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
      if(button === 1) {
         Screen.lockCursor = true;
         return true;
      } else if(button === 2) {
         Screen.lockCursor = true;
         var mouseX = Input.getAxis(Axis.MouseXRaw);
         var mouseY = Input.getAxis(Axis.MouseYRaw);
         var pick = Screen.pickEntity(mouseX, mouseY);

         if (pick === null) {

            pivot = osg.Vec3.add(camera.Position,
                                 osg.Vec3.mult(Screen.getPickRay(Input.getAxis(Axis.MouseX),
                                                                 Input.getAxis(Axis.MouseY)), 100));
         } else {
            pivot = pick.Position;
         }
         return true;
      }
   }

   this.mouseButtonUp = function(button, handled, cid) {
      if(!this.Enabled || cid != contextId) return;
      if(button === 1 || button === 2) {
         Screen.lockCursor = false;
         return true;
      }
   }

   this.mouseWheel = function(dir, handled, cid) {
      if(!this.Enabled) return;
      if(!handled && camera !== null && cid == contexId) {

         if(self.Projection == "3d") {
            var pos = camera.Position;
            var eyedir = camera.EyeDirection;
            osg.Vec3.mult(eyedir, dir * 20, tempvec);
            osg.Vec3.add(tempvec, pos, pos);
            camera.Position = pos;
            camera.finished();
         } else {

            var orthoZoomSpeed = 1.2;
            if(dir < 0) {
               zoom /= orthoZoomSpeed;
            } else {
               zoom *= orthoZoomSpeed;
            }

            camera.OrthoLeft = -zoom * 1000;
            camera.OrthoRight = zoom * 1000;
            camera.OrthoTop = -zoom * 1000;
            camera.OrthoBottom = zoom * 1000;

         }
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

      if(Input.getMouseButton(1)) {

         if(self.Projection == "3d") {

            osg.Quat.makeRotate(-mouseX * self.rotatespeed, up[0], up[1], up[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);
            osg.Quat.makeRotate(mouseY * self.rotatespeed, toRight[0], toRight[1], toRight[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);

            camera.Position = pos;
            camera.EyeDirection = eyedir;
            osg.Vec3.cross(eyedir, up, toRight);
            camera.Up = [0, 0, 1];

            camera.finished();
         } else {

            osg.Vec3.copy(toRight, tempvec);
            osg.Vec3.mult(tempvec, -mouseX * zoom * 4, tempvec);
            osg.Vec3.add(pos, tempvec, pos);

            osg.Vec3.cross(toRight, eyedir, tempvec);
            osg.Vec3.mult(tempvec, mouseY * zoom * 4, tempvec);
            osg.Vec3.add(pos, tempvec, pos);

            camera.Position = pos
            camera.OrthoLeft = -zoom * 1000;
            camera.OrthoRight = zoom * 1000;
            camera.OrthoTop = -zoom * 1000;
            camera.OrthoBottom = zoom * 1000;
            camera.finished();
         }
         return true;

      } else if(Input.getMouseButton(2) && self.Projection == "3d") {


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
      if(Input.getKey("Shift_L")) {
        speed *= 4;
      }

      var modified = false;
      if(Input.getKey("w")) {
        osg.Vec3.mult(eyedir, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("s")) {
        osg.Vec3.mult(eyedir, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("a")) {
        osg.Vec3.mult(toRight, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("d")) {
        osg.Vec3.mult(toRight, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("q")) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("e")) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }

      if(Input.getKey("Left")) {
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Right")) {
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Up") && eyedir[2] < 0.99) {

        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Down") && eyedir[2] > -0.99) {
        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }

      if(modified) {
         camera.Position = pos;
         camera.EyeDirection = eyedir;
         camera.Up = osg.Vec3.cross(toRight, eyedir);
         camera.finished();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
function EditorMotionSystem() {

  var self = this;
  // -----------------------------------------
  var components = [];

  this.componentType = "EditorMotion";

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
      Log.error("EditorMotion component with id " + eid + " already exists!");
      return;
    }

    var c = new EditorMotionComponent(eid);
    components[eid] = c;

    Input.addInputCallback(c);
    c.finished();
    return c;
  }

  // -----------------------------------------
  this.deleteComponent = function(eid) {
    if(self.hasComponent(eid)) {
      var comp = components[eid];
      comp.destruct();
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

EntityManager.addEntitySystem(new EditorMotionSystem());
