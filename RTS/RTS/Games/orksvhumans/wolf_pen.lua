registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 1,
	ANIMATION = 0,
	INPUT = 1,
	SCRIPT = 0,
	ASSET = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 1,
}

selected = false
mouseOver = false

function setup()
	print("setup wolf pen", entityId)
	setTag(entityId, "wolf_pen")
end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	if button == MOUSE_BUTTON_LEFT and not mouseOver then
		selected = false
	end
end

function onMouseDown(x, y, button)
end

function onKeyDown(keyId, ctrl, shft)
end

function onKeyUp(keyId, ctrl, shft)
end

-- Entity Mouse Events

function onMouseEnterEntity()
	mouseOver = true
end

function onMouseExitEntity()
	mouseOver = false
end

function onClickEntity(button)
	selected = true
end

function onDragEntity(button)
end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end