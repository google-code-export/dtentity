include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");

var ddm = new DebugDrawManager();
ddm.setEnabled(true);

var earthradius = 6371;
var camradius = 100000;

////////////////////////////////////////////////////////////////////////////////
// Create camera
////////////////////////////////////////////////////////////////////////////////
var camid = createEntity({
  Camera : {
    ContextId : 0,
    CullingMode : "NoAutoNearFarCulling",
    NearClip : 100,
    FarClip: 1000000,
  ClearColor: [0,0,0,1],
  EyeDirection: [0, 1, 0],
  Position: [0, -camradius , 0],
  FieldOfView : 10

  }
});
EntityManager.addToScene(camid);
var camera = getEntitySystem("Camera").getComponent(camid);

////////////////////////////////////////////////////////////////////////////////
// Create earth
////////////////////////////////////////////////////////////////////////////////
var earthid = createEntity({
  Layer : {
    AttachedComponent : "StaticMesh",
  },
  StaticMesh: {
    Mesh : "StaticMeshes/earth.osg"
  }
});
EntityManager.addToScene(earthid);

////////////////////////////////////////////////////////////////////////////////
// Create a light
////////////////////////////////////////////////////////////////////////////////
var lightid = createEntity({
  Layer : {
    AttachedComponent : "Light",
  },
  Light: {
    Ambient : [0.1843139976, 0.1843139976, 0.1843139976, 1.0000000000],
  Direction: [1,1,-1],
  Position: [1, -1, 1, 0],
  SpotCutoff: 90
  }
});
EntityManager.addToScene(lightid);
var light = getEntitySystem("Light").getComponent(lightid);

////////////////////////////////////////////////////////////////////////////////
// Get pick position of mouse on earth
////////////////////////////////////////////////////////////////////////////////
function getMousePickPos() {
  var campos = camera.Position;
  var pickray = Screen.getPickRay(Input.getAxis(Axis.MouseX), Input.getAxis(Axis.MouseY));
  var intersects = Screen.intersect(campos, osg.Vec3.add(campos, osg.Vec3.mult(pickray, 1000000)));
  if(intersects !== null && intersects.length !== 0) {
    return intersects[0].Position;
  }
  return null;
}

////////////////////////////////////////////////////////////////////////////////
// Create mouse camera control
////////////////////////////////////////////////////////////////////////////////
var InputCallback = {

  rotation : [0, 0, 0, 1],
  rotateOp : [0, 0, 0, 1],
  contextId : 0,
  mouse1pressed : false,
  mouse2pressed : false,
  mousePickPos : [0,0,0],
  
  mouseButtonDown : function(button, handled, cid) {
      if(button === 1) {
       mousePickPos = getMousePickPos();
     if(mousePickPos) {
       this.mouse1pressed = true;
       osg.Vec3.normalize(mousePickPos, mousePickPos);
       return true;
         }
     return false;

      }
    if(button === 2) {
      this.mouse2pressed = true;
    Screen.lockCursor = true;
    return true;
      }
   },

   mouseButtonUp : function(button, handled, cid) {
      if(button === 1) {
         this.mouse1pressed = false;
         return true;
      } else if(button === 2) {
         this.mouse2pressed = false;
     Screen.lockCursor = false;
    }
   },


   mouseMove : function(x, y, handled, cid) {

      if(this.mouse1pressed) {
      var newMousePickPos = getMousePickPos();

      if(mousePickPos === null || newMousePickPos === null) {
      this.mouse0pressed = false;
      return;
      }

      osg.Vec3.normalize(newMousePickPos, newMousePickPos);
      osg.Quat.makeRotateFromVecs(newMousePickPos, mousePickPos, this.rotateOp);
      osg.Quat.mult(this.rotation, this.rotateOp, this.rotation);

      var pos = [0,0,0];
      osg.Quat.rotate(this.rotation, [0, -camradius, 0], pos);
      camera.Position = pos;
      osg.Vec3.normalize(pos, pos);
      osg.Vec3.mult(pos, -1, pos);
      camera.EyeDirection = pos;
      camera.Up = osg.Quat.rotate(this.rotation, [0,0,1]);

      var lightpos = [1,-1,1,0];
      var dir = [1,1,-1];
      osg.Quat.rotate(this.rotation, lightpos, lightpos);
      osg.Quat.rotate(this.rotation, dir, dir);
      light.Position = lightpos;
      light.Direction = dir;

      camera.finished();
    }
    else if(this.mouse2pressed) {
       camradius += Input.getAxis(Axis.MouseDeltaYRaw) * 100;
     if(camradius < 7000) camradius = 7000;
     var pos = camera.Position;
     osg.Vec3.normalize(pos, pos);
     osg.Vec3.mult(pos, camradius, pos);
     camera.Position = pos;
     camera.finished();
    }
   },
   mouseWheel : function(dir) {
     camradius += dir * 10000;
   if(camradius < 7000) camradius = 7000;
   var pos = camera.Position;
   osg.Vec3.normalize(pos, pos);
   osg.Vec3.mult(pos, camradius, pos);
   camera.Position = pos;
   camera.finished();
   }
}

Input.addInputCallback(InputCallback);

////////////////////////////////////////////////////////////////////////////////
// Create libRocket GUI
//////////////////////////////////////////////////////////////////////////////// 

var rocketSystem = getEntitySystem("Rocket");
var hudSystem = getEntitySystem("HUD");
var guiId = createEntity(
  {
    Rocket : {
      FullScreen : true,
    ContextName : "Main",
    Debug : true
    }
  }
);

rocketSystem.loadFontFace("LibRocket/Delicious-Roman.otf");
rocketSystem.loadFontFace("LibRocket/Delicious-Italic.otf");
rocketSystem.loadFontFace("LibRocket/Delicious-Bold.otf");
rocketSystem.loadFontFace("LibRocket/Delicious-BoldItalic.otf");


var context = rocketSystem.getContext(guiId);  

var mouseCursor = context.loadMouseCursor("LibRocket/cursor.rml");

var huds = context.loadDocument("LibRocket/huds.rml");
huds.show();

var actors = context.loadDocument("LibRocket/earthactors.rml");
actors.show();

actors.getElementById("jet_draggable").addEventListener("dragend", function() {
   createThing("jet");

});

actors.getElementById("chopper_draggable").addEventListener("dragend", function() {
  createThing("chopper");
});

actors.getElementById("tank_draggable").addEventListener("dragend", function() {
  createThing("tank");
});

actors.getElementById("tank_draggable").addEventListener("dragstart", function() {
      mouseCursor.getElementById("cursorimage").src = "tank_sml.png";
});

actors.getElementById("chopper_draggable").addEventListener("dragstart", function() {
      mouseCursor.getElementById("cursorimage").src = "chopper_sml.png";
});

actors.getElementById("jet_draggable").addEventListener("dragstart", function() {
      mouseCursor.getElementById("cursorimage").src = "jet_sml.png";
});
////////////////////////////////////////////////////////////////////////////////
// Create entities
////////////////////////////////////////////////////////////////////////////////

function createThing(what) {

  // reset cursor
  mouseCursor.getElementById("cursorimage").src = "clear.png";

  var position = getMousePickPos();
  if(!position) {
    return;
    }
    //var quat1 = osg.Quat.makeRotate(anglex, 0,0,1);
  //var quat2 = osg.Quat.makeRotate(anglez, 0,1,0);
  //var quat = osg.Quat.mult(quat2, quat1);
  //var pos = osg.Quat.rotate(quat, [-earthradius, 0, 0]);
  osg.Vec3.normalize(position);
  osg.Vec3.mult(position, earthradius + 100, position);
  var target = [0,0,0];
    osg.Vec3.normalize(position, target);
  osg.Vec3.mult(target, -1, target);
  var rot = [0,0,0,1];
  osg.Quat.makeRotateFromVecs([0,0,-1], target, rot);

  var entityid = createEntity({
    Layer : {
    AttachedComponent : "PositionAttitudeTransform",
    },
    PositionAttitudeTransform: {
    Position : position,
    Attitude: rot,
    //Children: ["StaticMesh"],
    Scale : [100,100,100]

    },
    /*StaticMesh:
    {
      Mesh:"StaticMeshes/articulation_test.ive"
    },*/
    HUD : {
    Offset : [0,0,1],
    PixelOffset : [-32, 32],
    HideWhenNormalPointsAway : true
    }
  });

  var entitydiv = rocketSystem.instanceElement(null, "*", "div");
  entitydiv.style ="position: absolute;width:64px;height:64px;";
  entitydiv.setId("HUD_" + entityid);
  entitydiv.setInnerRML("<img src='" + what + "_sml.png' onmouseover='this.src=\"" + what + "_hil.png\"' onmouseout='this.src=\"" + what + "_sml.png\"'/>");
  huds.appendChild(entitydiv);
  var hudcomp = hudSystem.getComponent(entityid);
  hudcomp.Element = "HUD_" + entityid;
  hudcomp.finished();
  EntityManager.addToScene(entityid);
}



function mouseover(self) {
  self.src = "chopper_big.png";

}
