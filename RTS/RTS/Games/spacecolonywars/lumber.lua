registrar = {
	ASSET = 0,
	DRAWABLE = 1,
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
	WORLD = 1,
}

function setup()
	lumber = 100
	_pos = getPosition(entityId)
	world_coords = getTileCoordinatesAtPoint(_pos:getX(), _pos:getX())

	print("Setup lumber")

	broadcastMessage(entityId, "LUMBER_ID", string.format("%d, %d, %d", entityId, world_coords:getX(), world_coords:getY()))
end

-- Standard Mouse/Key events

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
	if button == 1 and lumber > 0 then
		broadcastMessage(entityId, "HARVEST", string.format("%d, %d, %d", entityId, world_coords:getX(), world_coords:getY()))
	end
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

end