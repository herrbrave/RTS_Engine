registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 1,
	UI = 1,
	SOUND = 1,
}

function setup()
	print("setup player ship", entityId)
	setTexture("Assets/KenneyJam/player_ship.png", entityId, 75, 99, 0, 0, 75, 99)
	setCircleCollision(entityId, 35)
	addParticle(entityId, "Assets/KenneyJam/Effects/yellowCloud1.json", 1500, 150, -1, 0, 16, 16, 120)
	enableMouseMove(entityId)

	boostProgressLabel = createLabel("BOOST:", 24, 50, 30)
	boostProgress = createProgress(170, 30, 150, 25, 100, 90)

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	keys[SDLK_SPACE] = false
	angle = 0
	accel = 500
	maxSpeed = 700
	vel = Vector2f.new(0, 0)
	position = getPosition(entityId)
	moveDirection = 0
 
 	boost = 0
 	boostRocket = 0
 	boostCurrent = 100
 	boostMax = 100
 	boostState = "CHARGING"
 	boostChargeStep = 250
 	boostChargeCurrent = 0

	sorted = false
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	screenPos = getScreenPosition(entityId)
	shipPos = getPosition(entityId)

	mousePos = Vector2f.new(x, y)
	mousePos:subtract(screenPos)

	angle = math.atan(mousePos:getY(), mousePos:getX())
	setAngle(entityId, angle * (180.0 / math.pi))

	lazerVel = Vector2f.new(math.cos(angle), math.sin(angle))
	lazerVel:scale(50)
	shipPos:add(lazerVel)
	lazerVel:normalize()
	bullet = createPhysics(math.floor(shipPos:getX()), math.floor(shipPos:getY()), 37, 9)
	setTexture("Assets/KenneyJam/Objects/laser.png", bullet, 37, 9, 0, 0, 37, 9)
	setCircleCollision(bullet, 5)
	setVelocity(bullet, lazerVel:getX(), lazerVel:getY())
	setSpeed(bullet, 400)
	setScript(bullet, "Games/KenneyJam/laser.lua")
	setAngle(bullet, angle * (180.0 / math.pi))
	setTag(bullet, "bullet")
	setZOrder(bullet, 10)

	shipPos = nil
	screenPos = nil
	mousPos = nil
	lazerVel = nil
end

function onKeyDown(keyId, ctrl, shft)
	keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	keys[keyId] = false
end

-- Entity Mouse Events

function onMouseEnterEntity()

end

function onMouseExitEntity()

end

function onClickEntity(button)

end

function onDragEntity(button)

end

-- Mouse Move Events --
function onMouseMove(x, y, button)
	shipPos = getScreenPosition(entityId)

	mousePos = Vector2f.new(x, y)
	mousePos:subtract(shipPos)

	angle = math.atan(mousePos:getY(), mousePos:getX())
	setAngle(entityId, angle * (180.0 / math.pi))

	grav = Vector2f.new(math.cos(angle), math.sin(angle))
	grav:scale(-100.0)


	addParticle(entityId, "Assets/KenneyJam/Effects/yellowCloud1.json", 1500, 150, grav:getX(), grav:getY(), 16, 16, 120 + boostRocket)
	mousePos = nil
	camPos = nil
	shipPos = nil
	grav = nil
end

-- Collition Callback --

function onCollision(id)
	setPosition(entityId, position:getX(), position:getY())
	vel:setX(0.0)
	vel:setY(0.0)
end

function update(delta)
	step = delta / 1000.0
	if sorted == false then
		setZOrder(entityId, 10)
		setParticleZOrder(entityId, 9)
		sorted = true
	end

	shipPos = getPosition(entityId)
	setCameraPosition(math.floor(shipPos:getX()), math.floor(shipPos:getY()))

	if keys[SDLK_SPACE] and boostCurrent > 0 then
		boost = 800
		accel = 700
		boostRocket = 45
		boostState = "BOOSTING"
	else
		boost = math.max(0, boost - 400 * step)
		accel = 500
		boostRocket = 0
		boostState = "CHARGING"
	end

	updateBoostState(delta)

	if keys[SDLK_w] then
		playParticle(entityId)
	else
		stopParticle(entityId)
	end
end

function onPhysics(delta)
	step = delta / 1000.0

	if keys[SDLK_w] then
		moveDirection = 1.0
	else
		 moveDirection = 0.0
	end

	if moveDirection ~= 0.0 then
		to = Vector2f.new(math.cos(angle), math.sin(angle))
		to:scale(moveDirection)
		to:scale(step * accel)
		vel:add(to)
		vel:truncate(maxSpeed + boost)
		to = nil
	end

	norm = moveAndSlide(entityId, vel:getX() * step, vel:getY() * step)
	norm = nil
end

-- Custom Functions --

function updateBoostState(delta)

	boostChargeCurrent = boostChargeCurrent + delta

	if boostChargeCurrent > boostChargeStep then
		if boostState == "BOOSTING" then
			boostCurrent = math.max(0, boostCurrent - 10)
		else 
			boostCurrent = math.min(boostMax, boostCurrent + 10)
		end

		boostChargeCurrent = boostChargeCurrent - boostChargeStep
	end

	setProgress(boostProgress, boostCurrent, boostMax)
end