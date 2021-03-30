registrar = {
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	UI = 0
}
angle = 0.0
leftDown = false
rightDown = false
upDown = false
function setup()

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
	if keyId == SDLK_LEFT then
		leftDown = true
	elseif keyId == SDLK_RIGHT then
		rightDown = true
	elseif keyId == SDLK_UP then
		upDown = true
	end
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);
	if keyId == SDLK_LEFT then
		leftDown = false
	elseif keyId == SDLK_RIGHT then
		rightDown = false
	elseif keyId == SDLK_UP then
		upDown = false
	elseif keyId == SDLK_SPACE then
		pos = getPosition(entityId)
		angle = getAngle(entityId)
		rads = (angle / 180.0) * math.pi
		vel = Vector2f.new(math.cos(rads), math.sin(rads))
		vel:scale(45)
		pos:add(vel)
		vel:normalize()
		print("pos", pos:getX(), pos:getY())
		bullet = createTextured("laserRed", math.floor(pos:getX()), math.floor(pos:getY()), 37, 9, 0, 0, 37, 9, true)
		setVelocity(bullet, vel:getX(), vel:getY())
		setSpeed(bullet, 300)
		setScript(bullet, "Games/asteroids/bullet.lua")
		setAngle(bullet, angle)
		setTag(bullet, "bullet")
	end
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

function onCollision(id)
	tag = getTag(id)
	print("Collsion ", id, tag)
end

function update(delta)

	if rightDown then
		angle = (360.0 + angle + 5.0) % 360.0
		setAngle(entityId, angle)
	elseif leftDown then
		angle = (360.0 + angle - 5.0) % 360.0
		setAngle(entityId, angle)
	elseif upDown then
		rads = (angle / 180.0) * math.pi
		vel = Vector2f.new(math.cos(rads), math.sin(rads))
		vel:normalize()
		addVelocity(entityId, vel:getX(), vel:getY())
		setSpeed(entityId, 100)
	end

	pos = getPosition(entityId)
	if pos:getX() < -50 or pos:getX() > 850 or pos:getY() < -50 or pos:getY() > 650 then
		center = Vector2f.new(400.0, 300.0)
		pos:subtract(center)
		pos:scale(-1.0)
		pos:add(center)
		setPosition(entityId, pos:getX(), pos:getY())
	end

end