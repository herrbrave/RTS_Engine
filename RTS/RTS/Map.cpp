#include"Map.h"

Map::Map(MapConfig* config, EntitySystem* entitySystem) {
	mMapConfig.reset(config);
	mEntitySystem = entitySystem;
}

Entity* Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return nullptr;
	}

	return mEntitySystem->getEntityById(mMapConfig->tiles[index]);
}

Entity* Map::tileAtPoint(const vector2f* point) {
	int xIndex = std::round(point->x / float(mMapConfig->tileWidth));
	int yIndex = std::round(point->y / float(mMapConfig->tileHeight));

	return this->getTileAt(xIndex, yIndex);
}

Entity** Map::findPath(int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return nullptr;
	}

	auto endTile = mEntitySystem->getEntityById(mMapConfig->tiles[endIndex]);

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {
		TileComponent* leftComponent = reinterpret_cast<TileComponent*>(left.tile->componentContainer->getComponentByType(TILE_COMPONENT_ID));
		TileComponent* rightComponent = reinterpret_cast<TileComponent*>(right.tile->componentContainer->getComponentByType(TILE_COMPONENT_ID));
		TileComponent* endComponent = reinterpret_cast<TileComponent*>(endTile->componentContainer->getComponentByType(TILE_COMPONENT_ID));

		// dumb heuristic based on distance.
		int deltaX = leftComponent->x - endComponent->x;
		int deltaY = leftComponent->y - endComponent->y;
		float leftDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		deltaX = rightComponent->x - endComponent->x;
		deltaY = rightComponent->y - endComponent->y;
		float rightDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		return (leftDist + left.cost) > (rightDist + right.cost);
	};

	int startIndex = getIndex(startX, startY);
	auto startTile = mEntitySystem->getEntityById(mMapConfig->tiles[startIndex]);
	std::unordered_set<Entity*> openSetLookup;
	std::priority_queue < Node, std::vector<Node>, decltype(comparitor)> openSet(comparitor);
	openSet.emplace(Node{ startTile, 0 });

	// set of explored tiles.
	std::unordered_set<Entity*> closedSet;
	// back path map.
	std::unordered_map<Entity*, Entity*> pathMap;


	while (openSet.size() > 0) {
		auto current = openSet.top();
		openSet.pop();
		closedSet.emplace(current.tile);

		TileComponent*  component = reinterpret_cast<TileComponent*>(current.tile->componentContainer->getComponentByType(TILE_COMPONENT_ID));
		if (component->x == endX && component->y == endY) {
			std::vector<Entity*> path;
			auto tile = current.tile;
			while (tile != startTile) {
				path.insert(path.begin(), tile);
				tile = pathMap[tile];
			}

			

			return &path[0];
		}

		int currX = component->x;
		int currY = component->y;
		for (int j = -1; j <= 1; j++) {
			for (int i = -1; i <= 1; i++) {
				// skip the diagonals.
				if (i != 0 && j != 0) {
					continue;
				}

				int index = getIndex(currX + i, currY + j);
				if (index == -1) {
					continue;
				}

				auto neighborTile = mEntitySystem->getEntityById(mMapConfig->tiles[index]);
				TileComponent*  component = reinterpret_cast<TileComponent*>(neighborTile->componentContainer->getComponentByType(TILE_COMPONENT_ID));
				if (!component->canOccupy || closedSet.find(neighborTile) != closedSet.end() || openSetLookup.find(neighborTile) != openSetLookup.end()) {
					continue;
				}

				pathMap[neighborTile] = current.tile;
				openSet.emplace(Node{ neighborTile, current.cost + 1 });
				openSetLookup.emplace(neighborTile);
			}
		}
	}
}

Entity* TileFactory::createTile(const std::string& assetTag, int xIndex, int yIndex, vector2f* position, float tx, float ty, float width, float height) {
	Entity* entity = createTexturedEntity(assetTag, tx, ty, width, height);

	TileComponent* tileComponent = new TileComponent(entity->id, xIndex, yIndex);
	entity->componentContainer->registerComponent(tileComponent);

	PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(entity->componentContainer->getComponentByType(PHYSICS_COMPONENT_TYPE));
	physicsComponent->setPosition(position);

	return entity;
}


MapFactory::MapFactory(TileFactory* tileFactory, EntitySystem* entitySystem, GraphicsSystem* graphicsSystem) {
	mTileFactory.reset(std::move(tileFactory));
	mGraphicsSystem = graphicsSystem;
	mEntitySystem = entitySystem;
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

	mGraphicsSystem->addTexture(imagePath, assetTag);

	MapConfig* mapConfig = new MapConfig();
	mapConfig->mapWidth = width;
	mapConfig->mapHeight = height;
	mapConfig->tileWidth = tileWidth;
	mapConfig->tileHeight = tileHeight;

	auto data = doc["layers"][0]["data"].GetArray();
	for (int index = 0; index < data.Size(); index++) {
		int tileVal(data[index].GetInt() - offset);
		int x = (index % width);
		int y = (index / height);
		int tx = tileWidth * (tileVal % columns);
		int ty = tileHeight * (tileVal / columns);

		Entity* tile = mTileFactory->createTile(assetTag, x, y, new vector2f(x * tileWidth, y * tileHeight), tx, ty, tileWidth, tileHeight);
		mapConfig->tiles.push_back(tile->id);
	}

	Map* map = new Map(mapConfig, mEntitySystem);

	return map;
}