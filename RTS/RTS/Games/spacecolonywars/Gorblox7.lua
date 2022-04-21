registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 1,
}

dx = 0
dy = 0

areaX = 0
areaY = 0

mouseDown = false

function setup()
	print("setup space colony wars", entityId)
	enableMouseMove(entityId)
	selectArea = createDefault(0, 0, 10, 10, 0, 122, 0, 90)
	setZOrder(selectArea, 100)
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)
	if x >= 1014 then
		dx = 10
	elseif x <= 10 then
		dx = -10
	else
		dx = 0
	end

	if y > 758 then
		dy = 10
	elseif y <= 10 then
		dy = -10
	else
		dy = 0
	end

	if mouseDown then
		camPos = getCameraPosition()
		mx = x + camPos:getX()
		my = y + camPos:getY()
		upperLeftX = math.min(areaX, mx)
		upperLeftY = math.min(areaY, my)
		lowerRightX = math.max(areaX, mx)
		lowerRightY = math.max(areaY, my)
		width = lowerRightX - upperLeftX
		height = lowerRightY - upperLeftY

		setPosition(selectArea, upperLeftX + (width / 2), upperLeftY + (height / 2))
		setSize(selectArea, width, height)
	end
end

function onMouseUp(x, y, button)
	if button == MOUSE_BUTTON_LEFT then
		mouseDown = false

		cameraPos = getCameraPosition()
		print("End sellect area", x + cameraPos:getX(), y + cameraPos:getY())

		bodies = checkCollisionsArea(x + cameraPos:getX(), y + cameraPos:getY(), areaX, areaY)

		print("body count", bodies:size())
		for i=0,bodies:size()-1 do
			print("body", i, bodies:at(i))
			tag = getTag(bodies:at(i))
			if tag == "WORKER" then
				sendMessage(bodies:at(i), "SELECTED", "true")
			end
		end

		setPosition(selectArea, 0, 0)
		setSize(selectArea, 1, 1)
	end
end

function onMouseDown(x, y, button)
	if button == MOUSE_BUTTON_LEFT then

		cameraPos = getCameraPosition()

		areaX = x + cameraPos:getX()
		areaY = y + cameraPos:getY()
		print("Start sellect area", areaX, areaY)
		print("cam pos", cameraPos:getX(), cameraPos:getY())
		mouseDown = true
	end
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

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	if dx ~= 0 or dy ~= 0 then
		moveCameraBy(dx, dy)
	end
end

function onBroadcast(message, value)
	print(message, value)
end