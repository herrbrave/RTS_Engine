#ifndef __MAP_FACTORY_H__
#define __MAP_FACTORY_H__

#include<iostream>
#include<cmath>

#include"EntityFactory.h"
#include"Map.h"
#include"System.h"
#include"ScriptFactory.h"

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

	EntityPtr createTile(const string& assetTag, int xIndex, int yIndex, const Vector2f& position, float width, float height, float tx, float ty, float tw, float th, bool canOccupy);
};

void applyTile(SystemManagerPtr systemManager, unsigned long entityId, int x, int y, bool canOccupy);

class MapFactory {
public:
	MapFactory(TileFactoryPtr tileFactory, LuaScriptFactoryPtr luaScriptFactory, SystemManagerPtr systemManager);

	MapPtr createMap(const string& pathToMap);

private:
	void loadTileLayer(const TMXLayerPtr& layer, int width, int height, int tileWidth, int tileHeight, MapConfig& mapConfig, int drawOrder, float scale);
	void loadObjectLayer(const TMXLayerPtr& layer , MapConfig& mapConfig, int drawOrder, float scale);

	void loadGridTileset(const TMXTilesetPtr& tileset, Tileset& tiles, TileAnimationSet& animations, float scale);

	SystemManagerPtr mSystemManager{ nullptr };
	TileFactoryPtr mTileFactory{ nullptr };
	LuaScriptFactoryPtr mLuaScriptFactory{ nullptr };
};

#endif // !__MAP_FACTORY_H__
