registrar = {
	ASSET = 0,
	DRAWABLE = 1,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 1,
	INPUT = 0,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}



function setup()
	print("setup kobold fireball", entityId)
	setTag(entityId, "kobold fireball")
	attachAsepriteAnimationSet(entityId, "Assets/HackNSlasher/Visual FX/magicOrb 16x16.json")
	setSpeed(entityId, 200);
	setZOrder(entityId, 3)

	setAnimation(entityId, "orb")
	loopAnimation(entityId)

	isExploding = false
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
	if not isExploding then
		isExploding = true
		setSize(entityId, 64, 64)
		setVelocity(entityId, 0, 0)
		attachAsepriteAnimationSet(entityId, "Assets/HackNSlasher/Visual FX/Explosions/MagicExplosion 32x32.json")
		setAnimation(entityId, "explode")
		playAnimation(entityId)
	end
end

function update(delta)
	if isExploding then
		playing = isAnimationPlaying(entityId)
		if not playing  then
			destroyEntity(entityId)
		end
	end
end

-- Custom Functions --