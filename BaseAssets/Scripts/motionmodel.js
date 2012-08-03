include_once("Scripts/osgveclib.js");

var positionAttitudeTransformSystem = getEntitySystem("PositionAttitudeTransform");
var layerSystem = getEntitySystem("Layer");
var soundSystem = getEntitySystem("Sound");
var cameraSystem = getEntitySystem("Camera");

var CameraTranslationSpeed = 50;
var CameraRotationSpeed = 3;


function MotionComponent(eid) {

   // public members
   this.componentType = "CameraMotion";
   createBoolProp(this, "Enabled", true);
   createBoolProp(this, "MouseEnabled", true);

  createBoolProp(this, "KeepUpright", false);
  createBoolProp(this, "ShowSelection", true);

   createBoolProp(this, "MoveParallelToGround", false);

   // private members

   var targetCamComp = null;
   var self = this; // not all callbacks receive a correct 'this'

   this.getCameraPosition = function() { return targetCamComp.Position; }
   this.getCameraEyeDir = function() { return targetCamComp.EyeDirection; }
   this.getCameraUp = function() { return targetCamComp.Up; }

   this.finished = function() {
      targetCamComp = getEntitySystem("Camera").getComponent(eid);
   }


   var rotateOp = [0, 0, 0, 1];
   var tempVec = [0, 0, 0];
   var movement = [0, 0, 0];


   /////////////////// camera transform update loop /////////////////////
   this.update = function(dt) {
      if(self.Enabled === false || targetCamComp === null) {
         return;
      }
      var mouseX = Input.getAxis(Axis.MouseX);
      var mouseY = Input.getAxis(Axis.MouseY);
      var up = targetCamComp.Up;
      var pos = targetCamComp.Position;
      var eyedir = targetCamComp.EyeDirection;
      osg.Vec3.normalize(eyedir, eyedir);

      if(Input.getKeyDown("KP_Add")) {
         CameraTranslationSpeed *= 1.5;
      }

      if(Input.getKeyDown("KP_Subtract")) {
         CameraTranslationSpeed /= 1.5;
      }

	  var rotate_delta = CameraRotationSpeed * dt;
	  var translate_delta = CameraTranslationSpeed * dt;
	  
      /////////////////////////////// handle rotation ///////////////////////////////
      if(Input.getKey("Left")) {
         osg.Quat.makeRotate(rotate_delta, up[0], up[1], up[2], rotateOp);
         osg.Quat.rotate(rotateOp, eyedir, eyedir);
      }
      if(Input.getKey("Right")) {
         osg.Quat.makeRotate(-rotate_delta, up[0], up[1], up[2], rotateOp);
         osg.Quat.rotate(rotateOp, eyedir, eyedir);
      }
      if(Input.getKey("Up")) {
         println("Up!");
         if(eyedir[2] < 0.99) {
            osg.Vec3.cross(up, eyedir, tempVec);
            osg.Quat.makeRotate(-rotate_delta, tempVec[0], tempVec[1], tempVec[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);
         }
      }
      if(Input.getKey("Down")) {
         if(eyedir[2] > -0.99) {
            osg.Vec3.cross(up, eyedir, tempVec);
            osg.Quat.makeRotate(rotate_delta, tempVec[0], tempVec[1], tempVec[2], rotateOp);
            osg.Quat.rotate(rotateOp, eyedir, eyedir);
         }
      }

      if(self.MouseEnabled && Input.getMouseButton(0))
      {

         osg.Vec3.cross(up, eyedir, tempVec);
         osg.Quat.makeRotate(-mouseX * rotate_delta, up[0], up[1], up[2], rotateOp);
         osg.Quat.rotate(rotateOp, eyedir, eyedir);
         osg.Quat.makeRotate(-mouseY * rotate_delta, tempVec[0], tempVec[1], tempVec[2], rotateOp);

         osg.Quat.rotate(rotateOp, eyedir, tempVec);
         if(tempVec[2] < 0.99 && tempVec[2] > -0.99)
         {
            osg.Vec3.copy(tempVec, eyedir);
         }
      }

      /////////////////////////// handle translation ////////////////////////////////
      if(Input.getKey("w")) {
		  if(self.MoveParallelToGround) {
			osg.Vec3.copy(eyedir, tempVec);
			tempVec[2] = 0;
			osg.Vec3.normalize(tempVec, tempVec);
			osg.Vec3.mult(tempVec, translate_delta, tempVec);
			osg.Vec3.add(pos, tempVec, pos);
		  } else {
			osg.Vec3.normalize(eyedir, tempVec);
			osg.Vec3.mult(tempVec, translate_delta, tempVec);
			osg.Vec3.add(pos, tempVec, pos);
		  }
		}
		if(Input.getKey("s")) {
		  if(self.MoveParallelToGround) {
			osg.Vec3.copy(eyedir, tempVec);
			tempVec[2] = 0;
			osg.Vec3.normalize(tempVec, tempVec);
			osg.Vec3.mult(tempVec, -translate_delta, tempVec);
			osg.Vec3.add(pos, tempVec, pos);
		  } else {
			osg.Vec3.normalize(eyedir, tempVec);
			osg.Vec3.mult(tempVec, -translate_delta, tempVec);
			osg.Vec3.add(pos, tempVec, pos);
		  }
		}
      if(Input.getKey("a")) {
         osg.Vec3.cross(eyedir, up, tempVec);
         osg.Vec3.normalize(tempVec, tempVec);
         osg.Vec3.mult(tempVec, -translate_delta, tempVec);
         osg.Vec3.add(pos, tempVec, pos);
      }
      if(Input.getKey("d")) {
         osg.Vec3.cross(eyedir, up, tempVec);
         osg.Vec3.normalize(tempVec, tempVec);
         osg.Vec3.mult(tempVec, translate_delta, tempVec);
         osg.Vec3.add(pos, tempVec, pos);
      }
      if(Input.getKey("e")) {
         tr = [up[0], up[1], up[2]];
         osg.Vec3.mult(tr, translate_delta, tr);
         osg.Vec3.add(pos, tr, pos);
      }
      if(Input.getKey("q")) {
         tr = [up[0], up[1], up[2]];
         osg.Vec3.mult(tr, -translate_delta, tr);
         osg.Vec3.add(pos, tr, pos);
      }
      if(Input.getMouseButton(2))
      {
         osg.Vec3.cross(eyedir, up, tempVec);
         osg.Vec3.normalize(tempVec, tempVec);
         osg.Vec3.mult(tempVec, mouseX * translate_delta, tempVec);
         osg.Vec3.add(pos, tempVec, pos);

         osg.Vec3.normalize(eyedir, tempVec);
         osg.Vec3.mult(tempVec, mouseY * translate_delta, tempVec);
         osg.Vec3.add(pos, tempVec, pos);
      }

      if(Input.getMouseButton(1))
      {
         pos[2] += mouseY * translate_delta;
      }

      /////////////////////////////// handle mouse wheel ///////////////////////////////
      var mouseWheelState = Input.getMouseWheelState();
      
      if(mouseWheelState != 0) {
         var fov = targetCamComp.FieldOfView;
         fov += mouseWheelState * -self.MouseWheelFactor;
         if(fov > 0 && fov < 120) {
            targetCamComp.FieldOfView = fov;
         }
      }

      targetCamComp.Position = pos;
      targetCamComp.EyeDirection = eyedir;
      targetCamComp.Up = up;

      targetCamComp.finished();

      // update sound
      if(soundSystem !== null) {
        soundSystem.ListenerTranslation = targetCamComp.Position;
        soundSystem.ListenerUp = targetCamComp.Up;
        soundSystem.ListenerEyeDirection = targetCamComp.EyeDirection;
        soundSystem.finished();
      }
   }

   /////////////////// react to EnableMotionModelMessages ////////////////////

   function enabledisable(msgname, params) {
      self.setEnabled(params.Enabled);
   }
   EntityManager.registerForMessages("EnableMotionModelMessage", enabledisable, Priority.highest);

   
   ////////////////////////////////// modify speed on keyboard number presses ////////////////////////////////
   var currentSpeedScale = 4;
   function ScaleSpeed(v) {
		currentSpeedScale = v;
		CameraTranslationSpeed = Math.pow(2, v + 1);
		CameraRotationSpeed = 1 + v * 0.2;
   }

   
   ////////////////////////////////////// Jump to entity ////////////////////////////////////////////
   function jumpTo(eid, distance) {
	   
      poscomp = positionAttitudeTransformSystem.getComponent(eid, true);
      if(poscomp === null) {
         Log.error("Cannot jump to: Has no position attitude transform component!");
         return;
      }
      
      actortranslation = poscomp.Position;   
      actortocam = [0, 0, 0];
      osg.Vec3.copy(targetCamComp.Position, actortocam);
      osg.Vec3.sub(actortocam, actortranslation, actortocam);
      actortocam[2] = 0;
      osg.Vec3.normalize(actortocam, actortocam);
      osg.Vec3.mult(actortocam, distance, actortocam);
      actortocam[2] = osg.Vec3.length(actortocam) / 4;
      osg.Vec3.add(actortocam, actortranslation, actortocam);
      targetCamComp.Position = actortocam;  
      osg.Vec3.sub(actortranslation, actortocam, tempVec);
      targetCamComp.EyeDirection = tempVec;
   }
   

   EntityManager.registerForMessages("JumpToEntityMessage", function(msgname, params) {

                                        if(!self.Enabled) { return; }

                                        jumpTo(params.AboutEntity, params.Distance);
                                     });
   
   EntityManager.registerForMessages("JumpToPositionMessage", function(msgname, params) {

                                        if(!self.Enabled) { return; }

                                        targetCamComp.Position = params.Position;
                                        eye = params.LookAt;
                                        osg.Vec3.sub(eye, params.Position, eye);
                                        if(osg.Vec3.length(eye) < 0.001) {
                                           eye = [0, 1, 0];
                                        }
                                        targetCamComp.EyeDirection = eye;
                                        targetCamComp.finished();
                                     });
   
}


////////////////////////////////////////////////////////////////////////////////
function MotionSystem() {

  var self = this;
  // -----------------------------------------
  var components = [];

  this.componentType = "Motion";

  setInterval(function() {
    for(k in components) {
      components[k].update(FRAME_DELTA_TIME);
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
      Log.error("Motion component with id " + eid + " already exists!");
      return;
    }

    var c = new MotionComponent(eid);
    components[eid] = c;

    Input.addInputCallback(c);

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

EntityManager.addEntitySystem(new MotionSystem());

