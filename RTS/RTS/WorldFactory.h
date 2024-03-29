#pragma once

#ifndef __WORLD_FACTORY_H__
#define __WORLD_FACTORY_H__

#include"MapFactory.h"
#include"World.h"

class WorldFactory;
typedef shared_ptr<WorldFactory> WorldFactoryPtr;
typedef weak_ptr<WorldFactory> WeakWorldFactoryPtr;

class WorldFactory {
public:
	WorldFactory(SystemManagerPtr systemManager, EntityFactoryPtr entityFactory);

	WorldPtr createWorldFromTMXMap(const string& path);
	WorldPtr createWorld(const string& path);
	WorldPtr createBlankWorld(int width, int height, int tileWidth, int tileHeight);

private:
	EntityFactoryPtr entityFactory;
	SystemManagerPtr systemManager;
};

#endif