#include"Map.h"


Map::Map(int width, int height, std::vector<TileType> mapInfo, std::shared_ptr<Camera> camera) {
	mMapWidth = width;
	mMapHeight = height;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			mMap.push_back(std::shared_ptr<Tile>(new Tile(x, y, mapInfo[getIndex(x, y)], camera)));
		}
	}
}
void Map::draw(std::shared_ptr<SDL_Renderer> renderer) {
	for (auto tile : mMap) {
		tile->draw(renderer);
	}
}

std::shared_ptr<Tile> Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return nullptr;
	}

	return mMap[index];
}

std::vector<std::shared_ptr<Tile>> Map::findPath(int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return std::vector < std::shared_ptr<Tile> > {};
	}

	auto endTile = mMap[endIndex];

	// tiles to select.
	auto comparitor = [endTile](std::shared_ptr<Node> left, std::shared_ptr<Node> right) {
		// dumb heuristic based on distance.
		int deltaX = left->tile->mXLoc - endTile->mXLoc;
		int deltaY = left->tile->mYLoc - endTile->mYLoc;
		float leftDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		deltaX = right->tile->mXLoc - endTile->mXLoc;
		deltaY = right->tile->mYLoc - endTile->mYLoc;
		float rightDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		return (leftDist + left->cost) > (rightDist + right->cost);
	};

	int startIndex = getIndex(startX, startY);
	auto startTile = mMap[startIndex];
	std::unordered_set<std::shared_ptr<Tile>> openSetLookup;
	std::priority_queue < std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(comparitor)> openSet(comparitor);
	openSet.emplace(std::shared_ptr<Node>(new Node{ startTile, 0 }));

	// set of explored tiles.
	std::unordered_set<std::shared_ptr<Tile>> closedSet;
	// back path map.
	std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<Tile>> pathMap;


	while (openSet.size() > 0) {
		auto current = openSet.top();
		openSet.pop();
		closedSet.emplace(current->tile);

		if (current->tile->mXLoc == endX && current->tile->mYLoc == endY) {
			std::vector<std::shared_ptr<Tile>> path;
			auto tile = current->tile;
			while (tile != startTile) {
				path.insert(path.begin(), tile);
				tile = pathMap[tile];
			}

			return path;
		}

		int currX = current->tile->mXLoc;
		int currY = current->tile->mYLoc;
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

				auto neighborTile = mMap[index];
				if (!neighborTile->canOccupy() || closedSet.find(neighborTile) != closedSet.end() || openSetLookup.find(neighborTile) != openSetLookup.end()) {
					continue;
				}

				pathMap[neighborTile] = current->tile;
				openSet.emplace(std::shared_ptr<Node>(new Node{ neighborTile,  current->cost + 1}));
				openSetLookup.emplace(neighborTile);
			}
		}
	}
}