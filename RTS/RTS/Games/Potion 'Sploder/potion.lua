registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
	SOUND = 1,
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

	MenuState = {}

	MenuState.new = function(context) 
		local self = State.new()

		self.context = context

		function self.setup()
			self.back = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 512, 384, 1024, 768, 128, 112, 16, 16)
			setZOrder(self.back, 200)
			self.panel = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 512, 384, 1014, 758, 144, 112, 16, 16)
			setZOrder(self.panel, 201)
			self.titleLabel = createLabel("POTION 'SPLODER", 35, 512, 32)
			setLabelZOrder(self.titleLabel, 202)
			self.playLabel = createLabel("PRESS SPACE TO PLAY", 24, 512, 725)
			setLabelZOrder(self.playLabel, 202)


			self.controlsLabel = createLabel("CONTROLS", 22, 64, 384)
			setLabelZOrder(self.controlsLabel, 202)
			self.movementControlsLabel = createLabel("MOVEMENT - WASD", 22, 100, 404)
			setLabelZOrder(self.movementControlsLabel, 202)
			self.torchControlsLabel = createLabel("DETONATE TORCH - 1", 22, 115, 424)
			setLabelZOrder(self.torchControlsLabel, 202)
			self.feedControlsLabel = createLabel("FEED POTION - 2", 22, 92, 444)
			setLabelZOrder(self.feedControlsLabel, 202)
			self.chestControlsLabel = createLabel("OPEN CHEST - E", 22, 88, 464)
			setLabelZOrder(self.chestControlsLabel, 202)


			self.necromancer = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Character_2.png", 300, 384, 64, 64, 48, 16, 16, 16)
			setZOrder(self.necromancer, 202)
			self.apprentance = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Character_2.png", 700, 384, 64, 64, 96, 0, 16, 16)
			setZOrder(self.apprentance, 202)
			self.potion = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 512, 384, 128, 128, 144, 128, 16, 16)
			setZOrder(self.potion, 202)

			self.necromanceSpeechLabel = createLabel("MINION! TEND TO MY POTION!", 22, 300, 300)
			setLabelZOrder(self.necromanceSpeechLabel, 202)
		end

		function self.update(dt)
			if context.keys[SDLK_SPACE] then
				context.stateMachine.pushState(context.states[STABLE])
			end
		end

		function self.onPhysics(dt)

		end

		function self.teardown()
			destroyEntity(self.back)
			destroyEntity(self.panel)
			destroyEntity(self.titleLabel)
			destroyEntity(self.playLabel)
			destroyEntity(self.controlsLabel)
			destroyEntity(self.movementControlsLabel)
			destroyEntity(self.torchControlsLabel)
			destroyEntity(self.feedControlsLabel)
			destroyEntity(self.chestControlsLabel)
			destroyEntity(self.necromancer)
			destroyEntity(self.apprentance)
			destroyEntity(self.potion)
			destroyEntity(self.necromanceSpeechLabel)
			context.gameStarted = true
		end

		return self
	end

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
			if context.gameOver then
				return
			end
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
			if context.gameOver then
				return
			end
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
			self.life = math.max(0, self.life - 3500)
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
			if context.gameOver then
				return
			end
			if self.life > self.life_span then
				self.context.stateMachine.pushState(BoomState.new(context))
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
			self.life = math.max(0, self.life - 5000)
		end

		return self
	end

	BoomState = {}

	BoomState.new = function(context) 
		local self = State.new()

		self.context = context

		function self.setup()
			self.explosion = createAnimated("Assets/Potion 'Sploder/explosion_animation.json", 0, 0, 1024, 1024)
			setChild(entityId, self.explosion)
			setAnimationZOrder(self.explosion, 18)
			playAnimation(self.explosion)
			playSound("potion_splosion", -1, 0)
			setLabelText(self.context.healthLabel, "BOOM", 45, 255, 32, 32)
		end

		function self.update(dt)
			if self.explosion ~= 0 and isAnimationPlaying(self.explosion) == false then
				destroyEntity(self.explosion)
				broadcastMessage(entityId, "DETONATE", "BOOM")
				self.explosion = 0
				destroyEntity(entityId)
			end
		end

		function self.onPhysics(dt)
		end

		function self.teardown()

		end

		function self.heal(ingredient)
		end

		return self
	end

	context.states = {}
	context.states[STABLE] = StableState.new(context)
	context.states[UNSTABLE] = UnstableState.new(context)
	context.states[VOLITILE] = VolitileState.new(context)

	context.keys = {}

	context.stateMachine = StateMachine.new()
	context.stateMachine.pushState(MenuState.new(context))

	context.gameStarted = false
	context.gameOver = false
	context.timer = 1 * 60 * 1000
	context.timerLabel = createLabel("--:--", 22, 512, 450)
	setLabelZOrder(context.timerLabel, 202)

	loadSound("Assets/Potion 'Sploder/Sounds/potion_splosion.wav", "potion_splosion")
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	context.keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	context.keys[keyId] = false
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
	updateTimer(delta)
end

function onBroadcast(message, value)
	if "INGREDIENT_ADDED" then
		context.stateMachine.peekState().heal(value)
	end
end

function onPhysics(delta)
	context.stateMachine.onPhysics(delta)
end

function updateTimer(dt)
	if context.gameStarted then
		context.timer = context.timer - dt
	end
	if context.timer <= 0 then
		context.gameOver = true
	    createLabel("You Survived!", 35, 512, 348)
	end

	local timer = context.timer
	local minutes = math.floor(timer / (60 * 1000))
	timer = timer - ((60 * 1000) * minutes)
	local seconds = math.floor(timer / 1000)
	timer = math.floor(timer - (1000 * seconds))

	local minutesString = "0" .. tostring(minutes)
	local secondsString = tostring(seconds)
	if seconds < 10 then
		secondsString = "0" .. secondsString
	end
	local millisecondsString = tostring(timer)
	if timer < 10 then
		millisecondsString = "00" .. millisecondsString
	elseif timer < 100 then
		millisecondsString = "0" .. millisecondsString
	end
	
	setLabelText(context.timerLabel, minutesString .. ":" .. secondsString .. ":" .. millisecondsString, 22, 255, 255, 255)
end