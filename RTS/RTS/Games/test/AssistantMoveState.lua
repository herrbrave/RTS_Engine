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
		if context.keys[SDLK_SPACE] then
			context.stateMachine.pushState(self.dashState)
		elseif context.keys[SDLK_e] then
			collisions = checkCollisions(entityId)
			onBox = false
			for _i=0,collisions:size()-1 do
				self.collisionTag = getTag(collisions:at(_i))
				if self.collisionTag == "BOX" then
					sendMessage(collisions:at(_i), "OPEN_CHEST", "open")
				end
			end
		elseif context.keys[SDLK_2] and context.inventorySize() > 0 then
			if context.inventoryCharge == 100 then
				local pos = getPosition(entityId)
				item = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 185, 700, 64, 64, 112, 112, 16, 16)
				setPosition(item, pos:getX(), pos:getY())
				setScript(item, "Games/test/BonesIngredient.lua")
				context.inventoryPop()
				context.inventoryCharge = 0
			end
		end

		if context.keys[SDLK_w] then
			context.input:setY(-1)
		elseif context.keys[SDLK_s] then
			context.input:setY(1)
		else
			context.input:setY(0)
		end

		if context.keys[SDLK_a] then
			context.input:setX(-1)
		elseif context.keys[SDLK_d] then
			context.input:setX(1)
		else
			context.input:setX(0)
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
			if context.health > 0 then
				context.stateMachine.popState()
			else
				context.stateMachine.pushState(DeadState.new(context))
			end
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

DeadState = {}

DeadState.new = function(context) 
	local self = State.new()

	self.deadText = createLabel("You're DEAD!", 35, 640, 348)
	self.timer = 0

	function self.setup()
		context.state = DEAD
		setColor(entityId, 64, 64, 64, 128)
		broadcastMessage(entityId, "PLAYER_DEAD", "TRUE")
	end

	function self.update(dt)
		self.prog = math.cos((self.timer / 1000.0) * 2 * math.pi)

		self.offset = self.prog * 128.0

		setColor(self.deadText, 255, math.floor(128 - self.prog), math.floor(128 - self.prog), 255)

		self.timer = (self.timer + dt) % 1000
	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end