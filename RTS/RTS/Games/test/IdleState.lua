include("Games/behaviors/State.lua")

BasicWASDMoveState = {}

BasicWASDMoveState.new = function() 
	local self = State.new()

	self.velocity = Vector2f.new(0, 0)
	self.speed = 180

	function self.setup()
		self.velocity:setX(0)
		self.velocity:setY(0)
		print("Idle State Begin")
	end

	function self.update(dt)
		step = dt / 1000.0
		characterPos = getPosition(entityId)
		setCameraPosition(math.floor(characterPos:getX()), math.floor(characterPos:getY()))

		if keys[SDLK_w] then
			self.velocity:setY(-1)
		elseif keys[SDLK_s] then
			self.velocity:setY(1)
		else
			self.velocity:setY(0)
		end

		if keys[SDLK_a] then
			self.velocity:setX(-1)
		elseif keys[SDLK_d] then
			self.velocity:setX(1)
		else
			self.velocity:setX(0)
		end

		self.velocity:normalize()
		self.velocity:scale(self.speed * step)
	end

	function self.onPhysics(dt)
		moveAndSlide(entityId, self.velocity:getX(), self.velocity:getY())
	end

	function self.teardown()
		print("Idle State End")
	end

	return self
end