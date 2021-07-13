registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
}

direction = "right"
dx = 0
dy = 0
jumping = false
speed = 100

function setup()
	print("setup player", entityId)
	setSpeed(entityId, speed)
	attachAnimationSet(entityId, "Assets/platformer/player.json")
	loopAnimation(entityId)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);

	if keyId == SDLK_RIGHT and not jumping then
		dx = 1
		direction = "right"
		setAnimation(entityId, "walk_right")
	elseif keyId == SDLK_LEFT and not jumping then
		dx = -1
		direction = "left"
		setAnimation(entityId, "walk_right")
	elseif keyID == SDLK_SPACE then
		dy = 1
		jumping = true
		if direction == "right" then
			setAnimation(entityId, "jump_right")
		elseif direction == "left" then
			setAnimation(entityId, "jump_left")
		end
	end
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);

	if keyId == SDLK_RIGHT and not jumping then
		dx = 0
		setAnimation(entityId, "idle_right")
	elseif keyId == SDLK_LEFT and not jumping then
		dx = 0
		setAnimation(entityId, "idle_right")
	elseif keyID == SDLK_SPACE then
		dy = 0
	end
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

-- Collition Callback --

function onCollision(id)
	if jumping then
		jumping = false
		dy = 0;
		if direction == "right" then
			setAnimation(entityId, "jump_right")
		elseif direction == "left" then
			setAnimation(entityId, "jump_left")
		end
	end
end

function update(delta)
	pos = getPosition(entityId)
	setCameraPosition(math.floor(pos:getX()) - 512, math.floor(pos:getY()) - 384)

	vel = getVelocity(entityId)
	vel:setX(dx)
	vel:setY(dy)
	vel:scale(speed)

	if jumping then
		grav = Vector2f.new(0, -10)
		vel:add(grav)
	end

	setVelocity(entityId, vel:getX(), vel:getY())
end