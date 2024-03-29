 #include"WorldFactory.h"

WorldFactory::WorldFactory(SystemManagerPtr sys, EntityFactoryPtr entityFactory) : systemManager(sys), entityFactory(entityFactory) {
	EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			LoadWorldData data = dynamic_cast<const LoadWorldData&>(eventData);
			WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

			worldSystem->destroyWorld();
			if (data.path.size() > 0) {
				worldSystem->addWorld(createWorldFromTMXMap(data.path));
			}
			else {
				worldSystem->addWorld(createBlankWorld(data.w, data.h, data.tw, data.th));
			}
		});

	EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
	EventManager::getInstance().addDelegate(destroyEntityListener, EventType::LOAD_WORLD);
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

	int gridWidthPx = map->getTileWidth() * GRID_WIDTH;
	int gridHeightPx = map->getTileHeight() * GRID_HEIGHT;

	int xoffset = (gridWidthPx / 2) - (map->getTileWidth() / 2);
	int yoffset = (gridHeightPx / 2) - (map->getTileHeight() / 2);

	for (int y = 0; y < gridHeight; y++) {
		for (int x = 0; x < gridWidth; x++) {
			EntityPtr entity = entityFactory->createPhysicsEntity(x * width + (width / 2), y * height + (height / 2), width, height);

			GridPtr grid = map->gridAt(x * GRID_WIDTH, y * GRID_HEIGHT);
			applyGrid(systemManager, entity->id, grid, map);

			int sx = x * gridWidthPx + gridWidthPx / 2;
			int sy = y * gridHeightPx + gridHeightPx / 2;

			for (int y0 = 0; y0 < grid->rows; y0++) {
				for (int x0 = 0; x0 < grid->columns; x0++) {
					CellPtr cell = grid->cells.at(y0 * grid->columns + x0);
					bool collision = false;
					string script("");
					for (auto tile : cell->tiles) {
						if (tile->collision) {
							collision = true;
						}
						if (!tile->script.empty()) {
							script = tile->script;
						}
					}
					if (collision || !script.empty()) {
						int lx = x0 * map->getTileWidth();
						int ly = y0 * map->getTileHeight();
						EntityPtr t = entityFactory->createPhysicsEntity(sx + lx - xoffset, sy + ly - yoffset, map->getTileWidth(), map->getTileHeight(), collision);
						if (!script.empty()) {
							applyScript(systemManager, t->id, script);
						}
						world->entities.push_back(t);
					}
				}
			}

			GridComponentPtr gridComponent = entity->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
			gridComponent->gridDrawable->setDrawDepth(1);

			world->grids[x].push_back(entity);
		}
	}

	for (auto obj : map->objects) {
		EntityPtr entity = entityFactory->createPhysicsEntity(obj->position->x, obj->position->y, obj->tile->collisionWidth, obj->tile->collisionHeight, true);
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

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	for (string image : mapConfig->images) {
		graphicsSystem->addTexture(image, image);
	}

	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	physicsSystem->setWorldSize(map->getTileHeight() * map->getMapWidth(), map->getTileHeight() * map->getMapHeight());

	return world;
}

WorldPtr WorldFactory::createWorld(const string& path) {
	std::ifstream file;
	file.open(path);
	if (!file.is_open()) {
		ERR("Unable to open file: " + path);
	}
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	return std::make_shared<World>((const rapidjson::Value&) doc);
}

WorldPtr WorldFactory::createBlankWorld(int w, int h, int tileWidth, int tileHeight) {
	MapConfigPtr mapConfig = std::make_shared<MapConfig>();
	mapConfig->mapWidth = w;
	mapConfig->mapHeight = h;
	mapConfig->tileset = std::make_shared<Tileset>();
	mapConfig->tileset->tileWidth = tileWidth;
	mapConfig->tileset->tileHeight = tileHeight;
	mapConfig->scale = 1.0f;

	MapPtr map = std::make_shared<Map>(mapConfig);
	WorldPtr world = std::make_shared<World>();
	world->map = map;

	for (int h = 0; h < mapConfig->mapHeight; h += GRID_HEIGHT) {
		for (int w = 0; w < mapConfig->mapWidth; w += GRID_WIDTH) {
			GridPtr grid = std::make_shared<Grid>();
			grid->startX = w;
			grid->startY = h;
			grid->columns = std::min((double)mapConfig->mapWidth, (double)w + GRID_WIDTH) - w;
			grid->rows = std::min((double)mapConfig->mapHeight, (double)h + GRID_HEIGHT) - h;
			grid->name = "Grid-" + std::to_string(w / GRID_WIDTH) + "," + std::to_string(h / GRID_HEIGHT);
			grid->tileW = mapConfig->tileset->tileWidth * mapConfig->scale;
			grid->tileH = mapConfig->tileset->tileHeight * mapConfig->scale;

			for (int y = 0; y < grid->rows; y++) {
				for (int x = 0; x < grid->columns; x++) {
					CellPtr cell = std::make_shared<Cell>();
					cell->x = grid->startX + x;
					cell->y = grid->startY + y;

					grid->cells.push_back(cell);
				}
			}

			map->grids.push_back(grid);
		}
	}

	int gridWidth = map->getGridWidth();
	int gridHeight = map->getGridHeight();
	int width = map->getTileWidth() * GRID_WIDTH;
	int height = map->getTileHeight() * GRID_HEIGHT;

	for (int x = 0; x < gridWidth; x++) {
		world->grids.push_back(vector<EntityPtr>());
	}

	int gridWidthPx = map->getTileWidth() * GRID_WIDTH;
	int gridHeightPx = map->getTileHeight() * GRID_HEIGHT;

	int xoffset = (gridWidthPx / 2) - (map->getTileWidth() / 2);
	int yoffset = (gridHeightPx / 2) - (map->getTileHeight() / 2);

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

	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	physicsSystem->setWorldSize(map->getTileHeight() * map->getMapWidth(), map->getTileHeight() * map->getMapHeight());


	return world;
}