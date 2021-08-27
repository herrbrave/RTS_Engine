#include"Map.h"

MapConfig::MapConfig(const rapidjson::Value& root) {
	mapWidth = root["mapWidth"].GetInt();
	mapHeight = root["mapHeight"].GetInt();
	scale = root["scale"].GetDouble();
	tileset = std::make_shared<Tileset>(root["tileset"]);
	script = root["script"].GetString();
}

void MapConfig::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("mapWidth");
	serializer.writer.Int(mapWidth);

	serializer.writer.String("mapHeight");
	serializer.writer.Int(mapHeight);

	serializer.writer.String("scale");
	serializer.writer.Double(scale);

	serializer.writer.String("script");
	serializer.writer.String(script.c_str());

	serializer.writer.String("tileset");
	tileset->serialize(serializer);

	serializer.writer.EndObject();
}

Tile::Tile(const Tile& tile) {
	if (tile.texture != nullptr) {
		texture = std::make_shared<Texture>((Texture&)*tile.texture.get());
	}
	if (tile.animation != nullptr) {
		animation = std::make_shared<Animation>((Animation&)*tile.animation.get());
	}
	animated = tile.animated;
	collision = tile.collision;
	collisionWidth = tile.collisionWidth;
	collisionHeight = tile.collisionHeight;
	script = string(tile.script);
}

Tile::Tile(const rapidjson::Value& root) {
	if (root.FindMember("texture") != root.MemberEnd()) {
		texture = std::make_shared<Texture>(root["texture"]);
	}

	if (root.FindMember("animation") != root.MemberEnd()) {
		animation = std::make_shared<Animation>(root["animation"]);
	}
	collision = root["collision"].GetBool();
	collisionWidth = root["collisionWidth"].GetInt();
	collisionHeight = root["collisionHeight"].GetInt();
	animated = root["animated"].GetBool();
	script = root["script"].GetString();
}

void Tile::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	if (texture != nullptr) {
		serializer.writer.String("texture");
		texture->serialize(serializer);
	}

	if (animation != nullptr) {
		serializer.writer.String("animation");
		animation->serialize(serializer);
	}

	serializer.writer.String("collision");
	serializer.writer.Bool(collision);

	serializer.writer.String("collisionWidth");
	serializer.writer.Int(collisionWidth);

	serializer.writer.String("collisionHeight");
	serializer.writer.Int(collisionHeight);

	serializer.writer.String("animated");
	serializer.writer.Bool(animated);

	serializer.writer.String("script");
	serializer.writer.String(script.c_str());

	serializer.writer.EndObject();
}

Object::Object(const rapidjson::Value& root) {
	tile = std::make_shared<Tile>(root["tile"]);
	position = std::make_shared<Vector2f>(root["position"]);
}

void Object::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("tile");
	tile->serialize(serializer);

	serializer.writer.String("position");
	position->serialize(serializer);

	serializer.writer.EndObject();
}

Tileset::Tileset(const rapidjson::Value& root) {
	tileWidth = root["tileWidth"].GetInt();
	tileHeight = root["tileHeight"].GetInt();

	auto it = root["tiles"].MemberBegin();
	auto end = root["tiles"].MemberEnd();
	while (it != end) {
		int key = std::stoi(it->name.GetString());
		this->tiles[key] = std::make_shared<Tile>(it->value);
	}
}

void Tileset::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("tileWidth");
	serializer.writer.Int(tileWidth);

	serializer.writer.String("tileHeight");
	serializer.writer.Int(tileHeight);

	serializer.writer.String("tiles");
	serializer.writer.StartObject();
	for (auto it = tiles.begin(); it != tiles.end(); it++) {
		serializer.writer.String(std::to_string(it->first).c_str());
		it->second->serialize(serializer);
	}
	serializer.writer.EndObject();

	serializer.writer.EndObject();
}

Cell::Cell(const rapidjson::Value& root) {
	for (auto it = root["tiles"].Begin(); it != root["tiles"].End(); it++) {
		this->tiles.push_back(std::make_shared<Tile>(*it));
	}

	x = root["x"].GetInt();
	y = root["y"].GetInt();
	r = root["r"].GetInt();
	g = root["g"].GetInt();
	b = root["b"].GetInt();
	a = root["a"].GetInt();
	animated = root["animated"].GetBool();
}

bool Cell::canOccupy() {
	for (auto tile : this->tiles) {
		if (tile->collision) {
			return false;
		}
	}

	return true;
}

void Cell::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("tiles");
	serializer.writer.StartArray();
	for (auto tile : this->tiles) {
		tile->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("x");
	serializer.writer.Int(x);
	serializer.writer.String("y");
	serializer.writer.Int(y);
	serializer.writer.String("r");
	serializer.writer.Int(r);
	serializer.writer.String("g");
	serializer.writer.Int(g);
	serializer.writer.String("b");
	serializer.writer.Int(b);
	serializer.writer.String("a");
	serializer.writer.Int(a);
	serializer.writer.String("animated");
	serializer.writer.Bool(animated);

	serializer.writer.EndObject();
}

Grid::Grid(const rapidjson::Value& root) {
	for (auto it = root["cells"].Begin(); it != root["cells"].End(); it++) {
		this->cells.push_back(std::make_shared<Cell>(*it));
	}

	rows = root["rows"].GetInt();
	columns = root["columns"].GetInt();
	startX = root["startX"].GetInt();
	startY = root["startY"].GetInt();
	tileW = root["tileW"].GetInt();
	tileH = root["tileH"].GetInt();
}

void Grid::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("cells");
	serializer.writer.StartArray();
	for (auto cell : this->cells) {
		cell->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("rows");
	serializer.writer.Int(rows);
	serializer.writer.String("columns");
	serializer.writer.Int(columns);
	serializer.writer.String("startX");
	serializer.writer.Int(startX);
	serializer.writer.String("startY");
	serializer.writer.Int(startY);
	serializer.writer.String("tileW");
	serializer.writer.Int(tileW);
	serializer.writer.String("tileH");
	serializer.writer.Int(tileH);

	serializer.writer.EndObject();
}

CellPtr Grid::at(int x, int y) {
	int index = (y - startY) * columns + ((x - startX) % columns);
	if (index >= columns * rows) {
		throw "Index out of bounds.";
	}

	return this->cells.at(index);
}

GridDrawable::GridDrawable(const rapidjson::Value& root) : Drawable(root) {
	texture = std::make_shared<Texture>(root["texture"]);
	grid = std::make_shared<Grid>(root["grid"]);
}

void GridDrawable::onSerialize(Serializer& serializer) const {
	serializer.writer.String("texture");
	texture->serialize(serializer);

	serializer.writer.String("grid");
	grid->serialize(serializer);
}

GridPtr GridDrawable::getGrid() {
	return this->grid;
}

void GridDrawable::forceRedraw() {
	this->redraw = true;
}

void GridDrawable::update(Graphics& graphicsRef) {
	int index = 0;
	graphicsRef.drawToTexture(this->grid->name);
	for (int y = 0; y < grid->rows; y++) {
		for (int x = 0; x < grid->columns; x++) {
			auto cell = this->grid->cells.at(index++);
			// TODO: Optimize this
			if (!redraw && !cell->animated) {
				continue;
			}

			for (auto tile : cell->tiles) {
				graphicsRef.renderTexture(
					tile->texture,
					x * grid->tileW + (grid->tileW / 2),
					y * grid->tileH + (grid->tileH / 2),
					grid->tileW,
					grid->tileH,
					tile->texture->angleRad,
					mColor->r,
					mColor->g,
					mColor->b,
					mColor->a);
			}
		}
	}
	graphicsRef.drawToScreen();

	redraw = false;
}

void GridDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	this->update(graphicsRef);

	graphicsRef.renderTexture(texture, position.x, position.y, width, height, angle, mColor->r, mColor->g, mColor->b, mColor->a);
}

GridComponent::GridComponent(const rapidjson::Value& root) : Component(0, ComponentType::GRID_COMPONENT) {
	gridDrawable = std::make_shared<GridDrawable>(root["gridDrawable"]);
}

GridDrawablePtr GridComponent::getGridDrawable() {
	return this->gridDrawable;
}

void GridComponent::setColorAtPoint(const Vector2f& point, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	this->setColor(x, y, r, g, b, a);
}

void GridComponent::setColor(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);
	cell->r = r;
	cell->g = g;
	cell->b = b;
	cell->a = a;
	this->gridDrawable->forceRedraw();
}

void GridComponent::setTextureAtPoint(const Vector2f& point, TexturePtr texture) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	setTexture(x, y, texture);
}

void GridComponent::setTexture(int x, int y, TexturePtr texture) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);
	TilePtr tile = std::make_shared<Tile>();

	tile->texture = texture;

	cell->tiles.clear();
	cell->tiles.push_back(tile);
	this->gridDrawable->forceRedraw();
}

void GridComponent::pushTextureAtPoint(const Vector2f& point, TexturePtr texture) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	pushTexture(x, y, texture);
}

void GridComponent::pushTexture(int x, int y, TexturePtr texture) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);
	TilePtr tile = std::make_shared<Tile>();

	tile->texture = texture;

	cell->tiles.push_back(tile);
	this->gridDrawable->forceRedraw();
}

void GridComponent::popTextureAtPoint(const Vector2f& point) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	popTexture(x, y);
}

void GridComponent::popTexture(int x, int y) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);

	if (cell->tiles.empty()) {
		return;
	}
	cell->tiles.pop_back();
	this->gridDrawable->forceRedraw();
}

CellPtr GridComponent::getCellAtPoint(const Vector2f& point) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	return this->gridDrawable->getGrid()->at(x, y);
}

Uint8 GridComponent::getZOrder() {
	return gridDrawable->getDrawDepth();
}

void GridComponent::setZOrder(Uint8 zOrder) {
	gridDrawable->setDrawDepth(zOrder);

	EntityZOrderSetEventData* eventData = GCC_NEW EntityZOrderSetEventData(entityId, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

void GridComponent::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("gridDrawable");
	gridDrawable->serialize(serializer);

	serializer.writer.EndObject();
}

Map::Map(MapConfigPtr config) {
	mapConfig = std::move(config);
}

Map::Map(const rapidjson::Value& root) { 
	for (auto it = root["grids"].Begin(); it != root["grids"].End(); it++) {
		grids.push_back(std::make_shared<Grid>(*it));
	}
	for (auto it = root["objects"].Begin(); it != root["objects"].End(); it++) {
		objects.push_back(std::make_shared<Object>(*it));
	}

	mapConfig = std::make_shared<MapConfig>(root["mapConfig"]);
}

void Map::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("grids");
	serializer.writer.StartArray();
	for (auto grid : grids) {
		grid->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("objects");
	serializer.writer.StartArray();
	for (auto object : objects) {
		object->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("grids");
	mapConfig->serialize(serializer);

	serializer.writer.EndObject();
}

CellPtr Map::getCellAt(int x, int y) {
	int index = getIndex(x, y);
	GridPtr grid = this->gridAt(x, y);

	return grid->at(x, y);
}

CellPtr Map::cellAtPoint(const Vector2f& point) {
	int xIndex = std::round(point.x / float(mapConfig->tileset->tileWidth));
	int yIndex = std::round(point.y / float(mapConfig->tileset->tileHeight));
	GridPtr grid = this->gridAt(xIndex, yIndex);

	return grid->at(xIndex, yIndex);
}

GridPtr Map::gridAt(int x, int y) {
	int gridWidth = std::ceil((float)mapConfig->mapWidth / (float)GRID_WIDTH);
	int gridHeight = std::ceil((float)mapConfig->mapHeight / (float)GRID_HEIGHT);
	x = x / GRID_WIDTH;
	y = y / GRID_HEIGHT;

	return this->grids.at(y * gridWidth + (x % gridWidth));
}

int Map::getMapWidthPixels() {
	return this->getMapWidth() * this->getTileWidth() * this->mapConfig->scale;
}

int Map::getMapHeightPixels() {
	return this->getMapHeight() * this->getTileHeight() * this->mapConfig->scale;
}

int Map::getMapWidth() {
	return mapConfig->mapWidth;
}

int Map::getMapHeight() {
	return mapConfig->mapHeight;
}

int Map::getGridWidth() {
	return std::ceil((float)getMapWidth() / (float)GRID_WIDTH);
}

int Map::getGridHeight() {
	return std::ceil((float)getMapHeight() / (float)GRID_HEIGHT);
}

int Map::getTileWidth() {
	return mapConfig->tileset->tileWidth * mapConfig->scale;
}

int Map::getTileHeight() {
	return mapConfig->tileset->tileHeight * mapConfig->scale;
}

MapConfigPtr Map::getMapConfig() {
	return this->mapConfig;
}

void Map::findPath(vector<Vector2fPtr> path, int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return;
	}
	/*

	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto endTile = entitySystem->getEntityById(mapConfig->tiles[endIndex]);

	// tiles to select.
	auto comparitor = [endTile](Node left, Node right) {

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
	auto startTile = entitySystem->getEntityById(mapConfig->tiles[startIndex]);
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

		TileComponentPtr component = current.tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);
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

				auto neighborTile = entitySystem->getEntityById(mapConfig->tiles[index]);
				TileComponentPtr component = neighborTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);
				if (!component->canOccupy || closedSet.find(neighborTile) != closedSet.end() || openSetLookup.find(neighborTile) != openSetLookup.end()) {
					continue;
				}

				pathMap[neighborTile] = current.tile;
				openSet.emplace(Node{ neighborTile, current.cost + 1 });
				openSetLookup.emplace(neighborTile);
			}
		}
	}
	*/
}