function MyComponent() {

  this.testNumber = 3.1415;
  this.testString = "Hello, World";
  this.testBool = false;
  
  this.onPropertyChanged = function(propname, val) {
	println("Component: Property " + propname + " changed value to " + val);
    this[propname] = val;
  }
  
  this.onFinishedSettingProperties = function() {}  
}

function MyEntitySystem() {

  var components = [];
  
  this.componentType = "MyEntitySystem";
  
  this.myVar = "Hello, World.";
  
  this.hasComponent = function(eid) { 
	  has =  (components[eid]) ? true : false; 
	  println("HasComp:" + has);
	  return has;
  };
  
  this.getComponent = function(eid) { 

	println("Getting component! Comps: " + eid);
    return components[eid];
  }
  
  this.createComponent = function(eid) {
	if(eid == 0) return;
    if(this.hasComponent(eid)) {
	  Log.error("Component with id " + eid 
	    + " already exists in entity system "
		+ this.getComponentType());
		return;
	}
	println("Creating Comp:" + eid);
    c = new MyComponent();
	components[eid] = c;
	return c;
  }

  this.deleteComponent = function(eid) {   
	println("Deleting Comp:" + eid);  
	if(this.hasComponent(eid)) {
      components.splice(eid, 0);
	}
  }
   
  this.getEntitiesInSystem = function() { 
	println("GEtting all");
    return components; 
  }

  this.onPropertyChanged = function(propname, val) {
	println("System: Property " + propname + " changed value to " + val);
    this[propname] = val;
  }
}

es = new MyEntitySystem();
EntityManager.addEntitySystem(es);

/*comp = es.createComponent(1);
comp.onPropertyChanged("testNumber", 666);
comp.onFinishedSettingProperties();
*/