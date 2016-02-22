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

const static unsigned long TILE_COMPONENT_ID = sComponentId.fetch_add(1);
class TileComponent : public Component {
public:
	int type, x, y;
	bool canOccupy{ true };

	TileComponent(unsigned long entityId, int x, int y, int type) : Component(entityId, TILE_COMPONENT_ID) {
		this->x = x;
		this->y = y;
		this->type = type;
	}
};

class Map {
public:
	Map(MapConfig* config, EntityFactory* factory);

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
	std::unique_ptr<MapConfig> mMapConfig{ nullptr };
	std::vector<std::shared_ptr<Entity>> mMap;
	EntityFactory* mTileFactory{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapConfig->mapWidth || y < 0 || y >= mMapConfig->mapHeight) {
			return -1;
		}

		return (y * mMapConfig->mapWidth) + x;
	}
};


#endif // !__MAP_H__
