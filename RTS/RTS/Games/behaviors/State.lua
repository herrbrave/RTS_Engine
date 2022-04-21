

State = {}

State.new = function()
	local self = setmetatable({}, State)

	function self.setup()

	end

	function self.update(dt)

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end


StateMachine = {}

StateMachine.new = function() 
	local self = setmetatable({}, StateMachine)
	self.states = {}

	function self.pushState(state)
		if #self.states > 0 then
			self.states[#self.states].teardown()
		end

		state.setup()

		table.insert(self.states, state)
	end

	function self.peekState()
		return self.states[#self.states]
	end

	function self.popState()
		if #self.states > 0 then
			self.states[#self.states].teardown()
			table.remove(self.states, #self.states)
		end

		if #self.states > 0 then
			self.states[#self.states].setup()
		end
	end

	function self.update(dt)
		if #self.states > 0 then  
			self.states[#self.states].update(dt)
		end
	end

	function self.onPhysics(dt)
		if #self.states > 0 then  
			self.states[#self.states].onPhysics(dt)
		end
	end

	return self
end