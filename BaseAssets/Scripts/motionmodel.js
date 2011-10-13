include_once("Scripts/osgveclib.js");
var positionAttitudeTransformSystem = EntityManager.getEntitySystem("PositionAttitudeTransform");
var layerSystem = EntityManager.getEntitySystem("Layer");
var soundSystem = EntityManager.getEntitySystem("Sound");

var CameraTranslationSpeed = 50;
var CameraRotationSpeed = 3;


function CameraMotionsSystem() {

   var self = this;
   this.componentType = "CameraMotion";
   this.Enabled = false;
   this.MouseEnabled = true;
   var currentlyEnabled = false;
   this.KeepUpright = false;
   this.ShowSelection = true;
   this.MoveParallelToGround = false;

   var camid = EntityManager.getEntitySystem("Map").getEntityIdByUniqueId("defaultCam_camera");

   var targetCamComp = EntityManager.getEntitySystem("Camera").getComponent(camid);

   this.getCameraPosition = function() { return targetCamComp.Position; }
   this.getCameraEyeDir = function() { return targetCamComp.EyeDirection; }
   this.getCameraUp = function() { return targetCamComp.Up; }

   this.hasComponent = function(eid) { return false; };
   this.getComponent = function(eid) { return null; }
   this.createComponent = function(eid) { return null; }
   this.deleteComponent = function(eid) { return false; }
   this.getEntitiesInSystem = function() { return []; }

   this.onPropertyChanged = function(propname) {

      if(propname == "Enabled") {
         if(self.Enabled && !currentlyEnabled) {
            EntityManager.registerForMessages("EndOfFrameMessage", update, Priority.default);
            currentlyEnabled = true;
         }
         else if(!self.Enabled && currentlyEnabled) {
            currentlyEnabled = false;
            EntityManager.unregisterForMessages("EndOfFrameMessage", update);
         }
      }
   }

   var rotateOp = [0, 0, 0, 1];
   var tempVec = [0, 0, 0];
   var movement = [0, 0, 0];


   /////////////////// camera transform update loop /////////////////////
   function update(msgname, params) {
      if(self.Enabled === false) {
         return;
      }
      var mouseX = Input.getAxis(Axis.MouseX);
      var mouseY = Input.getAxis(Axis.MouseY);
      var dt = params.DeltaSimTime;
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

      var wheel = 0;
      if(mouseWheelState == MouseWheelState.Up) {
         wheel = 1;
      }
      else if(mouseWheelState == MouseWheelState.Down) {
         wheel = -1;
      }

      if(wheel != 0) {
         var fov = targetCamComp.FieldOfView;
         fov += wheel * -self.MouseWheelFactor;
         if(fov > 0 && fov < 120) {
            targetCamComp.FieldOfView = fov;
         }
      }


      targetCamComp.Position = pos;
      targetCamComp.EyeDirection = eyedir;
      targetCamComp.Up = up;

      targetCamComp.onFinishedSettingProperties();

      // update sound
      if(soundSystem != null) {
        soundSystem.ListenerTranslation = targetCamComp.Position;
        soundSystem.ListenerUp = targetCamComp.Up;
        soundSystem.ListenerEyeDirection = targetCamComp.EyeDirection;
        soundSystem.onFinishedSettingProperties();
      }
   }

   // make sure to register for ticks
   this.Enabled = true;
   this.onPropertyChanged("Enabled");



   /////////////////// react to EnableMotionModelMessages ////////////////////

   function enabledisable(msgname, params) {
      if(params.Enable && self.Enabled === false) {
         self.Enabled = true;
         self.onPropertyChanged("Enabled");

      }
      else if(!params.Enable && self.Enabled === true) {
         self.Enabled = false;
         self.onPropertyChanged("Enabled");

      }
   }
   EntityManager.registerForMessages("EnableMotionModelMessage", enabledisable, Priority.highest);
   enabledisable("", {Enable: true});

   
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
                                        targetCamComp.onFinishedSettingProperties();
                                     });
   
}

var cameraMotionSystem = new CameraMotionsSystem();
EntityManager.addEntitySystem(cameraMotionSystem);
