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
	UI = 1,
}


selected = false
mouseOver = false
animState = "idle_right"
lastAnimState = "idle_right"

function setup()
	health = 100
	max_health = 100
	print("setup peon", entityId)
	--setProgress(entityId, "top", health, max_health)
	setTag(entityId, "peon")
	attachAnimationSet(entityId, "Assets/orksvhumans/Sprites/peon_animation.json")
	loopAnimation(entityId)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	if selected and button == MOUSE_BUTTON_RIGHT then
		cameraPos = getCameraPosition()
		mousePos = Vector2f.new(x, y)
		cameraPos:add(mousePos)
		setTarget(entityId, cameraPos:getX(), cameraPos:getY(), 32)
		setSpeed(entityId, 10)
	elseif button == MOUSE_BUTTON_LEFT and not mouseOver then
		selected = false
	end
end

function onMouseDown(x, y, button)
end

function onKeyDown(keyId, ctrl, shft)
end

function onKeyUp(keyId, ctrl, shft)
end

-- Entity Mouse Events

function onMouseEnterEntity()
	mouseOver = true
end

function onMouseExitEntity()
	mouseOver = false
end

function onClickEntity(button)
	selected = true
end

function onDragEntity(button)
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	vel = getVelocity(entityId)
	x = vel:getX()
	y = vel:getY()

	speed = getSpeed(entityId)
	if speed ~= 0 then
		moving = true
		change = true
		if x > y then
			if x > 0 then
				animState = "walk_right"
			else 
				direction = "walk_left"
			end
		else 
			if y > 0 then
				direction = "walk_down"
			else 
				direction = "walk_up"
			end
		end
	else 
		if animState == "walk_right" then
			anim_state = "idle_right"
		elseif animState == "walk_left" then
			animState = "idle_left"
		elseif anmState == "down" then
			animState = "idle_down"
		elseif animState == "up" then
			animState = "idle_up"
		end
	end

	

	if animState ~= lastAnimState then
		setAnimation(entityId, animState)
	end

	lastAnimState = animState
end
