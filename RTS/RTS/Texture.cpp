#include"Texture.h"

Texture::Texture(std::string tag, float tx, float ty, float width, float height) {
	x = tx;
	y = ty;
	w = width;
	h = height;
	assetTag = tag;
}

Texture::Texture(std::string tag) {
	x = 0;
	y = 0;
	w = -1;
	h = -1;
	assetTag = tag;
}

Texture::Texture(const rapidjson::Value& root) {
	x = root["x"].GetDouble();
	y = root["y"].GetDouble();
	w = root["w"].GetDouble();
	h = root["h"].GetDouble();
	assetTag = root["assetTag"].GetString();
}

void Texture::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("x");
	serializer.writer.Double(x);

	serializer.writer.String("y");
	serializer.writer.Double(y);

	serializer.writer.String("w");
	serializer.writer.Double(w);

	serializer.writer.String("h");
	serializer.writer.Double(h);

	serializer.writer.String("assetTag");
	serializer.writer.String(assetTag.c_str());

	serializer.writer.EndObject();
}