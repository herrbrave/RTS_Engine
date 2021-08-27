registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 1,
	UI = 1,
	WORLD = 1,
}

function setup()
	enableMouseMove(entityId)

	pos = getPosition(entityId)
	currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
	speed = 220
	target = Vector2f.new(0.0, 0.0)

	tileX = currentTileCoords:getX()
	tileY = currentTileCoords:getY()

	pos = nil
	currentTileCoords = nil
	len = 0
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

	pos = getPosition(entityId)
	currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
	sx = currentTileCoords:getX()
	sy = currentTileCoords:getY()

	camPos = getCameraPosition()
	tileCoords = getTileCoordinatesAtPoint(camPos:getX() + x, camPos:getY() + y)
	ex = tileCoords:getX()
	ey = tileCoords:getY()

	pathRef = getPath(sx, sy, ex, ey)
	path = {}
	len = pathRef:size() - 1
	for index=0,len do
		x = pathRef:getX(index)
		y = pathRef:getY(index)
		path[index] = {x, y}
	end
	pathIndex = 0
	pathTime = 0

	pos = nil
	currentTileCoords = nil
	camPos = nil
	tileCoords = nil
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

-- Mouse Move Events --
function onMouseMove(x, y, button)

	camPos = getCameraPosition()
	tileCoords = getTileCoordinatesAtPoint(camPos:getX() + x, camPos:getY() + y)

	if tileX ~= tileCoords:getX() or tileY ~= tileCoords:getY() then
		popTexture(tileX, tileY)
		pushTexture(tileCoords:getX(), tileCoords:getY(), "Assets/test/Sprites/Selected.png", 0, 0, 16, 16)
		tileX = tileCoords:getX()
		tileY = tileCoords:getY()
	end

	camPos = nil
	tileCoords = nil
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end

function onPhysics(delta)
	step = delta / 1000.0
	moveTowards(step)
end

-- Custom Functions --

function moveTowards(step)
	if path == nil or len <= 0 then
		return
	end

	dist = speed * step
	x = path[pathIndex][1]
	y = path[pathIndex][2]

	pos = getPosition(entityId)
	target:setX(x)
	target:setY(y)
	target:subtract(pos)

	moveDist = target:magnitude()

	if moveDist < dist then
		target:normalize()
		target:scale(moveDist)
		moveAndSlide(entityId, target:getX(), target:getY())
		pathIndex = pathIndex + 1
		if pathIndex > len then
			path = nil
			len = 0
		end
	else 
		target:normalize()
		target:scale(dist)
		moveAndSlide(entityId, target:getX(), target:getY())
	end

	pos = nil
end