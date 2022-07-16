include("Games/behaviors/State.lua")
include("Games/behaviors/Animation.lua")

ExploreState = {}

ExploreState.new = function(context) 
	local self = State.new()

	self.animationPlayer = AnimationPlayer.new()
	self.spriteState = SpriteState.new("Assets/BobTheSkull/GhostSkull_1.json", "idle")
	self.animationPlayer.pushConstantAnimation(SpriteAnimation.new(self.spriteState))

	self.clicked = false

	function self.setup()
		self.clicked = false
	end

	function self.update(dt)
		self.animationPlayer.update(dt)
		if inputState.mousePressed(MOUSE_BUTTON_LEFT) == true and self.clicked == false and self.animationPlayer.isPlaying() == false then
			self.clicked = true
			self.animationPlayer.pushAnimation(DisplayTextAnimation.new(inputState:mouseX(), inputState:mouseY(), 255, 174, 201, 25, "Hi Sweetie! <3", 2000))

		elseif inputState.mousePressed(MOUSE_BUTTON_LEFT) == false then
			self.clicked = false
		end

		if inputState.keyPressed(SDLK_SPACE) == true and self.animationPlayer.isPlaying() == false then
			self.animationPlayer.pushAnimation(ColorAnimation.new(255, 0, 0, 255, 0, 255, 0, 255, 800))
		elseif inputState.keyPressed(SDLK_1) == true and self.animationPlayer.isPlaying() == false then
			self.animationPlayer.pushAnimation(SizeChangeAnimation.new(16, 20, 64, 80, 800))
		end

		if inputState.keyPressed(SDLK_w) == true then
			self.spriteState.current_anim = "up"
		elseif inputState.keyPressed(SDLK_d) == true then
			self.spriteState.current_anim = "right"
		elseif inputState.keyPressed(SDLK_s) == true then
			self.spriteState.current_anim = "down"
		elseif inputState.keyPressed(SDLK_a) == true then
			self.spriteState.current_anim = "left"
		else
			self.spriteState.current_anim = "idle"
		end
	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end