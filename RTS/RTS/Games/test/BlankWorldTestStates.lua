include("Games/behaviors/State.lua")
include("Games/behaviors/Animation.lua")
include("Games/behaviors/Ui.lua")

GameOfLifeState = {}
GameOfLifeState.new = function(context) 
	local self = State.new()
	self.ticks = 0

	-- set up cells
	self.initialized = false
	self.cells = {}
	local index = 0
	while index < 64 do
		self.cells[index] = {}
		local bucket = 0
		while bucket < 48 do
			self.cells[index][bucket] = random(2)
			bucket = bucket + 1
		end
		index = index + 1
	end

	function self.update(dt)
		if self.initialized == false then
			local index = 0
			while index < 64 do
				local bucket = 0
				while bucket < 48 do
					pushTexture(index, bucket, "Assets/test/Sprites/Dungeon_Tileset.png", 6*16, 0*16, 16, 16)

					if self.cells[index][bucket] == 1 then
						pushTexture(index,bucket, "Assets/test/Sprites/Dungeon_Tileset.png", 6*16, 8*16, 16, 16)
					end
					bucket = bucket + 1
				end
				index = index + 1
			end
			self.initialized = true
		end

		self.ticks = self.ticks + dt
		if self.ticks > 4000 then
			self.processGeneration()
			self.ticks = 0
		end

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	function self.processGeneration()
		local index = 0
		while index < 64 do
			local bucket = 0
			while bucket < 48 do
				local neighbors = self.countNeighbors(index, bucket)
				if self.cells[index][bucket] == 1 and neighbors < 2 then
					self.cells[index][bucket] = 0
					popTexture(index, bucket)
				elseif self.cells[index][buckets] == 1 and (neighbors == 2 or neighbors == 3) then
					self.cells[index][bucket] = 1
				elseif self.cells[index][buckets] == 1 and neighbors > 3 then
					self.cells[index][bucket] = 0
					popTexture(index, bucket)
				elseif self.cells[index][buckets] == 0 and neighbors == 3 then
					self.cells[index][bucket] = 1
					pushTexture(index,bucket, "Assets/test/Sprites/Dungeon_Tileset.png", 6*16, 8*16, 16, 16)
				end

				bucket = bucket + 1
			end
			index = index + 1
		end
	end

	function self.countNeighbors(x, y)
		local neighbors = 0
		local i = -1
		while i < 2 do
			local j = -1
			while j < 2 do
				if i~=0 and j ~= 0 then
					neighbors = neighbors + self.getNeighbor(i + x, j + y)
				end
				j = j + 1
			end
			i = i + 1
		end

		return neighbors
	end

	function self.getNeighbor(x, y)
		if x < 0 or x >= 64 or y < 0 or y >= 48 then
			return 0
		end

		return self.cells[x][y]
	end

	return self
end

BSPNode = {}
BSPNode.new = function(x0, y0, x1, y1)
	local self = setmetatable({}, BSPNode)

	self.x0 = x0
	self.y0 = y0
	self.x1 = x1
	self.y1 = y1
	self.left = nil
	self.right = nil
	self.leaf = false

	function self.split() 
		local area = self.area()
		-- force node to be a leaf if it's under 8x8
		if area < 128 or (area < 256 and random(2) == 0) then
			self.leaf = true
			return false
		end

		-- if we're wider than tall split verticle
		if (x1 - x0) > (y1 - y0) then
			self.left = BSPNode.new(self.x0, self.y0, math.floor((self.x1 + self.x0) / 2), self.y1)
			self.right = BSPNode.new(math.floor((self.x1 + self.x0) / 2) + 1, self.y0, self.x1, self.y1)
		else
			self.left = BSPNode.new(self.x0, self.y0, self.x1, math.floor((self.y1 + self.y0) / 2))
			self.right = BSPNode.new(self.x0, math.floor((self.y1 + self.y0) / 2) + 1, self.x1, self.y1)
		end
		return true
	end

	function self.area() 
		return (self.x1 - self.x0) * (self.y1 - self.y0)
	end

	return self
end

Tiles = {
	UPPER_LEFT_CORNER = {
		[0] = {x=0*16, y=0*16}
	},
	TOP_WALL = {
		[0] = {x=1*16, y=0*16},
		[1] = {x=2*16, y=0*16},
		[2] = {x=3*16, y=0*16},
		[3] = {x=4*16, y=0*16}
	},
	UPPER_RIGHT_CORNER = {
		[0] = {x=5*16, y=0*16}
	},
	RIGHT_WALL = {
		[0] = {x=5*16, y=1*16},
		[1] = {x=5*16, y=2*16},
		[2] = {x=5*16, y=3*16}
	},
	LOWER_RIGHT_CORNER = {
		[0] = {x=5*16, y=4*16}
	},
	BOTTOM_WALL = {
		[0] = {x=1*16, y=4*16},
		[1] = {x=2*16, y=4*16},
		[2] = {x=3*16, y=4*16},
		[3] = {x=4*16, y=4*16}
	},
	LOWER_LEFT_CORNER = {
		[0] = {x=0*16, y=4*16}
	},
	LEFT_WALL = {
		[0] = {x=0*16, y=1*16},
		[1] = {x=0*16, y=2*16},
		[2] = {x=0*16, y=3*16}
	},
	FLOOR = {
		[0] = {x=2*16, y=2*16},
		[1] = {x=3*16, y=2*16}
	}
}

RandoDungeon = {}
RandoDungeon.new = function(context) 
	local self = State.new()

	self.bsp = BSPNode.new(0, 0, 63, 47)
	self.grid = {}
	local i = 0
	while i < 64 do
		self.grid[i] = {}
		i = i + 1
	end

	function self.generate()
		self.split(self.bsp)
	end

	function self.split(node)
		if node.split() == false then
			self.placeRoom(node)
			return
		end

		self.split(node.left)
		self.split(node.right)
	end

	function self.placeRoom(node)
		local w = node.x1 - node.x0
		local h = node.y1 - node.y0

		local roomW = math.floor(w / 2) + random(math.floor(w / 2) - 2)
		local roomH = math.floor(h / 2) + random(math.floor(h / 2) - 2)

		--local x = node.x0 + 1 + random(w - roomW)
		--local y = node.y0 + 1 + random(h - roomH)
		--room = {
		--	x0 = x,
		--	y0 = y,
		--	x1 = x + roomW,
		--	y1 = y + roomH
		--}

		node.room = {
			x0 = node.x0,
			y0 = node.y0,
			x1 = node.x1,
			y1 = node.y1
		}

		print("================================ DRAWING ROOM ===================================")
		for i=node.room.x0 + 1,node.room.x1 - 1, 1 do
			local row = ""
			for j=node.room.y0 + 1,node.room.y1 - 1, 1 do
				row = row .. "{" .. i .. ", " .. j .. "}"
				self.grid[i][j] = Tiles.FLOOR
			end
			print(row)
		end
		print("=================================================================================")
	end

	function self.drawDungeon()
		for c = 0, 63, 1 do
			for r = 0, 47, 1 do
				if self.grid[c][r] ~= nil then
					local cell = self.grid[c][r][random(#self.grid[c][r] + 1)]
					if cell ~= nil and cell.x ~= nil and cell.y ~= nil then
						pushTexture(c,r, "Assets/test/Sprites/Dungeon_Tileset.png", cell.x, cell.y, 16, 16)
					end
				end
			end
		end
	end

	self.initialized = false
	function self.initialize()
		self.generate()
		self.rooms = ArrayList.new(nil)

		-- split and place all of the walls
		local queue = ArrayList.new(nil)
		queue.push(self.bsp)
		while queue.length > 0 do
			local node = queue.pop()

			if node.leaf == true then
				self.placeRoom(node)
				self.rooms.push(node.room)
			else
				queue.push(node.left)
				queue.push(node.right)
			end
		end

		--sort the rooms from left to right
		--table.sort(self.rooms, function(v1, v2) return v1.x0 < v2.x0 end)

		local orderedRooms = {}
		local index = 0
		for i,r in ipairs(self.rooms) do
			orderedRooms[index] = r
			index = index + 1
		end

		index = 0
		while index < #orderedRooms do
			local room = orderedRooms[index]
			local nextRoom = orderedRooms[index + 1]
			local x0 = room.x0 + random(room.x1 - room.x0 - 2)
			local y0 = room.y0 + random(room.y1 - room.y0 - 2)
			local x1 = nextRoom.x0 + 1 + random(nextRoom.x1 - nextRoom.x0 - 2)
			local y1 = nextRoom.y0 + 1 + random(nextRoom.y1 - nextRoom.y0 - 2)

			print("Drawing Cooridoor from", x0, y0, "to", x1, y1)

			self.drawCooridoor(x0, y0, x1, y1)

			index = index + 1
		end

		local index = 0
		while index < 64 do
			local bucket = 0
			local row = ""
			while bucket < 48 do
				if self.grid[index][bucket] ~= nil then
					row = row .. "#"
				else
					row = row .. "0"
				end
				bucket = bucket + 1
			end
			index = index + 1
			print(row)
		end

		self.drawDungeon()
	end

	function self.drawCooridoor(x0, y0, x1, y1)
		local x = x0
		local y = y0

		while x ~= x1 and y ~= y1 do
			if x < x1 then
				x =  x + 1
			elseif x > x1 then
				x = x - 1
			elseif y < y1 then
				y = y + 1
			elseif y > y1 then
				y = y - 1
			end

			print("Adding floor at", x, y)
			self.grid[x][y] = Tiles.FLOOR
		end
	end

	function self.update(dt)
		if self.initialized == false then
			self.initialize()
			self.initialized = true
		end
	end

	return self
end