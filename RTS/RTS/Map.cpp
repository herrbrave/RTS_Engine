#include"Map.h"

unsigned int tmxClearFlags(unsigned int tileValue) {
	return tileValue  & ~(TMX_FLIPPED_HORIZONTAL_FLAG | TMX_FLIPPED_VERTICAL_FLAG | TMX_FLIPPED_DIAGONAL_FLAG);
}

TMXMapPtr parseMap(const string& path) {
	auto start = std::chrono::high_resolution_clock::now();

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

	auto end = std::chrono::high_resolution_clock::now();
	auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("Map - Parse Map File: " + std::to_string(res.count()));

	start = std::chrono::high_resolution_clock::now();

	TMXMapPtr map(GCC_NEW TMXMap());

	if (doc.FindMember("backgroundcolor") != doc.MemberEnd()) {
		map->backgroundColor = doc["backgroundcolor"].GetString();
	}

	map->width = doc["width"].GetInt();
	map->height = doc["height"].GetInt();
	map->tileWidth = doc["tilewidth"].GetInt();
	map->tileHeight = doc["tileheight"].GetInt();

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("Map - Read Map Attributes: " + std::to_string(res.count()));

	start = std::chrono::high_resolution_clock::now();

	if (doc.FindMember("properties") != doc.MemberEnd()) {
		parseProperties(doc["properties"], map->properties);
	}

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("Map - Read Map Properties: " + std::to_string(res.count()));

	start = std::chrono::high_resolution_clock::now();

	if (doc.FindMember("tilesets") != doc.MemberEnd()) {
		parseTilesets(doc["tilesets"], map->tilesets);
	}

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("Map - Read Tilesets: " + std::to_string(res.count()));

	start = std::chrono::high_resolution_clock::now();

	if (doc.FindMember("layers") != doc.MemberEnd()) {
		parseLayers(doc["layers"], map->layers);
	}

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("Map - Read Layers: " + std::to_string(res.count()));

	start = std::chrono::high_resolution_clock::now();

	return map;
}

void parseProperties(const rapidjson::Value& tiles, TMXProperties& props) {
	for (int index = 0; index < tiles.Size(); index++) {
		const rapidjson::Value& tile = tiles[index];
		TMXPropertyPtr prop(GCC_NEW TMXProperty());
		prop->name = tile["name"].GetString();
		prop->type = tile["type"].GetString();
		if (prop->type == "string") {
			auto v = tile["value"].GetString();
			auto len = strlen(v) + 1;
			char* cpy = new char[len];
			for (int index = 0; index < len; index++) {
				cpy[index] = v[index];
			}

			auto ptr = VoidPtr(static_cast<void*>(cpy));
			prop->value = ptr;
		}
		else if (prop->type == "int") {
			prop->setValue(VoidPtr(new int { tile["value"].GetInt() }));
		}
		else if (prop->type == "float") {
			prop->setValue(VoidPtr(new float { (float) tile["value"].GetDouble() }));
		}
		else if (prop->type == "bool") {
			prop->setValue(VoidPtr(new bool{ tile["value"].GetBool() }));
		}
		props.push_back(prop);
	}
}

void parseLayers(const rapidjson::Value& layers, TMXLayers& layerList) {
	for (int index = 0; index < layers.Size(); index++) {
		const rapidjson::Value& layer = layers[index];

		TMXLayerPtr newLayer(GCC_NEW TMXLayer());
		newLayer->name = layer["name"].GetString();
		newLayer->type = layer["type"].GetString();
		newLayer->gid = -1;
		if (layer.FindMember("gid") != layer.MemberEnd()) {
			newLayer->gid = layer["gid"].GetInt();
		}

		if (layer.FindMember("width") != layer.MemberEnd()) {
			newLayer->width = layer["width"].GetInt();
		}
		if (layer.FindMember("height") != layer.MemberEnd()) {
			newLayer->height = layer["height"].GetInt();
		}
		if (layer.FindMember("properties") != layer.MemberEnd()) {
			parseProperties(layer["properties"], newLayer->properties);
		}
		if (layer.FindMember("objects") != layer.MemberEnd()) {
			parseObjects(layer["objects"], newLayer->objects);
		}
		if (layer.FindMember("data") != layer.MemberEnd()) {
			const rapidjson::Value& data = layer["data"];
			newLayer->tileCount = data.Size();
			unsigned int* newData = new unsigned int[data.Size()];
			for (int d = 0; d < data.Size(); d++) {
				const rapidjson::Value& v = data[d];
				newData[d] = (unsigned int)data[d].GetUint();
			}
			newLayer->data = newData;
		}

		layerList.push_back(newLayer);
	}
}

void parseTilesets(const rapidjson::Value& tilesets, TMXTilesets& tilesetList) {
	for (int index = 0; index < tilesets.Size(); index++) {
		const rapidjson::Value& tileset = tilesets[index];
		TMXTilesetPtr newTileset(GCC_NEW TMXTileset());

		newTileset->firstgid = tileset["firstgid"].GetInt();
		newTileset->name = tileset["name"].GetString();
		newTileset->columns = tileset["columns"].GetInt();
		newTileset->tileWidth = tileset["tilewidth"].GetInt();
		newTileset->tileHeight = tileset["tileheight"].GetInt();
		newTileset->tileCount = tileset["tilecount"].GetInt();
		if (tileset.FindMember("image") != tileset.MemberEnd()) {
			newTileset->image = tileset["image"].GetString();
			newTileset->imageWidth = tileset["imagewidth"].GetInt();
			newTileset->imageHeight = tileset["imageheight"].GetInt();
			newTileset->margin = tileset["margin"].GetInt();
			newTileset->spacing = tileset["spacing"].GetInt();
		}
		if (tileset.FindMember("properties") != tileset.MemberEnd()) {
			parseProperties(tileset["properties"], newTileset->properties);
		}
		if (tileset.FindMember("tiles") != tileset.MemberEnd()) {
			parseTiles(tileset["tiles"], newTileset->tiles);
		}

		tilesetList.push_back(newTileset);
	}
}

void parseObjects(const rapidjson::Value& objects, TMXObjects& objectList) {
	for (int index = 0; index < objects.Size(); index++) {
		const rapidjson::Value& object = objects[index];
		TMXObjectPtr newObject(GCC_NEW TMXObject());

		if (object.FindMember("gid") != object.MemberEnd()) {
			newObject->gid = object["gid"].GetInt();
		}
		else {
			newObject->gid = -1;
		}
		newObject->name = object["name"].GetString();
		newObject->type = object["type"].GetString();
		newObject->rotation = object["rotation"].GetDouble();
		newObject->width = object["width"].GetInt();
		newObject->height = object["height"].GetInt();
		if (object.FindMember("x") != object.MemberEnd()) {
			newObject->x = (float) object["x"].GetDouble();
		}
		if (object.FindMember("y") != object.MemberEnd()) {
			newObject->y = (float) object["y"].GetDouble();
		}
		newObject->visible = object["visible"].GetBool();
		if (object.FindMember("properties") != object.MemberEnd()) {
			parseProperties(object["properties"], newObject->properties);
		}

		objectList.push_back(newObject);
	}
}

void parseTiles(const rapidjson::Value& tiles, TMXTiles& tileList) {
	for (int index = 0; index < tiles.Size(); index++) {
		const rapidjson::Value& tile = tiles[index];
		TMXTilePtr newTile(GCC_NEW TMXTile());

		newTile->id = tile["id"].GetInt();

		if (tile.FindMember("properties") != tile.MemberEnd()) {
			parseProperties(tile["properties"], newTile->properties);
		}
		if (tile.FindMember("image") != tile.MemberEnd()) {
			newTile->image = tile["image"].GetString();
			newTile->imageWidth = tile["imagewidth"].GetInt();
			newTile->imageHeight = tile["imageheight"].GetInt();
		}
		if (tile.FindMember("objectgroup") != tile.MemberEnd()) {
			parseLayers(tile["objectgroup"]["objects"], newTile->objectgroup);
		}
		if (tile.FindMember("animation") != tile.MemberEnd()) {
			parseFrames(tile["animation"], newTile->animation);
		}

		tileList.push_back(newTile);
	}
}

void parseFrames(const rapidjson::Value& frames, TMXFrames& frameList) {
	for (int index = 0; index < frames.Size(); index++) {
		const rapidjson::Value& frame = frames[index];
		TMXFramePtr newFrame(GCC_NEW TMXFrame());

		newFrame->duration = frame["duration"].GetInt();
		newFrame->tileid = frame["tileid"].GetInt();

		frameList.push_back(newFrame);
	}
}

Map::Map(MapConfigPtr config, EntityVendorPtr entityVendor) {
	mMapConfig = std::move(config);
	mEntityVendor = entityVendor;
}

WeakEntityPtr Map::getTileAt(int x, int y) {
	int index = getIndex(x, y);
	if (index == -1) {
		return WeakEntityPtr();
	}

	return mEntityVendor->getEntityById(mMapConfig->tiles[index]);
}

WeakEntityPtr Map::tileAtPoint(const Vector2f& point) {
	int xIndex = std::round(point.x / float(mMapConfig->tileWidth));
	int yIndex = std::round(point.y / float(mMapConfig->tileHeight));

	return this->getTileAt(xIndex, yIndex);
}

void Map::findPath(vector<WeakEntityPtr> path, int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return;
	}

	auto endTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[endIndex]));

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {
		TileComponentPtr leftComponent(left.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		TileComponentPtr rightComponent(right.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		TileComponentPtr endComponent(endTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));

		// dumb heuristic based on distance.
		int deltaX = leftComponent->x - endComponent->x;
		int deltaY = leftComponent->y - endComponent->y;
		float leftDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		deltaX = rightComponent->x - endComponent->x;
		deltaY = rightComponent->y - endComponent->y;
		float rightDist = sqrt(deltaX * deltaX + deltaY * deltaY);

		return (leftDist + left.cost) > (rightDist + right.cost);
	};

	int startIndex = getIndex(startX, startY);
	auto startTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[startIndex]));
	std::unordered_set<EntityPtr> openSetLookup;
	std::priority_queue < Node, std::vector<Node>, decltype(comparitor)> openSet(comparitor);
	openSet.emplace(Node{ startTile, 0 });

	// set of explored tiles.
	unordered_set<EntityPtr> closedSet;
	// back path map.
	unordered_map<EntityPtr, EntityPtr> pathMap;


	while (openSet.size() > 0) {
		auto current = openSet.top();
		openSet.pop();
		closedSet.emplace(current.tile);

		TileComponentPtr component(current.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		if (component->x == endX && component->y == endY) {
			auto tile = current.tile;
			while (tile != startTile) {
				path.insert(path.begin(), WeakEntityPtr(tile));
				tile = pathMap[tile];
			}

			return;
		}

		int currX = component->x;
		int currY = component->y;
		for (int j = -1; j <= 1; j++) {
			for (int i = -1; i <= 1; i++) {
				// skip the diagonals.
				if (i != 0 && j != 0) {
					continue;
				}

				int index = getIndex(currX + i, currY + j);
				if (index == -1) {
					continue;
				}

				auto neighborTile = makeShared(mEntityVendor->getEntityById(mMapConfig->tiles[index]));
				TileComponentPtr component(neighborTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
				if (!component->canOccupy || closedSet.find(neighborTile) != closedSet.end() || openSetLookup.find(neighborTile) != openSetLookup.end()) {
					continue;
				}

				pathMap[neighborTile] = current.tile;
				openSet.emplace(Node{ neighborTile, current.cost + 1 });
				openSetLookup.emplace(neighborTile);
			}
		}
	}
}

void GridDrawable::initialize(const SystemManager& systemManager) {
	GraphicsSystem& graphicsSystem = *makeShared(systemManager.getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	graphicsSystem.addTexture(grid->name, width, height);
	graphicsSystem.drawToTexture(grid->name);

	auto it = this->grid->tileLayers.begin();
	while (it != this->grid->tileLayers.end()) {
		auto layer = *it;

		for (int y = 0; y < this->grid->rows; y++) {
			for (int x = 0; x < this->grid->columns; x++) {
				auto tile = layer[x][y];
				if (tile == nullptr) {
					continue;
				}

				graphicsSystem.drawTexture(grid->name, x * tile->w, y * tile->h, tile->w, tile->h, tile->tx, tile->ty, tile->w, tile->h, angle, mColor->r, mColor->g, mColor->b, mColor->a);
			}
		}
	}

	texture = std::make_shared<Texture>(grid->name, 0, 0, width, height);

	graphicsSystem.drawToScreen();
}

void GridDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	graphicsRef.renderTexture(texture, position.x, position.y, width, height, angle, mColor->r, mColor->g, mColor->b, mColor->a);
}