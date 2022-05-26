registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
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
	print("setup Assistant", tostring(entityId))

	setTag(entityId, "PLAYER")
	setAABBCollision(entityId, 16,16)

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	keyChange = false

	include("Games/Delza/PlayerMoveState.lua")

	context = {}
	context.velocity = Vector2f.new(0, 0)
	context.hurtVector = Vector2f.new(0, 0)
	context.accelerate = 1200
	context.speed = 220
	context.friction = 2400
	context.box = 0

	moveState = BasicWASDMoveState.new(context)
	stateMachine = StateMachine.new()
	stateMachine.pushState(moveState)

	context.stateMachine = stateMachine
end

-- Standard Mouse/Key events

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
	stateMachine.update(delta)


end

function onMessage(message, value)

	return false
end

function onBroadcast(message, value)

end

function onPhysics(delta)
	stateMachine.onPhysics(delta)
	camPos = getCameraPosition()
	playerPos = getPosition(entityId)
	playerPos:subtract(camPos)

	if playerPos:getX() > 1024 then
		broadcastMessage(entityId, "OFF_SCREEN", "RIGHT")
	elseif playerPos:getX() < 0 then
		broadcastMessage(entityId, "OFF_SCREEN", "LEFT")
	elseif playerPos:getY() > 768 then
		broadcastMessage(entityId, "OFF_SCREEN", "DOWN")
	elseif playerPos:getY() < 0 then
		broadcastMessage(entityId, "OFF_SCREEN", "UP")
	end
end