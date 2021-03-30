registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0
}

function setup()

end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	print("onMouseUp", x, y, button)
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
end

function onMouseExitEntity()
	print("onMouseExit");
end

function onClickEntity(button)
	print("onClickEntity", button);
end

function onDragEntity(button)
	print("onDragEntity", button);
end

-- Collition Callback --

function onCollision(id)
	tag = getTag(id)
	if tag == "rock" then
		destroyEntity(entityId)
	end
end

function update(delta)
	pos = getPosition(entityId)
	if pos:getX() < -50 or pos:getX() > 850 or pos:getY() < -50 or pos:getY() > 650 then
		destroyEntity(entityId)
	end
end