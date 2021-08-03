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

			GridComponentPtr gridComponent = entity->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
			gridComponent->gridDrawable->setDrawDepth(1);

			world->grids[x].push_back(entity);
		}
	}

	for (auto obj : map->objects) {
		EntityPtr entity = entityFactory->createPhysicsEntity(obj->position->x, obj->position->y, obj->tile->collisionWidth, obj->tile->collisionHeight);
		if (obj->tile->animated) {
			AnimationSetPtr animation = std::make_shared<AnimationSet>();
			animation->spritesheet = obj->tile->animation->frames.at(0)->assetTag;
			animation->animations["default"] = obj->tile->animation;
			animation->defaultAnimationName = "default";
			animation->name = obj->name;
			applyAnimation(systemManager, entity->id, animation, obj->tile->collisionWidth, obj->tile->collisionHeight);

			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setZOrder(10);
			animationComponent->loop();
		}
		else if (obj->tile->texture != nullptr) {
			applyDrawable(systemManager, entity->id, obj->tile->texture, obj->tile->collisionWidth, obj->tile->collisionHeight);
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);
			drawableComponent->setZOrder(10);
		}
		
		if (!obj->tile->script.empty()) {;
			applyScript(systemManager, entity->id, obj->tile->script);
		}
		world->entities.push_back(entity);
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