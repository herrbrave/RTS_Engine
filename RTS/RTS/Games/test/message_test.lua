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
	UI = 0,
}

function setup()
	obj1 = createDefault(100, 100, 100, 100, 255, 255, 255, 255)
	setScript(obj1, "Games/test/message_test_1.lua")
	obj2 = createDefault(225, 100, 100, 100, 255, 255, 255, 255)
	setScript(obj2, "Games/test/message_test_2.lua")
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)

	if keyId == SDLK_1 then
		print(sendMessage(obj1, "val", "hello"))
	elseif keyId == SDLK_2 then
		print(sendMessage(obj2, "val", "world"))
	end

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

-- Messasge Callback --

function onMessage(message, value)

	return "Ok"
end

function update(delta)

end