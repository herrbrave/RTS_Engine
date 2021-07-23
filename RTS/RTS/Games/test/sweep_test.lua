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

	ent1 = createDefault(500, 500, 100, 100, 255, 255, 255, 255)
	ent2 = createDefault(100, 200, 300, 100, 255, 255, 255, 255)
	ent3 = createDefault(100, 600, 500, 50, 255, 255, 255, 255)
	ent4 = createDefault(200, 100, 100, 200, 255, 255, 255, 255)

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	keyChange = false
	setSpeed(entityId, 125)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

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
	if keyChange then
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
		keyChange = false
	end
end

-- Custom Functions --