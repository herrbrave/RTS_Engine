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

	print("setup inventory_item", x, y)
	inventory_data = "Games/test/data_test.csv"
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

	if "set_coords" then
		x, y = value:match("([^,]+),([^,]+)")
		inventory_item_slot_name = "inventory " .. x .. "x".. y
	end

	return "Ok"
end

function update(delta)

	inventory_item = getData(inventory_data, inventory_item_slot_name)
	print("inventory_item", inventory_item)
	if string.len(inventory_item) > 0 then
		updateIcon(inventory_item)
	end
end

-- Custom methods

function updateIcon(icon)
	if "POTION" == icon then
		setTexture("Assets/HackNSlasher/Items/Tools.png", entityId, 55, 55, 16, 48, 16, 16)
	elseif "KEY" == icon then
		setTexture("Assets/HackNSlasher/Items/Tools.png", entityId, 55, 55, 128, 0, 16, 16)
	elseif "HAM" == icon then
		setTexture("Assets/HackNSlasher/Items/Tools.png", entityId, 55, 55, 64, 32, 16, 16)
	end
end