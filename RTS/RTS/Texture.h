#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include"Asset.h"
#include"Component.h"
#include"Serializer.h"

#include<rapidjson\document.h>
#include<string>

class Texture {
public:
	float x;
	float y; 
	float w;
	float h;
	std::string assetTag;

	Texture(std::string tag, float tx, float ty, float width, float height);

	Texture(std::string tag);

	Texture(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const;
};

#endif // !__TEXTURE_H__
