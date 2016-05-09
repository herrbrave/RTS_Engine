#include"MapFactory.h"

EntityPtr TileFactory::createTile(const string& assetTag, int xIndex, int yIndex, const Vector2f& position, float tx, float ty, float width, float height) {
	EntityPtr entity = createTexturedEntity(assetTag, tx, ty, width, height);

	TileComponentPtr tileComponent(GCC_NEW TileComponent(entity->id, xIndex, yIndex));
	entity->addComponent(tileComponent);

	PhysicsComponentPtr physicsComponent = makeShared(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
	physicsComponent->setPosition(Vector2fPtr(GCC_NEW Vector2f(position)));

	return entity;
}

MapFactory::MapFactory(TileFactoryPtr tileFactory, SystemManagerPtr systemManager) {
	mTileFactory = tileFactory;
	mSystemManager = systemManager;
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

	int offset(doc["tilesets"][0]["firstgid"].GetInt());
	int width(doc["width"].GetInt());
	int height(doc["height"].GetInt());
	float tileWidth(doc["tilewidth"].GetInt());
	float tileHeight(doc["tileheight"].GetInt());
	std::string imagePath(doc["tilesets"][0]["image"].GetString());
	std::string assetTag(doc["tilesets"][0]["name"].GetString());
	int columns(doc["tilesets"][0]["columns"].GetInt());

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(imagePath, assetTag);

	MapConfig* mapConfig = new MapConfig();
	mapConfig->mapWidth = width;
	mapConfig->mapHeight = height;
	mapConfig->tileWidth = tileWidth;
	mapConfig->tileHeight = tileHeight;

	auto layers = doc["layers"].GetArray();
	for (int index = 0; index < layers.Size(); index++) {
		auto layer = layers[index].GetObject();
		std::string type = layer["type"].GetString();
		if (type == "tilelayer") {
			loadTileLayer(layer, assetTag, offset, width, height, tileWidth, tileHeight, columns, *mapConfig);
		}
		else {
			loadObjectLayer(layer, *mapConfig);
		}
	}

	// TODO: provide a factory for the entity vendor, or inject it into the map factory.
	EntitySystemPtr entitySystem(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	MapPtr map(GCC_NEW Map(MapConfigPtr(mapConfig), EntityVendorPtr(GCC_NEW EntitySystem::DefaultEntityVendor(entitySystem))));

	return map;
}

void MapFactory::loadTileLayer(const rapidjson::Value& tileLayer, const string& assetTag, int offset, int width, int height, int tileWidth, int tileHeight, int columns, MapConfig& mapConfig) {
	auto data = tileLayer["data"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		int tileVal(data[index].GetInt() - offset);
		int x = (index % width);
		int y = (index / height);
		int tx = tileWidth * (tileVal % columns);
		int ty = tileHeight * (tileVal / columns);

		EntityPtr tile = mTileFactory->createTile(assetTag, x, y, Vector2fPtr(GCC_NEW Vector2f(x * tileWidth, y * tileHeight)), tx, ty, tileWidth, tileHeight);
		mapConfig.tiles.push_back(tile->id);

		TileComponentPtr tileComponent(tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		// TODO: Add this value as a property of the tile.
		tileComponent->canOccupy = tileVal == 1;
	}
}

void MapFactory::loadObjectLayer(const rapidjson::Value& objectLayer, MapConfig& mapConfig) {
	auto data = objectLayer["objects"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		auto object = data[index].GetObject();
		int width = object["width"].GetInt();
		int height = object["height"].GetInt();

		// Tiled stored location from the top left corner.
		int x = (object["x"].GetInt() + (width / 2) - (mapConfig.tileWidth / 2));
		int y = (object["y"].GetInt() + (height / 2) - (mapConfig.tileHeight / 2));

		EntityPtr tile = mTileFactory->createPhysicsEntity(x, y, width, height);
		mapConfig.objects.push_back(tile->id);
	}
}