registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
	SOUND = 0,
}


function setup()
	print("setup Zombie", tostring(entityId))

	include("Games/OhNoZombies/OnNoZombiesStates.lua")
	setZOrder(entityId, 11)
	setCircleCollision(entityId, 128)

	context = {}
	context.stateMachine = StateMachine.new()
	context.zombieIdleState = ZombieIdleState.new(context)
	context.zombieSearchMoveState = ZombieSearchMoveState.new(context)
	context.zombieAttackMoveState = ZombieAttackMoveState.new(context)
	context.zombieAttackState = ZombieAttackState.new(context)
	context.stateMachine.pushState(context.zombieIdleState)
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

-- Collition Callback --

function onCollision(id)
	context.stateMachine.onCollision(id)
end

function update(delta)
	context.stateMachine.update(delta)
end

function onMessage(message, value)
	context.stateMachine.onMessage(message, value)
	return false
end

function onBroadcast(message, value)
	context.stateMachine.onBroadcast(message, value)
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end