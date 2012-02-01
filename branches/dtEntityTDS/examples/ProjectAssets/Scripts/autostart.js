include_once("Scripts/stdlib.js");
include_once("Scripts/motionmodel.js");
include_once("Scripts/editormotionmodel.js");
var camid = createEntity({
  Camera : {
    ContextId : 0,
    CullingMode : "NoAutoNearFarCulling",
    NearClip : 1,
    FarClip: 100000,
	ClearColor: [0.5,0.5,0.5,1]
  },
  Map: {
    EntityName : "cam_0",
    UniqueId : "cam_0"
  },
  Motion : {
    Enabled : false
  },
  EditorMotion : {
    Enabled : false
  }
});
EntityManager.addToScene(camid);
include_once("Scripts/demolist.js");

var motionComp = getEntitySystem("Motion").getComponent(camid);
var editorMotionComp = getEntitySystem("EditorMotion").getComponent(camid);

if(demolist_initialized) {

	include("Scripts/demo_anim.js");
	include("Scripts/demo_fps.js");
	include("Scripts/demo_layersystem.js");
	//include("Scripts/demo_particles.js");
	include("Scripts/demo_rocket.js");
	include("Scripts/demo_spiral.js");
	include("Scripts/demo_tools.js");
	//include("Scripts/demo_filesystem.js");
	include("Scripts/demo_windowmanagement.js");
	include("Scripts/demo_objectmotionmodel.js");
	include("Scripts/demo_particlesystem.js");

}
