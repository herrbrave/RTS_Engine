include("Games/behaviors/State.lua")

BasicWASDMoveState = {}

BasicWASDMoveState.new = function() 
	local self = State.new()

	self.input = Vector2f.new(0, 0)
	self.velocity = Vector2f.new(0, 0)
	self.accelerate = 500
	self.friction = 500
	self.speed = 180

	function self.setup()
		self.velocity:setX(0)
		self.velocity:setY(0)
		self.input:setX(0)
		self.input:setY(0)
		print("Idle State Begin")
	end

	function self.update(dt)
		step = dt / 1000.0
		characterPos = getPosition(entityId)
		setCameraPosition(math.floor(characterPos:getX()), math.floor(characterPos:getY()))

		if keys[SDLK_w] then
			self.input:setY(-1)
		elseif keys[SDLK_s] then
			self.input:setY(1)
		else
			self.input:setY(0)
		end

		if keys[SDLK_a] then
			self.input:setX(-1)
		elseif keys[SDLK_d] then
			self.input:setX(1)
		else
			self.input:setX(0)
		end

		self.input:normalize()
		self.input:scale(self.speed)

		if self.input:magnitude() > 0 then
			self.velocity:moveToward(self.input:getX(), self.input:getY(), step * self.accelerate)
		else
			self.velocity:moveToward(0, 0, step * self.friction)
		end

		print("velocity: " .. tostring(self.velocity:magnitude()))
	end

	function self.onPhysics(dt)
		if self.velocity:magnitude() > 0 then
			moveAndSlide(entityId, self.velocity:getX() * step, self.velocity:getY() * step)
		end
	end

	function self.teardown()
		print("Idle State End")
	end

	return self
end