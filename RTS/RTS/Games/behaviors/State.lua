

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

	-- Callbacks

	function self.onMessage(message, value)

	end

	function self.onBroadcast(message, value)

	end

	function self.onCollision(id)

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

	function self.onMessage(message, value)
		if #self.states > 0 then  
			self.states[#self.states].onMessage(message, value)
		end
	end

	function self.onBroadcast(message, value)
		if #self.states > 0 then  
			self.states[#self.states].onBroadcast(message, value)
		end
	end

	function self.onCollision(id)
		if #self.states > 0 then  
			self.states[#self.states].onCollision(id)
		end
	end

	return self
end