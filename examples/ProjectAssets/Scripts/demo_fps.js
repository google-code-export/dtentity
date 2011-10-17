include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/motionmodel.js");

////////////////////////////// Variables ////////////////////////////////

var camid = mapSystem.getEntityIdByUniqueId("defaultCam");
var clamper = EntityManager.getEntitySystem("GroundClamping").createComponent(camid);
clamper.ClampingMode = "KeepAboveTerrain";
clamper.VerticalOffset = 1;
clamper.finished();
var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);
var tempvec = [0,0,0];
var toRight = [0,0,0];
var movespeed = 100;
var rotatespeed = 0.5;

var up = [0, 0, 1];
var rotateOp = [0, 0, 0, 1];

function updateFPS(msgname, params) {       
  var dt = params.DeltaSimTime;
  
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
  
  var mouseX = Input.getAxis(Axis.MouseDeltaXRaw);	
  var mouseY = Input.getAxis(Axis.MouseDeltaYRaw);
  
  osg.Quat.makeRotate(-mouseX * rotatespeed * dt, up[0], up[1], up[2], rotateOp);
  osg.Quat.rotate(rotateOp, eyedir, eyedir);
  osg.Quat.makeRotate(mouseY * rotatespeed * dt, toRight[0], toRight[1], toRight[2], rotateOp);
  osg.Quat.rotate(rotateOp, eyedir, eyedir);
  
  cameraComponent.Position = pos;
  cameraComponent.EyeDirection = eyedir;
  cameraComponent.finished();
}

////////////////////////////////////////////////////////////////////////////////
function startFPSDemo() {

  cameraMotionSystem.Enabled = false;
  cameraMotionSystem.onPropertyChanged("Enabled");

  var camid = mapSystem.getEntityIdByUniqueId("defaultCam");
  var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);
  cameraComponent.Position = [0, 0, 2];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.FieldOfView = 80;
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  camMotion = EntityManager.getEntitySystem("CameraMotion");
  camMotion.StartPosition = cameraComponent.Position;
  camMotion.StartEyeDirection = [0, 0.9, -0.1];

  Screen.lockCursor = true;
  Screen.showCursor = false;
  Screen.fullScreen = true;
  
  EntityManager.registerForMessages("TickMessage", updateFPS);

  var mapsys = EntityManager.getEntitySystem("Map");
  mapsys.loadMap("maps/fpsdemo.dtemap");
  
  var text = "First Person Shooter steering.\n";
  text +=    "The mouse cursor is hidden and centered to the screen.\n";
  text +=    "Press space key to exit.\n";
  showHelp(text);  
}

////////////////////////////////////////////////////////////////////////////////
function stopFPSDemo() {
  cameraMotionSystem.Enabled = true;
  cameraMotionSystem.onPropertyChanged("Enabled");

  EntityManager.unregisterForMessages("TickMessage", updateFPS);
  Screen.fullScreen = false;
  Screen.showCursor = true;
  Screen.lockCursor = false;


  EntityManager.getEntitySystem("Map").unloadMap("maps/fpsdemo.dtemap");
  hideHelp();
}

addDemo("FPS Camera", startFPSDemo, stopFPSDemo);
