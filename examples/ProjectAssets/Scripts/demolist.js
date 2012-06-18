include_once("Scripts/stdlib.js");
include_once("Scripts/osgveclib.js");
var staticMeshSystem = getEntitySystem("StaticMesh");
var layerSystem = getEntitySystem("Layer");
var transformSystem = getEntitySystem("PositionAttitudeTransform");
var mapSystem = getEntitySystem("Map");

// use libRocket or CEGUI?

var demolist_initialized = false;
var rocketSystem = getEntitySystem("Rocket");
if(rocketSystem !== null) {
  println("Starting libRocket demo gui");
  initRocket();
  demolist_initialized = true;
} else {
  var ceguiSystem = getEntitySystem("CEGUI");
  if(ceguiSystem != null) {
    println("Starting CEGUI demo gui");
    initCegui();
    demolist_initialized = true;
  }
  else {
	Log.error("No GUI library found! Please compile dtEntity with either libRocket or CEGUI support!");
  }
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
  
  huds = context.loadDocument("LibRocket/huds.rml");
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

	
	ceguiSystem.loadScheme("WindowsLook.scheme");
	//var rootWidget = ceguiSystem.getWidget("Root");
	ceguiSystem.setMouseCursor("WindowsLook", "MouseMoveCursor");
	ceguiSystem.showCursor();
	
	////////////////////////////////////// Demo manager /////////////////////////////////////
	var demo_items = [];

	ceguiSystem.createWidget("Root", "WindowsLook/ItemListbox", "itemlist");
	ceguiSystem.setWidgetProperty("itemlist", "UnifiedAreaRect", "{{0,20},{0,20},{0,150},{0,150}}");
	ceguiSystem.setWidgetProperty("itemlist", "Visible", "true");

	var currentStopCB = null;

	demo_list.onSelectionChanged = function() {
	  for(i = 0; i < demo_items.length; ++i) {
		var item = demo_items[i];
		if(ceguiSystem.getWidgetProperty(item.name, "Selected") == "True") { 
		  
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
	  ceguiSystem.createWidget("itemlist", "WindowsLook/ListboxItem", demoname);
	  ceguiSystem.setWidgetProperty(demoname, "Text", demoname);
	  ceguiSystem.setWidgetProperty(demoname, "Visible", "true");
	  demo_items.push({name : demoname, startCB : startfunc, stopCB : stopfunc});  
	}

	var helpWidget = false;

	var showHelp = function(text) {
	  if(helpWidget)  {
		hideHelp();
	  }
	  helpWidget = true;
	  ceguiSystem.createWidget("Root", "WindowsLook/StaticText", "helpWidget");
	  ceguiSystem.setWidgetProperty("helpWidget", "Text", text);
	  ceguiSystem.setWidgetProperty("helpWidget", "UnifiedAreaRect", "{{0.02,0},{0.7,0},{0.98,0},{0.98,0}}");
	  ceguiSystem.setWidgetProperty("helpWidget", "FrameEnabled", "False");
	  ceguiSystem.setWidgetProperty("helpWidget", "Alpha", "0.7");
	  ceguiSystem.setWidgetProperty("helpWidget", "Font", "DejaVuSans-10");
	  ceguiSystem.setWidgetProperty("helpWidget", "VertFormatting", "TopAligned");
	  ceguiSystem.setWidgetProperty("helpWidget", "MousePassThroughEnabled", "True");
	}

	hideHelp = function() {
	  ceguiSystem.destroyWidget("helpWidget");
	  helpWidget = false;
	}
		
	return true;
}
