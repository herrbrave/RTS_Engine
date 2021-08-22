#include"MapFactory.h"

void applyGrid(SystemManagerPtr systemManager, unsigned long entityId, GridPtr grid, MapPtr map) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	EntityPtr entity = entitySystem->getEntityById(entityId);

	GridDrawablePtr gridDrawable = std::make_shared<GridDrawable>(grid);
	GridComponentPtr gridComponent = std::make_shared<GridComponent>(entity->id, gridDrawable, map->getMapConfig()->tileset);
	entity->addComponent(gridComponent);
	graphicsSystem->addTexture(grid->name, GRID_WIDTH * grid->tileW, GRID_HEIGHT * grid->tileH);

	graphicsSystem->registerDrawable(entity->id, gridDrawable);
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
		if (!tmxTileset->image.empty()) {
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
		}
		else {
			for (TMXTilePtr tmxTile : tmxTileset->tiles) {
				TilePtr tile = std::make_shared<Tile>();
				tile->texture = std::make_shared<Texture>(
					tmxTile->image,
					0,
					0,
					tmxTile->imageWidth,
					tmxTile->imageHeight);
				map->getMapConfig()->images.push_back(tmxTile->image);

				tileset->tiles[id++] = tile;
			}
		}

		id = tmxTileset->firstgid;
		for (TMXTilePtr tmxTile : tmxTileset->tiles) {
			TilePtr tile = tileset->tiles[id + tmxTile->id];

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
				for (TMXPropertyPtr prop : tmxTile->properties) {
					if (prop->name == "script") {
						tile->script = prop->getValue<char[]>().get();
					}
				}
			}
		}
	}

	return tileset;
}

void loadObjectLayerFromTMX(TMXLayerPtr tmxLayer, MapPtr map) {
	MapConfigPtr mapConfig = map->getMapConfig();
	auto data = tmxLayer->objects;
	for (TMXObjectPtr object : data) {
		ObjectPtr obj = std::make_shared<Object>();

		obj->name = object->name;

		unsigned int tileVal(object->gid);

		bool flippedHorizontal = tileVal & TMX_FLIPPED_HORIZONTAL_FLAG;
		bool flippedVertical = tileVal & TMX_FLIPPED_VERTICAL_FLAG;
		bool flippedDiagonal = tileVal & TMX_FLIPPED_DIAGONAL_FLAG;

		tileVal = tmxClearFlags(tileVal);

		int width = object->width * mapConfig->scale;
		int height = object->height * mapConfig->scale;

		string script = "";
		if (!object->properties.empty()) {
			auto properties = object->properties;
			for (TMXPropertyPtr prop : properties) {
				if (string("script").compare(prop->name) == 0) {
					auto ptr = prop->getValue<char[]>();
					script = ptr.get();
				}
			}
		}

		TilePtr tile;
		if (tileVal > 0) {
			tile = std::make_shared<Tile>(*map->getMapConfig()->tileset->tiles.at(tileVal).get());
			obj->tile = tile;

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

			obj->tile->texture->angleRad = rotation;
			obj->tile->texture->flippedDiagonal = flippedDiagonal;
			obj->tile->texture->flippedHorizontal = flippedHorizontal;
			obj->tile->texture->flippedVertical = flippedVertical;

			if (!tile->script.empty()) {
				script = tile->script;
			}
		}
		else {
			tile = std::make_shared<Tile>();
		}

		tile->script = script;

		tile->collision = true;
		tile->collisionWidth = width;
		tile->collisionHeight = height;

		// Tiled stored location from the top left corner.
		int x = (int)round(object->x) * mapConfig->scale;
		int y = (int)(round(object->y) - (object->height / 2)) * mapConfig->scale;

		obj->position = std::make_shared<Vector2f>((float) x, (float) y);

		map->objects.push_back(obj);
	}
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
			if (tile->animated) {
				cell->animated = true;
			}
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
				auto ptr = prop->getValue<char[]>();
				script = string(ptr.get());
			}
			else if (string("scale").compare(prop->name) == 0) {
				auto ptr = prop->getValue<float>();
				scale = *ptr.get();
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


	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	for (string image : mapConfig->images) {
		graphicsSystem->addTexture(image, image);
	}

	return map;
}