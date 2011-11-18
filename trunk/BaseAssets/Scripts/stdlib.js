var applicationSystem = EntityManager.getEntitySystem("Application");


////////////////////////////////////////////////////////////////////////////////
function random(maxNum) {
  return Math.ceil(Math.random() * maxNum);
}

////////////////////////////////////////////////////////////////////////////////
function round(num) {
   return Math.round(num * 1000) / 1000;
}

////////////////////////////////////////////////////////////////////////////////
var __TIMEOUT_IDX = 0;
var __TIMEOUTS = {};
var __TIMEOUTS_REGISTERED = false;
var __TIMEOUT_NOW = 0;

////////////////////////////////////////////////////////////////////////////////
function setTimeout(callback, delay) {
  
  var args = [];
  for(var i = 2; i < arguments.length; ++i) {
    args.push(arguments[i]);
  }
  var timeoutId = __TIMEOUT_IDX++;
  __TIMEOUTS[timeoutId] = [__TIMEOUT_NOW + delay, callback, args];
  return timeoutId;
}

////////////////////////////////////////////////////////////////////////////////
function clearTimeout(timeoutId) {
  if(timeoutId in __TIMEOUTS) {
    delete __TIMEOUTS[timeoutId];
  }
}

////////////////////////////////////////////////////////////////////////////////
function __triggerTimeoutCBs() {
  
  for(var k in __TIMEOUTS) {
    if(__TIMEOUTS[k][0] <= __TIMEOUT_NOW) {
      var timeout = __TIMEOUTS[k];
      timeout[1].apply(timeout[1], timeout[2]);
      delete __TIMEOUTS[k];
    }    
  }  
}

var __INTERVAL_IDX = 0;
var __INTERVALS = {};

////////////////////////////////////////////////////////////////////////////////
function setInterval(callback, delay) {
  var args = [];
  for(var i = 2; i < arguments.length; ++i) {
    args.push(arguments[i]);
  }
  var intervalId = __INTERVAL_IDX++;
  __INTERVALS[intervalId] = [__TIMEOUT_NOW + delay, callback, args, delay];
  return intervalId;
}
////////////////////////////////////////////////////////////////////////////////
function clearInterval(intervalId) {
  if(intervalId in __INTERVALS) {
    delete __INTERVALS[intervalId];
  }
}

////////////////////////////////////////////////////////////////////////////////
function __triggerIntervalCBs() {

  for(var k in __INTERVALS) {
    if(__INTERVALS[k][0] <= __TIMEOUT_NOW) {
      var timeout = __INTERVALS[k];
      timeout[1].apply(timeout[1], timeout[2]);
      timeout[0] = __TIMEOUT_NOW + timeout[3];
    }    
  }
}
////////////////////////////////////////////////////////////////////////////////
function __executeTimeOuts(dt, time, clocktime) 
{
  __TIMEOUT_NOW = clocktime;
  __triggerTimeoutCBs();
  __triggerIntervalCBs();
}



////////////////////////////////////////////////////////////////////////////////
function createEntity(proto) {
  
  var entityId = EntityManager.createEntity();
  var comps = [];
  for(var k in proto) {
  
    var properties = proto[k];
    var entitySystem = EntityManager.getEntitySystem(k);
    
    if(!entitySystem) {
      Log.error("Cannot create component: entity system not found: " + k);
      continue;
    }
    var component = entitySystem.createComponent(entityId);
    for(var prop in properties) {
      component[prop] = properties[prop];
    }
    comps.push(component);

  }

  for(k in comps) {
     comps[k].finished();
  }

  return entityId;
}

////////////////////////////////////////////////////////////////////////////////
function createSpawnerFromEntity(entityid) {

  var spawner = {};
  var components = EntityManager.getComponents(entityid);

  for(var i in components) {
    var component = components[i];

    var spawnerprops = [];
    var props = component.properties();
    for(var j in props) {
      spawnerprops[j] = props[j];
    }

    spawner[i] = spawnerprops;
  }
  return spawner;
}
