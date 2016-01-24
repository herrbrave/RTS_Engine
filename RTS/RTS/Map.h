#ifndef __MAP_H__
#define __MAP_H__

#include<queue>
#include<vector>
#include<unordered_map>
#include<unordered_set>

#include"Tile.h"

struct Node {
	std::shared_ptr<Tile> tile;
	int cost;
};

class Map {
public:
	Map(int width, int height, std::vector<TileType> mapInfo, std::shared_ptr<Camera> camera);

	std::shared_ptr<Tile> getTileAt(int x, int y);

	std::vector<std::shared_ptr<Tile>>& operator[] (int x) {
		std::vector<std::shared_ptr<Tile>> tiles;
		for (int y = 0; y < TILE_HEIGHT; y++) {
			int index = (y * TILE_WIDTH) + x;
			tiles.push_back(mMap[index]);
		}

		return tiles;
	}

	std::vector<std::shared_ptr<Tile>> findPath(int startX, int startY, int endX, int endY);

	void draw(std::shared_ptr<SDL_Renderer> renderer);

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

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapWidth || y < 0 || y >= mMapHeight) {
			return -1;
		}

		return (y * mMapWidth) + x;
	}
};


#endif // !__MAP_H__
