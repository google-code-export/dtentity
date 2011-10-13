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

////////////////////////////////////////////////////////////////////////////////
function setTimeout(callback, delay) {
  var now = Math.floor(applicationSystem.getRealClockTime() / 1000);
  var args = [];
  for(var i = 2; i < arguments.length; ++i) {
    args.push(arguments[i]);
  }
  var timeoutId = __TIMEOUT_IDX++;
  __TIMEOUTS[timeoutId] = [now + delay, callback, args];
  return timeoutId;
}

////////////////////////////////////////////////////////////////////////////////
function clearTimeout(timeoutId) {
  if(timeoutId in __TIMEOUTS) {
    delete __TIMEOUTS[timeoutId];
  }
}

////////////////////////////////////////////////////////////////////////////////
function __triggerTimeoutCBs(now) {
  
  for(var k in __TIMEOUTS) {
    if(__TIMEOUTS[k][0] <= now) {
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
  var now = Math.floor(applicationSystem.getRealClockTime() / 1000);
  var args = [];
  for(var i = 2; i < arguments.length; ++i) {
    args.push(arguments[i]);
  }
  var intervalId = __INTERVAL_IDX++;
  __INTERVALS[intervalId] = [now + delay, callback, args, delay];
  return intervalId;
}
////////////////////////////////////////////////////////////////////////////////
function clearInterval(intervalId) {
  if(intervalId in __INTERVALS) {
    delete __INTERVALS[intervalId];
  }
}

////////////////////////////////////////////////////////////////////////////////
function __triggerIntervalCBs(now) {

  for(var k in __INTERVALS) {
    if(__INTERVALS[k][0] <= now) {
      var timeout = __INTERVALS[k];
      timeout[1].apply(timeout[1], timeout[2]);
      timeout[0] += timeout[3];
    }    
  }
}
////////////////////////////////////////////////////////////////////////////////
function __executeTimeOuts(msgtype, params) 
{
  var now = Math.floor(applicationSystem.getRealClockTime() / 1000);
  __triggerTimeoutCBs(now);
  __triggerIntervalCBs(now);
}

EntityManager.registerForMessages("TickMessage", __executeTimeOuts);

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
     comps[k].onFinishedSettingProperties();
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