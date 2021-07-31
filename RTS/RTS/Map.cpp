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
	auto index = (y - startY) * columns + ((x - startX) % columns);
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
	serializer.writer.StartObject();

	serializer.writer.String("texture");
	texture->serialize(serializer);

	serializer.writer.String("grid");
	grid->serialize(serializer);

	serializer.writer.EndObject();
}

GridPtr GridDrawable::getGrid() {
	return this->grid;
}

void GridDrawable::forceRedraw() {
	this->redraw = true;
}

void GridDrawable::update(Graphics& graphicsRef) {
	if (!redraw) {
		return;
	}

	int index = 0;
	for (int y = 0; y < grid->rows; y++) {
		for (int x = 0; x < grid->columns; x++) {
			auto cell = this->grid->cells.at(index++);
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

	redraw = false;
}

void GridDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	graphicsRef.renderTexture(texture, position.x, position.y, width, height, angle, mColor->r, mColor->g, mColor->b, mColor->a);
}

GridComponent::GridComponent(const rapidjson::Value& root) : Component(0, ComponentType::GRID_COMPONENT) {
	gridDrawable = std::make_shared<GridDrawable>(root["gridDrawable"]);
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

void GridComponent::setTextureAtPoint(const Vector2f& point, unsigned int tileId, bool flipHorizontal, bool flipVertical, bool flipDiagonal, float angle) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	setTexture(x, y, tileId, flipHorizontal, flipHorizontal, flipDiagonal, angle);
}

void GridComponent::setTexture(int x, int y, unsigned int tileId, bool flipHorizontal, bool flipVertical, bool flipDiagonal, float angle) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);

	TilePtr tileToCopy = tileset->tiles.at(tileId);
	TilePtr tile = std::make_shared<Tile>((Tile&)*tileToCopy.get());

	cell->tiles.clear();
	cell->tiles.push_back(tile);
}

void GridComponent::pushTextureAtPoint(const Vector2f& point, unsigned int tileId, bool flipHorizontal, bool flipVertical, bool flipDiagonal, float angle) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	pushTexture(x, y, tileId, flipHorizontal, flipHorizontal, flipDiagonal, angle);
}

void GridComponent::pushTexture(int x, int y, unsigned int tileId, bool flipHorizontal, bool flipVertical, bool flipDiagonal, float angle) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);

	TilePtr tileToCopy = tileset->tiles.at(tileId);
	TilePtr tile = std::make_shared<Tile>((Tile&)*tileToCopy.get());

	cell->tiles.push_back(tile);
}

void GridComponent::popTextureAtPoint(const Vector2f& point) {
	int x = std::floor(point.x / (float)this->gridDrawable->grid->tileW);
	int y = std::floor(point.y / (float)this->gridDrawable->grid->tileH);

	popTexture(x, y);
}

void GridComponent::popTexture(int x, int y) {
	CellPtr cell = this->gridDrawable->getGrid()->at(x, y);

	cell->tiles.pop_back();
}

void GridComponent::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("gridDrawable");
	gridDrawable->serialize(serializer);

	serializer.writer.EndObject();
}

Map::Map(MapConfigPtr config) {
	mMapConfig = std::move(config);
}

CellPtr Map::getCellAt(int x, int y) {
	int index = getIndex(x, y);
	GridPtr grid = this->gridAt(x, y);

	return grid->at(x, y);
}

CellPtr Map::cellAtPoint(const Vector2f& point) {
	int xIndex = std::round(point.x / float(mMapConfig->tileset->tileWidth));
	int yIndex = std::round(point.y / float(mMapConfig->tileset->tileHeight));
	GridPtr grid = this->gridAt(xIndex, yIndex);

	return grid->at(xIndex, yIndex);
}

GridPtr Map::gridAt(int x, int y) {
	int gridWidth = std::ceil((float)mMapConfig->mapWidth / (float)GRID_WIDTH);
	int gridHeight = std::ceil((float)mMapConfig->mapHeight / (float)GRID_HEIGHT);
	x = x / GRID_WIDTH;
	y = y / GRID_HEIGHT;

	return this->grids.at(y * gridWidth + (x % gridWidth));
}

void Map::findPath(vector<Vector2fPtr> path, int startX, int startY, int endX, int endY) {
	int endIndex = getIndex(endX, endY);
	if (endIndex == -1) {
		// return an empty path.
		return;
	}
	/*

	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto endTile = entitySystem->getEntityById(mMapConfig->tiles[endIndex]);

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
	auto startTile = entitySystem->getEntityById(mMapConfig->tiles[startIndex]);
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

				auto neighborTile = entitySystem->getEntityById(mMapConfig->tiles[index]);
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