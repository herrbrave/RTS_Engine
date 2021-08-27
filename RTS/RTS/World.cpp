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

	int x = std::floor(point.x / ((float)GRID_WIDTH * this->getMap()->getTileWidth() * this->getMap()->getMapConfig()->scale));
	int y = std::floor(point.y / ((float)GRID_HEIGHT * this->getMap()->getTileHeight() * this->getMap()->getMapConfig()->scale));

	return this->getCellAt(x, y);
}

bool World::inBounds(int x, int y) {
	return 0 <= x && x < this->getMap()->getMapWidth() && 0 <= y && y < this->getMap()->getMapHeight();
}

bool World::pointInWorldBounds(const Vector2f& point) {
	return 0.0f <= point.x && point.x < this->getMap()->getMapWidthPixels() && 0.0f <= point.y && point.y < this->getMap()->getMapHeightPixels();
}

Path* World::buildPath(int sx, int sy, int ex, int ey) {
	if (!this->inBounds(sx, sy) || !this->inBounds(ex, ey)) {
		return GCC_NEW Path();
	}

	Path* path = GCC_NEW Path();
	CellPtr start = this->getCellAt(sx, sy);

	unordered_set<std::pair<int, int>, hash_pair, equal_pair> visited;
	unordered_map<std::pair<int, int>, std::pair<int, int>, hash_pair, equal_pair> backref;
	std::deque<std::pair<int, int>> queue;

	std::pair<int, int> cell(start->x, start->y);
	queue.push_back(cell);
	visited.insert(cell);

	while (!queue.empty()) {
		cell = queue.front();
		queue.pop_front();
		visited.insert(cell);

		if (cell.first == ex && cell.second == ey) {
			
			while (!(cell.first == sx && cell.second == sy)) {
				int x = (cell.first * this->getMap()->getTileWidth() * this->getMap()->getMapConfig()->scale) + ((this->getMap()->getTileWidth() * this->getMap()->getMapConfig()->scale) / 2);
				int y = (cell.second * this->getMap()->getTileHeight() * this->getMap()->getMapConfig()->scale) + ((this->getMap()->getTileWidth() * this->getMap()->getMapConfig()->scale) / 2);

				path->path.push_back(Vector2f(x, y));
				cell = backref[cell];
			}
			std::reverse(path->path.begin(), path->path.end());
			break;
		}

		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (std::abs(dx) == std::abs(dy)) {
					continue;
				}

				int px = cell.first + dx;
				int py = cell.second + dy;
				if (!this->inBounds(px, py)) {
					continue;
				}

				CellPtr neighbor = this->getCellAt(px, py);
				std::pair<int, int> neighborCell(px, py);
				if (visited.find(neighborCell) != visited.end() || std::find(queue.begin(), queue.end(), neighborCell) != queue.end() || !neighbor->canOccupy()) {
					continue;
				}

				backref[neighborCell] = cell;
				queue.push_back(neighborCell);
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