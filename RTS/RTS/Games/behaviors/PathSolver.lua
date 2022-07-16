

PathSolver = {}
PathSolver.new = function()
	local self = setmetatable({}, PathSolver)
	self.path = nil
	self.pathIndex = -1
	self.pathLength = 0
	self.pathTarget = Vector2f.new(0.0, 0.0)

	function self.isThereAPath()
		if self.path == nil or self.pathLength <= 0 then
			return false
		end
		return true
	end

	function self.getEndOfPath()
		if self.isThereAPath() == false then
			return -1, -1
		end

		return self.path[self.pathLength][1], self.path[self.pathLength][2]
	end

	function self.constructPath(_pathRef)
		self.path = {}
		self.pathLength = _pathRef:size() - 1
		for _index=0,self.pathLength do
			self.path[_index] = {_pathRef:getX(_index), _pathRef:getY(_index)}
		end
		self.pathIndex = 0
	end

	function self.updatePath(_id, _speed, _delta)
		if self.isThereAPath() == false then
			return
		end

		local _step = _delta / 1000.0
		local _dist = _speed * _step

		while _dist > 0 and self.pathIndex <= self.pathLength do
			_dist = self.moveAlongPath(_id, _dist)
		end
	end


	function self.moveAlongPath(_id, _dist)
		local _pathX = self.path[self.pathIndex][1]
		local _pathY = self.path[self.pathIndex][2]

		local _pathEntityPosition = getPosition(_id)
		self.pathTarget:setX(_pathX)
		self.pathTarget:setY(_pathY)
		self.pathTarget:subtract(_pathEntityPosition)

		local _moveDist = self.pathTarget:magnitude()

		if _moveDist < _dist then
			self.pathTarget:normalize()
			self.pathTarget:scale(_moveDist)
			moveAndSlide(_id, self.pathTarget:getX(), self.pathTarget:getY())
			self.pathIndex = self.pathIndex + 1
			if self.pathIndex > self.pathLength then
				self.path = nil
				self.pathLength = -1
			end

			return _dist - _moveDist
		else 
			self.pathTarget:normalize()
			self.pathTarget:scale(_dist)
			moveAndSlide(_id, self.pathTarget:getX(), self.pathTarget:getY())
		end

		_pathEntityPosition = nil
		return 0.0
	end

	return self
end