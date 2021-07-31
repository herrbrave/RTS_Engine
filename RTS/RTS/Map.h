#ifndef __MAP_H__
#define __MAP_H__

#include<chrono>
#include<queue>
#include<unordered_set>
#include<string.h>

#include"Animation.h"

static const unsigned int GRID_WIDTH = 10;
static const unsigned int GRID_HEIGHT = 10;

class Tile;
typedef shared_ptr<Tile> TilePtr;
typedef weak_ptr<Tile> WeakTilePtr;

class Tileset;
typedef shared_ptr<Tileset> TilesetPtr;
typedef weak_ptr<Tileset> WeakTilesetPtr;

class Cell;
typedef shared_ptr<Cell> CellPtr;
typedef weak_ptr<Cell> WeakCellPtr;

class Grid;
typedef shared_ptr<Grid> GridPtr;
typedef weak_ptr<Grid> WeakGridPtr;

class GridHandler;
typedef shared_ptr<GridHandler> GridHandlerPtr;
typedef weak_ptr<GridHandler> WeakGridHandlerPtr;

class GridDrawable;
typedef shared_ptr<GridDrawable> GridDrawablePtr;
typedef weak_ptr<GridDrawable> WeakGridDrawablePtr;

class GridComponent;
typedef shared_ptr<GridComponent> GridComponentPtr;
typedef weak_ptr<GridComponent> WeakGridComponentPtr;

class MapConfig;
typedef shared_ptr<MapConfig> MapConfigPtr;
typedef weak_ptr<MapConfig> WeakMapConfigPtr;

class Map;
typedef shared_ptr<Map> MapPtr;
typedef weak_ptr<Map> WeakMapPtr;

class MapConfig : public Serializable {
public:
	int mapWidth;
	int mapHeight;
	float scale;
	TilesetPtr tileset;
	string script;
	vector<string> images;

	MapConfig() :mapWidth(0), mapHeight(0), scale(0.0), script("") {}

	MapConfig(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const override;
};

class Tile : public Serializable {
public:
	TexturePtr texture;
	AnimationPtr animation;
	bool collision;
	int collisionWidth;
	int collisionHeight;
	bool animated;
	string script;

	Tile() : collision(false), collisionWidth(0), collisionHeight(0), animated(false), script("") {}

	Tile(const Tile& tile);

	Tile(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const override;
};

class Tileset : public Serializable {
public:
	int tileWidth;
	int tileHeight;
	unordered_map<unsigned int, TilePtr> tiles;

	Tileset() : tileWidth(0), tileHeight(0) {}

	Tileset(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const override;
};

class Cell : public Serializable {
public:
	vector<TilePtr> tiles;
	int x = 0;
	int y = 0;
	Uint8 r = 255;
	Uint8 g = 255;
	Uint8 b = 255;
	Uint8 a = 255;

	Cell() : x(0), y(0), r(255), g(255), b(255), a(255) {}

	Cell(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const override;
};

class Grid : public Serializable {
public:
	vector<CellPtr> cells;
	unsigned int rows;
	unsigned int columns;
	unsigned int startX;
	unsigned int startY;
	unsigned int tileW;
	unsigned int tileH;
	string name;

	Grid() : rows(0), columns(0), startX(0), startY(0), tileW(0), tileH(0), name("") {}

	Grid(const rapidjson::Value& root);

	CellPtr at(int x, int y);

	void serialize(Serializer& serializer) const override;
};

class GridHandler {
public:

	virtual GridPtr getGrid() = 0;

	virtual void update(Graphics& graphicsRef) = 0;
};

class GridDrawable : public Drawable, public GridHandler {
public:
	TexturePtr texture;
	GridPtr grid;

	GridDrawable(GridPtr grid) : Drawable(GRID_WIDTH * grid->tileW, GRID_HEIGHT * grid->tileH), grid(grid) {
		texture = std::make_shared<Texture>(grid->name, 0, 0, width, height);
	}

	GridDrawable(const rapidjson::Value& root);

	GridPtr getGrid() override;

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void update(Graphics& graphicsRef) override;

	void forceRedraw();

protected:
	bool redraw = true;
	void onSerialize(Serializer& serializer) const override;
};

class GridComponent : public Component {
public:
	GridDrawablePtr gridDrawable;

	GridComponent(unsigned long entityId, GridDrawablePtr gridDrawable, TilesetPtr tileset) : Component(entityId, ComponentType::GRID_COMPONENT), gridDrawable(gridDrawable) {}

	GridComponent(const rapidjson::Value& root);

	void setColorAtPoint(const Vector2f& point, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void setColor(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void setTextureAtPoint(const Vector2f& point, unsigned int tileId, bool flipHorizontal = false, bool flipVertical = false, bool flipDiagonal = false, float angle = 0.0f);

	void setTexture(int x, int y, unsigned int tileId, bool flipHorizontal = false, bool flipVertical = false, bool flipDiagonal = false, float angle = 0.0f);

	void pushTextureAtPoint(const Vector2f& point, unsigned int tileId, bool flipHorizontal = false, bool flipVertical = false, bool flipDiagonal = false, float angle = 0.0f);

	void pushTexture(int x, int y, unsigned int tileId, bool flipHorizontal = false, bool flipVertical = false, bool flipDiagonal = false, float angle = 0.0f);

	void popTextureAtPoint(const Vector2f& point);

	void popTexture(int x, int y);

	void serialize(Serializer& serializer) const override;

private: 
	TilesetPtr tileset;
};

class Map : public Serializable {
public:
	std::vector<GridPtr> grids;

	Map(MapConfigPtr config);

	Map(const rapidjson::Value& root) { }

	GridPtr gridAt(int x, int y);

	CellPtr getCellAt(int x, int y);

	CellPtr cellAtPoint(const Vector2f& point);

	void findPath(vector<Vector2fPtr> path, int startX, int startY, int endX, int endY);

	int getMapWidth() {
		return mMapConfig->mapWidth;
	}

	int getMapHeight() {
		return mMapConfig->mapHeight;
	}

	int getGridWidth() {
		return std::ceil((float)getMapWidth() / (float)GRID_WIDTH);
	}

	int getGridHeight() {
		return std::ceil((float)getMapHeight() / (float)GRID_HEIGHT);
	}

	int getTileWidth() {
		return mMapConfig->tileset->tileWidth * mMapConfig->scale;
	}

	int getTileHeight() {
		return mMapConfig->tileset->tileHeight * mMapConfig->scale;
	}

	MapConfigPtr getMapConfig() {
		return this->mMapConfig;
	}

	void serialize(Serializer& serializer) const override {}

private:
	MapConfigPtr mMapConfig{ nullptr };

	inline int getIndex(int x, int y) {
		if (x < 0 || x >= mMapConfig->mapWidth || y < 0 || y >= mMapConfig->mapHeight) {
			return -1;
		}

		return (y * mMapConfig->mapWidth) + x;
	}
};
#endif // !__MAP_H__
