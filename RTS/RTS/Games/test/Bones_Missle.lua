registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
}


function setup()
	print("setup Bones Missle", tostring(entityId))

	setCircleCollision(entityId, 24)
	setTag(entityId, "BONES_MISSLE")
	setZOrder(entityId, 11)

	top = Vector2f.new(512, 0)
	middle = Vector2f.new(512, 380)
	pos = getPosition(entityId)
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

end

function onDragEntity(button)

end

-- Mouse Move Events --
function onMouseMove(x, y, button)

end

-- Collition Callback --

function onCollision(id)
	if getTag(id) == "POTION" then
		destroyEntity(entityId)
	end
end

function update(delta)
	t = spawn_time / SPAWN_DURATION
	t_squared = t * t
	t_inverse_squared = (1.0 - t) * (1.0 - t)

	px =  t_inverse_squared * pos:getX() + (2 * t * top:getX() - 2 * t_squared * top:getX()) + t_squared * middle:getX()
	py =  t_inverse_squared * pos:getY() + (2 * t * top:getY() - 2 * t_squared * top:getY()) + t_squared * middle:getY()

	setPosition(entityId, px, py)
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	return false
end

function onPhysics(delta)

end