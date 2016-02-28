#ifndef __MAP_FACTORY_H__
#define __MAP_FACTORY_H__

#include"EntityFactory.h"
#include"SystemManager.h"

class TileFactory : public EntityFactory {
public:
	TileFactory(SystemManager* systemManager) : EntityFactory(systemManager) {}

	Entity* createTile(const std::string& assetTag, int xIndex, int yIndex, vector2f* position, float tx, float ty, float width, float height);
};

class MapFactory {
public:
	MapFactory(TileFactory* tileFactory, EntitySystem* entitySystem, GraphicsSystem* graphicsSystem);

	Map* createMap(const std::string pathToMap);

private:
	GraphicsSystem* mGraphicsSystem{ nullptr };
	EntitySystem* mEntitySystem{ nullptr };
	std::unique_ptr<TileFactory> mTileFactory{ nullptr };
};

#endif // !__MAP_FACTORY_H__
