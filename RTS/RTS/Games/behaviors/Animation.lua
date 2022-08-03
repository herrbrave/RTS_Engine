include("Games/behaviors/Util.lua")

AnimationPlayer = {}
AnimationPlayer.new = function() 
	local self = setmetatable({}, AnimationPlayer)

	self.animations = ArrayList.new()
	self.constantAnimations = ArrayList.new()
	self.newAnimation = true

	function self.update(dt)
		if self.constantAnimations.length > 0 then
			local index = 0
			while index < self.constantAnimations.length do
				local anim = self.constantAnimations.at(index)
				anim.update(dt)
				index = index + 1
			end
		end

		if self.animations.length <= 0 then
			return
		end

		local animation = self.animations.at(0)
		
		if self.newAnimation then
			animation.play()
			self.newAnimation = false
		end

		animation.update(dt)

		if animation.isPlaying() == false then
			animation.teardown()
			self.animations.removeAt(0)
			self.newAnimation = true
		end
	end

	function self.pushAnimation(animation)
		self.animations.push(animation)
	end

	function self.pushConstantAnimation(animation)
		self.constantAnimations.push(animation)
	end

	function self.clearConstantAnimation()
		self.constantAnimations.clear()
	end

	function self.isPlaying()
		if self.animations.length == 0 then
			return false
		end

		return self.animations.at(0).isPlaying()
	end

	return self
end

Animation = {}
Animation.new = function()
	local self = setmetatable({}, Animation)

	self.playing = false
	self.loop = false

	function self.isPlaying()
		return self.playing
	end

	function self.play()
		self.playing = true
		self.onPlay()
	end

	function self.onPlay()

	end

	function self.stop()
		self.playing = false
		self.onStop()
	end

	function self.onStop()

	end

	function self.teardown()

	end

	function self.update(dt)

	end

	return self
end

TranslateAnimation = {}
TranslateAnimation.new = function(startPos, endPos, ticks)
	local self = Animation.new()

	self.startPos = startPos
	self.endPos = endPos
	self.endTime = ticks + 0.0
	self.time = 0
	function self.onPlay()
		self.time = 0
		setPosition(entityId, self.startPos:getX(), self.startPos:getY())
	end

	function self.onStop()
		setPosition(entityId, self.endPos:getX(), self.endPos:getY())
	end

	function self.update(dt)
		self.time = self.time + dt
		if self.time > self.endTime then
			self.stop()
			return
		end
		local lerp = self.time / self.endTime
		local prog = Vector2f.new(self.endPos:getX(), self.endPos:getY())
		prog:subtract(self.startPos)
		prog:scale(lerp)

		setPosition(entityId, self.startPos:getX() + prog:getX(), self.startPos:getY() + prog:getY())
		prog = nil
	end

	function self.teardown()
		self.startPos = nil
		self.endPos = nil
	end

	return self
end

ColorAnimation = {}
ColorAnimation.new = function(sr, sg, sb, sa, er, eg, eb, ea, ticks)
	local self = Animation.new()

	self.sr = sr
	self.sg = sg
	self.sb = sb
	self.sa = sa
	self.er = er
	self.eg = eg
	self.eb = eb
	self.ea = ea
	self.endTime = ticks + 0.0
	self.time = 0

	function self.onPlay()
		self.time = 0
		setColor(entityId, self.sr, self.sg, self.sb, self.sa)
	end

	function self.onStop()
		setColor(entityId, self.er, self.eg, self.eb, self.ea)
	end

	function self.update(dt)
		self.time = self.time + dt
		if self.time > self.endTime then
			self.stop()
		end

		local prog = self.time / self.endTime
		local diff_r = (self.er - self.sr) * prog + self.sr
		local diff_g = (self.eg - self.sg) * prog + self.sg
		local diff_b = (self.eb - self.sb) * prog + self.sb
		local diff_a = (self.ea - self.sa) * prog + self.sa
		setColor(entityId, math.floor(diff_r), math.floor(diff_g), math.floor(diff_b), math.floor(diff_a))
	end

	return self
end

SizeChangeAnimation = {}
SizeChangeAnimation.new = function(bx, by, ex, ey, ticks)
	local self = Animation.new()

	self.bx = bx
	self.by = by
	self.ex = ex
	self.ey = ey
	self.endTime = ticks + 0.0
	self.time = 0

	function self.onPlay()
		self.time = 0
		setSize(entityId, self.bx, self.by)
	end

	function self.onStop()
		setSize(entityId, self.ex, self.ey)
	end

	function self.update(dt)
		self.time = self.time + dt
		if self.time > self.endTime then
			self.stop()
		end

		local prog = self.time / self.endTime
		local diff_x = (self.ex - self.bx) * prog + self.bx
		local diff_y = (self.ey - self.by) * prog + self.by
		setSize(entityId, math.floor(diff_x), math.floor(diff_y))
	end

	return self
end

DisplayTextAnimation = {}
DisplayTextAnimation.new = function(x, y, r, g, b, fontSize, text, ticks)
	local self = Animation.new()

	self.text = ""
	self.totalText = text
	self.time = 0.0
	self.endTime = ticks + 0.0
	self.fullTextTime = math.floor(self.endTime * 0.7)
	self.r = r
	self.g = g
	self.b = b
	self.fontSize = fontSize

	function self.onPlay()
		self.text = ""
		self.time = 0.0
		self.textObj = createPhysics(x, y, 1, 1)
		setLabelText(self.textObj, self.text, self.fontSize, self.r, self.g, self.b)
		setCircleCollision(self.textObj, 1)
	end

	function self.onStop()
		setLabelText(self.textObj, self.totalText, self.fontSize, self.r, self.g, self.b)
		destroyEntity(self.textObj)
	end

	function self.update(dt)
		self.time = self.time + dt
		if self.time > self.endTime then
			self.stop()
		end
		if self.time > self.fullTextTime then
			setLabelText(self.textObj, self.totalText, self.fontSize, self.r, self.g, self.b)
			return
		end

		local lerp = math.floor(string.len(self.totalText) * (self.time / self.fullTextTime))
		self.text = string.sub(self.totalText, 1, math.max(lerp, 1))
		setLabelText(self.textObj, self.text, self.fontSize, self.r, self.g, self.b)
	end

	return self
end

SpriteState = {}
SpriteState.new = function(anim_path, default_anim)
	local self = setmetatable({}, SpriteState)

	self.anim_path = anim_path
	self.default_anim = default_anim
	self.current_anim = self.default_anim
	self.playing = true

	return self
end

SpriteAnimation = {}
SpriteAnimation.new = function(state)
	local self = Animation.new()

	self.state = state
	attachAsepriteAnimationSet(entityId, self.state.anim_path)

	function self.onPlay()
		self.last_anim = self.state.current_anim
		setAnimation(entityId, self.state.current_anim)
		loopAnimation(entityId)
	end

	function self.update(dt)
		if self.last_anim ~= self.state.current_anim then
			setAnimation(entityId, self.state.current_anim)
			loopAnimation(entityId)
		end

		self.last_anim = self.state.current_anim
	end

	function self.teardown()
		self.startPos = nil
		self.endPos = nil
	end

	return self
end