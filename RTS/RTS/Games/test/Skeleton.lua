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
	print("setup Skeleton", tostring(entityId))

	setCircleCollision(entityId, 150)
	setTag(entityId, "SKELETON")
	setZOrder(entityId, 11)

	top = Vector2f.new(512, 0)
	middle = Vector2f.new(512, 280)
	pos = getPosition(entityId)

	SPAWNING = 0
	IDLE = 1
	ATTACK = 2

	SPAWN_DURATION = 550.0

	state = SPAWNING
	spawn_time = 0

	idle_pattern = {}
	idle_pattern[0] = Vector2f.new(700, 280)
	idle_pattern[1] = Vector2f.new(700, 480)
	idle_pattern[2] = Vector2f.new(324, 480)
	idle_pattern[3] = Vector2f.new(324, 280)
	idle_target = 0

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
	tag = getTag(id)
	if tag == "PLAYER" and state == IDLE then
		target = id
		state = ATTACK
	end
end

function update(delta)
	if state == SPAWNING then
		if spawn_time >= SPAWN_DURATION then
			setPosition(entityId, middle:getX(), middle:getY())
			state = IDLE
		else
			t = spawn_time / SPAWN_DURATION
			t_squared = t * t
			t_inverse_squared = (1.0 - t) * (1.0 - t)

			px =  t_inverse_squared * pos:getX() + (2 * t * top:getX() - 2 * t_squared * top:getX()) + t_squared * middle:getX()
			py =  t_inverse_squared * pos:getY() + (2 * t * top:getY() - 2 * t_squared * top:getY()) + t_squared * middle:getY()

			setPosition(entityId, px, py)
		end

		spawn_time = spawn_time + delta
	elseif state == IDLE then

		position = getPosition(entityId)
		move_dir = Vector2f.new(idle_pattern[idle_target]:getX() - position:getX(), idle_pattern[idle_target]:getY() - position:getY())
		mag = move_dir:magnitude()
		move_dir:normalize()
		move_dir:scale((delta / 1000) * 100)

		if mag <= move_dir:magnitude() then
			setPosition(entityId, move_dir:getX() + position:getX(), move_dir:getY() + position:getY())
			idle_target = (idle_target + 1) % 4
		else
			moveAndSlide(entityId, move_dir:getX(), move_dir:getY())
		end
	elseif state == ATTACK then
		collisions = checkCollisions(entityId)
		still_in_range = false
		for index=0,collisions:size()-1 do
			if collisions:at(index) == target then
				still_in_range = true
				break
			end
		end

		if still_in_range == false then
			state = IDLE
			return
		end

		position = getPosition(entityId)
		target_position = getPosition(target)
		move_dir = Vector2f.new(target_position:getX() - position:getX(), target_position:getY() - position:getY())
		if move_dir:magnitude() <= 45 then
			sendMessage(target, "HURT", tostring(entityId))
		end

		move_dir:normalize()
		move_dir:scale((delta / 1000) * 185)
		moveAndSlide(entityId, move_dir:getX(), move_dir:getY())
	end
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	if message == "DETONATE" then
		destroyEntity(entityId)
	end

	return false
end

function onPhysics(delta)

end