include_once("Scripts/motionmodel.js");

////////////////////////////// Variables ////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
function startWindowManagementDemo() {
  
  var text = "Shows how to create and manage multiple windows";
  showHelp(text);

  getEntitySystem("Map").loadMap("maps/fpsdemo.dtemap");

  var contextid = Screen.openWindow("MyWindow", "root", {width : 400, height: 600});
  editorMotionComp.Enabled = true;
}

EntityManager.registerForMessages("CameraAddedMessage", function(name, params) {
   println("Adding camera motion for " + params.AboutEntity);
   getEntitySystem("EditorMotion").createComponent(params.AboutEntity);
  }
);

////////////////////////////////////////////////////////////////////////////////
function stopWindowManagementDemo() {

  hideHelp();
  Screen.closeWindow("MyWindow");
  ddm.clear();

  getEntitySystem("Map").unloadMap("maps/fpsdemo.dtemap");
  editorMotionComp.Enabled = false;
}

addDemo("Window Management", startWindowManagementDemo, stopWindowManagementDemo);
