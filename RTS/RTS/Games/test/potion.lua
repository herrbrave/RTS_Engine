registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
}


function setup()
	print("setup Potion", tostring(entityId))
	setCircleCollision(entityId, 48)

	context = {}
	context.position = getPosition(entityId)

	context.healthLabel = createLabel("label", 25, 500, 70)
	setLabelText(context.healthLabel, "Unstable", 25, 255, 255, 255)
	setLabelZOrder(context.healthLabel, 100)
	context.healthProgress = createProgress(790, 70, 400, 25, 100, 100)


	include("Games/behaviors/State.lua")

	STABLE = 1
	UNSTABLE = 2
	VOLITILE = 3
	BOOM = 4

	StableState = {}

	StableState.new = function(context) 
		local self = State.new()

		self.context = context

		self.hover_value = 0
		self.hover_distance = 10
		self.life_span = 30 * 1000 -- 30 seconds
		self.max_life = 250
		self.life = 0

		function self.setup()
			self.context.state = STABLE
			self.hover_value = 0
			self.hover_distance = 10
			self.max_life = 250
			self.life = 0
			setLabelText(self.context.healthLabel, "Stable", 25, 255, 255, 255)
			setProgress(self.context.healthProgress, self.max_life, self.max_life)
		end

		function self.update(dt)
			if self.life > self.life_span then
				self.context.stateMachine.pushState(self.context.states[UNSTABLE])
			end

			self.health = self.max_life * (1 - ((self.life + 0.0) / (self.life_span + 0.0)))
			setProgress(self.context.healthProgress, math.floor(self.health), self.max_life)
			self.life = self.life + dt
		end

		function self.onPhysics(dt)
			self.prog = math.cos((self.hover_value / 2000.0) * 2 * math.pi)

			self.offset = self.prog * 10.0

			setPosition(entityId, context.position:getX(), context.position:getY() + self.offset)
			setSize(entityId, 96 - self.offset, 96 - self.offset)

			self.hover_value = (self.hover_value + dt) % 2000
		end

		function self.teardown()

		end

		function self.heal(ingredient)
			self.life = math.max(0, self.life - 5000)
		end

		return self
	end

	UnstableState = {}

	UnstableState.new = function(context) 
		local self = State.new()

		self.context = context


		self.hover_value = 0
		self.hover_distance = 10
		self.life_span = 30 * 1000 -- 30 seconds
		self.max_life = 250
		self.life = 0

		function self.setup()
			self.context.state = UNSTABLE
			self.hover_value = 0
			self.hover_distance = 10
			self.max_life = 250
			self.life = 0
			setLabelText(self.context.healthLabel, "Unstable", 30, 255, 128, 128)
			setProgress(self.context.healthProgress, self.max_life, self.max_life)
		end

		function self.update(dt)
			if self.life > self.life_span then
				self.context.stateMachine.pushState(self.context.states[VOLITILE])
			end

			self.health = self.max_life * (1 - ((self.life + 0.0) / (self.life_span + 0.0)))
			setProgress(self.context.healthProgress, math.floor(self.health), self.max_life)
			self.life = self.life + dt

		end

		function self.onPhysics(dt)
			self.prog = math.cos((self.hover_value / 1000.0) * 2 * math.pi)

			self.offset = self.prog * 20.0

			setPosition(entityId, self.context.position:getX(), self.context.position:getY() + self.offset)
			setSize(entityId, 96 - self.offset, 96 - self.offset)

			self.hover_value = (self.hover_value + dt) % 1000
		end

		function self.teardown()

		end

		function self.heal(ingredient)
			self.life = math.min(self.max_life, self.life + 25)
		end

		return self
	end

	VolitileState = {}

	VolitileState.new = function(context) 
		local self = State.new()

		self.context = context


		self.hover_value = 0
		self.hover_distance = 10
		self.life_span = 30 * 1000 -- 30 seconds
		self.max_life = 250
		self.life = 0

		function self.setup()
			self.context.state = UNSTABLE
			self.hover_value = 0
			self.hover_distance = 10
			self.max_life = 250
			self.life = 0
			setLabelText(self.context.healthLabel, "Volitile", 40, 255, 32, 32)
			setProgress(self.context.healthProgress, self.max_life, self.max_life)
		end

		function self.update(dt)
			if self.life > self.life_span then
				--self.context.stateMachine.pushState(self.context.states[UNSTABLE])
				return
			end

			self.health = self.max_life * (1 - ((self.life + 0.0) / (self.life_span + 0.0)))
			setProgress(self.context.healthProgress, math.floor(self.health), self.max_life)
			self.life = self.life + dt

		end

		function self.onPhysics(dt)
			self.prog = math.cos((self.hover_value / 500.0) * 2 * math.pi)

			self.angleProg = math.cos((self.hover_value / 250.0) * 2 * math.pi)
			self.offset = self.prog * 30.0
			self.angle = self.angleProg * 20.0

			setPosition(entityId, self.context.position:getX(), self.context.position:getY() + self.offset)
			setSize(entityId, 96 - self.offset, 96 - self.offset)
			setAngle(entityId, self.angle)

			self.hover_value = (self.hover_value + dt) % 500
		end

		function self.teardown()

		end

		function self.heal(ingredient)
			self.life = math.min(self.max_life, self.life + 25)
		end

		return self
	end

	context.states = {}
	context.states[STABLE] = StableState.new(context)
	context.states[UNSTABLE] = UnstableState.new(context)
	context.states[VOLITILE] = VolitileState.new(context)

	context.stateMachine = StateMachine.new()
	context.stateMachine.pushState(context.states[STABLE])
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)

end

function onKeyUp(keyId, ctrl, shft)

end

-- Entity Mouse Events

function onMouseEnterEntity()

end

function onMouseExitEntity()

end

function onClickEntity(button)

end

function onDragEntity(button)

end

-- Mouse Move Events --
function onMouseMove(x, y, button)

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	context.stateMachine.update(delta)
end

function onBroadcast(message, value)
	if "INGREDIENT_ADDED" then
		context.stateMachine.peekState().heal(value)
	end
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end