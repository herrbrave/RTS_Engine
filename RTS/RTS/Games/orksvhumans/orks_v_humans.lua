registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()
	dx = 0
	dy = 0
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	print("onMouseDown", x, y , button)
end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);

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
	print("onKeyUp", keyId, ctrl, shft);

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
	if dx ~= 0 or dy ~= 0 then
		print("Moving Camera")
		moveCameraBy(dx * 30, dy * 30)
	end
end