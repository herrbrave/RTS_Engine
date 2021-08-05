registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}



function setup()
	print("setup hero", entityId)

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	speed = 125.0
	vel = Vector2f.new(0, 0)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

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

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end

function onPhysics(delta)
	if keys[SDLK_w] then
		vel:setY(-1.0)
	elseif keys[SDLK_s] then
		vel:setY(1.0)
	else
		vel:setY(0.0)
	end

	if keys[SDLK_a] then
		vel:setX(-1.0)
	elseif keys[SDLK_d] then
		vel:setX(1.0)
	else
		vel:setX(0.0)
	end

	vel:normalize()
	vel:scale(speed * (delta / 1000.0))

	moveAndSlide(entityId, vel:getX(), vel:getY())
end

-- Custom Functions --