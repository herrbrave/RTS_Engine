registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()
	background = createDefault(512, 384, 512, 512, 128, 128, 128, 128)
	--setScript(obj1, "Games/test/message_test_1.lua")
	inventory = {}
	for i=0,63 do
		x = i % 8
		y = math.floor(i / 8)
		slots = {}
		--slots[0] = createDefault((x * 64) - 224, (y * 64) - 224, 60, 60, 255, 255, 255, 255)
		slots[0] = createTextured("Assets/HackNSlasher/Menu/Menu.png", (x * 64) - 224, (y * 64) - 224, 60, 60, 37, 53, 22, 22, false)
		slots[1] = createTextured("Assets/HackNSlasher/Menu/Menu.png", 0, 0, 55, 55, 33, 27, 1, 1, false)
		print("Making Cell:", x, y, (x * 64) - 224, (y * 64) - 224)
		inventory[i] = slots
		setChild(background, slots[0])
		setChild(slots[0], slots[1])
		setScript(slots[1], "Games/test/inventory_item.lua")
		sendMessage(slots[1], "set_coords", tostring(x) .. "," .. tostring(y))
	end

	inventory_data = "Games/test/data_test.csv"
	loadData(inventory_data)
	putData(inventory_data, "inventory 0x0", "POTION")
	putData(inventory_data, "inventory 1x0", "KEY")
	putData(inventory_data, "inventory 2x0", "HAM")


	--setScript(obj2, "Games/test/message_test_2.lua")
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

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

-- Collition Callback --

function onCollision(id)

end

-- Messasge Callback --

function onMessage(message, value)

	return "Ok"
end

function update(delta)

end
