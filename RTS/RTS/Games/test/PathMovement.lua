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
	include("Games/behaviors/Paths.lua")

	pos = getPosition(entityId)
	currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
	speed = 325

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
	constructPath(pathRef)

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
	if isThereAPath() then
		pos = getPosition(entityId)
		currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
		sx = currentTileCoords:getX()
		sy = currentTileCoords:getY()

		tileCoords = getTileCoordinatesAtPoint(getEndOfPath())
		ex = tileCoords:getX()
		ey = tileCoords:getY()

		pathRef = getPath(sx, sy, ex, ey)
		constructPath(pathRef)

		pos = nil
		currentTileCoords = nil
		tileCoords = nil
	end
end

function update(delta)
	playerPos = getPosition(entityId)
	setCameraPosition(math.floor(playerPos:getX()), math.floor(playerPos:getY()))
end

function onPhysics(delta)
	updatePath(entityId, speed, delta)
end

-- Custom Functions --