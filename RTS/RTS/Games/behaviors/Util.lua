ArrayList = {}
ArrayList.new = function(copy)
	local self = setmetatable({}, ArrayList)

	self.array = {}
	self.length = 0
	if copy then
		local index = 0
		while copy[index] ~= nil do
			self.array[index] = copy[index]
		end
		self.length = index
	end

	function self.at(at)
		if at < 0 or at >= self.length then
			error("Out of bounds.")
		end
		local val = self.array[at]
		return val
	end

	function self.indexOf(item)
		local index = 0
		while index < self.length do
			if item == self.array[index] then
				return index
			end
			index = index + 1
		end

		return -1
	end

	function self.push(item) 
		self.array[self.length] = item
		self.length = self.length + 1
	end

	function self.pop()
		local val = self.array[self.length - 1]
		self.array[self.length - 1] = nil
		self.length = self.length - 1
		return val
	end

	function self.peek()
		return self.array[self.length - 1]
	end

	function self.pushAt(item, at)
		if at < 0 or at >= self.length then
			error("Out of bounds.")
		end

		local index = self.length - 1
		while index >= at do
			self.array[index + 1] = self.array[index]
			index = index - 1
		end
		self.array[at] = item
		self.length = self.length + 1
	end

	function self.removeAt(at)
		if at < 0 or at >= self.length then
			error("Out of bounds.")
		end
		local val = self.array[at]

		local index = at
		while index < self.length - 1 do
			self.array[index] = self.array[index + 1]
			index = index + 1
		end
		self.length = self.length - 1

		return val
	end

	function self.clear()
		while self.length > 0 do
			self.removeAt(0)
		end
	end

	function self.sort(compare)
		local copy = {}
		for index=0, self.length - 1 do
			copy[index] = self.at(index)
		end

		table.sort(copy, compare)
		self.clear()
		for i,v in pairs(copy) do
			self.push(v)
		end
	end

	function self.toString()
		local str = "["

		local index = 0
		while index < self.length do
			str = str .. " " .. tostring(self.at(index)) .. " "
			index = index + 1
		end

		str = str .. "]"
	end

	return self
end