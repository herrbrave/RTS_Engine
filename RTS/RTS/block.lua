
registrar = {
	DRAWABLE = 0,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	UI = 1
}

life = 10

function setup()
	print("Block setup")
	setText(entityId, life, "Digital_tech", 255, 0, 0)
end

-- Collition Callback --

function onCollision(id)
	tag = getTag(id)
	life = life - 1
	setText(entityId, life, "Digital_tech",  255, 0, 0)
	if life <= 0 then
		destroyEntity(entityId)
	end
end

function update(delta) 

end
