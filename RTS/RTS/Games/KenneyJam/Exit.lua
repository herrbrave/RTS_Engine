registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 1,
}

function setup()
	setTag(entityId, "EXIT")
	setCircleCollision(entityId, 200)
	setText(entityId, "EXIT", 20, 255, 255, 255)
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

end

-- Collition Callback --

function onCollision(id)
	loadWorld("Assets/KenneyJam/KenneyJam2.json")
end

function update(delta)

end

-- Custom Functions --