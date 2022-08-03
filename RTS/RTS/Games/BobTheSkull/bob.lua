registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
	SOUND = 0
}


function setup()
	print("setup Bob", tostring(entityId))

	setTag(entityId, "BOB")

	include("Games/BobTheSkull/bob_states.lua")

	context = {}

	context.exploreState = ExploreState.new(context)
	context.stateMachine = StateMachine.new()
	context.stateMachine.pushState(context.exploreState)
	setAnimationZOrder(entityId, 11)
	setSize(entityId, 32, 40)
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

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
	context.stateMachine.onCollision(id)
end

function update(delta)
	context.stateMachine.update(delta)
end

function onMessage(message, value)
	context.stateMachine.onMessage(message, value)
end

function onBroadcast(message, value)
	context.stateMachine.onBroadcast(message, value)
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end