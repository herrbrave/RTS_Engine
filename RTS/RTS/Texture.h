#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include"Asset.h"
#include"Component.h"
#include"Serializer.h"

#include<string>

class Texture {
public:
	float x;
	float y; 
	float w;
	float h;
	std::string assetTag;

	Texture(std::string tag, float tx, float ty, float width, float height);

	void serialize(Serializer& serializer) const;
};

static const unsigned long TEXTURE_COMPONENT_TYPE = sComponentId++;

class TextureComponent : public Component {
public:
	TextureComponent(unsigned long entityId, Texture* texture) : Component(entityId, TEXTURE_COMPONENT_TYPE)  {
		mTexture = texture;
	}

	Texture* getTexture();

	void serialize(Serializer& serializer) const override;

private:
	Texture* mTexture{ nullptr };
};

#endif // !__TEXTURE_H__
