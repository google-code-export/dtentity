include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");


////////////////////////////////////////////////////////////////////////////////
function EditorMotionComponent(eid) {

  var camera = null;

  createInt32Prop(this, "contextId", 0);
  createUint32Prop(this, "movespeed", 100);
  createNumberProp(this, "rotatespeed", 0.001);
  createNumberProp(this, "rotatekeysspeed", 2);
  createBoolProp(this, "Enabled", true);

  createVec2Prop(this, "test2", [1,2]);
  createVec3Prop(this, "test3", [1,2,3]);
  createVec4Prop(this, "test4", [1,2,3,4]);
  createQuatProp(this, "testq", [1,2,3,4]);

  createStringProp(this, "Projection", "3d",
         function() { return this.value; },
         function(val) {
           this.value = val;

           if(camera === null) return;

           if(val === "3d") {
             if(camera.ProjectionMode !== "ModePerspective") {
                camera.ProjectionMode = "ModePerspective";
                camera.EyeDirection = last3dEyeDirection;
                camera.Up = [0,0,1];
             }
           }
           else
           {
              if(camera.ProjectionMode === "ModePerspective") {
                 last3dEyeDirection = camera.EyeDirection;
              }
              camera.ProjectionMode = "ModeOrtho";
              if(val === "x") {
                 camera.EyeDirection = [-1,0,0];
                 camera.Up = [0, 0, 1];
              }
              else if(val === "y") {
                 camera.EyeDirection = [0, -1,0];
                 camera.Up = [0, 0, 1];
              }
              else if(val === "z") {
                 camera.EyeDirection = [0, 0, -1];
                 camera.Up = [0, 1, 0];
              }

              camera.OrthoLeft = -zoom;
              camera.OrthoRight = zoom;
              camera.OrthoTop = zoom;
              camera.OrthoBottom = -zoom;
           }
           camera.finished();
        }
   );

  var rotateOp = [0, 0, 0, 1];
  var toRight = [0,0,0];
  var tempvec = [0,0,0];
  var zoom = 100;
  
  var pivot = [0,0,0];

  var last3dEyeDirection = [0,1,0];

  var self = this;


  this.finished = function() {
     camera = getEntitySystem("Camera").getComponent(eid);
     if(camera) {
       this.contextId = camera.ContextId;
       this.Projection = "3d";
     }
   }




  this.destruct = function() {}

   this.keyDown = function(key, handled, cid) {

       if(!handled && this.Enabled && cid === this.contextId) {

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
      if(handled) return;
      if(!this.Enabled || cid !== this.contextId) return;
      if(button === 1) {
         Screen.lockCursor = true;
         return true;
      } else if(button === 2) {
         Screen.lockCursor = true;
         var mouseX = Input.getAxis(Axis.MouseXRaw);
         var mouseY = Input.getAxis(Axis.MouseYRaw);
         var pickray = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));
                                                                 
         var isects = Screen.intersect(camera.Position, osg.Vec3.add(camera.Position, osg.Vec3.mult(pickray, 10000)));
         if (isects.length === 0) {
            pivot = osg.Vec3.add(camera.Position, osg.Vec3.mult(pickray, 100));
                                                               
         } else {
           pivot = isects[0].Position;
         }

         return true;
      }
   }

   this.mouseButtonUp = function(button, handled, cid) {
      if(!this.Enabled || cid !== this.contextId) return;
      if(button === 1 || button === 2) {
         Screen.lockCursor = false;
         return true;
      }
   }

   this.mouseWheel = function(dir, handled, cid) {
      if(!this.Enabled) return;
      if(!handled && camera !== null && cid === this.contextId) {

        if(self.Projection === "3d") {
            var pos = camera.Position;
            var eyedir = camera.EyeDirection;
            osg.Vec3.mult(eyedir, dir * 20, tempvec);
            osg.Vec3.add(tempvec, pos, pos);
            camera.Position = pos;
            camera.finished();
         } else {

            var orthoZoomSpeed = 1.2;
            if(dir > 0) {
               zoom /= orthoZoomSpeed;
            } else {
               zoom *= orthoZoomSpeed;
            }

            camera.OrthoLeft = -zoom;
            camera.OrthoRight = zoom;
            camera.OrthoTop = zoom;
            camera.OrthoBottom = -zoom;

         }
      }
   }

   this.mouseMove = function(x, y, handled, cid) {
      if(handled) return;

      if(camera === null || !this.Enabled || this.contextId !== cid) return;

      var pos = camera.Position;
      var up = camera.Up;
      var eyedir = camera.EyeDirection;
      var mouseX = Input.getAxis(Axis.MouseDeltaXRaw);
      var mouseY = Input.getAxis(Axis.MouseDeltaYRaw);
      osg.Vec3.cross(eyedir, up, toRight);

      if(Input.getMouseButton(1, this.contextId)) {

        if(self.Projection === "3d") {

            osg.Quat.makeRotate(-mouseX * self.rotatespeed, up[0], up[1], up[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);
            osg.Quat.makeRotate(mouseY * self.rotatespeed, toRight[0], toRight[1], toRight[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);

            camera.Position = pos;
            camera.EyeDirection = eyedir;
            osg.Vec3.cross(eyedir, up, toRight);
            camera.Up = [0, 0, 1];
            camera.finished();

         }
         return true;

      } else if(Input.getMouseButton(2, this.contextId)) {

        if(self.Projection === "3d") {

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

         } else {

            osg.Vec3.copy(toRight, tempvec);
            osg.Vec3.mult(tempvec, -mouseX * zoom * 0.004, tempvec);
            osg.Vec3.add(pos, tempvec, pos);

            osg.Vec3.cross(toRight, eyedir, tempvec);
            osg.Vec3.mult(tempvec, -mouseY * zoom * 0.004, tempvec);
            osg.Vec3.add(pos, tempvec, pos);

            camera.Position = pos
            camera.OrthoLeft = -zoom;
            camera.OrthoRight = zoom;
            camera.OrthoTop = zoom;
            camera.OrthoBottom = -zoom;
            camera.finished();
         }
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
      if(Input.getKey("Shift_L", this.contextId)) {
        speed *= 4;
      }

      var modified = false;
      if(Input.getKey("w", this.contextId)) {
        osg.Vec3.mult(eyedir, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("s", this.contextId)) {
        osg.Vec3.mult(eyedir, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("a", this.contextId)) {
        osg.Vec3.mult(toRight, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("d", this.contextId)) {
        osg.Vec3.mult(toRight, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("q", this.contextId)) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * -speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }
      if(Input.getKey("e", this.contextId)) {
        osg.Vec3.cross(toRight, eyedir, tempvec);
        osg.Vec3.mult(tempvec, dt * speed, tempvec);
        osg.Vec3.add(tempvec, pos, pos);
        modified = true;
      }

      if(Input.getKey("Left", this.contextId)) {
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Right", this.contextId)) {
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Up", this.contextId) && eyedir[2] < 0.99) {

        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(-dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Down", this.contextId) && eyedir[2] > -0.99) {
        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(dt * self.rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }

      if(modified) {
         camera.Position = pos;
         camera.EyeDirection = eyedir;
         camera.Up = [0,0,1];
         camera.finished();
      }
   }

  this.jumpToEntity = function(entityId, distance, keepCameraDirection) {

    var transcomp = getEntitySystem("PositionAttitudeTransform").getComponent(entityId);
    if(camera === null || transcomp === null) {
      return;
    }
    var targetpos = transcomp.Position;

    if(keepCameraDirection) {
      var eyedir = camera.EyeDirection;
      osg.Vec3.normalize(eyedir, eyedir);
      osg.Vec3.mult(eyedir, distance, eyedir);

      camera.Position = osg.Vec3.sub(targetpos, eyedir);
      camera.finished();
    } else {
      var eyedir = osg.Vec3.sub(targetpos, camera.Position);
      osg.Vec3.normalize(eyedir, eyedir);
      camera.EyeDirection = eyedir;
      camera.Position = osg.Vec3.add(targetpos, osg.Vec3.mult(eyedir, -distance));
      camera.finished();
    }
  }

  this.jumpToPosition = function(position, lookat, up) {

    if(lookat[0] === position[0] && lookat[1] === position[1] && lookat[2] === position[2]) {
      var diff = osg.Vec3.sub(position, camera.Position);
      if(diff[0] !== 0 || diff[1] !== 0 || diff[2] !== 0) {
        camera.EyeDirection = osg.Vec3.normalize(diff);
      }
    } else {
      camera.EyeDirection = osg.Vec3.normalize(osg.Vec3.sub(lookat, position));
    }

    camera.Position = position;
    if(camera.EyeDirection[2] === 1 && up[2] === 1) {
      up = [0,1,0];
    }

    camera.Up = up;
    camera.finished();

  }
}



var editorMotionSystem = new JSEntitySystem(EditorMotionComponent, "EditorMotion");

// -----------------------------------------
editorMotionSystem.getComponentByContextId = function(contextid)  {
  var component = null;
  for(k in editorMotionSystem.components) {
    if(editorMotionSystem.components[k].contextId === contextid) {
      return editorMotionSystem.components[k];
    }
  }
  return null;
}

// don't store motion component to map, should be created by script at load time
editorMotionSystem.storeComponentToMap = function() { return false; }
editorMotionSystem.storePropertiesToScene = function() { return false; }

setInterval(function() {
  for(k in editorMotionSystem.components) {
    editorMotionSystem.components[k].update();
  }
}, 0);

// -----------------------------------------
function doJumpToEntity(name, params) {
  var context = params.ContextId;
  var component = editorMotionSystem.getComponentByContextId(context);
  if(component !== null) {
    component.jumpToEntity(params.AboutEntity, params.Distance, params.KeepCameraDirection);
  }
}
EntityManager.registerForMessages("MoveCameraToEntityMessage", doJumpToEntity);

// -----------------------------------------
function doJumpToPosition(name, params) {
  var context = params.ContextId;
  var component = editorMotionSystem.getComponentByContextId(context);
  if(component !== null) {
    component.jumpToPosition(params.Position, params.LookAt, params.Up);
  }
}
EntityManager.registerForMessages("MoveCameraToPositionMessage", doJumpToPosition);

editorMotionSystem.created = function(id, component) {
      println("Adding input callback");
  Input.addInputCallback(component);
}

editorMotionSystem.destroyed = function(id, component) {
  Input.removeInputCallback(component);
}
EntityManager.addEntitySystem(editorMotionSystem);
