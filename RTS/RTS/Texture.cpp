#include"Texture.h"

Texture::Texture(std::string tag, float tx, float ty, float width, float height) {
	x = tx;
	y = ty;
	w = width;
	h = height;
	assetTag = tag;
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

Texture* TextureComponent::getTexture() {
	return mTexture;
}

void TextureComponent::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("mTexture");
	mTexture->serialize(serializer);

	serializer.writer.EndObject();
}