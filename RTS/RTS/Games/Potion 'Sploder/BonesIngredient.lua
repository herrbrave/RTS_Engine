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
	setTag(entityId, "BONES_INGREDIENT")
	setZOrder(entityId, 11)

	include("Games/Potion 'Sploder/BonesState.lua")

	context = {}

	context.stateMachine = StateMachine.new()
	context.stateMachine.pushState(BonesThrowState.new(context))

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
	context.stateMachine.update(delta)
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	return false
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end