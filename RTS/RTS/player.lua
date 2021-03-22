
registrar = {
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1
}

function setup()
	print("setup");
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)
	print("onMouseUp", x, y, button)
	setTarget(entityId, x, y, 5)
	setSpeed(entityId, 500)
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

function update(delta) 

end
