include("Games/behaviors/State.lua")


DashMoveState = {}

DashMoveState.new = function(context) 
	local self = State.new()

	self.context = context

	function self.setup()
		context.state = DASH
		self.context.velocity:scale(3.0)
		self.dashTime = 0
	end

	function self.update(dt)

		characterPos = getPosition(entityId)

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
		context.state = IDLE
	end

	function self.update(dt)
		step = dt / 1000.0
		characterPos = getPosition(entityId)
		if keys[SDLK_SPACE] then
			self.context.stateMachine.pushState(self.dashState)
		elseif keys[SDLK_e] then
			collisions = checkCollisions(entityId)
			onBox = false
			if collisions:size() > 0 then
				for index=0,collisions:size()-1 do
					if collisions:at(index) == context.box then
						onBox = true
						break
					end
				end
				if onBox then
					sendMessage(context.box, "OPEN_CHEST", "open")
				end
			end
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

		if keys[SDLK_1] and context.torch ~= 0 then
			sendMessage(context.torch, "DETONATE", "TRUE")
			context.torch = 0
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

HurtMoveState = {}

HurtMoveState.new = function(context) 
	local self = State.new()

	self.context = context

	function self.setup()
		context.state = HURT
		self.hurtTime = 0
		context.hurtVector:scale(750)
		setColor(entityId, 255, 0, 0, 255)
	end

	function self.update(dt)

		characterPos = getPosition(entityId)

		self.hurtTime = self.hurtTime + dt
		if self.hurtTime > 100 then
			self.context.stateMachine.popState()
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if self.context.hurtVector:magnitude() > 0 then
			moveAndSlide(entityId, self.context.hurtVector:getX() * step, self.context.hurtVector:getY() * step)
		end
	end

	function self.teardown()
		setColor(entityId, 255, 255, 255, 255)
	end

	return self
end