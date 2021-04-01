registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1
}

dx = 0
dy = 0

function setup()

end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)
	print("Mouse Move", x, y, button)
	if x >= 790 then
		dx = 10
	elseif x <= 10 then
		dx = -10
	else
		dx = 0
	end

	if y > 590 then
		dy = 10
	elseif y <= 10 then
		dy = -10
	else
		dy = 0
	end
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

end

function update(delta)
	if dx ~= 0 or dy ~= 0 then
		print("Moving Camera By:", dx, dy)
		moveCameraBy(dx, dy)
	end
end