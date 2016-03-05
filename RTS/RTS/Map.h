#ifndef __MAP_H__
#define __MAP_H__

#include<queue>
#include<vector>
#include<unordered_map>
#include<unordered_set>

#include"EntityFactory.h"

struct MapConfig {
	int tileWidth;
	int tileHeight;
	int mapWidth;
	int mapHeight;
	std::vector<int	> tiles;
};

struct Node {
	Entity* tile;
	int cost;
};

class TileComponent : public Component {
public:
	int x, y;
	bool canOccupy{ true };

	TileComponent(unsigned long entityId, int x, int y) : Component(entityId, ComponentType::TILE_COMPONENT) {
		this->x = x;
		this->y = y;
	}

	TileComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::TILE_COMPONENT) {
		x = root["x"].GetInt();
		y = root["y"].GetInt(); 
		canOccupy = root["canOccupy"].GetBool();
	}

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint(componentId);

		serializer.writer.String("x");
		serializer.writer.Uint(x);

		serializer.writer.String("y");
		serializer.writer.Uint(y);

		serializer.writer.String("canOccupy");
		serializer.writer.Bool(canOccupy);

		serializer.writer.EndObject();
	}
};

class Map {
public:
	Map(MapConfig* config, EntityVendor* entityVendor);

	Entity* getTileAt(int x, int y);

	Entity** findPath(int startX, int startY, int endX, int endY);

	int getMapWidth() {
		return mMapConfig->mapWidth;
	}

	int getMapHeight() {
		return mMapConfig->mapHeight;
	}

	Entity* tileAtPoint(const vector2f* point);

private:
	EntityVendor* mEntityVendor{ nullptr };
	std::unique_ptr<MapConfig> mMapConfig{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapConfig->mapWidth || y < 0 || y >= mMapConfig->mapHeight) {
			return -1;
		}

		return (y * mMapConfig->mapWidth) + x;
	}
};

#endif // !__MAP_H__
