#include"World.h"

World::World(const rapidjson::Value& root) {
	for (auto it = root["entities"].Begin(); it != root["entities"].End(); it++) {
		entities.push_back(std::make_shared<Entity>(*it));
	}

	for (auto it = root["grids"].Begin(); it != root["grids"].End(); it++) {
		grids.push_back(vector<EntityPtr>());
	}

	int x = 0;
	int y = 0;
	for (auto it = root["grids"].Begin(); it != root["grids"].End(); it++, x++) {
		y = 0;
		for (auto i = it->Begin(); i != it->End(); i++, y++) {
			grids[x].push_back(std::make_shared<Entity>(*i));
		}
	}

	map = std::make_shared<Map>(root["map"]);
	script = std::make_shared<Entity>(root["script"]);
}

EntityPtr World::getGridAtPoint(const Vector2f& point) {
	int x = std::floor(point.x / ((float)GRID_WIDTH * this->getMap()->getTileWidth() * this->getMap()->getMapConfig()->scale));
	int y = std::floor(point.y / ((float)GRID_HEIGHT * this->getMap()->getTileHeight() * this->getMap()->getMapConfig()->scale));

	return this->getGridAt(x, y);
}

EntityPtr World::getGridWithTileAt(int x, int y) {
	if (x < 0 || x >= this->map->getMapWidth() || y < 0 || y >= this->map->getMapHeight()) {
		throw "WORLD: Out of bounds";
	}
	int tx = std::floor((float)x / (float)GRID_WIDTH);
	int ty = std::floor((float)y / (float)GRID_HEIGHT);

	return this->getGridAt(tx, ty);
}

EntityPtr World::getGridAt(int x, int y) const {
	if (x < 0 || x >= this->map->getGridWidth() || y < 0 || y >= this->map->getGridHeight()) {
		throw "WORLD: Out of bounds";
	}

	return this->grids[x][y];
}

CellPtr World::getCellAt(int x, int y) {
	if (x < 0 || x >= this->map->getMapWidth() || y < 0 || y >= this->map->getMapWidth()) {
		throw "WORLD: Out of bounds";
	}

	EntityPtr grid = this->getGridWithTileAt(x, y);
	GridComponentPtr gridComponent = grid->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
	return gridComponent->getGridDrawable()->getGrid()->at(x, y);
}

CellPtr World::getCellAtPoint(const Vector2f& point) {
	if (!this->pointInWorldBounds(point)) {
		throw "WORLD: Out of Bounds";
	}

	int x = std::floor(point.x / ((float)this->getMap()->getTileWidth()));
	int y = std::floor(point.y / ((float)this->getMap()->getTileHeight()));

	return this->getCellAt(x, y);
}

bool World::inBounds(int x, int y) {
	return 0 <= x && x < this->getMap()->getMapWidth() && 0 <= y && y < this->getMap()->getMapHeight();
}

bool World::pointInWorldBounds(const Vector2f& point) {
	return 0.0f <= point.x && point.x < this->getMap()->getMapWidthPixels() && 0.0f <= point.y && point.y < this->getMap()->getMapHeightPixels();
}

double tileDistance(int x0, int y0, int x1, int y1) {
	return std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

Path* World::buildPath(int sx, int sy, int ex, int ey) {
	if (!this->inBounds(sx, sy) || !this->inBounds(ex, ey)) {
		return GCC_NEW Path();
	}

	Path* path = GCC_NEW Path();
	CellPtr start = this->getCellAt(sx, sy);

	unordered_set<std::pair<int, int>, hash_pair, equal_pair> visited;
	unordered_map<std::pair<int, int>, std::pair<int, int>, hash_pair, equal_pair> backref;
	unordered_map<std::pair<int, int>, double, hash_pair, equal_pair> gVals;
	unordered_map<std::pair<int, int>, double, hash_pair, equal_pair> fVals;

	auto astarComp = [&](const std::pair<int, int> left, const std::pair<int, int> right) {
		return fVals[right] < fVals[left];
	};
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(astarComp)> searchPriority(astarComp);

	std::pair<int, int> cell(start->x, start->y);
	gVals[cell] = 0;
	fVals[cell] = tileDistance(sx, sy, ex, ey);
	searchPriority.push(cell);

	while (!searchPriority.empty()) {
		cell = searchPriority.top();
		searchPriority.pop();
		visited.insert(cell);

		if (cell.first == ex && cell.second == ey) {
			
			while (!(cell.first == sx && cell.second == sy)) {
				int x = (cell.first * this->getMap()->getTileWidth()) + ((this->getMap()->getTileWidth()) / 2);
				int y = (cell.second * this->getMap()->getTileHeight()) + ((this->getMap()->getTileWidth()) / 2);

				path->path.push_back(Vector2f(x, y));
				cell = backref[cell];
			}

			// make sure to add the first tile location to the map too.
			int x = (cell.first * this->getMap()->getTileWidth()) + ((this->getMap()->getTileWidth()) / 2);
			int y = (cell.second * this->getMap()->getTileHeight()) + ((this->getMap()->getTileWidth()) / 2);

			path->path.push_back(Vector2f(x, y));

			std::reverse(path->path.begin(), path->path.end());
			break;
		}

		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0) {
					continue;
				}

				int px = cell.first + dx;
				int py = cell.second + dy;
				if (!this->inBounds(px, py)) {
					continue;
				}

				CellPtr neighbor = this->getCellAt(px, py);
				std::pair<int, int> neighborCell(px, py);
				if (visited.find(neighborCell) != visited.end() || !neighbor->canOccupy()) {
					continue;
				}

				if (dx != 0 && dy != 0) {
					// makeing sure we don't try to cut any corners
					// e.g. # = current position, 0 = can occupy, x = cannot occupy
					// 000
					// 0#x
					// 00x
					// Moving diagonal from # would result in clipping the wall just to the right.

					CellPtr xAdjacent = this->getCellAt(px, cell.second);
					CellPtr yAdjacent = this->getCellAt(cell.first, py);
					if (!xAdjacent->canOccupy() || !yAdjacent->canOccupy()) {
						continue;
					}
				}

				double gScore = gVals[cell] + tileDistance(cell.first, cell.second, neighbor->x, neighbor->y);
				if (gVals.find(neighborCell) == gVals.end() || gScore < gVals[neighborCell]) {
					backref[neighborCell] = cell;
					gVals[neighborCell] = gScore;
					fVals[neighborCell] = gScore + tileDistance(neighborCell.first, neighborCell.second, ex, ey);
					searchPriority.push(neighborCell);
				}
			}
		}
	}

	return path;
}

MapPtr World::getMap() {
	return this->map;
}

void World::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("grids");
	serializer.writer.StartArray();
	for (int y = 0; y < map->getGridHeight(); y++) {
		serializer.writer.StartArray();
		for (int x = 0; x < map->getGridWidth(); x++) {
			EntityPtr grid = getGridAt(x, y);
			grid->serialize(serializer);
		}
		serializer.writer.EndArray();
	}
	serializer.writer.EndArray();

	serializer.writer.String("entities");
	serializer.writer.StartArray();
	for (EntityPtr entity : this->entities) {
		entity->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("map");
	map->serialize(serializer);

	serializer.writer.String("script");
	script->serialize(serializer);

	serializer.writer.EndObject();
}