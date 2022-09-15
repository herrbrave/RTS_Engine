include("Games/behaviors/State.lua")
include("Games/behaviors/Util.lua")

DungeonTools = {}

DungeonTools.new = function()
	local self = setmetatable({}, DungeonTools)

	function self.track(dungeon, x, y, id, vision)

		local neighbors = {
			{x = 1, y = 0},
			{x = 0, y = 1},
			{x = -1, y = 0},
			{x = 0, y = -1}
		}
		local queue = ArrayList.new(nil)
		local visited = {}
		for index=0,63,1 do
			visited[index] = {}
		end

		local trackback = {}
		for index=0,63,1 do
			trackback[index] = {}
		end
		queue.push({x = x, y = y})
		while queue.length > 0 do
			local tile = queue.pop()
			visitied[tile.x][tile.y] = true

			if dungeon[tile.x][tile.y].id == id then
				local path = ArrayList.new(nil)
				local px = tile.x
				local py = tile.y
				while px ~= x or py ~= y do
					path.push({x = px, y = py})
					local back = trackback[px][py]
					px = back.x
					py = back.y
				end
				return path
			end

			for index=1,4,1 do
				local dx = tile.x + neighbors[index].x
				local dy = tile.y + neighbors[index].y
				if dx >= 0 and dx < 64 and dy >= 0 and dy < 48 and visited[dx][dy] == nil and dungeon[dx][dy] and dungeon[dx][dy].id == nil then
					trackback[dx][dy] = {x = tile.x, y = tile.y}
					queue.push(trackback[dx][dy])
				end
			end

		end

		return ArrayList.new(nil)
	end
	
	return self
end

PlayerState = {}

PlayerState.new = function(context) 
	local self = State.new()
	self.context = context

	function self.updateMap(x, y, vision)

		print("updating map")
		local neighbors = {
			{x = 1, y = 0},
			{x = 0, y = 1},
			{x = -1, y = 0},
			{x = 0, y = -1}
		}
		local queue = ArrayList.new(nil)
		local visited = {}
		for index=0,63,1 do
			visited[index] = {}
		end

		queue.push({x = x, y = y, dist = 0})
		while queue.length > 0 do
			local tile = queue.removeAt(0)
			local tileGrid = self.context.dungeon[tile.x][tile.y]
			visited[tile.x][tile.y] = true
			if tileGrid.visible ~= true then
				tileGrid.visible = true
				local cell = self.context.dungeon[tile.x][tile.y].tile[random(#(self.context.dungeon[tile.x][tile.y].tile) + 1)]
				if cell ~= nil and cell.x ~= nil and cell.y ~= nil then
					print("Pushing Texture at", tile.x, tile.y)
					pushTexture(tile.x, tile.y, "Assets/test/Sprites/Dungeon_Tileset.png", cell.x, cell.y, 16, 16)
				end
			end

			for index=1,4,1 do
				local dx = tile.x + neighbors[index].x
				local dy = tile.y + neighbors[index].y
				if dx >= 0 and dx < 64 and dy >= 0 and dy < 48 and visited[dx][dy] ~= true and self.context.dungeon[dx][dy] and tile.dist < vision then
					queue.push({x = dx, y = dy, dist = tile.dist + 1})
				end
			end
		end
	end

	function self.moveTo(x, y)
		if x >= 0 and x < 64 and y >= 0 and y < 48 and self.context.dungeon[x][y] then
			self.context.x = x
			self.context.y = y
			setPosition(self.context.playerId, x * 16 + 8, y * 16 + 8)
			self.updateMap(x, y, 10)
			return true
		end

		return false
	end

	function self.setup()
		self.moveTo(self.context.x, self.context.y)
		self.ticks = 0
	end

	function self.update(dt)
		if self.ticks < 300 then
			self.ticks = self.ticks + dt
			return
		end

		self.ticks = 0
		if inputState.keyPressed(SDLK_UP) then
			self.moveTo(self.context.x, self.context.y - 1)
		elseif inputState.keyPressed(SDLK_RIGHT) then
			self.moveTo(self.context.x + 1, self.context.y)
		elseif inputState.keyPressed(SDLK_DOWN) then
			self.moveTo(self.context.x, self.context.y + 1)
		elseif inputState.keyPressed(SDLK_LEFT) then
			self.moveTo(self.context.x - 1, self.context.y)
		end
	end

	return self
end

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