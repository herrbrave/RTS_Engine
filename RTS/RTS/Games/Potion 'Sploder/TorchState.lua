include("Games/behaviors/State.lua")

TorchSpawnState = {}

TorchSpawnState.new = function(context) 
	local self = State.new()

	self.idleState = TorchIdleState.new(context)

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

TorchIdleState = {}

TorchIdleState.new = function(context) 
	local self = State.new()

	context.idle_active = false
	context.onPickup = function() 
		if context.idle_active == false then
			return
		end
		-- Pop idle state
		context.stateMachine.popState()
		-- Pop spawning state
		context.stateMachine.popState()
		-- Push the weapon state 
		context.stateMachine.pushState(TorchWeaponState.new(context))
	end

	function self.setup()
		context.idle_active = true
		self.hover_value = 0
		self.middle = Vector2f.new(512, 280)
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
		context.idle_active = false
	end

	return self
end

TorchWeaponState = {}

TorchWeaponState.new = function(context) 
	local self = State.new()

	self.charges = 3
	self.weapon_active = false
	function self.setup()
		self.weapon_active = true
		setCircleCollision(entityId, 150)
		setPosition(entityId, 30, 0)
		if self.charges == 0 then
			broadcastMessage(entityId, "TORCH_EXPENDED", "TRUE")
			destroyEntity(entityId)
		end
	end

	function self.update(dt)
		-- detonate torch
		if inputState.keyPressed(SDLK_1) == true and self.weapon_active then
			context.stateMachine.pushState(TorchExplosionState.new(context))
			self.charges = self.charges - 1
			if self.charges <= 0 then

			end
		end
	end

	function self.onPhysics(dt)
	end

	function self.teardown()
		context.weapon_active = false
	end

	return self
end

TorchExplosionState = {}

TorchExplosionState.new = function(context) 
	local self = State.new()

	function self.setup()
		self.explosion = createAnimated("Assets/Potion 'Sploder/explosion_animation.json", 0, 0, 300, 300)
		setChild(entityId, self.explosion)
		setAnimationZOrder(self.explosion, 14)
		playAnimation(self.explosion)
		playSound("torch_detonate", -1, 0)

		local ids = checkCollisions(entityId)
		for index=0,ids:size()-1 do
			local tag = getTag(ids:at(index))
			if tag == "SKELETON" then
				sendMessage(ids:at(index), "DETONATE", "TRUE")
			end
		end
	end

	function self.update(dt)
		if isAnimationPlaying(self.explosion) == false then
			context.stateMachine.popState()
		end
	end

	function self.onPhysics(dt)

	end

	function self.teardown()
		removeChild(entityId, self.explosion)
		destroyEntity(self.explosion)
	end

	return self
end