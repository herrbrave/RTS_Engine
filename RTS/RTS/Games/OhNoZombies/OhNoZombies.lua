registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
	SOUND = 0,
}


function setup()
	print("setup OhNoZombies", tostring(entityId))
	enableInputListeners(entityId)
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	if button == MOUSE_BUTTON_LEFT then
		local id = createDefault(x, y, 16, 16, 255, 255, 255, 255)
		setScript(id, "Games/OhNoZombies/Human.lua")
	elseif button == MOUSE_BUTTON_RIGHT then
		local id = createDefault(x, y, 16, 16, 255, 255, 255, 255)
		setScript(id, "Games/OhNoZombies/Zombie.lua")
	end
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
end

function onMessage(message, value)
	return false
end

function onBroadcast(message, value)
end

function onPhysics(delta)
end