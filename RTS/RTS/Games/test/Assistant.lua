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
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
}


function setup()
	print("setup Assistant", tostring(entityId))

	setTag(entityId, "PLAYER")

	keys = {}
	keys[SDLK_w] = false
	keys[SDLK_a] = false
	keys[SDLK_s] = false
	keys[SDLK_d] = false
	keyChange = false

	include("Games/test/AssistantMoveState.lua")

	IDLE = 0
	DASH = 1
	HURT = 2

	context = {}
	context.velocity = Vector2f.new(0, 0)
	context.hurtVector = Vector2f.new(0, 0)
	context.accelerate = 1200
	context.speed = 220
	context.friction = 2400
	context.box = 0

	moveState = BasicWASDMoveState.new(context)
	hurtState = HurtMoveState.new(context)
	stateMachine = StateMachine.new()
	stateMachine.pushState(moveState)

	context.stateMachine = stateMachine

	-- Set up Player health
	health = 100
	healthLabel = createLabel("label", 25, 200, 70)
	setLabelText(healthLabel, "Health", 25, 255, 255, 255)
	setLabelZOrder(healthLabel, 100)
	healthProgress = createProgress(290, 70, 100, 25, 100, 100)

	-- Set up Torch
	context.torch = 0

	-- Set up Inventory
	inventory_label = createLabel("label", 30, 220, 650)
	setLabelText(inventory_label, "Inventory", 25, 255, 255, 255)
	setLabelZOrder(inventory_label, 100)

	context.INVENTORY_EMPTY = 0
	context.INVENTORY_BONES = 1
	context.INVENTORY_GOLD = 2
	context.inventory_type_map = {}
	context.inventory_type_map["EMPTY"] = context.INVENTORY_EMPTY
	context.inventory_type_map["BONES"] = context.INVENTORY_BONES

	context.inventorySizeMax = 2
	context.inventory = {}
	_i = 0
	while _i <= context.inventorySizeMax do
		context.inventory[_i] = {}
		context.inventory[_i].type = INVENTORY_EMPTY
		context.inventory[_i].id = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 185 + (_i * 80), 700, 80, 80, 0, 128, 16, 16)
		setZOrder(context.inventory[_i].id, 100)
		_i = _i + 1
	end
	context.inventorySize = function()
		_i = 0
		while _i <= context.inventorySizeMax  do
			if context.inventory[_i].type ~= INVENTORY_EMPTY then
				_i = _i + 1
			else
				break
			end
		end

		return _i
	end
	context.inventoryPush = function(item)
		print("ADDING TO INVENTORY")
		for _i=0,context.inventorySizeMax do
			if context.inventory[_i].type == INVENTORY_EMPTY then
				context.inventory[_i].type = context.inventory_type_map[item]
				print("ADD TO INVENTORY AT:", _i, item, context.inventory[_i].type)
				inventory_change = true
				return true
			end
		end
		return false
	end
	context.inventoryPop = function()

	end

	inventory_change = true
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	if keys[keyId] == false then
		keyChange = true
	end
	keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	if keys[keyId] == true then
		keyChange = true
	end
	keys[keyId] = false
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
	stateMachine.update(delta)
	if inventory_change then
		print("Updating Invetory")
		for i=0,2 do
			if context.inventory[i].type == context.INVENTORY_EMPTY then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 128, 112, 16, 16)
			elseif context.inventory[i].type == context.INVENTORY_BONES then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 112, 112, 16, 16)
			elseif context.inventory[i].type == context.INVENTORY_GOLD then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 96, 128, 16, 16)
			end
		end

		inventory_change = false
	end
end

function onMessage(message, value)

	if message == "ADD_INVENTORY" then
		return context.inventoryPush(value)
	elseif message == "HURT" then
		if context.state ~= HURT then
			id = tonumber(value)
			skelePos = getPosition(id)
			playerPosition = getPosition(entityId)
			playerPosition:subtract(skelePos)
			playerPosition:normalize()
			context.hurtVector:setX(playerPosition:getX())
			context.hurtVector:setY(playerPosition:getY())
			context.stateMachine.pushState(hurtState)

			hurtVal = 7
			health = math.max(0, health - hurtVal)
			setProgress(healthProgress, health, 100)
		end
	elseif message == "HEAL" then
		hurtVal = value
		health = math.min(100, health + hurtVal)
		setProgress(healthProgress, health, 100)
	elseif message == "TORCH" then
		if context.torch == nil or context.torch == 0 then
			context.torch = tonumber(value)
			setChild(entityId, context.torch)
			setPosition(context.torch, 32, 0)
			sendMessage(context.torch, "PICK_UP", "true")
		end
	end

	return false
end

function onBroadcast(message, value)

end

function onPhysics(delta)
	stateMachine.onPhysics(delta)
end