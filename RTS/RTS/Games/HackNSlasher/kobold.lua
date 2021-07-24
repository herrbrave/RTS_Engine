registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
}



function setup()
	print("setup hero", entityId)
	setTag(entityId, "hero")
	attachAsepriteAnimationSet(entityId, "Assets/HackNSlasher/Characters/Humanoid/Reptiles & Kobold/kobold.json")
	setSpeed(entityId, 64);
	setSize(entityId, 64, 64)

	weapon = createPhysics(0, 0, 64, 64)
	attachAsepriteAnimationSet(weapon, "Assets/HackNSlasher/Characters/Humanoid/Reptiles & Kobold/Shortbow 48x48.json")
	setChild(entityId, weapon)
	setSize(weapon, 96, 96)

	direction = "RIGHT"
	setAnimation(entityId, "idle_right")
	loopAnimation(entityId)

	stateTime = 0
	isAttacking = false
	isMoving = false

	math.randomseed(os.time())
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
end

function onKeyDown(keyId, ctrl, shft)

end

function onKeyUp(keyId, ctrl, shft)

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
			if isMoving then
				setAnimation(entityId, "walk_right")
			else
				setAnimation(entityId, "idle_right")
			end
			loopAnimation(entityId)
		end
	elseif angle > -135 and angle < -45 then -- up
		direction = "UP"
		if not isAttacking then
			if isMoving then
				setAnimation(entityId, "walk_up")
			else
				setAnimation(entityId, "idle_up")
			end
			loopAnimation(entityId)
		end
	elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
		direction = "LEFT"
		if not isAttacking then
			if isMoving then
				setAnimation(entityId, "walk_left")
			else
				setAnimation(entityId, "idle_left")
			end
			loopAnimation(entityId)
		end
	else -- down
		direction = "DOWN"
		if not isAttacking then
			if isMoving then
				setAnimation(entityId, "walk_down")
			else
				setAnimation(entityId, "idle_down")
			end
		loopAnimation(entityId)
		end
	end
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	updateIdle(delta)
end

-- Custom Functions --

function updateIdle(delta)
	stateTime = stateTime + delta
	if stateTime > 1500 then

		angle = math.random(-180, 180)

		-- if even idle
		if math.random(2) % 2 == 0 then
			setVelocity(entityId, 0, 0)
			if (angle >= 0 and angle <= 45) or (angle <= 0 and angle >= -45) then -- right
				direction = "RIGHT"
				setAnimation(entityId, "idle_right")
				loopAnimation(entityId)
			elseif angle > -135 and angle < -45 then -- up
				direction = "UP"
				setAnimation(entityId, "idle_up")
				loopAnimation(entityId)
			elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
				direction = "LEFT"
				setAnimation(entityId, "idle_left")
				loopAnimation(entityId)
			else -- down
				direction = "DOWN"
				setAnimation(entityId, "idle_down")
				loopAnimation(entityId)
			end
		else
			rads = angle * (math.pi / 180.0)
			setVelocity(entityId, math.sin(rads), math.cos(rads))
			if (angle >= 0 and angle <= 45) or (angle <= 0 and angle >= -45) then -- right
				direction = "RIGHT"
				setAnimation(entityId, "walk_right")
				loopAnimation(entityId)
			elseif angle > -135 and angle < -45 then -- up
				direction = "UP"
				setAnimation(entityId, "walk_up")
				loopAnimation(entityId)
			elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
				direction = "LEFT"
				setAnimation(entityId, "walk_left")
				loopAnimation(entityId)
			else -- down
				direction = "DOWN"
				setAnimation(entityId, "walk_down")
				loopAnimation(entityId)
			end
		end


		stateTime = 0
	end
end