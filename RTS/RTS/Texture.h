#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include"Asset.h"
#include"Component.h"
#include"Serializer.h"

#include<rapidjson\document.h>
#include<string>

class Texture;
typedef shared_ptr<Texture> TexturePtr;
typedef weak_ptr<Texture> WeakTexturePtr;

class Texture {
public:
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	std::string assetTag = "";
	bool flippedHorizontal = false;
	bool flippedVertical = false;
	bool flippedDiagonal = false;
	float angleRad = 0.0f;

	Texture() {}

	Texture(const Texture& texture) : assetTag(texture.assetTag), x(texture.x), y(texture.y), w(texture.w), h(texture.h), flippedDiagonal(texture.flippedDiagonal), flippedHorizontal(texture.flippedHorizontal), flippedVertical(texture.flippedVertical), angleRad(texture.angleRad) {}

	Texture(std::string tag, float tx, float ty, float width, float height);

	Texture(std::string tag);

	Texture(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const;
};

#endif // !__TEXTURE_H__
