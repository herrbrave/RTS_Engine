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
	print("setup Bones", tostring(entityId))

	setCircleCollision(entityId, 24)
	setTag(entityId, "BONES")
	setZOrder(entityId, 11)

	top = Vector2f.new(512, 0)
	middle = Vector2f.new(512, 280)
	pos = getPosition(entityId)

	SPAWNING = 0
	IDLE = 1

	SPAWN_DURATION = 550.0

	state = SPAWNING
	spawn_time = 0

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
	if getTag(id) == "PLAYER" and state == IDLE then
		sendMessage(id, "ADD_INVENTORY", "BONES")
		destroyEntity(entityId)
	end
end

function update(delta)
	if state == SPAWNING then
		if spawn_time >= SPAWN_DURATION then
			setPosition(entityId, middle:getX(), middle:getY())
			state = IDLE
			hover_value = 0
		else
			t = spawn_time / SPAWN_DURATION
			t_squared = t * t
			t_inverse_squared = (1.0 - t) * (1.0 - t)

			px =  t_inverse_squared * pos:getX() + (2 * t * top:getX() - 2 * t_squared * top:getX()) + t_squared * middle:getX()
			py =  t_inverse_squared * pos:getY() + (2 * t * top:getY() - 2 * t_squared * top:getY()) + t_squared * middle:getY()

			setPosition(entityId, px, py)
		end

		spawn_time = spawn_time + delta
	else
		prog = (hover_value / 1000.0) * 2 * math.pi
		offset = math.cos(prog) * 10.0

		setPosition(entityId, middle:getX(), middle:getY() + offset)

		hover_value = (hover_value + delta) % 1000
	end
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	return false
end

function onPhysics(delta)

end