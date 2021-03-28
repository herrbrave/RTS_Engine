
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
		print("========== START ==========")
		boxPos = getPosition(id)
		print("boxPos:", boxPos:getX(), boxPos:getY())
		bulletPos = getPosition(entityId)
		print("bulletPos:", bulletPos:getX(), bulletPos:getY())
		bulletVel = getVelocity(entityId)
		print("bulletVel:", bulletVel:getX(), bulletVel:getY())
		boxPos:subtract(bulletPos)
		print("diff:", boxPos:getX(), boxPos:getY())

		norm = Vector2f.new(0, 0)
		if math.abs(boxPos:getX()) >= math.abs(boxPos:getY()) then
			if boxPos:getX() > 0 then
				print("right")
				norm:setX(1)
			else
				print("left")
				norm:setX(-1)
			end
		else
			if boxPos:getY() > 0 then
				norm:setY(1)
				print("bottom")
			else
				norm:setY(0)
				print("top")
			end
		end
		print("norm:", norm:getX(), norm:getY())

		dot = 2 * bulletVel:dot(norm)
		norm:scale(dot)
		bulletVel:subtract(norm)
		setVelocity(entityId, bulletVel:getX(), bulletVel:getY())
		print("setVelocity:", bulletVel:getX(), bulletVel:getY())
		print("==========  END  ==========")
	end
end

function update(delta) 

end