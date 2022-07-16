include("Games/behaviors/State.lua")
include("Games/behaviors/Util.lua")

HumanIdleState = {}

HumanIdleState.new = function(context) 
	local self = State.new()
	context.zombie = ArrayList.new(nil)

	function self.setup()
		setTag(entityId, "HUMAN")
		setColor(entityId, 0, 128, 0, 255)
		self.idleTimer = 0
	end

	function self.update(dt)
		self.idleTimer = self.idleTimer + dt
		if self.idleTimer > 3000 then
			context.stateMachine.pushState(context.humanMoveState)
		end
	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end

HumanMoveState = {}

HumanMoveState.new = function(context) 
	local self = State.new()

	self.speed = 100

	function self.setup()
		local xDiff = random(200) - 100
		local yDiff = random(200) - 100
		local pos = getPosition(entityId)
		xDiff = math.min(1024, math.max(0, xDiff + pos:getX()))
		yDiff = math.min(768, math.max(0, yDiff + pos:getY()))

		self.target = Vector2f.new(xDiff, yDiff)
	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		local step = self.speed * (dt / 1000.0)
		local pos = getPosition(entityId)
		local dir = Vector2f.new(self.target:getX() - pos:getX(), self.target:getY() - pos:getY())
		local dist = dir:magnitude()
		if dist < step then
			dir:normalize()
			dir:scale(dist)
			moveAndSlide(entityId, dir:getX(), dir:getY())
			context.stateMachine.popState()
		else
			dir:normalize()
			dir:scale(step)
			moveAndSlide(entityId, dir:getX(), dir:getY())
		end
	end

	function self.teardown()

	end

	function self.onCollision(id)
		local tag = getTag(id)
		if "ZOMBIE" == tag then
			if context.zombie.indexOf(id) == -1 then
				context.zombie.push(id)
			end
			context.stateMachine.pushState(context.humanRunState)
		end
	end

	return self
end

HumanRunState = {}

HumanRunState.new = function(context) 
	local self = State.new()
	self.velocity = Vector2f.new(0, 0)

	function self.setup()
		self.runSpeed = 50.0
		self.velocity:setX(0)
		self.velocity:setY(0)
	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		local step = dt / 1000.0

		local pos = getPosition(entityId)
		local flightDir = Vector2f.new(0, 0)

		local index = 0
		while index < context.zombie.length do
			local zombiePos = getPosition(context.zombie.at(index))
			zombiePos:subtract(pos)
			flightDir:add(zombiePos)

			index = index + 1
		end

		flightDir:normalize()
		flightDir:scale(-1.0)

		self.velocity:moveToward(flightDir:getX(), flightDir:getY(), 1000.0 * step * self.runSpeed);
		self.velocity:normalize()
		self.velocity:scale(self.runSpeed)

		moveAndSlide(entityId, self.velocity:getX() * step, self.velocity:getY() * step)
	end

	function self.teardown()

	end

	function self.onMessage(message, value)
		context.stateMachine.pushState(context.zombieIdleState)
	end

	function self.onCollision(id)
		local tag = getTag(id)
		if "ZOMBIE" == tag then
			if context.zombie.indexOf(id) == -1 then
				context.zombie.push(id)
			end
		end
	end

	return self
end

ZombieIdleState = {}

ZombieIdleState.new = function(context) 
	local self = State.new()

	function self.setup()
		setTag(entityId, "ZOMBIE")
		setColor(entityId, 128, 0, 0, 255)
		self.idleTimer = 0
		context.human = nil
	end

	function self.update(dt)
		self.idleTimer = self.idleTimer + dt
		if self.idleTimer > 3000 then
			context.stateMachine.pushState(context.zombieSearchMoveState)
		end
	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end

ZombieSearchMoveState = {}

ZombieSearchMoveState.new = function(context) 
	local self = State.new()

	self.speed = 25

	function self.setup()
		local xDiff = random(200) - 100
		local yDiff = random(200) - 100
		local pos = getPosition(entityId)
		xDiff = math.min(1024, math.max(0, xDiff + pos:getX()))
		yDiff = math.min(768, math.max(0, yDiff + pos:getY()))

		self.target = Vector2f.new(xDiff, yDiff)
	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		local step = self.speed * (dt / 1000.0)
		local pos = getPosition(entityId)
		local dir = Vector2f.new(self.target:getX() - pos:getX(), self.target:getY() - pos:getY())
		local dist = dir:magnitude()
		if dist < step then
			dir:normalize()
			dir:scale(dist)
			moveAndSlide(entityId, dir:getX(), dir:getY())
			context.stateMachine.popState()
		else
			dir:normalize()
			dir:scale(step)
			moveAndSlide(entityId, dir:getX(), dir:getY())
		end
	end

	function self.teardown()

	end

	function self.onCollision(id)
		local tag = getTag(id)
		if "HUMAN" == tag then
			context.human = id
			context.stateMachine.pushState(context.zombieAttackMoveState)
		end
	end

	return self
end

ZombieAttackMoveState = {}

ZombieAttackMoveState.new = function(context) 
	local self = State.new()

	function self.setup()
		self.runSpeed = 45.0
		local humanPos = getPosition(entityId)
		self.velocity = getPosition(context.human)
		self.velocity:subtract(humanPos)
		self.velocity:normalize()
		self.velocity:scale(self.runSpeed)
	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		local step = dt / 1000.0

		local humanPos = getPosition(context.human)
		local pos = getPosition(entityId)
		humanPos:subtract(pos)

		if humanPos:magnitude() < 48 then
			context.stateMachine.pushState(context.zombieAttackState)
			return
		end

		self.velocity:moveToward(humanPos:getX(), humanPos:getY(), step * self.runSpeed);
		self.velocity:normalize()
		self.velocity:scale(self.runSpeed)

		moveAndSlide(entityId, self.velocity:getX() * step, self.velocity:getY() * step)
	end

	function self.teardown()

	end

	return self
end

ZombieAttackState = {}

ZombieAttackState.new = function(context) 
	local self = State.new()

	self.LEAP_STATE = 1
	self.RECOVER_STATE = 2

	function self.setup()
		self.runSpeed = 145.0
		local humanPos = getPosition(entityId)
		self.leapVector = getPosition(context.human)
		self.leapEndPos = Vector2f.new(self.leapVector:getX(), self.leapVector:getY())
		self.leapVector:subtract(humanPos)
		self.leapVector:normalize()
		self.leapVector:scale(self.runSpeed)
		self.state = self.LEAP_STATE
	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		local step = dt / 1000.0

		if self.state == self.LEAP_STATE then
			local humanPos = getPosition(context.human)
			local pos = getPosition(entityId)
			humanPos:subtract(pos)
			if humanPos:magnitude() < (step * self.runSpeed) then
				self.leapVector:normalize()
				self.leapVector:scale(step * self.runSpeed)
				moveAndSlide(entityId, self.leapVector:getX(), self.leapVector:getY())
				self.state = self.RECOVER_STATE
				self.recoverTime = 0
				sendMessage(context.human, "ZOMBIE_BITE", "True")
			else
				moveAndSlide(entityId, self.leapVector:getX() * step, self.leapVector:getY() * step)
			end
		elseif self.state == self.RECOVER_STATE then
			self.recoverTime = self.recoverTime + dt
			if self.recoverTime > 3000 then
				context.stateMachine.popState()
				context.stateMachine.popState()
			end
		end
	end

	function self.teardown()

	end

	return self
end