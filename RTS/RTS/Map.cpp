#include"Map.h"


Map::Map(int width, int height, TileType* mapInfo, TileFactory* factory) {
	mMapWidth = width;
	mMapHeight = height;
	mTileFactory.reset(factory);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Tile* tile = mTileFactory->createTile(mapInfo[getIndex(x, y)]);
			tile->xIndex = x;
			tile->yIndex = y;
			vector2f position(x * TILE_WIDTH, y * TILE_HEIGHT);

			tile->getBody()->setPosition(&position);

			mMap.push_back(std::shared_ptr<Tile>(tile));
		}
	}
}

Tile* Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return nullptr;
	}

	return mMap[index].get();
}

Tile** Map::findPath(int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return nullptr;
	}

	auto endTile = mMap[endIndex];

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {
		// dumb heuristic based on distance.
		int deltaX = left.tile->xIndex - endTile->xIndex;
		int deltaY = left.tile->yIndex - endTile->yIndex;
		float leftDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		deltaX = right.tile->xIndex - endTile->xIndex;
		deltaY = right.tile->yIndex - endTile->yIndex;
		float rightDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		return (leftDist + left.cost) > (rightDist + right.cost);
	};

	int startIndex = getIndex(startX, startY);
	auto startTile = mMap[startIndex];
	std::unordered_set<Tile*> openSetLookup;
	std::priority_queue < Node, std::vector<Node>, decltype(comparitor)> openSet(comparitor);
	openSet.emplace(Node{ startTile.get(), 0 });

	// set of explored tiles.
	std::unordered_set<Tile*> closedSet;
	// back path map.
	std::unordered_map<Tile*, Tile*> pathMap;


	while (openSet.size() > 0) {
		auto current = openSet.top();
		openSet.pop();
		closedSet.emplace(current.tile);

		if (current.tile->xIndex == endX && current.tile->yIndex == endY) {
			std::vector<Tile*> path;
			auto tile = current.tile;
			while (tile != startTile.get()) {
				path.insert(path.begin(), tile);
				tile = pathMap[tile];
			}

			

			return &path[0];
		}

		int currX = current.tile->xIndex;
		int currY = current.tile->yIndex;
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

				auto neighborTile = mMap[index].get();
				if (!neighborTile->canOccupy() || closedSet.find(neighborTile) != closedSet.end() || openSetLookup.find(neighborTile) != openSetLookup.end()) {
					continue;
				}

				pathMap[neighborTile] = current.tile;
				openSet.emplace(Node{ neighborTile, current.cost + 1 });
				openSetLookup.emplace(neighborTile);
			}
		}
	}
}