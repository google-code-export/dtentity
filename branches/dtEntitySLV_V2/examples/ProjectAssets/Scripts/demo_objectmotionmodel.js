include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");
include_once("Scripts/objectmotionmodel.js");

////////////////////////////// Variables ////////////////////////////////
var meshEntity = 0;





////////////////////////////////////////////////////////////////////////////////
function startObjMotionModel() {
  
  var camid = mapSystem.getEntityIdByUniqueId("cam_0");
  var cameraComponent = getEntitySystem("Camera").getComponent(camid);


  cameraComponent.Position = [60, 8, -14];
  cameraComponent.EyeDirection = [-1, -1, -0.2];
  cameraComponent.CullingMode = "NoAutoNearFarCulling";
  cameraComponent.NearClip = 0.1;
  cameraComponent.FarClip = 10000;
  cameraComponent.Up = [0, 0, 1];
  cameraComponent.FieldOfViewId = 45;
  cameraComponent.finished();

   var mapsys = getEntitySystem("Map");
   mapsys.loadMap("maps/fpsdemo.dtemap");

   var entityProto = {

     Layer : {
       Layer: "default",
       AttachedComponent : "PositionAttitudeTransform"
     },
     PositionAttitudeTransform : {
       Children : ["StaticMesh"],
       Scale:[1,1,1]
     },
     StaticMesh : {
       Mesh : "StaticMeshes/physics_crate.ive",
       CacheMode: "None"
     }
   };

   meshEntity = createEntity(entityProto);
   EntityManager.addToScene(meshEntity);

   var omm = getEntitySystem("ObjectMotion").createComponent(camid);
   println("Setting mesh entity: " + meshEntity);
   omm.TargetEntityId =  meshEntity;
   omm.finished();

  var text = "Demonstrates the simple object motion model";
  showHelp(text);

}

////////////////////////////////////////////////////////////////////////////////
function stopObjMotionModel() {
  EntityManager.removeFromScene(meshEntity);
  EntityManager.killEntity(meshEntity);

   getEntitySystem("ObjectMotion").deleteComponent(camid);
  hideHelp();
  var mapsys = getEntitySystem("Map");
  mapsys.unloadMap("maps/fpsdemo.dtemap");
}

addDemo("ObjectMotionModel", startObjMotionModel, stopObjMotionModel);

