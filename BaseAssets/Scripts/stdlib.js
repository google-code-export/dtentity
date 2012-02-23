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
var FRAME_DELTA_TIME = 0;

////////////////////////////////////////////////////////////////////////////////
function setTimeout(callback, delay, arguments) {
  
  var args = [];
  if(typeof arguments != 'undefined') {
     for(var i = 0; i < arguments.length; ++i) {
		args.push(arguments[i]);
	  }
  }
  var timeoutId = __TIMEOUT_IDX++;
  __TIMEOUTS[timeoutId] = [__TIMEOUT_NOW + delay, callback, args];
  return timeoutId;
}

////////////////////////////////////////////////////////////////////////////////
function clearTimeout(timeoutId) {
  if(timeoutId in __TIMEOUTS) {
    delete __TIMEOUTS[timeoutId];
  } else {
    Log.error("Cannot clear timeout, no timeout with this id found");
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
function setInterval(callback, delay, arguments) {
  var args = [];
  if(typeof arguments != 'undefined') {
     for(var i = 0; i < arguments.length; ++i) {
		args.push(arguments[i]);
	  }
  }
  var intervalId = __INTERVAL_IDX++;
  __INTERVALS[intervalId] = [__TIMEOUT_NOW + delay, callback, args, delay];
  return intervalId;
}
////////////////////////////////////////////////////////////////////////////////
function clearInterval(intervalId) {
  if(intervalId in __INTERVALS) {
    delete __INTERVALS[intervalId];
  } else {
    Log.error("Cannot clear interval, no interval with this id found");
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
   FRAME_DELTA_TIME = dt;
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
var __ENTITYSYSTEMS = {};

function getEntitySystem(name) {
  if(name in __ENTITYSYSTEMS) {
     return __ENTITYSYSTEMS[name];
  }
  var es = EntityManager.getEntitySystem(name);

  __ENTITYSYSTEMS[name = es];
  return es;
}

////////////////////////////////////////////////////////////////////////////////
function createSpawnerFromEntity(entityid) {

  var spawner = {};
  var components = EntityManager.getComponents(entityid);

  for(var componentType in components) {
    var component = components[componentType];

    var entitySystem = getEntitySystem(componentType);
    if(entitySystem !== null && entitySystem.allowComponentCreationBySpawner()) {

       var spawnerprops = [];
       var props = component.properties();
       for(var j in props) {
         spawnerprops[j] = props[j];
       }

       spawner[componentType] = spawnerprops;
    }
  }
  return spawner;
}

////////////////////////////////////////////////////////////////////////////////
// function for formatting numbers.
// number is padded with "0" until <length> positive digits are
// shown. <decimals> decimals are shown.
function formatNum(num, length, decimals)
{
   var text = "" + Math.abs(num).toFixed(decimals);
   while(text.length < length + decimals + 1) {
      text = "0" + text;
   }
   return (num < 0) ? "-" + text : " " + text;
}


// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
// returns [ heading, attitude, bank]
function quatToEuler(q, r)
{
   if (r === undefined) {
       r = [];
   }
   var X = q[0];
   var Y = q[1];
   var Z = q[2];
   var W = q[3];

   var t = X * Y + Z * W;
   if(t > 0.499) {
      r[0] = 2 * Math.atan2(X,W);
      r[1] = Math.PI / 2;
      r[2] = 0;

   } else if(t < -0.499) {
      r[0] = -2 * Math.atan2(X,W);
      r[1] = - Math.PI / 2;
      r[2] = 0;
   } else  {

      var sqx = X*X;
      var sqy = Y*Y;
      var sqz = Z*Z;

      r[0] = Math.atan2(2 * Y * W - 2 * X * Z, 1 - 2 * sqy - 2 * sqz);
      r[1] = Math.asin(2 * t);
      r[2] = Math.atan2(2 * X * W - 2 * Y * Z, 1 - 2 * sqx - 2 * sqz);


   }
   return r;
}

function eulerToQuat(euler, r)
{
   if (r === undefined) {
       r = [];
   }
   var heading = euler[0];
   var attitude = euler[1];
   var bank = euler[2];
   var c1 = Math.cos(heading / 2);
   var c2 = Math.cos(attitude / 2);
   var c3 = Math.cos(bank / 2);
   var s1 = Math.sin(heading / 2);
   var s2 = Math.sin(attitude / 2);
   var s3 = Math.sin(bank / 2);

   r[0] = s1 * s2 * c3 + c1 * c2 * s3
   r[1] = s1 * c2 * c3 + c1 * s2 * s3
   r[2] = c1 * s2 * c3 - s1 * c2 * s3
   r[3] = c1 * c2 * c3 - s1 * s2 * s3
   return r;
}

var degToRadFac = Math.PI / 180.0;
function degToRad(deg) {
   return deg * degToRadFac;
}


var radToDegFac = 180.0 / Math.PI ;
function radToDeg(deg) {
   return deg * radToDegFac;
}

function makeVec2(x,y) {
   var ret = [x,y];
   ret.__TYPE_HINT = "V2";
   return ret;
}

function makeVec3(x,y,z) {
   var ret = [x,y,z];
   ret.__TYPE_HINT = "V3";
   return ret;
}

function makeVec4(x,y,z,w) {
   var ret = [x,y,z,w];
   ret.__TYPE_HINT = "V4";
   return ret;
}

function makeQuat(x,y,z, w) {
   var ret = [x,y,z,w];
   ret.__TYPE_HINT = "QT";
   return ret;
}

function makeMatrix(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16) {
   var ret = [[p1,p2,p3,p4],[p5,p6,p7,p8],[p9,p10,p11,p12],[p13,p14,p15,p16]];
   ret.__TYPE_HINT = "MT";
   return ret;
}
