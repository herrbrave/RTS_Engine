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