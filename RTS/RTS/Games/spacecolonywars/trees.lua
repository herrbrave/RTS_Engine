registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()
	print("setup tree", entityId)
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

-- Collition Callback --

function onCollision(id)

end

-- Message endpoint

function update(delta)

end