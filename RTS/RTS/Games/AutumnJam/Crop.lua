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
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
}

function setup()
	print("setup Crop")
	selected = false

	CROPS = {}
	CROPS["BEET"] = {
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/beetroot_01.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/beetroot_02.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/beetroot_03.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/beetroot_04.png",
	}

	CROPS["CABBAGE"] = {
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/cabbage_01.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/cabbage_02.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/cabbage_03.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/cabbage_04.png",
	}

	CROPS["CARROT"] = {
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/carrot_01.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/carrot_02.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/carrot_03.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/carrot_04.png",
	}

	CROPS["RADISH"] = {
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/radish_01.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/radish_02.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/radish_03.png",
		"Assets/AutumnJam/SUNNYSIDE_WORLD_CROPS_V0.01/ASSETS/radish_04.png",
	}

	CROPS["CROP"] = {}
	CROPS["CROP"]["TYPE"] = "EMPTY"
	CROPS["CROP"]["CAN_HARVEST"] = false
	CROPS["CROP"]["WATER"] = 0
	CROPS["CROP"]["COMPOST"] = 0
	CROPS["CROP"]["STAGE"] = 1
	CROPS["CROP"]["ENTITY"] = 0
	CROPS["CROP"]["PROGRESS_ENTITY"] = 0
	CROPS["CROP"]["TIME"] = 0

	GROWTH_STEP_TIME = 500
	GROWTH_STEP = 25
	GROWTH_MAX = 100
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)

end

function onKeyUp(keyId, ctrl, shft)
	if selected and CROPS["CROP"]["TYPE"] == "EMPTY" then
		if keyId == SDLK_1 then
			createCrop("BEET")
		elseif keyId == SDLK_2 then
			createCrop("CABBAGE")
		elseif keyId == SDLK_3 then
			createCrop("CARROT")
		elseif keyId == SDLK_4 then
			createCrop("RADISH")
		end
		print("child " .. tostring(CROPS["CROP"]["ENTITY"]))
	end
end

-- Entity Mouse Events

function onMouseEnterEntity()
	setColor(entityId, 128, 128, 128, 255)
end

function onMouseExitEntity()
	setColor(entityId, 255, 255, 255, 255)
end

function onClickEntity(button)
	if button == 0 then
		broadcastMessage(entityId, "SELECTED", tostring(entityId))
		print("Selected " .. tostring(entityId))
		selected = true
	end
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
	if CROPS["CROP"]["CAN_HARVEST"] == true or CROPS["CROP"]["TYPE"] == "EMPTY" then
		return
	end

	CROPS["CROP"]["TIME"] = CROPS["CROP"]["TIME"] + delta
	if CROPS["CROP"]["TIME"] > GROWTH_STEP_TIME then
		CROPS["CROP"]["WATER"] = CROPS["CROP"]["WATER"] + GROWTH_STEP;
		if CROPS["CROP"]["WATER"] >= GROWTH_MAX then
			CROPS["CROP"]["STAGE"] = CROPS["CROP"]["STAGE"] + 1
			if CROPS["CROP"]["STAGE"] > 4 then
				CROPS["CROP"]["CAN_HARVEST"] = true
			else
				setTexture(CROPS[CROPS["CROP"]["TYPE"]][CROPS["CROP"]["STAGE"]], CROPS["CROP"]["ENTITY"], 32, 32, 0, 0, 16, 16)
			end
			CROPS["CROP"]["WATER"] = 0
		end

		setProgress(CROPS["CROP"]["PROGRESS_ENTITY"], CROPS["CROP"]["WATER"], GROWTH_MAX);

		CROPS["CROP"]["TIME"] = CROPS["CROP"]["TIME"] - GROWTH_STEP_TIME;
	end
end

function onBroadcast(message, value)
	if message == "SELECTED" then
		if selected then
			print("Deselected " .. tostring(entityId))
		end
		selected = false
	end
end

-- Custom Functions --

function createCrop(cropType)
	-- Create the crop entity
	CROPS["CROP"]["ENTITY"] = createTextured(CROPS[cropType][1], 0, 0, 32, 32, 0, 0, 16, 16, false)
	CROPS["CROP"]["TYPE"] = cropType
	setChild(entityId, CROPS["CROP"]["ENTITY"])
	setZOrder(CROPS["CROP"]["ENTITY"], 11)

	-- Create crop UI
	CROPS["CROP"]["PROGRESS_ENTITY"] = createProgress(0, -32, 64, 14, 100, 0)
	setChild(entityId, CROPS["CROP"]["PROGRESS_ENTITY"])
	setUI(CROPS["CROP"]["PROGRESS_ENTITY"], false)
end