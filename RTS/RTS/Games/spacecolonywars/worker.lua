registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
}

selected = false
mouseOver = false

function setup()
	print("setup worker", entityId)
	setProgress(entityId, "top", 80, 100)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	print("onMouseUp", x, y, button)
	if selected and button == MOUSE_BUTTON_RIGHT then
		cameraPos = getCameraPosition()
		mousePos = Vector2f.new(x, y)
		cameraPos:add(mousePos)
		setTarget(entityId, cameraPos:getX(), cameraPos:getY(), 32)
		setSpeed(entityId, 125)
	elseif button == MOUSE_BUTTON_LEFT and not mouseOver then
		selected = false
	end
end

function onMouseDown(x, y, button)
	print("onMouseDown", x, y , button)
end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);
end

-- Entity Mouse Events

function onMouseEnterEntity()
	print("onMouseEnter");
	mouseOver = true
end

function onMouseExitEntity()
	print("onMouseExit");
	mouseOver = false
end

function onClickEntity(button)
	print("onClickEntity", button);
	selected = true
end

function onDragEntity(button)
	print("onDragEntity", button);
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end