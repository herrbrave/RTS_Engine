registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 1,
	WORLD = 0,
}


function setup()
	print("setup Assistant", tostring(entityId))

	setTag(entityId, "PLAYER")

	include("Games/test/AssistantMoveState.lua")

	IDLE = 0
	DASH = 1
	HURT = 2
	DEAD = 3

	context = {}
	context.velocity = Vector2f.new(0, 0)
	context.hurtVector = Vector2f.new(0, 0)
	context.accelerate = 1200
	context.speed = 220
	context.friction = 2400
	context.box = 0

	context.keys = {}
	context.keys[SDLK_w] = false
	context.keys[SDLK_a] = false
	context.keys[SDLK_s] = false
	context.keys[SDLK_d] = false
	context.keyChange = false

	moveState = BasicWASDMoveState.new(context)
	hurtState = HurtMoveState.new(context)
	stateMachine = StateMachine.new()
	stateMachine.pushState(moveState)

	context.stateMachine = stateMachine

	-- Set up Player health
	context.health = 100
	context.healthLabel = createLabel("label", 25, 200, 70)
	setLabelText(context.healthLabel, "Health", 25, 255, 255, 255)
	setLabelZOrder(context.healthLabel, 100)
	context.healthProgress = createProgress(290, 70, 100, 25, 100, 100)

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
		context.inventory[_i].type = context.INVENTORY_EMPTY
		context.inventory[_i].id = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 185 + (_i * 80), 700, 80, 80, 0, 128, 16, 16)
		setZOrder(context.inventory[_i].id, 100)
		_i = _i + 1
	end
	context.inventorySize = function()
		_i = 0
		while _i <= context.inventorySizeMax  do
			if context.inventory[_i].type ~= context.INVENTORY_EMPTY then
				_i = _i + 1
			else
				break
			end
		end

		return _i
	end
	context.inventoryPush = function(item)
		for _i=0,context.inventorySizeMax do
			if context.inventory[_i].type == context.INVENTORY_EMPTY then
				context.inventory[_i].type = context.inventory_type_map[item]
				break
			end
		end
		updateInventoryImages()
		return false
	end
	context.inventoryPop = function()
		local size = context.inventorySize()
		if size <= 0 then
			print("Can't pop and empty inventory.")
			return
		end
		context.inventory[size-1].type = context.INVENTORY_EMPTY
		updateInventoryImages()
		return false
	end
	context.inventoryCharge = 100
	context.inventoryChargeProgress = createProgress(280, 745, 250, 25, 100, 100)
end

-- Standard Mouse/Key events

function onMouseUp(x, y, button)

end

function onMouseDown(x, y, button)

end

function onKeyDown(keyId, ctrl, shft)
	if context.keys[keyId] == false then
		context.keyChange = true
	end
	context.keys[keyId] = true
end

function onKeyUp(keyId, ctrl, shft)
	if context.keys[keyId] == true then
		context.keyChange = true
	end
	context.keys[keyId] = false
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
	if context.inventoryCharge < 100 then
		context.inventoryCharge = math.min(context.inventoryCharge + (20.0 * (delta / 1000)), 100)
		setProgress(context.inventoryChargeProgress, math.floor(context.inventoryCharge), 100)
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
			context.health = math.max(0, context.health - hurtVal)
			setProgress(context.healthProgress, context.health, 100)
		end
	elseif message == "HEAL" then
		hurtVal = value
		context.health = math.min(100, context.health + hurtVal)
		setProgress(context.healthProgress, context.health, 100)
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
	if message == "TORCH_EXPENDED" then
		removeChild(entityId, context.torch)
		context.torch = 0
	end
end

function onPhysics(delta)
	stateMachine.onPhysics(delta)
end

function updateInventoryImages()
	print("Updating Inventory")
	for i=0,context.inventorySizeMax do
		print("Inventory at: ", i, context.inventory[i].type)
		if context.inventory[i].type == context.INVENTORY_EMPTY then
			setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 0, 128, 16, 16)
		elseif context.inventory[i].type == context.INVENTORY_BONES then
			setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 112, 112, 16, 16)
		elseif context.inventory[i].type == context.INVENTORY_GOLD then
			setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 96, 128, 16, 16)
		end
	end
end