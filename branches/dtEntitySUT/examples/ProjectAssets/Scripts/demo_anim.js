include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");

////////////////////////////// Variables ////////////////////////////////
var animEntity = 0;
var animlist = null;
var playing = "";
var animsystem = getEntitySystem("OSGAnimation");

function addAnimation(position) {

  var entityProto = {

    Layer : {
      Layer: "default",
      AttachedComponent : "PositionAttitudeTransform"
    },
    PositionAttitudeTransform : {
        Children : ["StaticMesh"],
        Position: position
      },
    StaticMesh : {
      CacheHint : "CacheHardwareMeshes",
      Mesh : "SkeletalMeshes/monster.osgb"
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
  
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);

  cameraComponent.Position = [6, 8, -14];
  cameraComponent.EyeDirection = [-1, -1, -0.2];
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.finished();
  
  var animsystem = getEntitySystem("OSGAnimation");
  if(animsystem === null) {
    Log.error("Cannot start animation demo: No animation system found!");
    return;
  }


  addAnimation([0, 0, -15]);

  var anims = animsystem.getAnimations(animEntity);
  /*if(anims.length > 0) {
    playing = anims[0];
	animsystem.playAnimation(animEntity, playing);
  }*/
  playing = "run";
  animsystem.playAnimation(animEntity, playing);
  if(rocketSystem !== null) {

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
  var text = "Shows how to load an animated mesh and to start / stop animations. Free model from www.3drt.com";
  showHelp(text);
  var mapsys = getEntitySystem("Map");
  mapsys.loadMap("maps/fpsdemo.dtemap");
}

////////////////////////////////////////////////////////////////////////////////
function stopAnimDemo() {
  EntityManager.removeFromScene(animEntity);
  EntityManager.killEntity(animEntity);
  animEntity = 0;


   if(rocketSystem !== null) {
      var animbox = window.getElementById("animationbox");
      animbox.style = "visibility:hidden;"
   } else {
    GUI.destroyWidget(animlist);
  }
  hideHelp();
  var mapsys = getEntitySystem("Map");
  mapsys.unloadMap("maps/fpsdemo.dtemap");
}
if(EntityManager.hasEntitySystem("OSGAnimation")) {
  addDemo("Animations", startAnimDemo, stopAnimDemo);
}
