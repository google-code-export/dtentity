include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");

function EditorMotionModel(camera) {

   var active = false;
   var movespeed = 100;
   var rotatespeed = 0.001;
   var rotatekeysspeed = 2;
   var up = [0, 0, 1];
   var rotateOp = [0, 0, 0, 1];
   var toRight = [0,0,0];
   var tempvec = [0,0,0];

   this.keyDown = function(key, handled) {
      if(!handled) {

         switch(key) {
            case "1": movespeed = 5; break;
            case "2": movespeed = 15;
            case "3": movespeed = 40; break;
            case "4": movespeed = 100; break;
            case "5": movespeed = 250; break;
            case "6": movespeed = 600; break;
            case "7": movespeed = 1500; break;
            case "8": movespeed = 4000; break;
            case "9": movespeed = 9000; break;
            case "0": movespeed = 20000; break;
            case "KP_Add": movespeed *= 1.1; break;
            case "KP_Subtract": movespeed /= 1.1; break;
         }
      }
   }

   this.mouseButtonDown = function(button, handled) {
      if(!active && !handled) {
         active = true;
         Screen.lockCursor = true;
      } else if(button === 1) {
         active = false;
         Screen.lockCursor = false;
      }
   }

   this.mouseWheel = function(dir, handled) {
      if(!handled) {
         var pos = camera.Position;
         var eyedir = camera.EyeDirection;
         osg.Vec3.mult(eyedir, dir * movespeed, tempvec);
         osg.Vec3.add(tempvec, pos, pos);
         camera.Position = pos;
         camera.finished();
      }
   }

   this.mouseMove = function(x, y, handled) {

      if(!active) return;

      var pos = camera.Position;
      var eyedir = camera.EyeDirection;
      var mouseX = Input.getAxis(Axis.MouseDeltaXRaw);
      var mouseY = Input.getAxis(Axis.MouseDeltaYRaw);
      osg.Vec3.cross(eyedir, up, toRight);

      osg.Quat.makeRotate(-mouseX * rotatespeed, up[0], up[1], up[2], rotateOp);
      osg.Quat.rotate(rotateOp, eyedir, eyedir);
      osg.Quat.makeRotate(mouseY * rotatespeed, toRight[0], toRight[1], toRight[2], rotateOp);
      osg.Quat.rotate(rotateOp, eyedir, eyedir);

      camera.Position = pos;
      camera.EyeDirection = eyedir;
      camera.finished();
   }

   var self = this;
   this.update = function() {

      var dt = FRAME_DELTA_TIME;

      var pos = camera.Position;
      var eyedir = camera.EyeDirection;
      osg.Vec3.cross(eyedir, up, toRight);

      var speed = movespeed;
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
        osg.Quat.makeRotate(dt * rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Right")) {
        osg.Quat.makeRotate(-dt * rotatekeysspeed, up[0], up[1], up[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Up") && eyedir[2] < 0.99) {

        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(-dt * rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
        osg.Quat.rotate(rotateOp, eyedir, eyedir);
        modified = true;
      }
      if(Input.getKey("Down") && eyedir[2] > -0.99) {
        osg.Vec3.cross(up, eyedir, tempvec);
        osg.Quat.makeRotate(dt * rotatekeysspeed, tempvec[0], tempvec[1], tempvec[2], rotateOp);
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

function registerEditorMotionModel(camera) {
   var handler = new EditorMotionModel(camera);
   Input.addInputCallback(handler);
   setInterval(handler.update, 0);
   return handler;
}

var cameraSystem = getEntitySystem("Camera");
var mainCamera = null;

var camid = getEntitySystem("Map").getEntityIdByUniqueId("defaultCam");
if(camid !== 0) {
   registerEditorMotionModel(EntityManager.getEntitySystem("Camera").getComponent(camid));

}

// create camera motion system when camera is created
function onCameraAdded(name, params) {

  var camid = params.AboutEntity;
  var cam = cameraSystem.getComponent(camid);
  if(cam && cam.IsMainCamera) {
    registerEditorMotionModel(cam);
  }
}
EntityManager.registerForMessages("CameraAddedMessage", onCameraAdded);



