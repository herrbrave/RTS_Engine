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
	Tile(Body* body, Drawable* drawable, TileType tileType) : Entity(body, drawable) {
		mTileType = tileType;
	}

	void setOccupied(bool occupied);
	bool canOccupy();

	void onUpdate() override;

	int xIndex;
	int yIndex;
private:
	bool mOccupied{ false };
	TileType mTileType;
};

class TileFactory : EntityFactory {
public:
	TileFactory(GraphicsSystem* graphics, PhysicsSystem* physics) : EntityFactory(graphics, physics) {}

	Tile* createTile(TileType tileType);
};

#endif // !__TILE_H__
