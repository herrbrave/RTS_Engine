#include"MapFactory.h"

EntityPtr TileFactory::createTile(const string& assetTag, int xIndex, int yIndex, const Vector2f& position, float tx, float ty, float width, float height) {
	EntityPtr entity = createTexturedEntity(assetTag, position.x, position.y, width, height, tx, ty, width, height, false);

	TileComponentPtr tileComponent(GCC_NEW TileComponent(entity->id, xIndex, yIndex));
	entity->addComponent(tileComponent);

	return entity;
}

MapFactory::MapFactory(TileFactoryPtr tileFactory, LuaScriptFactoryPtr luaScriptFactory, SystemManagerPtr systemManager) {
	mTileFactory = tileFactory;
	mSystemManager = systemManager;
	mLuaScriptFactory = luaScriptFactory;
}

MapPtr MapFactory::createMap(const string& pathToMap) {
	std::ifstream file(pathToMap);
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	Tileset tileset;
	TileAnimationSet animations;
	auto tilesets = doc["tilesets"].GetArray();
	for (int index = 0; index < tilesets.Size(); index++) {
		this->loadGridTileset(tilesets[index].GetObject(), tileset, animations);
	}

	int width(doc["width"].GetInt());
	int height(doc["height"].GetInt());
	float tileWidth(doc["tilewidth"].GetInt());
	float tileHeight(doc["tileheight"].GetInt());

	MapConfig* mapConfig = GCC_NEW MapConfig();
	mapConfig->mapWidth = width;
	mapConfig->mapHeight = height;
	mapConfig->tileWidth = tileWidth;
	mapConfig->tileHeight = tileHeight;
	mapConfig->tileset = tileset;
	mapConfig->animatedTiles = animations;

	auto layers = doc["layers"].GetArray();
	for (int index = 0; index < layers.Size(); index++) {
		auto layer = layers[index].GetObject();
		std::string type = layer["type"].GetString();
		if (type == "tilelayer") {
			loadTileLayer(layer, width, height, tileWidth, tileHeight, *mapConfig, index);
		}
		else {
			loadObjectLayer(layer, *mapConfig, index);
		}
	}

	// TODO: provide a factory for the entity vendor, or inject it into the map factory.
	EntitySystemPtr entitySystem(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	MapPtr map(GCC_NEW Map(MapConfigPtr(mapConfig), EntityVendorPtr(GCC_NEW EntitySystem::DefaultEntityVendor(entitySystem))));

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	physicsSystem->setWorldSize(map->getMapWidth() * map->getTileWidth(), map->getMapHeight() * map->getTileHeight());

	MapLoadedSetEventData* eventData = GCC_NEW MapLoadedSetEventData(SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);

	return map;
}

void MapFactory::loadGridTileset(const rapidjson::Value& tileset, Tileset& tiles, TileAnimationSet& animations) {
	// If the tileset contains an image then it is a grid based tileset.
	if (tileset.FindMember("image") != tileset.MemberEnd()) {
		string image = tileset["image"].GetString();

		GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
		graphicsSystem->addTexture(image, image);

		int tileWidth = tileset["tilewidth"].GetInt();
		int tileHeight = tileset["tileheight"].GetInt();
		int columns = tileset["columns"].GetInt();
		int tileCount = tileset["tilecount"].GetInt();
		int id = tileset["firstgid"].GetInt();
		int count = 0;
		int row = 0;

		while (count <= tileCount) {
			for (int i = 0; i < columns && count <= tileCount; i++, count++) {
				tiles.emplace(id++, TilePtr(GCC_NEW Tile(image, i * tileWidth, row * tileHeight, tileHeight, tileWidth)));
			}
			row++;
		}

		id = tileset["firstgid"].GetInt();
		auto tilesArray = tileset["tiles"].GetArray();
		for (int index = 0; index < tilesArray.Size(); index++) {
			auto tile = tilesArray[index].GetObject();

			TilePtr newTile = tiles[id++];
			if (tile.FindMember("objectgroup") != tile.MemberEnd()) {
				auto objectGroup = tile["objectgroup"].GetObject();
				auto objects = objectGroup["objects"].GetArray();
				auto collisionObject = objects[0].GetObject();
				newTile->collision = true;
				newTile->xOff = collisionObject["x"].GetInt();
				newTile->yOff = collisionObject["y"].GetInt();
				newTile->collisionWidth = collisionObject["width"].GetInt();
				newTile->collisionHeight = collisionObject["height"].GetInt();
			}

			if (tile.FindMember("animation") != tile.MemberEnd()) {
				AnimationSetPtr animationSet(GCC_NEW AnimationSet());
				id = tileset["firstgid"].GetInt();
				newTile->animated = true;
				animations[id] = animationSet;
				
				animationSet->spritesheet = image;
				animationSet->fps = 0;
				animationSet->defaultAnimationName = "default";
				animationSet->name = "TileAnimation";

				AnimationPtr anim(GCC_NEW Animation());
				animationSet->animations["default"] = anim;

				auto animation = tile["animation"].GetArray();
				for (int animIndex = 0; animIndex < animation.Size(); animIndex++) {
					auto anim = animation[animIndex].GetObject();
					if (animationSet->fps == 0) {
						int frameTime = anim["duration"].GetInt();
						int fps = 1000 / frameTime;
						animationSet->fps = fps;
					}
					int tileId = anim["tileid"].GetInt() + id;
					auto animTile = tiles[tileId];

					TexturePtr texture(GCC_NEW Texture(animTile->textureAssetTag, animTile->tx, animTile->ty, animTile->w, animTile->h));
					animationSet->animations["default"]->frames.push_back(texture);
				}
			}
		}
	}
	else {
		int id = tileset["firstgid"].GetInt();
		auto tilesArray = tileset["tiles"].GetArray();
		for (int index = 0; index < tilesArray.Size(); index++) {
			auto tile = tilesArray[index].GetObject();
			string image = tile["image"].GetString();

			GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
			graphicsSystem->addTexture(image, image);

			int tileWidth = tile["imagewidth"].GetInt();
			int tileHeight = tile["imageheight"].GetInt();
			auto newTile = TilePtr(GCC_NEW Tile(image, 0, 0, tileHeight, tileWidth));
			tiles.emplace(id++, newTile);

			if (tile.FindMember("objectgroup") != tile.MemberEnd()) {
				auto objectGroup = tile["objectgroup"].GetObject();
				auto objects = objectGroup["objects"].GetArray();
				auto collisionObject = objects[0].GetObject();
				newTile->collision = true;
				newTile->xOff = collisionObject["x"].GetInt();
				newTile->yOff = collisionObject["y"].GetInt();
				newTile->collisionWidth = collisionObject["width"].GetInt();
				newTile->collisionHeight = collisionObject["height"].GetInt();
			}
		}
	}
}

void MapFactory::loadTileLayer(const rapidjson::Value& tileLayer,int width, int height, int tileWidth, int tileHeight, MapConfig& mapConfig, int drawOrder) {
	auto data = tileLayer["data"].GetArray();
	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	for (int index = 0; index < data.Size(); index++) {
		int tileVal(data[index].GetInt());
		if (mapConfig.tileset.find(tileVal) == mapConfig.tileset.end()) {
			continue;
		}

		int x = (index % width);
		int y = (index / height);
		TilePtr t = mapConfig.tileset.at(tileVal);

		EntityPtr tile;
		if (t->animated) {
			auto animationSet = mapConfig.animatedTiles[tileVal];
			tile = mTileFactory->createPhysicsEntity(x * tileWidth, y * tileHeight, tileWidth, tileHeight); 
			TexturePtr texture(GCC_NEW Texture(""));
			shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
			graphicsSystem->registerDrawable(tile->id, textureDrawable);
			AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, animationSet, animationSet->fps));
			animationHandler->loop();
			animationSystem->registerAnimation(tile->id, animationHandler);
			AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(tile->id, animationHandler));

			TileComponentPtr tileComponent(GCC_NEW TileComponent(tile->id, x, y));
			tile->addComponent(tileComponent);
		}
		else {
			tile = mTileFactory->createTile(t->textureAssetTag, x, y, Vector2fPtr(GCC_NEW Vector2f(x * tileWidth, y * tileHeight)), t->tx, t->ty, t->w, t->h);
			auto drawableComponent = makeShared(tile->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
			//drawableComponent->setZOrder(drawOrder);
		}
		mapConfig.tiles.push_back(tile->id);


		TileComponentPtr tileComponent(tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		// TODO: Add this value as a property of the tile.
		tileComponent->canOccupy = true;

		if (t->collision) {
			BodyPtr blockBody(GCC_NEW Body(tile->id, x, y, width, height));
			physicsSystem->registerBody(tile->id, blockBody);
			PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(tile->id, blockBody));
			physicsComponent->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), t->collisionWidth, t->collisionHeight))));
		}
	}
}

void MapFactory::loadObjectLayer(const rapidjson::Value& objectLayer, MapConfig& mapConfig, int drawOrder) {
	auto data = objectLayer["objects"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		auto object = data[index].GetObject();
		int width = object["width"].GetInt();
		int height = object["height"].GetInt();
		int id = object.FindMember("gid") != object.MemberEnd() ? object["gid"].GetInt() : -1;

		// Tiled stored location from the top left corner.
		int x = ((int)floor(object["x"].GetDouble()) + (width / 2) - (mapConfig.tileWidth / 2));
		int y = ((int)floor(object["y"].GetDouble()) + (height / 2) - (mapConfig.tileHeight / 2));

		string script = "";
		if (object.FindMember("properties") != object.MemberEnd()) {
			auto properties = object["properties"].GetArray();
			for (int i = 0; i < properties.Size(); i++) {
				auto prop = properties[i].GetObject();
				if (string("script").compare(prop["name"].GetString()) == 0) {
					script = prop["value"].GetString();
				}
			}
		}

		EntityPtr tile;
		if (id > 0) {
			TilePtr t = mapConfig.tileset.at(id);
			tile = mTileFactory->createPhysicsEntity(x, y - height / 2, width, height);
			if (t->animated) {
				GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
				AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
				tile = mTileFactory->createPhysicsEntity(x, y, width, height);
				TexturePtr texture(GCC_NEW Texture(""));
				shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
				graphicsSystem->registerDrawable(tile->id, textureDrawable);
				AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, mapConfig.animatedTiles[id], mapConfig.animatedTiles[id]->fps));
				animationHandler->loop();
				animationSystem->registerAnimation(tile->id, animationHandler);
				AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(tile->id, animationHandler));

				TileComponentPtr tileComponent(GCC_NEW TileComponent(tile->id, x, y));
				tile->addComponent(tileComponent);
			}
			else {
				tile = mTileFactory->createTexturedEntity(t->textureAssetTag, x, y - height / 2, width, height, t->tx, t->ty, t->w, t->h, true);
				auto drawableComponent = makeShared(tile->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
				//drawableComponent->setZOrder(drawOrder);
			}
		}
		else {
			tile = mTileFactory->createPhysicsEntity(x, y, width, height);
		}

		mapConfig.objects.push_back(tile->id);

		if (script.size() > 0) {
			InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
			InputListenerPtr inputListener(GCC_NEW InputListener(tile->id));
			inputSystem->registerEventListener(inputListener);

			LuaScriptPtr script(mLuaScriptFactory->create(script, tile->id));

			LuaScriptComponent* luaScriptComponent = GCC_NEW LuaScriptComponent(tile->id, script);
			tile->addComponent(ComponentPtr(luaScriptComponent));

			LuaScriptSystemPtr scriptSystem = makeShared<LuaScriptSystem>(mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
			scriptSystem->registerLuaScript(tile->id, script);
		}
	}
}