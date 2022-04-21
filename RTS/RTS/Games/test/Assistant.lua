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

	inventory_0 = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 185, 700, 80, 80, 0, 128, 16, 16)
	setZOrder(inventory_0, 100)

	inventory_1 = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 265, 700, 80, 80, 0, 128, 16, 16)
	setZOrder(inventory_1, 100)

	inventory_2 = createTextured("Assets/test/Sprites/Dungeon_Tileset.png", 345, 700, 80, 80, 0, 128, 16, 16)
	setZOrder(inventory_2, 100)

	INVENTORY_EMPTY = 0
	INVENTORY_BONES = 1
	INVENTORY_GOLD = 2

	context.inventory = {}
	context.inventory[0] = {}
	context.inventory[0].type = INVENTORY_EMPTY
	context.inventory[0].id = inventory_0
	context.inventory[1] = {}
	context.inventory[1].type = INVENTORY_EMPTY
	context.inventory[1].id = inventory_1
	context.inventory[2] = {}
	context.inventory[2].type = INVENTORY_EMPTY
	context.inventory[2].id = inventory_2
	context.inventorySize = function()
		_i = 0
		while _i <= 2  do
			if context.inventory[_i].type ~= INVENTORY_EMPTY then
				_i = _i + 1
			end
		end

		return _i
	end
	context.inventoryPush = function(item)
		for _i=0,2 do
			if context.inventory[_i].type == INVENTORY_EMPTY then
				context.inventory[_i].type = item
				inventory_change = true
				break
			end
		end
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
	tag = getTag(id)
	if tag == "BOX" then
		context.box = id
	end
end

function update(delta)
	stateMachine.update(delta)
	if inventory_change then

		for i=0,2 do
			if context.inventory[i].type == INVENTORY_EMPTY then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 128, 112, 16, 16)
			elseif context.inventory[i].type == INVENTORY_BONES then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 112, 112, 16, 16)
			elseif context.inventory[i].type == INVENTORY_GOLD then
				setTexture("Assets/test/Sprites/Dungeon_Tileset.png", context.inventory[i].id, 64, 64, 96, 128, 16, 16)
			end
		end

		inventory_change = false
	end
end

function onMessage(message, value)

	if message == "ADD_INVENTORY" then
		print(message, value)
		for index=0,2 do
			if context.inventory[index].type == INVENTORY_EMPTY then
				context.inventory[index].type = INVENTORY_BONES
				inventory_change = true
				break
			end
		end
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
		if context.torch == 0 then
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