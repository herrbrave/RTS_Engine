#ifndef __MAP_H__
#define __MAP_H__

#include<queue>
#include<vector>
#include<unordered_map>
#include<unordered_set>

#include"Entity.h"

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

const static unsigned long TILE_COMPONENT_ID = sComponentId++;
class TileComponent : public Component {
public:
	int type, x, y;
	bool canOccupy{ true };

	TileComponent(unsigned long entityId, int x, int y) : Component(entityId, TILE_COMPONENT_ID) {
		this->x = x;
		this->y = y;
	}

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint64(componentId);

		serializer.writer.String("type");
		serializer.writer.Uint(type);

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
	Map(MapConfig* config, EntitySystem* entitySystem);

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
	EntitySystem* mEntitySystem{ nullptr };
	std::unique_ptr<MapConfig> mMapConfig{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapConfig->mapWidth || y < 0 || y >= mMapConfig->mapHeight) {
			return -1;
		}

		return (y * mMapConfig->mapWidth) + x;
	}
};

class TileFactory : public EntityFactory {
public:
	TileFactory(EntitySystem* entitySystem, GraphicsSystem* graphics, PhysicsSystem* physics) : EntityFactory(entitySystem, graphics, physics) {}

	Entity* createTile(const std::string& assetTag, int xIndex, int yIndex, vector2f* position, float tx, float ty, float width, float height);
};

class MapFactory {
public:
	MapFactory(TileFactory* tileFactory, EntitySystem* entitySystem, GraphicsSystem* graphicsSystem);

	Map* createMap(const std::string pathToMap);

private:
	GraphicsSystem* mGraphicsSystem{ nullptr };
	EntitySystem* mEntitySystem{ nullptr };
	std::unique_ptr<TileFactory> mTileFactory{ nullptr };
};

#endif // !__MAP_H__
