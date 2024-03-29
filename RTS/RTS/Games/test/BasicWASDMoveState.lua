include("Games/behaviors/State.lua")


DashMoveState = {}

DashMoveState.new = function(context) 
	local self = State.new()

	self.context = context

	function self.setup()
		self.context.velocity:scale(3.0)
		self.dashTime = 0
	end

	function self.update(dt)

		characterPos = getPosition(entityId)
		setCameraPosition(math.floor(characterPos:getX()), math.floor(characterPos:getY()))

		self.dashTime = self.dashTime + dt
		if self.dashTime > 125 then
			self.context.stateMachine.popState()
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if self.context.velocity:magnitude() > 0 then
			moveAndSlide(entityId, self.context.velocity:getX() * step, self.context.velocity:getY() * step)
		end
	end

	function self.teardown()
		self.context.velocity:scale(0.3333)
	end

	return self
end

BasicWASDMoveState = {}

BasicWASDMoveState.new = function(context) 
	local self = State.new()

	self.context = context

	self.context.input = Vector2f.new(0, 0)
	self.dashState = DashMoveState.new(context)

	function self.setup()
	end

	function self.update(dt)
		step = dt / 1000.0
		characterPos = getPosition(entityId)
		setCameraPosition(math.floor(characterPos:getX()), math.floor(characterPos:getY()))
		if keys[SDLK_SPACE] then
			self.context.stateMachine.pushState(self.dashState)
		end

		if keys[SDLK_w] then
			self.context.input:setY(-1)
		elseif keys[SDLK_s] then
			self.context.input:setY(1)
		else
			self.context.input:setY(0)
		end

		if keys[SDLK_a] then
			self.context.input:setX(-1)
		elseif keys[SDLK_d] then
			self.context.input:setX(1)
		else
			self.context.input:setX(0)
		end

		self.context.input:normalize()
		self.context.input:scale(self.context.speed)

		if self.context.input:magnitude() > 0 then
			self.context.velocity:moveToward(self.context.input:getX(), self.context.input:getY(), step * self.context.accelerate)
		else
			self.context.velocity:moveToward(0, 0, step * self.context.friction)
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if self.context.velocity:magnitude() > 0 then
			moveAndSlide(entityId, self.context.velocity:getX() * step, self.context.velocity:getY() * step)
		end
	end

	function self.teardown()
	end

	return self
end