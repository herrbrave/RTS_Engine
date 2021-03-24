
registrar = {
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1
}

function setup()
	print("setup");

	temp = createDefault(100, 100, 50, 50, 255, 0, 255, 255)
	setTag(temp, "block")
	setScript(temp, "block.lua")
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)
	
end

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

-- Collition Callback --

function onCollision(id)
	tag = getTag(id)
	print("Collsion ", id, tag)
end

function update(delta) 

end
