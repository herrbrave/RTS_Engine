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
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
}

selected = false
mouseOver = false

function setup()
	setTag(entityId, "skeleman")

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
	if keyId == SDLK_UP then
		dy = -1
	elseif keyId == SDLK_RIGHT then
		dx = 1
	elseif keyId == SDLK_DOWN then
		dy = 1
	elseif keyId == SDLK_LEFT then
		dx = -1
	end
end

function onKeyUp(keyId, ctrl, shft)
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

	setVelocity(entityId, dx, dy)
	setSpeed(entityId, 300)
end