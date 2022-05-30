include("Games/behaviors/State.lua")

BonesSpawnState = {}

BonesSpawnState.new = function(context) 
	local self = State.new()

	self.idleState = BonseIdleState.new(context)

	context.canCollect = false

	function self.setup()
		self.top = Vector2f.new(512, 0)
		self.middle = Vector2f.new(512, 280)
		self.pos = getPosition(entityId)

		self.SPAWN_DURATION = 550.0
		self.spawn_time = 0
	end

	function self.update(dt)
		if self.spawn_time >= self.SPAWN_DURATION then
			context.stateMachine.pushState(self.idleState)
		else
			local t = self.spawn_time / self.SPAWN_DURATION
			local t_squared = t * t
			local t_inverse_squared = (1.0 - t) * (1.0 - t)

			local px =  t_inverse_squared * self.pos:getX() + (2 * t * self.top:getX() - 2 * t_squared * self.top:getX()) + t_squared * self.middle:getX()
			local py =  t_inverse_squared * self.pos:getY() + (2 * t * self.top:getY() - 2 * t_squared * self.top:getY()) + t_squared * self.middle:getY()

			setPosition(entityId, px, py)
		end

		self.spawn_time = self.spawn_time + dt
	end

	function self.onPhysics(dt)
	end

	function self.teardown()
		setPosition(entityId, self.middle:getX(), self.middle:getY())
	end

	return self
end

BonseIdleState = {}

BonseIdleState.new = function(context) 
	local self = State.new()

	function self.setup()
		context.idle_active = true
		self.hover_value = 0
		self.middle = Vector2f.new(512, 280)
		context.canCollect = true
	end

	function self.update(dt)
		local prog = (self.hover_value / 1000.0) * 2 * math.pi
		local offset = math.cos(prog) * 10.0

		setPosition(entityId, self.middle:getX(), self.middle:getY() + offset)

		self.hover_value = (self.hover_value + dt) % 1000
	end

	function self.onPhysics(dt)
	end

	function self.teardown()
		destroyEntity(entityId)
	end

	return self
end

BonesThrowState = {}

BonesThrowState.new = function(context) 
	local self = State.new()

	function self.setup()
		self.top = Vector2f.new(512, 0)
		self.middle = Vector2f.new(512, 384)
		self.pos = getPosition(entityId)

		self.SPAWN_DURATION = 550.0
		self.spawn_time = 0
	end

	function self.update(dt)
		if self.spawn_time >= self.SPAWN_DURATION then
			broadcastMessage(entityId, "INGREDIENT_ADDED", "BONES")
			context.stateMachine.popState()
		else
			local t = self.spawn_time / self.SPAWN_DURATION
			local t_squared = t * t
			local t_inverse_squared = (1.0 - t) * (1.0 - t)

			local px =  t_inverse_squared * self.pos:getX() + (2 * t * self.top:getX() - 2 * t_squared * self.top:getX()) + t_squared * self.middle:getX()
			local py =  t_inverse_squared * self.pos:getY() + (2 * t * self.top:getY() - 2 * t_squared * self.top:getY()) + t_squared * self.middle:getY()

			setPosition(entityId, px, py)
		end

		self.spawn_time = self.spawn_time + dt
	end

	function self.onPhysics(dt)
	end

	function self.teardown()
		destroyEntity(entityId)
	end

	return self
end