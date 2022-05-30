registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 1,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
	SOUND = 1,
}


function setup()
	print("setup Torch", tostring(entityId))

	setCircleCollision(entityId, 24)
	setTag(entityId, "TORCH")
	setZOrder(entityId, 11)
	charges = 3

	include("Games/Potion 'Sploder/TorchState.lua")

	context = {}

	context.keys = {}
	context.keys[SDLK_1] = false
	context.keyChange = false

	context.stateMachine = StateMachine.new()
	context.stateMachine.pushState(TorchSpawnState.new(context))

	loadSound("Assets/Potion 'Sploder/Sounds/torch_detonate.wav", "torch_detonate")
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	if context.keys[keyId] == false then
		context.keyChange = true
	end
	context.keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	if context.keys[keyId] == true then
		context.keyChange = true
	end
	context.keys[keyId] = false
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
		if context.idle_active then
			sendMessage(id, "TORCH", tostring(entityId))
		end
	end
end

function update(delta)
	context.stateMachine.update(delta)
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	if message == "PICK_UP" then
		context.onPickup()
	end

	return false
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end