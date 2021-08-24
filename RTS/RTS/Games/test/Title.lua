registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
}

function setup()
	print("Title Setup")

	setText(entityId, "Mega Miner: King of the Mountain", 40, 255, 255, 255)
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

function update(delta)

end

function onPhysics(delta)

end