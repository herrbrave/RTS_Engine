include("Games/behaviors/State.lua")
include("Games/behaviors/Animation.lua")
include("Games/behaviors/Ui.lua")

RunnerState = {}

RunnerState.new = function(context) 
	local self = State.new()

	self.clicked = false

	function self.update(dt)

		if self.clicked == false and inputState.mousePressed(MOUSE_BUTTON_LEFT) == true then
			self.clicked = true
			local x = inputState.mouseX()
			local y = inputState.mouseY()
			pushTextureAtPoint(x, y, "Assets/test/Sprites/Dungeon_Tileset.png", 8*16, 4*16, 16, 16)

		elseif inputState.mousePressed(MOUSE_BUTTON_LEFT) == false then
			self.clicked = false
		end 

	end

	function self.onPhysics(dt)

	end

	function self.teardown()

	end

	return self
end