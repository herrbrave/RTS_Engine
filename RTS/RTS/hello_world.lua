function default()
	createDefault(100, 100, 50, 50, 255, 0, 0, 255)
end

function textured(x, y) 
	createTextured("Assets/character.png", x, y, 66, 92, 0, 0, 66, 92, true)
end

-- EntityFactory:createDefault(100, 100, 50, 50, 255, 0, 0, 255)