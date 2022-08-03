
TextPanel = {}
TextPanel.new = function(x, y, width, height, text, font_size, zorder)
	local self = setmetatable({}, TextPanel)

	self.panel_image_path = panel_image_path
	self.text = text
	self.text_count = #self.text + 1
	self.font_size = font_size
	self.zorder = zorder
	self.tx = tx
	self.ty = ty
	self.tw = tw
	self.th = th
	self.r = 128
	self.g = 0
	self.b = 0
	self.width = width
	self.height = height
	self.x = x
	self.y = y

	self.panel_id = createPanel(self.x, self.y, self.width, self.height)
	setUI(self.panel_id, true)

	self.text_ids = {}
	local index = 0
	while index < self.text_count do
		self.text_ids[index] = {}
		self.text_ids[index].id = createPhysics(self.x, self.y, 1, 1)
		self.text_ids[index].highlighted = false
		
		setChild(self.panel_id, self.text_ids[index].id)
		index = index + 1
	end

	function self.redraw()
		setPosition(self.panel_id, self.x, self.y)
		setSize(self.panel_id, self.width, self.height)
		setPanelZOrder(self.panel_id, self.zorder)

		local text_y = math.max(-(self.height / 2) + (self.font_size / 2) + 2, -((#self.text_ids + 1) * self.font_size) / 2)
		local index = 0
		while index < self.text_count do
			if self.text_ids[index].highlighted == true then
				setLabelText(self.text_ids[index].id, self.text[index], self.font_size, 255 - self.r, 255 - self.g, 255 - self.b)
			else
				setLabelText(self.text_ids[index].id, self.text[index], self.font_size, self.r, self.g, self.b)
			end
			setPosition(self.text_ids[index].id, 0, text_y)
			setBodySize(self.text_ids[index].id, self.width, self.font_size)
			-- For some reason I have to bump up the zorder by 100... investigate that later.
			setLabelZOrder(self.text_ids[index].id, self.zorder + 100)
			text_y = text_y + self.font_size + 2
			index = index + 1
		end
	end

	self.redraw()

	return self
end

SelectionPanel = {}
SelectionPanel.new = function(x, y, width, height, text, font_size, zorder)
	local self = TextPanel.new(x, y, width, height, text, font_size, zorder)

	self.highlighted = -1
	self.clicked = false
	self.onClick = function(text)
		print("clicked on", text)
	end

	function self.update(dt)
		if checkPoint(self.panel_id, inputState.mouseX(), inputState.mouseY()) == false then
			return
		end

		if self.checkHighlighted() == false then
			local index = 0
			local max = #self.text_ids + 1
			local mx = inputState.mouseX()
			local my = inputState.mouseY()
			while index < max do
				if checkPoint(self.text_ids[index].id, mx, my) == true then
					self.setHighlighted(index)
					return
				end
				index = index + 1
			end

			self.setHighlighted(-1)
		elseif self.highlighted ~= -1 then

			if self.clicked == false and inputState.mousePressed(MOUSE_BUTTON_LEFT) == true then
				self.clicked = true
				self.onClick(self.text[self.highlighted])
			elseif inputState.mousePressed(MOUSE_BUTTON_LEFT) == false then
				self.clicked = false
			end
		end
	end

	function self.checkHighlighted()
		if self.highlighted == -1 then
			return false
		end

		return checkPoint(self.text_ids[self.highlighted].id, inputState.mouseX(), inputState.mouseY())
	end

	function self.setHighlighted(index)
		if self.highlighted ~= -1 then
			self.text_ids[self.highlighted].highlighted = false
		end

		if index ~= -1 then
			self.text_ids[index].highlighted = true
		end
		self.redraw()
		self.highlighted = index
	end

	return self
end