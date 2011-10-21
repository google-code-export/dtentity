include_once("Scripts/stdlib.js");

createEntity({
  Camera : {
    IsMainCamera : true,
    CullingMode : "NoAutoNearFarCulling",
    NearClip : 1,
    FarClip: 100000
  },
  Map: {
    EntityName : "defaultCam",
    UniqueId : "defaultCam"
  }
});

include_once("Scripts/motionmodel.js");

include_once("Scripts/demolist.js");



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

}
