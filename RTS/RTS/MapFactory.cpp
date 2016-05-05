#include"MapFactory.h"

Entity* TileFactory::createTile(const std::string& assetTag, int xIndex, int yIndex, vector2f* position, float tx, float ty, float width, float height) {
	Entity* entity = createTexturedEntity(assetTag, tx, ty, width, height);

	TileComponent* tileComponent = new TileComponent(entity->id, xIndex, yIndex);
	entity->componentContainer->registerComponent(tileComponent);

	PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(entity->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
	physicsComponent->setPosition(position);

	return entity;
}

MapFactory::MapFactory(TileFactory* tileFactory, SystemManager* systemManager) {
	mTileFactory = tileFactory;
	mSystemManager = systemManager;
}

Map* MapFactory::createMap(const std::string pathToMap) {
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
	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	Map* map = new Map(mapConfig, new EntitySystem::DefaultEntityVendor(entitySystem.get()));

	return map;
}

void MapFactory::loadTileLayer(const rapidjson::Value& tileLayer, std::string assetTag, int offset, int width, int height, int tileWidth, int tileHeight, int columns, MapConfig& mapConfig) {
	auto data = tileLayer["data"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		int tileVal(data[index].GetInt() - offset);
		int x = (index % width);
		int y = (index / height);
		int tx = tileWidth * (tileVal % columns);
		int ty = tileHeight * (tileVal / columns);

		Entity* tile = mTileFactory->createTile(assetTag, x, y, new vector2f(x * tileWidth, y * tileHeight), tx, ty, tileWidth, tileHeight);
		mapConfig.tiles.push_back(tile->id);

		TileComponent* tileComponent = reinterpret_cast<TileComponent*>(tile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
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

		Entity* tile = mTileFactory->createPhysicsEntity(x, y, width, height);
		mapConfig.objects.push_back(tile->id);
	}
}