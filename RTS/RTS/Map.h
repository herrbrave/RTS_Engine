#ifndef __MAP_H__
#define __MAP_H__

#include<queue>
#include<unordered_set>

#include"EntityFactory.h"

struct MapConfig {
	int tileWidth;
	int tileHeight;
	int mapWidth;
	int mapHeight;
	std::vector<unsigned long> tiles;
	std::vector<unsigned long> objects;
};
typedef shared_ptr<MapConfig> MapConfigPtr;
typedef weak_ptr<MapConfig> WeakMapConfigPtr;

struct Node {
	EntityPtr tile;
	int cost;
};
typedef shared_ptr<Node> NodePtr;
typedef weak_ptr<Node> WeakNodePtr;

class TileComponent;
typedef shared_ptr<TileComponent> TileComponentPtr;
typedef weak_ptr<TileComponent> WeakTileComponentPtr;

class Map;
typedef shared_ptr<Map> MapPtr;
typedef weak_ptr<Map> WeakMapPtr;

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
		serializer.writer.Uint((Uint8)componentId);

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
	Map(MapConfigPtr config, EntityVendorPtr entityVendor);

	WeakEntityPtr getTileAt(int x, int y);

	void findPath(vector<WeakEntityPtr> path, int startX, int startY, int endX, int endY);

	int getMapWidth() {
		return mMapConfig->mapWidth;
	}

	int getMapHeight() {
		return mMapConfig->mapHeight;
	}

	WeakEntityPtr tileAtPoint(const Vector2f& point);

private:
	EntityVendorPtr mEntityVendor{ nullptr };
	MapConfigPtr mMapConfig{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapConfig->mapWidth || y < 0 || y >= mMapConfig->mapHeight) {
			return -1;
		}

		return (y * mMapConfig->mapWidth) + x;
	}
};

#endif // !__MAP_H__
