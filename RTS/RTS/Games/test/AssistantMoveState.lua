include("Games/behaviors/State.lua")


DashMoveState = {}

DashMoveState.new = function(context) 
	local self = State.new()

	function self.setup()
		context.state = DASH
		context.velocity:scale(3.0)
		self.dashTime = 0
	end

	function self.update(dt)

		characterPos = getPosition(entityId)

		self.dashTime = self.dashTime + dt
		if self.dashTime > 125 then
			context.stateMachine.popState()
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if context.velocity:magnitude() > 0 then
			moveAndSlide(entityId, context.velocity:getX() * step, context.velocity:getY() * step)
		end
	end

	function self.teardown()
		context.velocity:scale(0.3333)
	end

	return self
end

BasicWASDMoveState = {}

BasicWASDMoveState.new = function(context) 
	local self = State.new()

	context.input = Vector2f.new(0, 0)
	self.dashState = DashMoveState.new(context)

	function self.setup()
		context.state = IDLE
	end

	function self.update(dt)
		step = dt / 1000.0
		if keys[SDLK_SPACE] then
			context.stateMachine.pushState(self.dashState)
		elseif keys[SDLK_e] then
			collisions = checkCollisions(entityId)
			onBox = false
			for _i=0,collisions:size()-1 do
				self.collisionTag = getTag(collisions:at(_i))
				if self.collisionTag == "BOX" then
					sendMessage(collisions:at(_i), "OPEN_CHEST", "open")
				end
			end
		end

		if keys[SDLK_w] then
			context.input:setY(-1)
		elseif keys[SDLK_s] then
			context.input:setY(1)
		else
			context.input:setY(0)
		end

		if keys[SDLK_a] then
			context.input:setX(-1)
		elseif keys[SDLK_d] then
			context.input:setX(1)
		else
			context.input:setX(0)
		end
		-- detonate torch
		if keys[SDLK_1] and context.torch > 0 then
			sendMessage(context.torch, "DETONATE", "TRUE")
			context.torch = 0
		end

		context.input:normalize()
		context.input:scale(context.speed)

		if context.input:magnitude() > 0 then
			context.velocity:moveToward(context.input:getX(), context.input:getY(), step * context.accelerate)
		else
			context.velocity:moveToward(0, 0, step * context.friction)
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if context.velocity:magnitude() > 0 then
			moveAndSlide(entityId, context.velocity:getX() * step, context.velocity:getY() * step)
		end
	end

	function self.teardown()
	end

	return self
end

HurtMoveState = {}

HurtMoveState.new = function(context) 
	local self = State.new()

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
			context.stateMachine.popState()
		end
	end

	function self.onPhysics(dt)
		step = dt / 1000.0
		if context.hurtVector:magnitude() > 0 then
			moveAndSlide(entityId, context.hurtVector:getX() * step, context.hurtVector:getY() * step)
		end
	end

	function self.teardown()
		setColor(entityId, 255, 255, 255, 255)
	end

	return self
end