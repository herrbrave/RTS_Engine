#include"MapFactory.h"

void applyGrid(SystemManagerPtr systemManager, unsigned long entityId, GridPtr grid, MapPtr map) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	MapSystemPtr mapSystem = systemManager->getSystemByType<MapSystem>(SystemType::MAP);

	EntityPtr entity = entitySystem->getEntityById(entityId);

	GridDrawablePtr gridDrawable = std::make_shared<GridDrawable>(grid);
	GridComponentPtr gridComponent = std::make_shared<GridComponent>(entity->id, gridDrawable, map->getMapConfig()->tileset);
	entity->addComponent(gridComponent);
	graphicsSystem->addTexture(grid->name, GRID_WIDTH * grid->tileW, GRID_HEIGHT * grid->tileH);

	graphicsSystem->registerDrawable(entity->id, gridDrawable);
	mapSystem->registerGridHandler(entity->id, static_pointer_cast<GridHandler>(gridDrawable));
}

TilesetPtr loadTilesetFromTMX(const TMXMapPtr tmxMap, MapPtr map) {
	TilesetPtr tileset = std::make_shared<Tileset>();
	tileset->tileWidth = tmxMap->tileWidth;
	tileset->tileHeight = tmxMap->tileHeight;

	for (TMXTilesetPtr tmxTileset : tmxMap->tilesets) {
		string image = tmxTileset->image;
		if (!image.empty()) {
			map->getMapConfig()->images.push_back(image);
		}

		unsigned int id = tmxTileset->firstgid;
		int spacing = tmxTileset->spacing;
		int margin = tmxTileset->margin;
		int tilecount = tmxTileset->tileCount;

		int count = 0;
		// Construct all of the tiles first, then we can go and fill in the collision, and the animations.
		for (; count < tilecount; id++, count++) {
			TilePtr tile = std::make_shared<Tile>();
			
			int x = count % tmxTileset->columns;
			int y = count / tmxTileset->columns;

			tile->texture = std::make_shared<Texture>(
				image,
				x * tileset->tileWidth + spacing + (count * margin),
				y * tileset->tileHeight + spacing + (count * margin),
				tileset->tileWidth,
				tileset->tileHeight); 

			tileset->tiles[id] = tile;
		}

		id = tmxTileset->firstgid;
		for (TMXTilePtr tmxTile : tmxTileset->tiles) {
			TilePtr tile = tileset->tiles[id + tmxTile->id];

			if (!tmxTileset->image.empty()) {
				tile->texture->assetTag = tmxTile->image;
				map->getMapConfig()->images.push_back(tmxTile->image);
			}

			if (!tmxTile->animation.empty()) {
				tile->animated = true;
				tile->animation = std::make_shared<Animation>();
				for (TMXFramePtr frame : tmxTile->animation) {
					TilePtr animationFrame = tileset->tiles.at(frame->tileid);
					tile->animation->frames.push_back(animationFrame->texture);
				}
			}

			if (!tmxTile->objectgroup.empty()) {
				// TODO: Add complex collision shapes for tiles.
				tile->collision = true;
				tile->collisionWidth = tileset->tileWidth;
				tile->collisionWidth = tileset->tileHeight;
			}

			if (!tmxTile->properties.empty()) {
				string script;
				for (TMXPropertyPtr prop : tmxTile->properties) {
					if (prop->name == "script") {
						script = string((char*) makeShared(prop->getValue<char*>()).get());
					}
				}
				tile->script = script;
			}
		}
	}

	return tileset;
}

void loadObjectLayerFromTMX(TMXLayerPtr tmxLayer, MapPtr map) {

}

void loadTileLayerFromTMX(TMXLayerPtr tmxLayer, MapPtr map) {
	MapConfigPtr mapConfig = map->getMapConfig();
	int index = 0;
	for (int y = 0; y < map->getMapHeight(); y++) {
		for (int x = 0; x < map->getMapWidth(); x++) {
			unsigned int tileVal(tmxLayer->data[index++]);

			bool flippedHorizontal = tileVal & TMX_FLIPPED_HORIZONTAL_FLAG;
			bool flippedVertical = tileVal & TMX_FLIPPED_VERTICAL_FLAG;
			bool flippedDiagonal = tileVal & TMX_FLIPPED_DIAGONAL_FLAG;

			tileVal = tmxClearFlags(tileVal); 

			if (mapConfig->tileset->tiles.find(tileVal) == mapConfig->tileset->tiles.end()) {
				continue;
			}

			float rotation = 0.0f;
			if (flippedDiagonal && flippedHorizontal) {
				rotation = M_PI / 2.0f;
			}
			else if (flippedDiagonal && flippedVertical) {
				rotation = -(M_PI / 2.0f);
			}
			else if (flippedDiagonal) {
				flippedVertical = true;
				rotation = M_PI / 2.0f;
			}
			
			TilePtr tileToCopy = mapConfig->tileset->tiles.at(tileVal);
			TilePtr tile = std::make_shared<Tile>((Tile&) *tileToCopy.get());

			TexturePtr texture = tile->texture;
			texture->flippedDiagonal = flippedDiagonal;
			texture->flippedHorizontal = flippedHorizontal;
			texture->flippedVertical = flippedVertical;
			texture->angleRad = rotation;

			GridPtr grid = map->gridAt(x, y);
			CellPtr cell = grid->at(x, y);
			cell->tiles.push_back(tile);
		}

		std::cout << std::endl;
	}
}

MapPtr loadMapFromTMX(const string& path, SystemManagerPtr systemManager) {
	TMXMapPtr tmxMap = parseTMXMap(path);

	MapConfigPtr mapConfig = std::make_shared<MapConfig>();
	MapPtr map = std::make_shared<Map>(mapConfig);

	float scale = 1.0f;
	string script;
	if (!tmxMap->properties.empty()) {
		auto properties = tmxMap->properties;
		for (TMXPropertyPtr prop : properties) {
			if (string("script").compare(prop->name) == 0) {
				auto ptr = prop->getValue<char*>();
				auto shPtr = makeShared(ptr);
				auto c = (char*)shPtr.get();
				script = string(c);
			}
			else if (string("scale").compare(prop->name) == 0) {
				auto ptr = prop->getValue<float>();
				auto shPtr = makeShared(ptr);
				scale = *shPtr.get();
			}
		}
	}

	mapConfig->mapWidth = tmxMap->width;
	mapConfig->mapHeight = tmxMap->height;
	mapConfig->scale = scale;
	mapConfig->script = script;

	mapConfig->tileset = loadTilesetFromTMX(tmxMap, map);

	// Construct the grids
	for (int h = 0; h < mapConfig->mapHeight; h += GRID_HEIGHT) {
		for (int w = 0; w < mapConfig->mapWidth; w += GRID_WIDTH) {
			GridPtr grid = std::make_shared<Grid>();
			grid->startX = w;
			grid->startY = h;
			grid->columns = std::min((double)mapConfig->mapWidth, (double) w + GRID_WIDTH) - w;
			grid->rows = std::min((double)mapConfig->mapHeight, (double) h + GRID_HEIGHT) - h;
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

	// load the cells into the grids
	for (TMXLayerPtr tmxLayer : tmxMap->layers) {
		if (tmxLayer->type == "tilelayer") {
			loadTileLayerFromTMX(tmxLayer, map);
		}
		else if (tmxLayer->type == "objectgroup") {
			loadObjectLayerFromTMX(tmxLayer, map);
		}
	}

	return map;
}

MapFactory::MapFactory(EntityFactoryPtr factory, LuaScriptFactoryPtr luaScriptFactory, SystemManagerPtr systemManager) {
	mFactory = factory;
	mSystemManager = systemManager;
	mLuaScriptFactory = luaScriptFactory;
}

MapPtr MapFactory::createTMXMap(const string& path) {
	MapPtr map = loadMapFromTMX(path, mSystemManager);

	MapConfigPtr mapConfig = map->getMapConfig();
	int gridWidth = map->getGridWidth();
	int gridHeight = map->getGridHeight();
	int width = map->getTileWidth() * GRID_WIDTH;
	int height = map->getTileHeight() * GRID_HEIGHT;

	for (int y = 0; y < gridHeight; y++) {
		for (int x = 0; x < gridWidth; x++) {
			EntityPtr entity = mFactory->createPhysicsEntity(x * width + (width / 2), y * height + (height / 2), width, height);

			GridPtr grid = map->gridAt(x * GRID_WIDTH, y * GRID_HEIGHT);
			applyGrid(mSystemManager, entity->id, grid, map);
		}
	}

	if (!mapConfig->script.empty()) {
		EntityPtr scriptObject = mFactory->createPhysicsEntity(-1, -1, 1, 1);
		applyScript(mSystemManager, scriptObject->id, mapConfig->script);
	}

	MapSystemPtr mapSystem = mSystemManager->getSystemByType<MapSystem>(SystemType::MAP);
	mapSystem->setMap(map);

	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	for (string image : mapConfig->images) {
		graphicsSystem->addTexture(image, image);
	}

	return map;
}