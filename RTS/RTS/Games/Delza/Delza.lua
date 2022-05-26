registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
}


function setup()
	print("setup Delza Game", tostring(entityId))

	sceneX = 0
	sceneY = 0
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)

	if keyId == SDLK_RIGHT then
		sceneX = sceneX + 1
		moveCamera()
	elseif keyId == SDLK_DOWN then
		sceneY = sceneY + 1
		moveCamera()
	elseif keyId == SDLK_LEFT then
		sceneX = sceneX - 1
		moveCamera()
	elseif keyId == SDLK_UP then
		sceneY = sceneY - 1
		moveCamera()
	end
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

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end

function onBroadcast(message, value)
	if message == "OFF_SCREEN" then
		if value == "RIGHT" then
			sceneX = sceneX + 1
			moveCamera()
		elseif value == "LEFT" then
			sceneX = sceneX - 1
			moveCamera()
		elseif value == "UP" then
			sceneY = sceneY - 1
			moveCamera()
		elseif value == "DOWN" then
			sceneY = sceneY + 1
			moveCamera()
		end
	end
end

function onMessage(message, value)

	return false
end

function onPhysics(delta)

end

function moveCamera()
	x = (sceneX * 16 * 64) + (32 * 16)
	y = (sceneY * 16 * 48) + (24 * 16)

	print(tostring(x), tostring(y))
	setCameraPosition(x, y)
end