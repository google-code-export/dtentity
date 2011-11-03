include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");
var staticMeshSystem = EntityManager.getEntitySystem("StaticMesh");
var layerSystem = EntityManager.getEntitySystem("Layer");
var transformSystem = EntityManager.getEntitySystem("PositionAttitudeTransform");
var mapSystem = EntityManager.getEntitySystem("Map");

// use libRocket or CEGUI?
var rocketSystem = EntityManager.getEntitySystem("Rocket");
var demolist_initialized = false;
if(rocketSystem !== null) {
  initRocket();
  demolist_initialized = true;
} else if(typeof GUI != "undefined") {
    initCegui();
	demolist_initialized = true;
}
else {
	Log.error("No GUI library found! Please compile dtEntity with either libRocket or CEGUI support!");
}

var window;

function initRocket() {

  ////////////////////////////////////// Setup libRocket /////////////////////////////////////
	
  var entityProto = {
    Rocket : {
		FullScreen : true,
		ContextName : "DemoChooser",
		Debug : true
    }
  };
  
  // create entity and setup components from prototype
  rocketId = createEntity(entityProto);
    
  rocketSystem.loadFontFace("LibRocket/Delicious-Roman.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-Italic.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-Bold.otf");
  rocketSystem.loadFontFace("LibRocket/Delicious-BoldItalic.otf");
  
  var context = rocketSystem.getContext(rocketId);  
  //context.loadMouseCursor("LibRocket/cursor.rml");
  window = context.loadDocument("LibRocket/demo_select.rml");
  window.style = "position: absolute; width: 200px; height: 400px; top: 30px; left: 30px;";
  window.getElementById("title").setInnerRML("dtEntity Demos");
  
  var huds = context.loadDocument("LibRocket/huds.rml");
  huds.show();

  var listDemoSelect = window.getElementById("demo_chooser");
  window.show();
  
  EntityManager.addToScene(rocketId);
  demos = {};
  started = "";
  addDemo = function(demoname, startfunc, stopfunc) {
	var option = rocketSystem.instanceElement(listDemoSelect, "*", "option", {value: demoname});
	option.setInnerRML(demoname);
    listDemoSelect.appendChild(option);
	demos[demoname] = [startfunc, stopfunc];
  }
  
  startDemo = function(demoname) {
    if(started !== "") {
		demos[started][1]();
	}
	started = demoname;	
	demos[demoname][0]();
  };
  
  showHelp = function(text) {
    var messagebox = window.getElementById("messagebox");
    messagebox.setInnerRML(text);
  };
  hideHelp = function() {
    var messagebox = window.getElementById("messagebox");
      messagebox.setInnerRML("");
  };
  return true;
}
 
 
 function initCegui() {
 ////////////////////////////////////// Setup CEGUI /////////////////////////////////////

	
	GUI.loadScheme("WindowsLook.scheme");
	var rootWidget = GUI.getWidget("Root");
	GUI.setMouseCursor("WindowsLook", "MouseMoveCursor");
	GUI.showCursor();
	
	////////////////////////////////////// Demo manager /////////////////////////////////////
	var demo_items = [];

	var demo_list = GUI.createWidget(rootWidget, "WindowsLook/ItemListbox", "itemlist");
	demo_list.UnifiedAreaRect = "{{0,20},{0,20},{0,150},{0,150}}";
	demo_list.Visible = true;

	var currentStopCB = null;

	demo_list.onSelectionChanged = function() {
	  for(i = 0; i < demo_items.length; ++i) {
		var item = demo_items[i];
		if(item.widget.Selected == "True") { 
		  
		  if(currentStopCB != null) {
			currentStopCB();
		  }
		  
		  println("Selected: " + item.name);
		  currentStopCB = item.stopCB;
		  item.startCB();      
		}
	  }
	};


	addDemo = function(demoname, startfunc, stopfunc) {
	  var widgt = GUI.createWidget(demo_list, "WindowsLook/ListboxItem", demoname);
	  widgt.Text = demoname;
	  widgt.Visible = true;
	  demo_items.push({name : demoname, widget : widgt, startCB : startfunc, stopCB : stopfunc});  
	}

	var helpWidget = null;

	showHelp = function(text) {
	  if(helpWidget != null)  {
		hideHelp();
	  }
	  helpWidget = GUI.createWidget(rootWidget, "WindowsLook/StaticText", "helpWidget");
	  helpWidget.Text = text;
	  helpWidget.UnifiedAreaRect = "{{0.02,0},{0.7,0},{0.98,0},{0.98,0}}";
	  helpWidget.FrameEnabled = "False";
	  helpWidget.Alpha = "0.7";
	  helpWidget.Font = "DejaVuSans-10";
	  helpWidget.VertFormatting = "TopAligned";
	  helpWidget.MousePassThroughEnabled = "True";
	}

	hideHelp = function() {
	  GUI.destroyWidget(helpWidget);
	  helpWidget = null;
	}
	
	return true;
}
