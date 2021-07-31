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
	int x = std::floor(point.x / (float)GRID_WIDTH);
	int y = std::floor(point.y / (float)GRID_HEIGHT);

	return this->getGridAt(x, y);
}

EntityPtr World::getGridAt(int x, int y) const {
	if (x < 0 || x >= this->map->getGridWidth() || y < 0 || y >= this->map->getGridHeight()) {
		throw "WORLD: Out of bounds";
	}

	return this->grids[x][y];
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

	serializer.writer.String("entitites");
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