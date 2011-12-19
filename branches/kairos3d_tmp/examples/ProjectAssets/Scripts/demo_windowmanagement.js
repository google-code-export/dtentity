include_once("Scripts/motionmodel.js");

////////////////////////////// Variables ////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
function startWindowManagementDemo() {
  
  var text = "Shows how to create and manage multiple windows";
  showHelp(text);

  getEntitySystem("Map").loadMap("maps/fpsdemo.dtemap");

  var contextid = Screen.openWindow("MyWindow", "root", {width : 400, height: 600});
  editorMotionComp.Enabled = true;

   var camid = createEntity({
     Camera : {
       ContextId : contextid,
       CullingMode : "NoAutoNearFarCulling",
       NearClip : 1,
       FarClip: 100000
     },
     Map: {
       EntityName : "cam_0",
       UniqueId : "cam_0"
     },
     EditorMotion : {
       Enabled : true
     }
   });
   EntityManager.addToScene(camid);
}



////////////////////////////////////////////////////////////////////////////////
function stopWindowManagementDemo() {

  hideHelp();
  //Screen.closeWindow("MyWindow");
  ddm.clear();

  getEntitySystem("Map").unloadMap("maps/fpsdemo.dtemap");
  editorMotionComp.Enabled = false;
}

addDemo("Window Management", startWindowManagementDemo, stopWindowManagementDemo);
