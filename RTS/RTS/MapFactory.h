#ifndef __MAP_FACTORY_H__
#define __MAP_FACTORY_H__

#include<iostream>

#include"EntityFactory.h"
#include"Map.h"
#include"System.h"

class TileFactory : public EntityFactory {
public:
	TileFactory(SystemManager* systemManager) : EntityFactory(systemManager) {}

	Entity* createTile(const std::string& assetTag, int xIndex, int yIndex, vector2f* position, float tx, float ty, float width, float height);
};

class MapFactory {
public:
	MapFactory(TileFactory* tileFactory, SystemManager* systemManager);

	Map* createMap(const std::string pathToMap);

private:
	void loadTileLayer(const rapidjson::Value& tileLayer, std::string assetTag, int offset, int width, int height, int tileWidth, int tileHeight, int columns, MapConfig& mapConfig);
	void loadObjectLayer(const rapidjson::Value& objectLayer, MapConfig& mapConfig);

	SystemManager* mSystemManager{ nullptr };
	TileFactory* mTileFactory{ nullptr };
};

#endif // !__MAP_FACTORY_H__
