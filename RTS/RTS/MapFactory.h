#ifndef __MAP_FACTORY_H__
#define __MAP_FACTORY_H__

#include<iostream>
#include<cmath>

#include"EntityFactory.h"
#include"Map.h"
#include"System.h"
#include"ScriptFactory.h"
#include"TMXMap.h"

class MapFactory;
typedef shared_ptr<MapFactory> MapFactoryPtr;
typedef weak_ptr<MapFactory> WeakMapFactoryPtr;

TilesetPtr loadTilesetFromTMX(const TMXMapPtr tmxMap, MapPtr map);
void loadObjectLayerFromTMX(TMXLayerPtr tmxLayer, MapPtr map);
void loadTileLayerFromTMX(TMXLayerPtr tmxLayer, MapPtr map);
MapPtr loadMapFromTMX(const string& path, SystemManagerPtr systemManager);

void applyGrid(SystemManagerPtr systemManager, unsigned long entityId, GridPtr grid, MapPtr map);

class MapFactory {
public:
	MapFactory(EntityFactoryPtr factory, LuaScriptFactoryPtr luaScriptFactory, SystemManagerPtr systemManager);

	MapPtr createTMXMap(const string& path);

private:

	SystemManagerPtr mSystemManager{ nullptr };
	EntityFactoryPtr mFactory{ nullptr };
	LuaScriptFactoryPtr mLuaScriptFactory{ nullptr };
};

#endif // !__MAP_FACTORY_H__
