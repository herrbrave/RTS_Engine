registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 1,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 0,
	SCRIPT = 1,
	ASSET = 0,
	CAMERA = 1,
	MOUSE_MOVE = 0,
	UI = 0,
}



function setup()
	print("setup skeleton mage", entityId)
	setTag(entityId, "skeleton mage")
	attachAsepriteAnimationSet(entityId, "Assets/HackNSlasher/Characters/Humanoid/SkeletonMage/SkeletonMage.json")
	setSpeed(entityId, 64);
	setZOrder(entityId, 3)
	setSize(entityId, 64, 64)

	weapon = createPhysics(0, 0, 64, 64)
	attachAsepriteAnimationSet(weapon, "Assets/HackNSlasher/Characters/Humanoid/Attack FX/Bowshot and Spells/Green Staff 48x48.json")
	setChild(entityId, weapon)
	setSize(weapon, 96, 96)
	setZOrder(weapon, 4)

	direction = "RIGHT"
	setAnimation(entityId, "idle_right")
	loopAnimation(entityId)

	stateTime = 0
	isAttacking = false
	isMoving = false

	math.randomseed(os.time())
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

-- Mouse Move Events --
function onMouseMove(x, y, button)

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)
	updateIdle(delta)
end

-- Custom Functions --

function updateIdle(delta)
	stateTime = stateTime + delta
	if stateTime > 1500 then

		angle = math.random(0, 360)

		-- if even idle
		rand = math.random(3)
		if rand == 0 then
			setVelocity(entityId, 0, 0)
			if (angle >= 0 and angle <= 45) or (angle <= 0 and angle >= -45) then -- right
				direction = "RIGHT"
				setAnimation(entityId, "idle_right")
				loopAnimation(entityId)
			elseif angle > -135 and angle < -45 then -- up
				direction = "UP"
				setAnimation(entityId, "idle_up")
				loopAnimation(entityId)
			elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
				direction = "LEFT"
				setAnimation(entityId, "idle_left")
				loopAnimation(entityId)
			else -- down
				direction = "DOWN"
				setAnimation(entityId, "idle_down")
				loopAnimation(entityId)
			end
		elseif rand == 1 then
			rads = angle * (math.pi / 180.0)
			setVelocity(entityId, math.sin(rads), math.cos(rads))
			if (angle >= 0 and angle <= 45) or (angle <= 0 and angle >= -45) then -- right
				direction = "RIGHT"
				setAnimation(entityId, "walk_right")
				loopAnimation(entityId)
			elseif angle > -135 and angle < -45 then -- up
				direction = "UP"
				setAnimation(entityId, "walk_up")
				loopAnimation(entityId)
			elseif (angle >= 135 and angle < 180) or (angle < -135 and angle > -180) then -- left
				direction = "LEFT"
				setAnimation(entityId, "walk_left")
				loopAnimation(entityId)
			else -- down
				direction = "DOWN"
				setAnimation(entityId, "walk_down")
				loopAnimation(entityId)
			end
		else
			rads = angle * (math.pi / 180.0)
			pos = getPosition(entityId)
			dir = Vector2f.new(math.sin(rads), math.cos(rads))
			dir:scale(55)
			pos:add(dir)


			fireball = createPhysics(math.floor(pos:getX()), math.floor(pos:getY()), 16, 16)
			setScript(fireball, "Games/HackNSlasher/skeleton_mage_magic_missle.lua")
			setVelocity(fireball, math.floor(dir:getX()), math.floor(dir:getY()))
		end

		stateTime = 0
	end
end