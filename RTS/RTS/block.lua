
registrar = {
	DRAWABLE = 0,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0
}

life = 10

function setup()
	print("Block setup")
end

-- Collition Callback --

function onCollision(id)
	tag = getTag(id)
	print("box hit ", life)
	life = life - 1
	if life <= 0 then
		destroyEntity(entityId)
	end
end

function update(delta) 

end
