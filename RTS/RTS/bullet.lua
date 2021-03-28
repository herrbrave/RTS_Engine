
registrar = {
	DRAWABLE = 0,
	ENTITY = 1,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	UI = 0
}

function setup()
	print("Bullet setup")
	setTag(entityId, "bullet")
end

-- Collition Callback --

function onCollision(id)
	tag = getTag(id)
	if tag == "bound" then
		destroyEntity(entityId)
	elseif tag == "block" then
		boxVel = getVelocity(id)
		bulletVel = getVelocity(entityId)
		boxVel:subtract(bulletVel)

		norm = Vector2f.new(0, 0)
		if math.abs(boxVel:getX()) >= math.abs(boxVel:getY()) then
			if boxVel:getX() > 0 then
				print("right")
				norm:setX(1)
			else
				print("left")
				norm:setX(-1)
			end
		else
			if boxVel:getY() > 0 then
				norm:setY(1)
				print("bottom")
			else
				norm:setY(0)
				print("top")
			end
		end

		dot = 2 * bulletVel:dot(norm)
		norm:scale(dot)
		bulletVel:subtract(norm)
		setVelocity(entityId, bulletVel:getX(), bulletVel:getY())
	end
end

function update(delta) 

end