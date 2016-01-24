#include"Tile.h"

bool Tile::canOccupy() {
	return !mOccupied && (mTileType == DIRT || mTileType == GRASS);
}

void Tile::setOccupied(bool occupied) {
	mOccupied = occupied;
}