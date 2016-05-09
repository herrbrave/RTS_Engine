#include"Map.h"

Map::Map(MapConfigPtr config, EntityVendorPtr entityVendor) {
	mMapConfig = std::move(config);
	mEntityVendor = entityVendor;
}

WeakEntityPtr Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return WeakEntityPtr();
	}

	return mEntityVendor->getEntityById(mMapConfig->tiles[index]);
}

WeakEntityPtr Map::tileAtPoint(const Vector2f& point) {
	int xIndex = std::round(point.x / float(mMapConfig->tileWidth));
	int yIndex = std::round(point.y / float(mMapConfig->tileHeight));

	return this->getTileAt(xIndex, yIndex);
}

void Map::findPath(vector<WeakEntityPtr> path, int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return;
	}

	auto endTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[endIndex]));

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {
		TileComponentPtr leftComponent(left.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		TileComponentPtr rightComponent(right.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		TileComponentPtr endComponent(endTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));

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
	auto startTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[startIndex]));
	std::unordered_set<EntityPtr> openSetLookup;
	std::priority_queue < Node, std::vector<Node>, decltype(comparitor)> openSet(comparitor);
	openSet.emplace(Node{ startTile, 0 });

	// set of explored tiles.
	unordered_set<EntityPtr> closedSet;
	// back path map.
	unordered_map<EntityPtr, EntityPtr> pathMap;


	while (openSet.size() > 0) {
		auto current = openSet.top();
		openSet.pop();
		closedSet.emplace(current.tile);

		TileComponentPtr component(current.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		if (component->x == endX && component->y == endY) {
			auto tile = current.tile;
			while (tile != startTile) {
				path.insert(path.begin(), WeakEntityPtr(tile));
				tile = pathMap[tile];
			}

			return;
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

				auto neighborTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[index]));
				TileComponentPtr component(neighborTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
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