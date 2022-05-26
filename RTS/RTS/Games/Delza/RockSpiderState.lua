include("Games/behaviors/State.lua")

IdleState = {}

IdleState.new = function(context) 
	local self = State.new()

	self.context = context

	self.speed = 200

	self.position = getPosition(entityId)
	self.idle_pattern = {}
	self.idle_pattern[0] = Vector2f.new(self.position:getX() + 200, self.position:getY())
	self.idle_pattern[1] = Vector2f.new(self.position:getX() - 200, self.position:getY())
	self.idle_target = 0

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		self.position = getPosition(entityId)
		self.move_dir = Vector2f.new(self.idle_pattern[self.idle_target]:getX() - self.position:getX(), self.idle_pattern[self.idle_target]:getY() - self.position:getY())
		self.mag = self.move_dir:magnitude()
		self.move_dir:normalize()
		self.move_dir:scale((dt / 1000) * self.speed)

		if self.mag <= self.move_dir:magnitude() then
			setPosition(entityId, self.move_dir:getX() + self.position:getX(), self.move_dir:getY() + self.position:getY())
			self.idle_target = (self.idle_target + 1) % 2
		else
			moveAndSlide(entityId, self.move_dir:getX(), self.move_dir:getY())
		end
	end

	function self.teardown()

	end

	return self
end


AttackState = {}

AttackState.new = function(context) 
	local self = State.new()

	self.context = context

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)
		self.collisions = checkCollisions(entityId)
		self.still_in_range = false
		for index=0,self.collisions:size()-1 do
			if self.collisions:at(index) == self.context.target then
				still_in_range = true
				break
			end
		end

		if self.still_in_range == false then
			self.context.stateMachine.popState()
			return
		end

		position = getPosition(entityId)
		target_position = getPosition(target)
		move_dir = Vector2f.new(target_position:getX() - position:getX(), target_position:getY() - position:getY())
		if move_dir:magnitude() <= 45 then
			sendMessage(target, "HURT", tostring(entityId))
		end

		move_dir:normalize()
		move_dir:scale((delta / 1000) * 185)
		moveAndSlide(entityId, move_dir:getX(), move_dir:getY())
	end

	function self.teardown()

	end

	return self
end