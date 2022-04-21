registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 1,
	UI = 1,
	WORLD = 0,
}

function setup()
	selected = false
	mouseOver = false

	enableMouseMove(entityId)

	NONE = 0
	PLACING = 1

	mouseX = 0
	mouseY = 0

	state = NONE
	newWorker = 0
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)
	if mouseOver == false then
		selected = false
		setColor(entityId, 255, 255, 255, 255)
	end
end

function onMouseDown(x, y, button)
	if button == MOUSE_BUTTON_LEFT and state == PLACING then
		state = NONE
		newWorker = 0
	end
end

function onKeyDown(keyId, ctrl, shft)
	if keyId == SDLK_1 and state == NONE then
		state = PLACING
		createWorker()
	end
end

function onKeyUp(keyId, ctrl, shft)

end

-- Entity Mouse Events

function onMouseEnterEntity()
	mouseOver = true
end

function onMouseExitEntity()
	mouseOver = false
end

function onClickEntity(button)
	if button == MOUSE_BUTTON_LEFT then
		selected = true
		setColor(entityId, 128, 128, 128, 255)
	end
end

function onDragEntity(button)

end

-- Mouse Move Events --
function onMouseMove(x, y, button)
	mouseX = x
	mouseY = y
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	if state == PLACING then
		camPos = getCameraPosition()
		setPosition(newWorker, camPos:getX() + mouseX, camPos:getY() + mouseY)
	end
end

function onBroadcast(message, value)

end

function createWorker()
	camPos = getCameraPosition()
	newWorker = createTextured("Assets/spacecolonywars/Unit/scifiUnit_01.png", camPos:getX() + mouseX, camPos:getY() + mouseY, 64, 64, 0, 0, 64, 64)
	setScript(newWorker, "Games/spacecolonywars/worker.lua")
end