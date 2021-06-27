registrar = {
	ASSET = 1,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()
	aabb = createDefault(100, 100, 128, 128, 255, 255, 255, 255)
	setAABBCollision(aabb, 128, 128)
	setScript(aabb, "Games/test/collider.lua")

	loadTexture("Assets/test/circle.png", "Assets/test/circle.png")
	circle = createTextured("Assets/test/circle.png", 250, 100, 128, 128, 0, 0, 128, 128, true)
	setCircleCollision(circle, 64)
	setScript(circle, "Games/test/collider.lua")

	loadTexture("Assets/test/square.png", "Assets/test/square.png")
	obb = createTextured("Assets/test/square.png", 450, 100, 128, 128, 0, 0, 128, 128, true)
	setOBBCollision(obb, 128, 128, 45)
	setAngle(obb, 45)
	setScript(obb, "Games/test/collider.lua")

	selected = aabb

	aabb2 = createDefault(100, 500, 128, 128, 255, 255, 255, 255)
	setAABBCollision(aabb2, 128, 128)
	
	dx = 0
	dy = 0
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)
	print("onMouseDown", x, y , button)
end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);

	if keyId == SDLK_1 then
		selected = aabb
		print("selected ", selected)
	elseif keyId == SDLK_2 then
		selected = circle
		print("selected ", selected)
	elseif keyId == SDLK_3 then
		selected = obb
		print("selected ", selected)
	elseif keyId == SDLK_UP then
		dy = -1
	elseif keyId == SDLK_RIGHT then
		dx = 1
	elseif keyId == SDLK_DOWN then
		dy = 1
	elseif keyId == SDLK_LEFT then
		dx = -1
	end
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);

	if keyId == SDLK_UP then
		dy = 0
	elseif keyId == SDLK_RIGHT then
		dx = 0
	elseif keyId == SDLK_DOWN then
		dy = 0
	elseif keyId == SDLK_LEFT then
		dx = 0
	end
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
	setVelocity(selected, dx, dy)
	setSpeed(selected, 300)
end