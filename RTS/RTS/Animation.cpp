#include"Animation.h"

AnimationSetPtr loadAsepriteAnimation(const string& path) {
	string image = path.substr(0U, path.find_last_of(".") + 1) + "png";
	AsepriteAnimationPtr asepriteAnimation = parseAnimation(path);

	AnimationSetPtr animationSet = std::make_shared<AnimationSet>();
	animationSet->spritesheet = image;
	animationSet->name = path;
	animationSet->defaultAnimationName = asepriteAnimation->meta->frameTags[0]->name;
	animationSet->fps = 1000 / asepriteAnimation->frames[0]->duration;

	LoadAssetEventData* assetData = GCC_NEW LoadAssetEventData(SDL_GetTicks(), image, image);
	EventManager::getInstance().pushEvent(assetData);

	auto it = asepriteAnimation->meta->frameTags.begin();
	while (it != asepriteAnimation->meta->frameTags.end()) {
		auto val = *it;
		AnimationPtr animation = std::make_shared<Animation>();
		animation->name = val->name;

		for (int index = val->from; index <= val->to; index++) {
			auto frame = asepriteAnimation->frames[index];

			TexturePtr texture = std::make_shared<Texture>(image);
			texture->x = frame->x;
			texture->y = frame->y;
			texture->w = frame->sourceW;
			texture->h = frame->sourceH;

			animation->frames.push_back(texture);
		}

		animationSet->animations[val->name] = animation;

		it++;
	}

	return animationSet;
}