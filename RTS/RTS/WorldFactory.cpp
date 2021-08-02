#include"WorldFactory.h"

WorldFactory::WorldFactory(SystemManagerPtr systemManager, EntityFactoryPtr entityFactory) : systemManager(systemManager), entityFactory(entityFactory) {

}

WorldPtr WorldFactory::createWorldFromTMXMap(const string& path) {
	MapPtr map = loadMapFromTMX(path, systemManager);
	WorldPtr world = std::make_shared<World>();
	world->map = map;

	MapConfigPtr mapConfig = map->getMapConfig();
	int gridWidth = map->getGridWidth();
	int gridHeight = map->getGridHeight();
	int width = map->getTileWidth() * GRID_WIDTH;
	int height = map->getTileHeight() * GRID_HEIGHT;

	for (int x = 0; x < gridWidth; x++) {
		world->grids.push_back(vector<EntityPtr>());
	}

	for (int y = 0; y < gridHeight; y++) {
		for (int x = 0; x < gridWidth; x++) {
			EntityPtr entity = entityFactory->createPhysicsEntity(x * width + (width / 2), y * height + (height / 2), width, height);

			GridPtr grid = map->gridAt(x * GRID_WIDTH, y * GRID_HEIGHT);
			applyGrid(systemManager, entity->id, grid, map);
			world->grids[x].push_back(entity);
		}
	}

	if (!mapConfig->script.empty()) {
		EntityPtr scriptObject = entityFactory->createPhysicsEntity(-1, -1, 1, 1);
		applyScript(systemManager, scriptObject->id, mapConfig->script);
		world->script = scriptObject;
	}

	MapSystemPtr mapSystem = systemManager->getSystemByType<MapSystem>(SystemType::MAP);
	mapSystem->setMap(map);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	for (string image : mapConfig->images) {
		graphicsSystem->addTexture(image, image);
	}

	return world;
}