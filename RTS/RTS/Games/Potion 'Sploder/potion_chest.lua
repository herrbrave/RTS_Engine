registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 0,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
	WORLD = 0,
	SOUND = 1,
}


function setup()
	print("setup Potion Chest", tostring(entityId))

	setCircleCollision(entityId, 24)
	setTag(entityId, "BOX")

	math.randomseed(os.time())

	CLOSE_STATE = 1
	OPEN_STATE = 2
	OPEN_STATE_DURATION = 3000

	state = CLOSE_STATE
	change = true

	loadSound("Assets/Potion 'Sploder/Sounds/chest_creak.wav", "chest_creak")
end

-- Standard Mouse/Key events

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

-- Mouse Move Events --
function onMouseMove(x, y, button)

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

	-- state logic
	if state == OPEN_STATE then
		open_counter = open_counter + delta
		if open_counter > OPEN_STATE_DURATION then
			state = CLOSE_STATE
			change = true
		end
	end

	if change then
		change = false
		if state == OPEN_STATE then
			setTexture("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", entityId, 64, 64, 16, 128, 16, 16)
		else
			setTexture("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", entityId, 64, 64, 80, 128, 16, 16)
		end
	end
end

function onBroadcast(message, value)

end

function onMessage(message, value)

	if message == "OPEN_CHEST" and state ~= OPEN_STATE then
		state = OPEN_STATE
		open_counter = 0
		change = true
		item = random(100)
		createItem(item)
		playSound("chest_creak", -1, 0)
	end

	return false
end

function onPhysics(delta)

end

function createItem(item_type)
	pos = getPosition(entityId)

	if 0 < item_type and item_type < 50  then
		item = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 185, 700, 64, 64, 112, 112, 16, 16)
		setPosition(item, pos:getX(), pos:getY())
		setScript(item, "Games/Potion 'Sploder/Bones.lua")
	elseif 50 <= item_type and item_type < 90 then
		item = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Character_2.png", 185, 700, 64, 64, 64, 16, 16, 16)
		setPosition(item, pos:getX(), pos:getY())
		setScript(item, "Games/Potion 'Sploder/Skeleton.lua")
	elseif 90 <= item_type and item_type < 95 then
		item = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 185, 700, 64, 64, 144, 128, 16, 16)
		setPosition(item, pos:getX(), pos:getY())
		setScript(item, "Games/Potion 'Sploder/HealingPotion.lua")
	elseif 95 <= item_type and item_type <= 100 then
		item = createTextured("Assets/Potion 'Sploder/Sprites/Dungeon_Tileset.png", 185, 700, 64, 64, 0, 144, 16, 16)
		setPosition(item, pos:getX(), pos:getY())
		setScript(item, "Games/Potion 'Sploder/Torch.lua")
	else
		print("Can't create item for value: ", item_type)
	end
end