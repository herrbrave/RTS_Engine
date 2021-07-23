#ifndef __MAP_H__
#define __MAP_H__

#include<chrono>
#include<queue>
#include<unordered_set>
#include<string.h>

#include"EntityFactory.h"
#include"Animation.h"
#include"System.h"

const unsigned int TMX_FLIPPED_HORIZONTAL_FLAG = 0x80000000;
const unsigned int TMX_FLIPPED_VERTICAL_FLAG = 0x40000000;
const unsigned int TMX_FLIPPED_DIAGONAL_FLAG = 0x20000000;
unsigned int tmxClearFlags(unsigned int tileValue);

class TMXMap;
typedef shared_ptr<TMXMap> TMXMapPtr;
typedef weak_ptr<TMXMap> WeakTMXMapPtr;

class TMXLayer;
typedef shared_ptr<TMXLayer> TMXLayerPtr;
typedef weak_ptr<TMXLayer> WeakTMXLayerPtr;
typedef vector<TMXLayerPtr> TMXLayers;

class TMXObject;
typedef shared_ptr<TMXObject> TMXObjectPtr;
typedef weak_ptr<TMXObject> WeakTMXObjectPtr;
typedef vector<TMXObjectPtr> TMXObjects;

class TMXTileset;
typedef shared_ptr<TMXTileset> TMXTilesetPtr;
typedef weak_ptr<TMXTileset> WeakTMXTilesetPtr;
typedef vector<TMXTilesetPtr> TMXTilesets;

class TMXProperty;
typedef shared_ptr<TMXProperty> TMXPropertyPtr;
typedef weak_ptr<TMXProperty> WeakTMXPropertyPtr;
typedef vector<TMXPropertyPtr> TMXProperties;

class TMXTile;
typedef shared_ptr<TMXTile> TMXTilePtr;
typedef weak_ptr<TMXTile> WeakTMXTilePtr;
typedef vector<TMXTilePtr> TMXTiles;

class TMXFrame;
typedef shared_ptr<TMXFrame> TMXFramePtr;
typedef weak_ptr<TMXFrame> WeakTMXFramePtr;
typedef vector<TMXFramePtr> TMXFrames;

class TMXMap {
public:
	string backgroundColor;
	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	TMXProperties properties;
	TMXLayers layers;
	TMXTilesets tilesets;
};

class TMXLayer {
public:
	int gid;
	string name;
	string type;
	unsigned int width;
	unsigned int height;
	int x;
	int y;
	int tileCount;
	unsigned int* data;
	TMXObjects objects;
	TMXProperties properties;
};

class TMXObject {
public:
	int gid;
	string name;
	unsigned int width;
	unsigned int height;
	string type;
	float rotation;
	float x;
	float y;
	bool visible;
	TMXProperties properties;
};

class TMXTileset {
public:
	unsigned int firstgid;
	string name;
	unsigned int columns;
	unsigned int tileCount;
	unsigned int tileWidth;
	unsigned int tileHeight;
	string image;
	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int margin;
	unsigned int spacing;
	TMXTiles tiles;
	TMXProperties properties;
};

class TMXTile {
public:
	unsigned int id;
	string image;
	unsigned int imageWidth;
	unsigned int imageHeight;
	TMXLayers objectgroup;
	TMXFrames animation;
	TMXProperties properties;
};

class TMXFrame {
public:
	unsigned int duration;
	unsigned int tileid;
};

class TMXProperty {
public:
	string name;
	string type;
	VoidPtr value;

	void setValue(VoidPtr voidPtr) {
		value = std::move(voidPtr);
	}

	template<class ClassType>
	weak_ptr<ClassType> getValue() {
		shared_ptr<void> val = value;
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(val);
		return weak_ptr<ClassType>(converted);
	}
};

TMXMapPtr parseMap(const string& file);
void parseProperties(const rapidjson::Value& root, TMXProperties& props);
void parseLayers(const rapidjson::Value& root, TMXLayers& layers);
void parseTilesets(const rapidjson::Value& root, TMXTilesets& tilesets);
void parseObjects(const rapidjson::Value& root, TMXObjects& objects);
void parseTiles(const rapidjson::Value& root, TMXTiles& tiles);
void parseFrames(const rapidjson::Value& root, TMXFrames& frames);


class Tile;
typedef shared_ptr<Tile> TilePtr;
typedef weak_ptr<Tile> WeakTilePtr;
typedef vector<TilePtr> Tiles;
typedef unordered_map<unsigned int, TilePtr> Tileset;
typedef unordered_map<unsigned int, AnimationSetPtr> TileAnimationSet;

class Grid;
typedef shared_ptr<Grid> GridPtr;
typedef weak_ptr<Grid> WeakGridPtr;

struct MapConfig {
	int tileWidth;
	int tileHeight;
	int mapWidth;
	int mapHeight;
	std::vector<unsigned long> tiles;
	std::vector<unsigned long> objects;
	Tileset tileset;
	TileAnimationSet animatedTiles;
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

class Tile {
public:
	string textureAssetTag;
	int tx;
	int ty;
	int w;
	int h;
	bool collision = false;
	int xOff = 0;
	int yOff = 0;
	int collisionWidth = 0;
	int collisionHeight = 0;
	bool animated = false;
	string script;
	int angle = 0;

	Tile(const string& textureAssetTag, int tx, int ty, int w, int h) : textureAssetTag(textureAssetTag), tx(tx), ty(ty), w(w), h(h) {
	}
};

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

	int getTileWidth() {
		return mMapConfig->tileWidth;
	}

	int getTileHeight() {
		return mMapConfig->tileHeight;
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

class Grid {
public:
	vector<Tile***> tileLayers;
	unsigned int rows;
	unsigned int columns;
	unsigned int tileW;
	unsigned int tileH;
	string name;

};

class GridDrawable : public Drawable {
public:
	TexturePtr texture;
	GridPtr grid;

	GridDrawable(GridPtr grid) : Drawable(grid->rows * grid->tileW, grid->columns * grid->tileH) {
		 
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void initialize(const SystemManager& systemManager);
};
#endif // !__MAP_H__
