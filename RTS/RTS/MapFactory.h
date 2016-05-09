#ifndef __MAP_FACTORY_H__
#define __MAP_FACTORY_H__

#include<iostream>

#include"EntityFactory.h"
#include"Map.h"
#include"System.h"

class TileFactory;
typedef shared_ptr<TileFactory> TileFactoryPtr;
typedef weak_ptr<TileFactory> WeakTileFactoryPtr;

class TileFactory;
typedef shared_ptr<TileFactory> TileFactoryPtr;
typedef weak_ptr<TileFactory> WeakTileFactoryPtr;

class MapFactory;
typedef shared_ptr<MapFactory> MapFactoryPtr;
typedef weak_ptr<MapFactory> WeakMapFactoryPtr;

class TileFactory : public EntityFactory {
public:
	TileFactory(SystemManagerPtr systemManager) : EntityFactory(systemManager) {}

	EntityPtr createTile(const string& assetTag, int xIndex, int yIndex, const Vector2f& position, float tx, float ty, float width, float height);
};

class MapFactory {
public:
	MapFactory(TileFactoryPtr tileFactory, SystemManagerPtr systemManager);

	MapPtr createMap(const string& pathToMap);

private:
	void loadTileLayer(const rapidjson::Value& tileLayer, const string& assetTag, int offset, int width, int height, int tileWidth, int tileHeight, int columns, MapConfig& mapConfig);
	void loadObjectLayer(const rapidjson::Value& objectLayer, MapConfig& mapConfig);

	SystemManagerPtr mSystemManager{ nullptr };
	TileFactoryPtr mTileFactory{ nullptr };
};

#endif // !__MAP_FACTORY_H__
