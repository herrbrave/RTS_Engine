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
	print("setup RockSpider", tostring(entityId))

	setCircleCollision(entityId, 150)
	setTag(entityId, "ROCK_SPIDER")

	include("Games/Delza/RockSpiderState.lua")

	context = {}

	moveState = IdleState.new(context)
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

end

function update(delta)
	stateMachine.update(delta)
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	return false
end

function onPhysics(delta)
	stateMachine.onPhysics(delta)
end