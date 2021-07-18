registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 1,
	UI = 0,
}



function setup()
	print("setup hero", entityId)
	setTag(entityId, "hero")
	enableMouseMove(entityId)
	attachAsepriteAnimationSet(entityId, "Assets/HackNSlasher/Characters/Humanoid/Colt/colt.json")

	weapon = createPhysics(0, 0, 32, 32)
	attachAsepriteAnimationSet(weapon, "Assets/HackNSlasher/Characters/Humanoid/Attack FX/Long Slash/great_sword_long_slash.json")
	setChild(entityId, weapon)

	direction = "RIGHT"
	setAnimation(entityId, "idle_right")
	loopAnimation(entityId)

	isAttacking = false

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	if button == 0 then
		if not isAttacking then
			playAnimation(weapon);
			isAttacking = true
			if direction == "UP" then
				setAnimation(entityId, "attack_up")
			elseif direction == "RIGHT" then
				setAnimation(entityId, "attack_right")
			elseif direction == "DOWN" then
				setAnimation(entityId, "attack_down")
			elseif direction == "LEFT" then
				setAnimation(entityId, "attack_left")
			end
			playAnimation(entityId)
		end
	end
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
	camPos = getCameraPosition()
	heroPos = getPosition(entityId)

	mx = x + camPos:getX()
	my = y + camPos:getY()

	mousePos = Vector2f.new(mx, my)
	mousePos:subtract(heroPos)

	angle = math.atan(mousePos:getY(), mousePos:getX()) * (180.0 / math.pi)
	setAngle(weapon, angle)

	if (angle >= 0 and angle <= 45) or (angle <= 0 and angle >= -45) then -- right
		direction = "RIGHT"
		if not isAttacking then
			setAnimation(entityId, "idle_right")
			loopAnimation(entityId)
		end
	elseif angle > -135 and angle < -45 then -- up
		direction = "UP"
		if not isAttacking then
			setAnimation(entityId, "idle_up")
			loopAnimation(entityId)
		end
	elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
		direction = "LEFT"
		if not isAttacking then
			setAnimation(entityId, "idle_left")
			loopAnimation(entityId)
		end
	else -- down
		direction = "DOWN"
		if not isAttacking then
			setAnimation(entityId, "idle_down")
			loopAnimation(entityId)
		end
	end
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	dx = 0
	dy = 0

	if keys[SDLK_w] then
		dy = dy - 1
	end
	if keys[SDLK_a] then
		dx = dx - 1
	end
	if keys[SDLK_s] then
		dy = dy + 1
	end
    if keys[SDLK_d] then
		dx = dx + 1
	end

	setVelocity(entityId, dx, dy)
	setSpeed(entityId, 300)
	if isAttacking then 
		if not isAnimationPlaying(entityId) then
			isAttacking = false
			if direction == "UP" then
				setAnimation(entityId, "idle_up")
			elseif direction == "RIGHT" then
				setAnimation(entityId, "idle_right")
			elseif direction == "DOWN" then
				setAnimation(entityId, "idle_down")
			elseif direction == "LEFT" then
				setAnimation(entityId, "idle_left")
			end
			loopAnimation(entityId)
		end
	end
end

-- Custom Functions --