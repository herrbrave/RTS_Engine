registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()

end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	print("onMouseDown", x, y , button)
end

function onKeyDown(keyId, ctrl, shft)

end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);
end

-- Entity Mouse Events

function onMouseEnterEntity()
	print("onMouseEnter");
end

function onMouseExitEntity()
	print("onMouseExit");
end

function onClickEntity(button)
end

function onDragEntity(button)
	print("onDragEntity", button);
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

	collisions = checkCollisions(entityId)
	if collisions:size() > 0 then
		setColor(entityId, 255, 0, 0, 255)
		print("Collision!")
	else
		setColor(entityId, 255, 255, 255, 255)
	end
end