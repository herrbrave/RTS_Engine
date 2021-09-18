registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 1,
}

function setup()
	include("Games/behaviors/Paths.lua")
	attachAsepriteAnimationSet(entityId, "Assets/test/Sprites/Sprites/Peon.json")
	setAnimation(entityId, "idle_down")
	loopAnimation(entityId)

	speed = 325

	tileX = -1	
	tileY = -1

	animationState = "IDLE"
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

end

-- Collition Callback --

function onCollision(id)
end

function update(delta)
	playerPos = getPosition(entityId)
	setCameraPosition(math.floor(playerPos:getX()), math.floor(playerPos:getY()))
end

function onPhysics(delta)
	updatePath(entityId, speed, delta)
end

-- Custom Functions --