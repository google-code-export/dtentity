

////////////////////////////// Variables ////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
function startWindowManagementDemo() {
  
  var text = "Shows how to create and manage multiple windows";
  showHelp(text);

   Screen.openWindow("MyWindow", "default", {width : 400, height: 600});

}

////////////////////////////////////////////////////////////////////////////////
function stopWindowManagementDemo() {

  hideHelp();
  ddm.clear();
}

addDemo("Window Management", startWindowManagementDemo, stopWindowManagementDemo);
