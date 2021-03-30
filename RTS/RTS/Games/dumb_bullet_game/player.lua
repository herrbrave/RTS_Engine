
registrar = {
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	UI = 0
}

function setup()
	print("setup");

	boundaryTop = createDefault(400, 5, 800, 10, 255, 255, 255, 255)
	setTag(boundaryTop, "bound")

	boundaryLeft = createDefault(5, 300, 10, 600, 255, 255, 255, 255)
	setTag(boundaryLeft, "bound")

	boundaryBottom = createDefault(400, 595, 800, 10, 255, 255, 255, 255)
	setTag(boundaryBottom, "bound")

	boundaryRight = createDefault(795, 300, 10, 600, 255, 255, 255, 255)
	setTag(boundaryRight, "bound")

	map = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
		{1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
	}

	offset = 32
	for y=1,13 do
		for x=1,22 do
			if map[y][x] == 1 then
				x0 = 32 * (x - 1) + offset
				y0 = 32 * (y - 1) + offset
				temp = createDefault(x0, y0, 32, 32, 255, 255, 255, 255)
				setTag(temp, "block")
				setScript(temp, "block.lua")
			end
		end
	end
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	print("onMouseUp", x, y, button)

	vecX = x - 400
	vecY = y - 580

	bullet = createDefault(400, 580, 8, 8, 255, 0, 0, 255)
	setScript(bullet, "bullet.lua")
	setVelocity(bullet, vecX, vecY)
	setSpeed(bullet, 350)

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
