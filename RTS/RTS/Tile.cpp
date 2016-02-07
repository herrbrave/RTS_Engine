#include"Tile.h"

bool Tile::canOccupy() {
	return !mOccupied && (mTileType == DIRT || mTileType == GRASS);
}

void Tile::setOccupied(bool occupied) {
	mOccupied = occupied;
}

void Tile::onUpdate() {

}

Tile* TileFactory::createTile(TileType tileType) {
	Body* body;
	Drawable* drawable;
	int width = TILE_WIDTH;
	int height = TILE_HEIGHT;
	if (tileType == GRASS) {
		body = new BlockBody(0, 0, width, height);
		drawable = new BlockDrawable(width, height, 0, 255, 0, 255);
	}
	else if (tileType == DIRT) {
		body = new BlockBody(0, 0, width, height);
		drawable = new BlockDrawable(width, height, 130, 100, 0, 255);
	}
	else if (tileType == WATER) {
		body = new BlockBody(0, 0, width, height);
		drawable = new BlockDrawable(width, height, 0, 0, 200, 255);
	}
	else if (tileType == TREES) {
		body = new BlockBody(0, 0, width, height);
		drawable = new BlockDrawable(width, height, 0, 128, 0, 255);
	}
	else {
		body = new BlockBody(0, 0, width, height);
		drawable = new BlockDrawable(width, height, 255, 255, 255, 255);
	}
	Tile* tile = new Tile(body, drawable, tileType);
	mPhysicsSystem->registerBody(tile->getId(), body);
	mGraphicsSystem->registerDrawable(tile->getId(), drawable);

	return tile;
}