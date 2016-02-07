#ifndef __MAP_H__
#define __MAP_H__

#include<queue>
#include<vector>
#include<unordered_map>
#include<unordered_set>

#include"Tile.h"

struct Node {
	Tile* tile;
	int cost;
};

class Map {
public:
	Map(int width, int height, TileType* mapInfo, TileFactory* factory);

	Tile* getTileAt(int x, int y);

	Tile** findPath(int startX, int startY, int endX, int endY);

	inline int getMapWidth() {
		return mMapWidth;
	}

	inline int getMapHeight() {
		return mMapHeight;
	}

private:
	int mMapWidth;
	int mMapHeight;
	std::vector<std::shared_ptr<Tile>> mMap;
	std::unique_ptr<TileFactory> mTileFactory{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapWidth || y < 0 || y >= mMapHeight) {
			return -1;
		}

		return (y * mMapWidth) + x;
	}
};


#endif // !__MAP_H__
