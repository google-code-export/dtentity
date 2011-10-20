include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");

////////////////////////////// Variables ////////////////////////////////
var animEntity = 0;
var animlist = null;
var playing = "";
var animsystem = EntityManager.getEntitySystem("OSGAnimation");

function addAnimation(position) {

  var entityProto = {

    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    },
    PositionAttitudeTransform : {
        Children : ["StaticMesh"],
        Position: position,
        Scale : [10,10,10],
        Attitude : osg.Quat.makeRotate(1.5, 1,0,0)
      },
    StaticMesh : {
      CacheHint : "CacheHardwareMeshes",
      Mesh : "SkeletalMeshes/zombie.osgb"
    },
    OSGAnimation : {
    }
  };

  // create entity and setup components from prototype
  animEntity = createEntity(entityProto);
  EntityManager.addToScene(animEntity);

}

function changeAnimation(animname) {
   if(playing !== "" && playing != "None") {
     animsystem.stopAnimation(animEntity, playing);
   }
   if(animname != "None") {
     animsystem.playAnimation(animEntity, animname);
   }
   playing = animname;
}

////////////////////////////////////////////////////////////////////////////////
function startAnimDemo() {
  
  var camid = mapSystem.getEntityIdByUniqueId("defaultCam");
  var cameraComponent = EntityManager.getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [4, -8, 2.8];
  cameraComponent.EyeDirection = [0, 1, 0];
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  var animsystem = EntityManager.getEntitySystem("OSGAnimation");
  if(animsystem === null) {
    Log.error("Cannot start animation demo: No animation system found!");
    return;
  }


  addAnimation([0, 0, 0]);

  var anims = animsystem.getAnimations(animEntity);
  if(anims.length > 0) {
	animsystem.playAnimation(animEntity, anims[0]);
  }
  if(rocketSystem) {
     var animbox = window.getElementById("animationbox");
     animbox.style = "visibility:visible;"

     var animSelect = window.getElementById("AnimationChooser");
     for(var k in anims) {
       var anim = anims[k];
       var option = rocketSystem.instanceElement(animSelect, "*", "option", {value: anim});
       option.setInnerRML(anim);
       animSelect.appendChild(option);
     }
  } else {

    var rootWidget = GUI.getWidget("Root");
    animlist = GUI.createWidget(rootWidget, "WindowsLook/ItemListbox", "animlist");
    animlist.UnifiedAreaRect = "{{0,20},{0,160},{0,150},{0,450}}";
    animlist.Visible = true;

    var animwidgets = [];
    for(var k in anims) {
      var anim = anims[k];
      var widgt = GUI.createWidget(animlist, "WindowsLook/ListboxItem", anim);
      widgt.Text = anim;
      widgt.Visible = true;
      animwidgets.push(widgt);
    }

    animlist.onSelectionChanged = function() {
      for(i = 0; i < animwidgets.length; ++i) {
        var item = animwidgets[i];
        if(item.Selected == "True") {
          if(playing !== "") {
            animsystem.stopAnimation(animEntity, playing);
          }
          animsystem.playAnimation(animEntity, item.Text);
          playing = item.Text;
        }
      }
    };
  }
  var text = "Shows how to load an animated mesh and to start / stop animations.";
  showHelp(text);
}

////////////////////////////////////////////////////////////////////////////////
function stopAnimDemo() {
  EntityManager.removeFromScene(animEntity);
  EntityManager.killEntity(animEntity);
  animEntity = 0;

   if(rocketSystem) {
      var animbox = window.getElementById("animationbox");
      animbox.style = "visibility:hidden;"
   } else {
    GUI.destroyWidget(animlist);
  }
  hideHelp();
}
if(EntityManager.hasEntitySystem("OSGAnimation")) {
  addDemo("Animations", startAnimDemo, stopAnimDemo);
}
