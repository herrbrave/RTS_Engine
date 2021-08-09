registrar = {
	ASSET = 0,
	DRAWABLE = 0,
	ENTITY = 0,
	FACTORY = 0,
	PHYSICS = 0,
	ANIMATION = 0,
	INPUT = 0,
	SCRIPT = 0,
	CAMERA = 0,
	MOUSE_MOVE = 0,
	UI = 0,
}

function setup()

end

-- Standard Mouse/Key events

function onMouseMove(x, y, button)

end

function onMouseUp(x, y, button)
	print("mouseUp")
end

function onMouseDown(x, y, button)
	print("mouseDown")
end

-- Entity Mouse Events

function onMouseEnterEntity()
	print("mouseEnter")
end

function onMouseExitEntity()
	print("mouseExit")
end

function onClickEntity(button)
	print("clickedMe")
end

function onDragEntity(button)

end

-- Collition Callback --

function onCollision(id)

end

function update(delta)

end