
function setup()
	print("setup");
end

function onMouseEnter()
	print("onMouseEnter");
end

function onMouseExit()
	print("onMouseExit");
end

function onMouseDown()
	print("onMouseDown")
end

function onClick()
	print("onClick");
end

function onDrag()
	print("onDrag");
end

function onKeyDown(keyId, ctrl, shft)
	print("onKeyDown", keyId, ctrl, shft);
	if keyId == SDLK_RIGHT then
		setAnimation(entityId, "walk")
		playAnimation(entityId)
	end
end

function onKeyUp(keyId, ctrl, shft)
	print("onKeyUp", keyId, ctrl, shft);
	if keyId == SDLK_RIGHT then
		setAnimation(entityId, "walk")
		stopAnimation(entityId)
	end
end

function update(delta) 

end
