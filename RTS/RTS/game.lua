
borderColor = 100

function setup()
	-- Initialize random
	math.randomseed(os.time())

	-- setup game
	game = {}

	game["boundryTop"] = createDefault(512, 0, 1024, 3, borderColor, borderColor, borderColor, 255)
	game["boundryLeft"] = createDefault(0, 384, 3, 768, borderColor, borderColor, borderColor, 255)
	game["boundryBottom"] = createDefault(512, 765, 1024, 3, borderColor, borderColor, borderColor, 255)
	game["boundryRight"] = createDefault(1024, 0, 3, 768, borderColor, borderColor, borderColor, 255)

	game["enemies"] = {}
	for index=1, 6 do
		table.insert(game["enemies"], createDefault(100 * index, 100, 50, 50, 0, 255, 0, 255))
	end

	game["playerBullets"] = {}
	game["enemyBullets"] = {}
	game["playerId"] = createDefault(450, 600, 50, 50, 255, 255, 255, 255)

end

function onKeyDown(keyId)
	local playerId = game["playerId"]

	if SDLK_RIGHT == keyId then
		addVelocity(playerId, 1.0, 0.0)
		setSpeed(playerId, 200.0)

	elseif SDLK_LEFT == keyId then
		addVelocity(playerId, -1.0, 0.0)
		setSpeed(playerId, 200.0)

	elseif SDLK_SPACE == keyId then
		local position = getPosition(game["playerId"])
		local x = position:getX()
		local bulletId = createDefault(x, 500, 20, 20, 125, 0, 0, 255)
		table.insert(game["playerBullets"], bulletId)

		addVelocity(bulletId, 0.0, -1.0)
		setSpeed(bulletId, 350.0)

	end
end

function onKeyUp(keyId)
	local playerId = game["playerId"]

	if SDLK_RIGHT == keyId then
		addVelocity(playerId, -1.0, 0.0)
		setSpeed(playerId, 200.0)

	elseif SDLK_LEFT == keyId then
		addVelocity(playerId, 1.0, 0.0)
		setSpeed(playerId, 200.0)

	end
end

function isEnemy(id)
	local enemies = game["enemies"]
	for index = 1, #enemies do
		if enemies[index] == id then
			return true
		end
	end

	return false
end

function onCollision(collidedId, colliderId)
	-- Check for bullet
	local playerBullets = game["playerBullets"]
	for index = 1, #playerBullets do
		if playerBullets[index] == colliderId then
			destroyEntity(playerBullets[index])
			table.remove(playerBullets, index)
			break
		end
	end

	local enemyBullets = game["enemyBullets"]
	for index = 1, #enemyBullets do
		if enemyBullets[index] == colliderId then
			destroyEntity(enemyBullets[index])
			table.remove(enemyBullets, index)
			break
		end
	end

	-- Check enemies
	local enemies = game["enemies"]
	for index = 1, #enemies do
		if enemies[index] == collidedId then
			destroyEntity(enemies[index])
			table.remove(enemies, index)
			break
		end
	end

end

function update(delta)
	if #game["enemyBullets"] < 5 then
		local randomEnemy = math.random(#game["enemies"])
		local position = getPosition(randomEnemy)
		local bulletId = createDefault(position:getX(), 150, 20, 20, 0, 127, 0, 255)
		table.insert(game["enemyBullets"], bulletId)

		addVelocity(bulletId, 0.0, 1.0)
		setSpeed(bulletId, 350.0)
	end
end