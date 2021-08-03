#include"TMXMap.h"


unsigned int tmxClearFlags(unsigned int tileValue) {
	return tileValue & ~(TMX_FLIPPED_HORIZONTAL_FLAG | TMX_FLIPPED_VERTICAL_FLAG | TMX_FLIPPED_DIAGONAL_FLAG);
}

TMXMapPtr parseTMXMap(const string& path) {
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

	TMXMapPtr map(GCC_NEW TMXMap());

	if (doc.FindMember("backgroundcolor") != doc.MemberEnd()) {
		map->backgroundColor = doc["backgroundcolor"].GetString();
	}

	map->width = doc["width"].GetInt();
	map->height = doc["height"].GetInt();
	map->tileWidth = doc["tilewidth"].GetInt();
	map->tileHeight = doc["tileheight"].GetInt();

	if (doc.FindMember("properties") != doc.MemberEnd()) {
		parseTMXProperties(doc["properties"], map->properties);
	}

	if (doc.FindMember("tilesets") != doc.MemberEnd()) {
		parseTMXTilesets(doc["tilesets"], map->tilesets);
	}

	if (doc.FindMember("layers") != doc.MemberEnd()) {
		parseTMXLayers(doc["layers"], map->layers);
	}

	return map;
}

void parseTMXProperties(const rapidjson::Value& tiles, TMXProperties& props) {
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
			prop->setValue(VoidPtr(new int{ tile["value"].GetInt() }));
		}
		else if (prop->type == "float") {
			prop->setValue(VoidPtr(new float{ (float)tile["value"].GetDouble() }));
		}
		else if (prop->type == "bool") {
			prop->setValue(VoidPtr(new bool{ tile["value"].GetBool() }));
		}
		props.push_back(prop);
	}
}

void parseTMXLayers(const rapidjson::Value& layers, TMXLayers& layerList) {
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
			parseTMXProperties(layer["properties"], newLayer->properties);
		}
		if (layer.FindMember("objects") != layer.MemberEnd()) {
			parseTMXObjects(layer["objects"], newLayer->objects);
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

void parseTMXTilesets(const rapidjson::Value& tilesets, TMXTilesets& tilesetList) {
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
			parseTMXProperties(tileset["properties"], newTileset->properties);
		}
		if (tileset.FindMember("tiles") != tileset.MemberEnd()) {
			parseTMXTiles(tileset["tiles"], newTileset->tiles, newTileset->firstgid);
		}

		tilesetList.push_back(newTileset);
	}
}

void parseTMXObjects(const rapidjson::Value& objects, TMXObjects& objectList) {
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
			newObject->x = (float)object["x"].GetDouble();
		}
		if (object.FindMember("y") != object.MemberEnd()) {
			newObject->y = (float)object["y"].GetDouble();
		}
		newObject->visible = object["visible"].GetBool();
		if (object.FindMember("properties") != object.MemberEnd()) {
			parseTMXProperties(object["properties"], newObject->properties);
		}

		objectList.push_back(newObject);
	}
}

void parseTMXTiles(const rapidjson::Value& tiles, TMXTiles& tileList, int firstgid) {
	for (int index = 0; index < tiles.Size(); index++) {
		const rapidjson::Value& tile = tiles[index];
		TMXTilePtr newTile(GCC_NEW TMXTile());

		newTile->id = tile["id"].GetInt();

		if (tile.FindMember("properties") != tile.MemberEnd()) {
			parseTMXProperties(tile["properties"], newTile->properties);
		}
		if (tile.FindMember("image") != tile.MemberEnd()) {
			newTile->image = tile["image"].GetString();
			newTile->imageWidth = tile["imagewidth"].GetInt();
			newTile->imageHeight = tile["imageheight"].GetInt();
		}
		if (tile.FindMember("objectgroup") != tile.MemberEnd()) {
			parseTMXLayers(tile["objectgroup"]["objects"], newTile->objectgroup);
		}
		if (tile.FindMember("animation") != tile.MemberEnd()) {
			parseTMXFrames(tile["animation"], newTile->animation, firstgid);
		}

		tileList.push_back(newTile);
	}
}

void parseTMXFrames(const rapidjson::Value& frames, TMXFrames& frameList, int firstGid) {
	for (int index = 0; index < frames.Size(); index++) {
		const rapidjson::Value& frame = frames[index];
		TMXFramePtr newFrame(GCC_NEW TMXFrame());

		newFrame->duration = frame["duration"].GetInt();
		newFrame->tileid = frame["tileid"].GetInt() + firstGid;

		frameList.push_back(newFrame);
	}
}

