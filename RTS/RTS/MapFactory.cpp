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
	auto tilesets = doc["tilesets"].GetArray();
	for (int index = 0; index < tilesets.Size(); index++) {
		this->loadGridTileset(tilesets[index].GetObject(), tileset);
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

	auto layers = doc["layers"].GetArray();
	for (int index = 0; index < layers.Size(); index++) {
		auto layer = layers[index].GetObject();
		std::string type = layer["type"].GetString();
		if (type == "tilelayer") {
			loadTileLayer(layer, width, height, tileWidth, tileHeight, *mapConfig);
		}
		else {
			loadObjectLayer(layer, *mapConfig);
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

void MapFactory::loadGridTileset(const rapidjson::Value& tileset, Tileset& tiles) {
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
			tiles.emplace(id++, TilePtr(GCC_NEW Tile(image, 0, 0, tileHeight, tileWidth)));
		}
	}
}

void MapFactory::loadTileLayer(const rapidjson::Value& tileLayer,int width, int height, int tileWidth, int tileHeight, MapConfig& mapConfig) {
	auto data = tileLayer["data"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		int tileVal(data[index].GetInt());
		int x = (index % width);
		int y = (index / height);
		TilePtr t = mapConfig.tileset.at(tileVal);

		EntityPtr tile = mTileFactory->createTile(t->textureAssetTag, x, y, Vector2fPtr(GCC_NEW Vector2f(x * tileWidth, y * tileHeight)), t->tx, t->ty, t->w, t->h);
		mapConfig.tiles.push_back(tile->id);

		TileComponentPtr tileComponent(tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		// TODO: Add this value as a property of the tile.
		tileComponent->canOccupy = true;
	}
}

void MapFactory::loadObjectLayer(const rapidjson::Value& objectLayer, MapConfig& mapConfig) {
	auto data = objectLayer["objects"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		auto object = data[index].GetObject();
		int width = object["width"].GetInt();
		int height = object["height"].GetInt();
		int id = object["gid"].GetInt();
		TilePtr t = mapConfig.tileset.at(id);

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

		EntityPtr tile = mTileFactory->createTexturedEntity(t->textureAssetTag, x, y, width, height, t->tx, t->ty, t->w, t->h, true);

		mapConfig.objects.push_back(tile->id);

		if (script.size() > 0) {
			InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
			InputListenerPtr inputListener(GCC_NEW InputListener(tile->id));
			inputSystem->registerEventListener(inputListener);

			LuaScriptPtr script(mLuaScriptFactory->create(script, tile->id));

			LuaScriptComponent* luaScriptComponent = GCC_NEW LuaScriptComponent(tile->id, script);
			tile->addComponent(ComponentPtr(luaScriptComponent));

			inputListener->eventCallbacks.emplace(Input::ON_MOUSE_ENTER, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onMouseEnterEntity");
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_MOUSE_EXIT, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onMouseExitEntity");
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_CLICK, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onClickEntity", static_cast<int>(evt->mouseEvent->button));
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_DRAG, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onDragEntity", static_cast<int>(evt->mouseEvent->button));
				return false;
			}));
		}
	}
}