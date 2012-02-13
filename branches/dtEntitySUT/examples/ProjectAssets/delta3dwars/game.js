

////////////////////////////// Set up entity systems //////////////////////////////////

var debugDrawManager = new DebugDrawManager(EntityManager);
debugDrawManager.setEnabled(false);
var mapSystem = EntityManager.getEntitySystem("Map");
var transformSystem = EntityManager.getEntitySystem("PositionAttitudeTransform");
var movementSystem = EntityManager.getEntitySystem("Movement");
var layerSystem = EntityManager.getEntitySystem("Layer");
var collisionSystem = EntityManager.getEntitySystem("Collision");
var soundSystem = EntityManager.getEntitySystem("Sound");


////////////////////////////// Set up user input //////////////////////////////////
var keyboard = GameManager.getView(0).getKeyboard();
var mouse = GameManager.getView(0).getMouse();
var camera = GameManager.getView(0).getCamera();

////////////////////////////// Load map /////////////////////////////////////

mapSystem.loadScene("delta3dwars/scene.xml");

// get player entity and components from map
var playerId = mapSystem.getEntityIdByUniqueId("Player");
var playerTransformComponent = transformSystem.getComponent(playerId);
var playerMovementComponent = movementSystem.getComponent(playerId);
var playerTranslation = playerTransformComponent.Position;
var playerRotation = playerTransformComponent.Attitude;
var playerAngle = 0;

///////////////////////////// Global game variables ////////////////////////

var numberOfPlayerShots = 500;
var playerShotInterval = 0.015;
var playerShotLifetime = 1.0;
var timeSinceLastShot = playerShotInterval;
var cameraOffset = new Vec3(0, -30, 20);
var playerSpeed = 10.0;
var shotSpeed = 50.0;
var numberOfMonsters = 1;
var monsterDistance = 100;
var monsterForce = 50;
var timeTilMonsterReappears = 5;

///////////////////////////// Helper variables ////////////////////////
var upVector = new Vec3(0, 0, 1);
var forwardVec = new Vec3(0,1,0);
var pickPos = new Vec3();
var campos = new Vec3();
var camdir = new Vec3();
var cameraTransform = new Vec3(playerTranslation);

function getRandom(min, max) {
	return(min + Math.random() * ( max - min + 1));
}

///////////////////////////////// Get intersection of mouse pick ray and z=0 plane/////////////////////////////////

function updatePickPos() {
	camera.getPickRay(mouse.getPosition(), campos, camdir);
	if(camdir[2] == 0) {
		return false;
	}
		
	l = - campos[2] / camdir[2];

	if(l == 0) {
		return false;
	}
	pickPos[0] = campos[0] + camdir[0] * l;
	pickPos[1] = campos[1] + camdir[1] * l;
	pickPos[2] = 0; // by definition
	return true;
}


///////////////////////////// Create player bullets ///////////////////////////////////

// bullets are not created and deleted on demand, instead they are reused
var bullets = [];
var nextShot = 0;

function createPlayerBullets() {
	for(i = 0; i < numberOfPlayerShots; ++i) {
		
		bulletId = EntityManager.createEntity();
		mapSystem.spawn("Bullet", bulletId);
		EntityManager.addToScene(bulletId);	
		layerSystem.getComponent(bulletId).Visible = false;
		
		// shots stores an array of arrays. 
		// Inner array has meaning [entityId of shot, time since shooting event]
		bullets[i] = [bulletId, 0];
	}
}
createPlayerBullets();

var monsters = [];
function createMonsters() {
	for(i = 0; i < numberOfMonsters; ++i) {
		
		monsterId = EntityManager.createEntity();
		mapSystem.spawn("Monster", monsterId);
		EntityManager.addToScene(monsterId);	
		layerSystem.getComponent(monsterId).Visible = false;
		
		// shots stores an array of arrays. 
		// Inner array has meaning [entityId of shot, time to next appearance]
		monsters[i] = [monsterId, timeTilMonsterReappears + i];
		
		soundSystem.playSound(monsterId);
	}
}
createMonsters();

//////////////////////////////////////////////////////////////////////////////

shipToPickPos = new Vec3();

function updateBullets(dt) {
	// Check all bullets to see if they should be deactivated ( = turned invisible)
	for(i = 0; i < numberOfPlayerShots; ++i) {
		if(layerSystem.getComponent(bullets[i][0]).Visible) {
			
			bullets[i][1] += dt;
			if(bullets[i][1] > playerShotLifetime) {
				layerSystem.getComponent(bullets[i][0]).Visible = false;
				collisionSystem.getComponent(bullets[i][0]).Enabled = false;
			}	
			else {
				debugDrawManager.addSphere(transformSystem.getComponent(bullets[i][0]).Position, collisionSystem.getComponent(bullets[i][0]).Radius, new Vec4(0,1,1,1));
			}				
		}
	}
}
	
function updatePlayer(dt) {
	
	
	wp = keyboard.getKey("w");
	ap = keyboard.getKey("a");
	sp = keyboard.getKey("s");
	dp = keyboard.getKey("d");
	
	// apply forces to move player according to WASD keys
	force = new Vec2();
	if(wp) {	
		force[1] += 1000;
	}
	if(sp) {		
		force[1] -= 1000;
	}
	if(ap) {		
		force[0] -= 1000;
	}
	if(dp) {		
		force[0] += 1000;
	}

	playerTranslation = playerTransformComponent.Position;
	
	soundSystem.ListenerPosition = playerTranslation;
	
	playerMovementComponent.Force = force;
	
	// turn player towards pick position
	hasPickPos = updatePickPos();
	
	if(hasPickPos) {
		shipToPickPos.set(pickPos);
		shipToPickPos.subtract(playerTranslation);
		shipToPickPos[2] = 0;
		if(shipToPickPos.length() == 0) {
			hasPickPos = false;
		}
		else {
			shipToPickPos.normalize();		
			playerRotation.makeRotate(forwardVec, shipToPickPos);		
			playerTransformComponent.Attitude = playerRotation;
		}
	}			
	else {
		shipToPickPos.set(0,1,0);
		playerRotation.mult(shipToPickPos);
	}
		
	// shoot a single bullet if left mouse button  is pressed and min time since last
	// shot is reached
	timeSinceLastShot += dt;
	if(hasPickPos && mouse.getButtonPressed("LeftButton") && timeSinceLastShot > playerShotInterval) {
		
		timeSinceLastShot = 0;		
		if(nextShot >= numberOfPlayerShots) {
			nextShot = 0;
		}

		// only shoot if next bullet is available again
		bulletId = bullets[nextShot][0];
		if(!layerSystem.getComponent(bulletId).Visible) {
			bullets[nextShot][1] = 0;
			// move to player position
			mtcomp = transformSystem.getComponent(bulletId);
			mtcomp.Position = playerTranslation;
			
			// make shot move in shooting direction
			mcomp = movementSystem.getComponent(bulletId);
			mcomp.Velocity = new Vec2(shipToPickPos[0] * shotSpeed, shipToPickPos[1] * shotSpeed);
			layerSystem.getComponent(bulletId).Visible = true;
			collisionSystem.getComponent(bulletId).Enabled = true;
			
			++nextShot;
		}		
	}	
}

//////////////////////////////////////////////////////////////////////////////

function updateMonsters(dt) {
	// make monsters reappear
	for(i = 0; i < numberOfMonsters; ++i) {
		monsterid = monsters[i][0];
		layerComp = layerSystem.getComponent(monsterid);
		mtcomp = transformSystem.getComponent(monsterid);
			
		if(!layerComp.Visible) {
			
			monsters[i][1] -= dt;
			if(monsters[i][1] < 0) {
				layerComp.Visible = true;
				randAngle = getRandom(-3.1415, 3.1415);
				tmpQuat = new Quat();
				tmpQuat.makeRotate(randAngle, new Vec3(0,0,1));
				spawnPoint = new Vec3(0, monsterDistance, 0);
				tmpQuat.mult(spawnPoint);
				spawnPoint.add(playerTranslation);
				mtcomp.Position = spawnPoint;
				//debugDrawManager.addLine(playerTranslation, spawnPoint, new Vec4(1,1,0,1), 1, 10);
				collisionSystem.getComponent(monsterid).Enabled = true;
			}			
		}
		
		if(layerComp.Visible) {
			
			force = new Vec2(playerTranslation[0], playerTranslation[1]).subtract(mtcomp.Position);
			force.normalize();
			diagonal = new Vec2(force[1], -force[0]);
			force.mult(monsterForce);
			diagonal.mult(20 * Math.sin(monsterid));
			force.add(diagonal);
			movementSystem.getComponent(monsterid).Force = force;
			
			debugDrawManager.addSphere(mtcomp.Position, collisionSystem.getComponent(monsterid).Radius, new Vec4(0,1,0,1));
			
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
timeSinceLastBulletCheck = 0;
function mainLoop(msgtype, params) {
	dt = params["DeltaSimTime"];
	
	// I could optimize the updateBullet method, but why bother
	timeSinceLastBulletCheck += dt;
	if(timeSinceLastBulletCheck > 1) 
	{
		timeSinceLastBulletCheck = 0;
		updateBullets(dt);
	}
	updatePlayer(dt);
	updateMonsters(dt);
}
	
//////////////////////////////////////////////////////////////////////////////

function UpdateCamera(msgtype, params) {
	cameraTransform.set(playerTransformComponent.Position);
	cameraTransform.add(cameraOffset);
	camera.setTransformByLookat(cameraTransform, playerTransformComponent.Position, upVector);
	
	debugDrawManager.addLine(pickPos, new Vec3(pickPos[0], pickPos[1], 1), new Vec4(0,1,0,1), 1);
}
	
function OnPlayerCollideWithMonster()
{
	println("ARRRGH!");
}

function OnMonsterShot(monsterId, bulletId)
{
	collisionSystem.getComponent(monsterId).Enabled = false;
	layerSystem.getComponent(monsterId).Visible = false;
	
	collisionSystem.getComponent(bulletId).Enabled = false;
	layerSystem.getComponent(bulletId).Visible = false;
}

function OnCollision(msgtype, params) {

	group1 = params["Group1"];
	group2 = params["Group2"];
	if(group1 == "Monsters" && group2 == "Player" || group2 == "Monsters" && group1 == "Player") {
		OnPlayerCollideWithMonster();
	}
	else if(group1 == "Monsters" && group2 == "Bullets") {
		OnMonsterShot(params["Entity1"], params["Entity2"]);
	}
	else if(group2 == "Monsters" && group1 == "Bullets") {
		OnMonsterShot(params["Entity2"], params["Entity1"]);
	}
}

EntityManager.registerForMessages("TickMessage", mainLoop);
EntityManager.registerForMessages("EndOfFrameMessage", UpdateCamera);
EntityManager.registerForMessages("CollisionMessage", OnCollision);
