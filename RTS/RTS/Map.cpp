#include"Map.h"

Map::Map(MapConfig* config, EntityVendor* entityVendor) {
	mMapConfig.reset(config);
	mEntityVendor = entityVendor;
}

Entity* Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return nullptr;
	}

	return mEntityVendor->getEntityById(mMapConfig->tiles[index]);
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

	auto endTile = mEntityVendor->getEntityById(mMapConfig->tiles[endIndex]);

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {
		TileComponent* leftComponent = reinterpret_cast<TileComponent*>(left.tile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
		TileComponent* rightComponent = reinterpret_cast<TileComponent*>(right.tile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
		TileComponent* endComponent = reinterpret_cast<TileComponent*>(endTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));

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
	auto startTile = mEntityVendor->getEntityById(mMapConfig->tiles[startIndex]);
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

		TileComponent*  component = reinterpret_cast<TileComponent*>(current.tile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
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

				auto neighborTile = mEntityVendor->getEntityById(mMapConfig->tiles[index]);
				TileComponent*  component = reinterpret_cast<TileComponent*>(neighborTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
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