registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
}

function setup()
	print("setup Character " .. tostring(entityId))
	setTag(entityId, "Character")
	attachAsepriteAnimationSet(entityId, "Assets/AutumnJam/CHARACTERS/ANIMATION/BASE CHARACTER/PNG/WITH_FX/character_animations.json")
	setAABBCollision(entityId, 20, 20)
	setAnimationZOrder(entityId, 11)
	direction = "LEFT"

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	keyChange = false
	velocity = Vector2f.new(0.0, 0.0)
	speed = 180
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	if keys[keyId] == false then
		keyChange = true
	end
	keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	if keys[keyId] == true then
		keyChange = true
	end
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

end

-- Collition Callback --

function onCollision(id)
	
end

function update(delta)
	step = delta / 1000.0
	characterPos = getPosition(entityId)
	setCameraPosition(math.floor(characterPos:getX()), math.floor(characterPos:getY()))

	if keys[SDLK_w] then
		velocity:setY(-1)
	elseif keys[SDLK_s] then
		velocity:setY(1)
	else
		velocity:setY(0)
	end

	if keys[SDLK_a] then
		velocity:setX(-1)
		direction = "LEFT"
	elseif keys[SDLK_d] then
		velocity:setX(1)
		direction = "RIGHT"
	else
		velocity:setX(0)
	end

	velocity:normalize()
	velocity:scale(speed * step)

	if velocity:magnitude() > 0.0 then
		if direction == "RIGHT" then
			setAnimation(entityId, "walk_right")
			loopAnimation(entityId)
		else
			setAnimation(entityId, "walk_left")
			loopAnimation(entityId)
		end
	else
		if direction == "RIGHT" then
			setAnimation(entityId, "idle_right")
			loopAnimation(entityId)
		else
			setAnimation(entityId, "idle_left")
			loopAnimation(entityId)
		end
	end
end

function onPhysics(delta)
	if velocity:magnitude() > 0.0 then
		moveAndSlide(entityId, velocity:getX(), velocity:getY())
	end
end

-- Custom Functions --