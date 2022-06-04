registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
	SOUND = 0,
}


function setup()
	print("setup ALIEN", tostring(entityId))

	setTag(entityId, "ALIEN")

	include("Games/Alienators/alien_state.lua")

	context.stateMachine = StateMachine.new()
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
end