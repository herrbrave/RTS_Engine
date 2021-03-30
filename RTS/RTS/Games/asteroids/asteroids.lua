registrar = {
	ASSET = 1,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 1,
	ASSET = 1
}

ship = 0

lastRockGenerated = 0
metreors = {
	"meteorBrown_big1",
	"meteorBrown_big2",
	"meteorBrown_big3",
	"meteorBrown_big4",
}

function setup()
	loadTexture("Assets/asteroids/playerShip_red.png", "playerShip_red")
	loadTexture("Assets/asteroids/meteorBrown_big1.png", "meteorBrown_big1")
	loadTexture("Assets/asteroids/meteorBrown_big2.png", "meteorBrown_big2")
	loadTexture("Assets/asteroids/meteorBrown_big3.png", "meteorBrown_big3")
	loadTexture("Assets/asteroids/meteorBrown_big4.png", "meteorBrown_big4")
	loadTexture("Assets/asteroids/meteorBrown_med1.png", "meteorBrown_med1")
	loadTexture("Assets/asteroids/meteorBrown_med2.png", "meteorBrown_med2")
	loadTexture("Assets/asteroids/laserRed.png", "laserRed")
	ship = createTextured("playerShip_red", 400, 300, 39, 49, 0, 0, 75, 98, true)
	setScript(ship, "Games/asteroids/ship.lua")
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

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
	lastRockGenerated = lastRockGenerated + delta
	if lastRockGenerated > 3000 then
		lastRockGenerated = 0
		rockTexture = math.random(4)
		rock = createTextured(metreors[rockTexture], 0, 0, 101, 84, 0, 0, 101, 84, true)
		rockAngle = math.random(360)
		rads = (rockAngle / 180.0) * math.pi
		vel = Vector2f.new(math.cos(rads), math.sin(rads))
		vel:normalize()
		setVelocity(rock, vel:getX(), vel:getY())
		setSpeed(rock, 80)
		setTag(rock, "rock")
		setScript(rock, "Games/asteroids/big_rock.lua")
	end
end