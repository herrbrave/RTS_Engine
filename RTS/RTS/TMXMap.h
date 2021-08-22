#pragma once

#include"Constants.h"

#include<rapidjson/document.h>
#include<fstream>
#include<iostream>

using std::string;
using std::shared_ptr;
using std::weak_ptr;
using std::vector;

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
	shared_ptr<ClassType> getValue() {
		shared_ptr<void> val = value;
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(val);
		return converted;
	}
};

TMXMapPtr parseTMXMap(const string& file);
void parseTMXProperties(const rapidjson::Value& root, TMXProperties& props);
void parseTMXLayers(const rapidjson::Value& root, TMXLayers& layers);
void parseTMXTilesets(const rapidjson::Value& root, TMXTilesets& tilesets);
void parseTMXObjects(const rapidjson::Value& root, TMXObjects& objects);
void parseTMXTiles(const rapidjson::Value& root, TMXTiles& tiles, int firstgid);
void parseTMXFrames(const rapidjson::Value& root, TMXFrames& frames, int firstgid);