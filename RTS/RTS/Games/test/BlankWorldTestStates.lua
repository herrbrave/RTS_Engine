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
BSPNode.new = function(parent, x0, y0, x1, y1)
	local self = setmetatable({}, BSPNode)

	self.parent = parent
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
			self.left = BSPNode.new(self, self.x0, self.y0, math.floor((self.x1 + self.x0) / 2), self.y1)
			self.right = BSPNode.new(self, math.floor((self.x1 + self.x0) / 2) + 1, self.y0, self.x1, self.y1)
		else
			self.left = BSPNode.new(self, self.x0, self.y0, self.x1, math.floor((self.y1 + self.y0) / 2))
			self.right = BSPNode.new(self, self.x0, math.floor((self.y1 + self.y0) / 2) + 1, self.x1, self.y1)
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

	self.bsp = BSPNode.new(nil, 0, 0, 63, 47)
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
			return
		end

		self.split(node.left)
		self.split(node.right)
	end

	function self.placeRoom(node)
		local w = node.x1 - node.x0
		local h = node.y1 - node.y0

		local roomW = math.floor(w / 2) + random(math.floor(w / 2) - 1)
		local roomH = math.floor(h / 2) + random(math.floor(h / 2) - 1)

		local x = node.x0 + 1 + random(w - roomW)
		local y = node.y0 + 1 + random(h - roomH)

		node.room = {
			x0 = x,
			y0 = y,
			x1 = x + roomW,
			y1 = y + roomH
		}

		--node.room = {
		--	x0 = node.x0,
		--	y0 = node.y0,
		--	x1 = node.x1,
		--	y1 = node.y1
		--}

		self.addDoors(node.room)

		for i=node.room.x0 + 1,node.room.x1 - 1, 1 do
			for j=node.room.y0 + 1,node.room.y1 - 1, 1 do
				self.grid[i][j] = Tiles.FLOOR
			end
		end
	end

	function self.addDoors(room)
		room.doors = {}
		-- top
		room.doors[0] = {
			x = math.floor((room.x0 + room.x1) / 2),
			y = room.y0,
			cooridoor = nil
		}
		-- right
		room.doors[1] = {
			x = room.x1,
			y = math.floor((room.y0 + room.y1) / 2),
			cooridoor = nil
		}
		-- bottum
		room.doors[2] = {
			x = math.floor((room.x0 + room.x1) / 2),
			y = room.y1,
			cooridoor = nil
		}
		-- left
		room.doors[3] = {
			x = room.x0,
			y = math.floor((room.y0 + room.y1) / 2),
			cooridoor = nil
		}

	end

	function self.findClosestDoor(doors, room)

		local x = math.floor((room.x1 + room.x0) / 2)
		local y = math.floor((room.y1 + room.y0) / 2)
		
		return self.findClosestDoorToPoint(doors, x, y)
	end

	function self.findClosestDoorToPoint(doors, x, y)
		local shortestDist = 10000
		local doorIndex = 0
		for index = 0,3,1 do
			local door = doors[index]
			local dist = math.sqrt((x - door.x)^2 + (y - door.y)^2)
			if dist < shortestDist then
				doorIndex = index
				shortestDist = dist
			end
		end

		return doors[doorIndex]
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

	function self.checkAdjacent(n1, n2)
		return math.abs(n1.x0 - n2.x1) == 1 or math.abs(n1.y0 - n2.y1) == 1 or math.abs(n1.x1 - n2.x0) == 1 or math.abs(n1.y1 - n1.y0) == 1
	end

	function self.findAdjacent(n1, leaf)
		if n1 and n1.leaf then
			if self.checkAdjacent(n1, leaf) then
				return n1
			else 
				return nil
			end
		end

		local left = self.findAdjacent(n1.left, leaf)
		if left then
			return left
		end

		return self.findAdjacent(n1.right, leaf)
	end

	function self.getSibling(node)
		if node == node.parent.left then
			return node.parent.right
		end

		return node.parent.left
	end

	function self.buildHalls(node)
		if node.leaf then
			if node.parent.hall == nil then
				local sibling = self.getSibling(node)
				if sibling.leaf == false then
					sibling = self.findAdjacent(sibling, node)
				end
				if sibling then
					node.parent.hall = self.joinAdjacentRooms(node.room, sibling.room)
				end
			end
			return
		end

		self.buildHalls(node.left)
		self.buildHalls(node.right)

		if node.parent and node.parent.hall == nil then
			local sibling = self.getSibling(node)
			if sibling.leaf and node.hall then
				node.parent.hall = self.joinRoomToHall(sibling.room, node.hall)
			elseif node.hall and sibling.hall then
				node.parent.hall = self.joinHallToHall(node.hall, sibling.hall)
			end
		end
	end

	function self.joinNode(node)
		if node.left then
			if node.left.leaf then
				local adjacent = self.findAdjacent(node.right, node.left)
				if adjacent then
					node.hall = self.joinAdjacentRooms(node.left.room, adjacent.room)
				end
			else
				self.joinNode(node.left)
			end
		end
		if node and node.right then
			if node.right.leaf then
				local adjacent = self.findAdjacent(node.left, node.right)
				if adjacent then
					self.joinAdjacentRooms(node.right.room, adjacent.room)
				end
			else
				self.joinNode(node.right) 
			end
		end
	end

	function self.joinAdjacentRooms(r1, r2)

		local r1Door = self.findClosestDoor(r1.doors, r2)
		local x0 = r1Door.x
		local y0 = r1Door.y
		local r2Door = self.findClosestDoor(r2.doors, r1)
		local x1 = r2Door.x
		local y1 = r2Door.y

		local hall = {
			tiles = self.drawHall(x0, y0, x1, y1),
			r1 = r1, 
			r2 = r2
		}
		print("hall", hall, hall.tiles, hall.tiles.length, hall.r1, hall.r2)
		return hall
	end

	function self.joinRoomToHall(room, hall)
		print("blah", room, hall, hall.tiles, hall.tiles.length)
		local tile = hall.tiles.at(math.floor(hall.tiles.length / 2))
		local door = self.findClosestDoorToPoint(room.doors, tile.x, tile.y)

		local hall = {
			tiles = self.drawHall(tile.x, tile.y, door.x, door.y),
			r1 = r1, 
			r2 = hall
		}
		print("hall", hall, hall.tiles, hall.tiles.length, hall.r1, hall.r2)
		return hall
	end

	function self.joinHallToHall(h1, h2)
		print(h1.tiles.length, h2.tiles.length)
		print(math.max(math.floor(h1.tiles.length / 2), 0), math.max(math.floor(h2.tiles.length / 2), 0))
		local tile1 = h1.tiles.at(math.max(math.floor(h1.tiles.length / 2), 0))
		local tile2 = h2.tiles.at(math.max(math.floor(h2.tiles.length / 2), 0))

		local hall = {
			tiles = self.drawHall(tile1.x, tile1.y, tile2.x, tile2.y),
			r1 = h1, 
			r2 = h2
		}
		print("hall", hall, hall.tiles, hall.tiles.length, hall.r1, hall.r2)
		return hall
	end

	self.initialized = false
	function self.initialize()
		self.rooms = ArrayList.new(nil)
		self.leaves = ArrayList.new(nil)
		self.generate()

		-- split and place all of the walls
		local queue = ArrayList.new(nil)
		queue.push(self.bsp)
		while queue.length > 0 do
			local node = queue.pop()

			if node.leaf == true then
				self.placeRoom(node)
				self.rooms.push(node.room)
				self.leaves.push(node)
			else
				queue.push(node.left)
				queue.push(node.right)
			end
		end

		--self.joinNode(self.bsp)
		self.buildHalls(self.bsp)

		self.drawDungeon()
	end

	function self.drawHall(x0, y0, x1, y1)
		local x = x0
		local y = y0

		local hall = ArrayList.new(nil)
		self.grid[x][y] = Tiles.FLOOR
		while (x ~= x1 or y ~= y1) do
			if x < x1 then
				x =  x + 1
			elseif x > x1 then
				x = x - 1
			elseif y < y1 then
				y = y + 1
			elseif y > y1 then
				y = y - 1
			end

			hall.push({
				x = x,
				y = y
			})
			self.grid[x][y] = Tiles.FLOOR
		end

		return hall
	end

	function self.update(dt)
		if self.initialized == false then
			self.initialize()
			self.initialized = true
		end
	end

	return self
end