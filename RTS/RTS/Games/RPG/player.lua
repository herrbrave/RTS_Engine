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

selected = false
mouseOver = false

function setup()
	print("setup player", entityId)
	setTag(entityId, "player")

	attachAnimationSet(entityId, "Assets/RPG/Character/Char_one/animation_set.json")

	pos = getPosition(entityId)
	setCameraPosition(pos:getX() - 256, pos:getY() - 256)

	direction = "RIGHT"
	setAnimation(entityId, "idle_right")
	loopAnimation(entityId)

	isAttacking = false

	dx = 0
	dy = 0
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	print("Keydown")
	if keyId == SDLK_UP then
		dy = -1
		direction = "UP"
		if not isAttacking then
			setAnimation(entityId, "idle_up")
			loopAnimation(entityId)
		end
	elseif keyId == SDLK_RIGHT then
		dx = 1
		direction = "RIGHT"
		if not isAttacking then
			setAnimation(entityId, "idle_right")
			loopAnimation(entityId)
		end
	elseif keyId == SDLK_DOWN then
		dy = 1
		direction = "DOWN"
		if not isAttacking then
			setAnimation(entityId, "idle_down")
			loopAnimation(entityId)
		end
	elseif keyId == SDLK_LEFT then
		dx = -1
		direction = "LEFT"
		if not isAttacking then
			setAnimation(entityId, "idle_left")
			loopAnimation(entityId)
		end
	elseif keyId == SDLK_SPACE then
		if not isAttacking then
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

function onKeyUp(keyId, ctrl, shft)
	print("Keyup")
	if keyId == SDLK_UP then
		dy = 0
	elseif keyId == SDLK_RIGHT then
		dx = 0
	elseif keyId == SDLK_DOWN then
		dy = 0
	elseif keyId == SDLK_LEFT then
		dx = 0
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

end

function update(delta)
	pos = getPosition(entityId)
	-- setCameraPosition(pos:getX(), pos:getY())

	-- setVelocity(entityId, dx, dy)
	-- setSpeed(entityId, 300)
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