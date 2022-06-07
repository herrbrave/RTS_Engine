include("Games/behaviors/State.lua")

IdleState = {}

IdleState.new = function(context) 
	local self = State.new()

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end

SearchMoveState = {}

SearchMoveState.new = function(context) 
	local self = State.new()

	include("Games/behaviors/PathSolver.lua")

	self.pathSolver = PathSolver.new()

	function self.getPathAtMouse()
		local x = inputState.mouseX()
		local y = inputState.mouseY()
		local pos = getPosition(entityId)
		local currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
		local sx = currentTileCoords:getX()
		local sy = currentTileCoords:getY()

		local camPos = getCameraPosition()
		local tileCoords = getTileCoordinatesAtPoint(camPos:getX() + x, camPos:getY() + y)
		local ex = tileCoords:getX()
		local ey = tileCoords:getY()

		return getPath(sx, sy, ex, ey)
	end

	function self.updatePath(newTileX, newTileY)
		if self.displayPath ~= nil then
			for _index=0,self.displayPathLength do
				popTexture(self.displayPath[_index][1], self.displayPath[_index][2])
			end
		end

		local pos = getPosition(entityId)
		local currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())
		local tempPath = getPath(currentTileCoords:getX(), currentTileCoords:getY(), newTileX, newTileY)
		self.displayPath = {}
		if tempPath:size() == 0 then
			self.displayPath = nil
			return
		else
			self.displayPathLength = tempPath:size() - 2
			for _index=0,self.displayPathLength do
				local tx = math.floor(tempPath:getX(_index) / 48)
				local ty = math.floor(tempPath:getY(_index) / 48)
				pushTexture(tx, ty, "Assets/test/Sprites/Target.png", 0, 0, 16, 16)
				self.displayPath[_index] = {tx, ty}
			end
		end

		popTexture(self.tileX, self.tileY)
		self.tileX = newTileX
		self.tileY = newTileY
		pushTexture(self.tileX, self.tileY, "Assets/test/Sprites/Selected.png", 0, 0, 16, 16)
	end

	function self.setup()
		local pos = getPosition(entityId)
		local currentTileCoords = getTileCoordinatesAtPoint(pos:getX(), pos:getY())

		self.tileX = currentTileCoords:getX()
		self.tileY = currentTileCoords:getY()
		pushTexture(self.tileX, self.tileY, "Assets/test/Sprites/Selected.png", 0, 0, 16, 16)
		self.speed = 325
		self.selecting = false
		self.displayPath = nil
	end

	function self.update(dt)
		-- on mouse down
		if self.pathSolver.isThereAPath() == false then
			if inputState.mousePressed(MOUSE_BUTTON_LEFT) == true then
				local pathRef = self.getPathAtMouse()
				self.pathSolver.constructPath(pathRef)
			else
				local x = inputState.mouseX()
				local y = inputState.mouseY()
				local camPos = getCameraPosition()
				local tileCoords = getTileCoordinatesAtPoint(camPos:getX() + x, camPos:getY() + y)
				if self.tileX ~= tileCoords:getX() or self.tileY ~= tileCoords:getY() then
					local newTileX = tileCoords:getX()
					local newTileY = tileCoords:getY()
					self.updatePath(newTileX, newTileY)
				end
			end
		end

	end

	function self.onPhysics(dt)
		self.pathSolver.updatePath(entityId, self.speed, dt)
	end

	function self.teardown()
		popTexture(self.tileX, self.tileY)
	end

	return self
end

MoveState = {}

MoveState.new = function(context) 
	local self = State.new()

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end

AttackState = {}

AttackState.new = function(context) 
	local self = State.new()

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end

DeadState = {}

DeadState.new = function(context) 
	local self = State.new()

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end