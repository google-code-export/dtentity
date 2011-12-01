include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/motionmodel.js");

////////////////////////////// Variables ////////////////////////////////

var camid = mapSystem.getEntityIdByUniqueId("cam_0");
var clamper = getEntitySystem("GroundClamping").createComponent(camid);
clamper.ClampingMode = "KeepAboveTerrain";
clamper.VerticalOffset = 1;
clamper.finished();
var cameraComponent = getEntitySystem("Camera").getComponent(camid);
var tempvec = [0,0,0];
var toRight = [0,0,0];
var movespeed = 100;
var rotatespeed = 0.001;

var up = [0, 0, 1];
var rotateOp = [0, 0, 0, 1];

var inputCallback = {};

inputCallback.mouseMove = function(x, y, handled) {

   var pos = cameraComponent.Position;
   var eyedir = cameraComponent.EyeDirection;
   osg.Vec3.cross(eyedir, up, toRight);

   var mouseX = Input.getAxis(Axis.MouseDeltaXRaw);
   var mouseY = Input.getAxis(Axis.MouseDeltaYRaw);

   osg.Quat.makeRotate(-mouseX * rotatespeed, up[0], up[1], up[2], rotateOp);
   osg.Quat.rotate(rotateOp, eyedir, eyedir);
   osg.Quat.makeRotate(mouseY * rotatespeed, toRight[0], toRight[1], toRight[2], rotateOp);
   osg.Quat.rotate(rotateOp, eyedir, eyedir);

   cameraComponent.Position = pos;
   cameraComponent.EyeDirection = eyedir;
   cameraComponent.finished();
}

function update() {
   var dt = FRAME_DELTA_TIME;

   var pos = cameraComponent.Position;
   var eyedir = cameraComponent.EyeDirection;

   osg.Vec3.cross(eyedir, up, toRight);

   var speed = movespeed;
   if(Input.getKey("Shift_L")) {
     speed *= 4;
   }
   if(Input.getKey("w")) {
     osg.Vec3.mult(eyedir, dt * speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }
   if(Input.getKey("s")) {
     osg.Vec3.mult(eyedir, dt * -speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }
   if(Input.getKey("a")) {
     osg.Vec3.mult(toRight, dt * -speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }
   if(Input.getKey("d")) {
     osg.Vec3.mult(toRight, dt * speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }
   if(Input.getKey("q")) {
     osg.Vec3.cross(toRight, eyedir, tempvec);
     osg.Vec3.mult(tempvec, dt * -speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }
   if(Input.getKey("e")) {
     osg.Vec3.cross(toRight, eyedir, tempvec);
     osg.Vec3.mult(tempvec, dt * speed, tempvec);
     osg.Vec3.add(tempvec, pos, pos);
   }

   if(Input.getKey("Space")) {
     Screen.fullScreen = false;
     Screen.showCursor = true;
     Screen.lockCursor = false;
   }

   cameraComponent.Position = pos;
   cameraComponent.EyeDirection = eyedir;
   cameraComponent.finished();
}


var interval = 0;
////////////////////////////////////////////////////////////////////////////////
function startFPSDemo() {


  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);
  cameraComponent.Position = [0, 0, 2];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.FieldOfView = 80;
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  Screen.lockCursor = true;
  Screen.fullScreen = true;
  Screen.showCursor = false;
  
  Input.addInputCallback(inputCallback);
  interval = setInterval(update, 0);

  var mapsys = getEntitySystem("Map");
  mapsys.loadMap("maps/fpsdemo.dtemap");
  
  var text = "First Person Shooter steering.\n";
  text +=    "The mouse cursor is hidden and centered to the screen.\n";
  text +=    "Press space key to exit.\n";
  showHelp(text);  


}

////////////////////////////////////////////////////////////////////////////////
function stopFPSDemo() {

  Input.removeInputCallback(inputCallback);
  clearInterval(interval);
  Screen.fullScreen = false;
  Screen.showCursor = true;
  Screen.lockCursor = false;


  getEntitySystem("Map").unloadMap("maps/fpsdemo.dtemap");
  hideHelp();
}

addDemo("FPS Camera", startFPSDemo, stopFPSDemo);
