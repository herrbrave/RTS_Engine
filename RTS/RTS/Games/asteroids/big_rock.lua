registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 0,
	SCRIPT = 1,
	ASSET = 0
}

ship = 0
lastRockGenerated = 0
turnRight = false
angle = 0
metreors = {
	"meteorBrown_med1",
	"meteorBrown_med2"
}

function setup()
	if math.random(100) > 50 then
		turnRight = true
	else
		turnRight = false
	end
end 

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	print("onMouseUp", x, y, button)
end

function onMouseDown(x, y, button)
	print("onMouseDown", x, y , button)
end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);
end

-- Entity Mouse Events

function onMouseEnterEntity()
	print("onMouseEnter");
end

function onMouseExitEntity()
	print("onMouseExit");
end

function onClickEntity(button)
	print("onClickEntity", button);
end

function onDragEntity(button)
	print("onDragEntity", button);
end

-- Collition Callback --

function createExplosion()
	pos = getPosition(entityId)
	explosion = createAnimated("Assets/asteroids/explosion_animation.json", math.floor(pos:getX()), math.floor(pos:getY()), 45, 40, 0, 0, 45, 40, true)
	setScript(explosion, "Games/asteroids/explosion.lua")
	playAnimation(explosion)
end

function createMediumRock()
	pos = getPosition(entityId)
	angle = math.random(360)
	rads = (angle / 180.0) * math.pi
	vel = Vector2f.new(math.cos(rads), math.sin(rads))
	vel:normalize()
	rock = createTextured(metreors[math.random(2)], math.floor(pos:getX()), math.floor(pos:getY()), 45, 40, 0, 0, 45, 40, true)
	setVelocity(rock, vel:getX(), vel:getY())
	setSpeed(rock, 150)
	setScript(rock, "Games/asteroids/medium_rock.lua")
	setAngle(rock, angle)
	setTag(rock, "rock")
end

function onCollision(id)
	tag = getTag(id)
	if tag == "bullet" then
		createMediumRock()
		createMediumRock()
		createExplosion()
		destroyEntity(entityId)
	end

end

function update(delta)
	if turnRight then
		angle = (360.0 + angle + 5.0) % 360.0
		setAngle(entityId, angle)
	else
		angle = (360.0 + angle - 5.0) % 360.0
		setAngle(entityId, angle)
	end

	pos = getPosition(entityId)
	if pos:getX() < -50 or pos:getX() > 850 or pos:getY() < -50 or pos:getY() > 650 then
		lastTransition = delta
		center = Vector2f.new(400.0, 300.0)
		pos:subtract(center)
		pos:scale(-1.0)
		pos:add(center)
		setPosition(entityId, pos:getX(), pos:getY())
	end
end