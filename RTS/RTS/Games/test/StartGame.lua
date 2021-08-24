registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 1,
}

function setup()
	print("Title Setup")

	setText(entityId, "Play Game", 25, 255, 255, 255)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

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
	setColor(entityId, 0, 0, 255, 255)
end

function onMouseExitEntity()
	setColor(entityId, 255, 255, 255, 255)
end

function onClickEntity(button)
	if button == 0 then
		loadWorld("Assets/test/MenuTestLevel.json")
	end
end

function onDragEntity(button)

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end

function onPhysics(delta)

end