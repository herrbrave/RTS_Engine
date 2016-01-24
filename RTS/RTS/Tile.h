#ifndef __TILE_H__
#define __TILE_H__

#include"Constants.h"
#include"Entity.h"

enum TileType {
	TREES,
	DIRT,
	WATER,
	GRASS,
	LAST
};


class Tile : public Entity {
public:
	bool mOccupied{ false };
	int mXLoc, mYLoc;
	TileType mTileType;

	Tile(int xLoc, int yLoc, TileType tileType, std::shared_ptr<Camera> camera) : Entity(xLoc * TILE_WIDTH, yLoc * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 0, 255, 0, camera) {
		mXLoc = xLoc;
		mYLoc = yLoc;
		mTileType = tileType;

		switch (tileType) {
		case TREES:
			mBlock->m_r = 0;
			mBlock->m_g = 128;
			mBlock->m_b = 0;
			break;
		case DIRT:
			mBlock->m_r = 150;
			mBlock->m_g = 75;
			mBlock->m_b = 0;
			break;
		case WATER:
			mBlock->m_r = 150;
			mBlock->m_g = 150;
			mBlock->m_b = 255;
			break;
		case GRASS:
			mBlock->m_r = 0;
			mBlock->m_g = 200;
			mBlock->m_b = 0;
			break;
		default:
			break;
		}
	}

	void setOccupied(bool occupied);
	bool canOccupy();
};

#endif // !__TILE_H__
