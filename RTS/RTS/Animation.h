#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include"AsepriteAnimation.h"
#include"Graphics.h"
#include"Texture.h"

#include<vector>

enum class AnimationState {
	PLAYING = 0,
	STOPPED = 1,
	LOOPING = 2,
};

class Animation;
typedef shared_ptr<Animation> AnimationPtr;
typedef weak_ptr<Animation> WeakAnimationPtr;

class AnimationSet;
typedef shared_ptr<AnimationSet> AnimationSetPtr;
typedef weak_ptr<AnimationSet> WeakAnimationSetPtr;

class AnimationHandler;
typedef shared_ptr<AnimationHandler> AnimationHandlerPtr;
typedef weak_ptr<AnimationHandler> WeakAnimationHandlerPtr;

class AnimationDrawable;
typedef shared_ptr<AnimationDrawable> AnimationDrawablePtr;
typedef weak_ptr<AnimationDrawable> WeakAnimationDrawablePtr;

class AnimationComponent;
typedef shared_ptr<AnimationComponent> AnimationComponentPtr;
typedef weak_ptr<AnimationComponent> WeakAnimationComponentPtr;

class Animation : public Serializable {
public:
	std::vector<TexturePtr>  frames;
	std::string name;

	Animation() {}

	Animation(const Animation& animation) : name(animation.name), frames(animation.frames) {}

	Animation(const rapidjson::Value& root) {
		this->name = root["name"].GetString();

		for (rapidjson::SizeType index = 0; index < root["frames"].Size(); index++) {
			const rapidjson::Value& frame = root["frames"][index];
			this->frames.push_back(TexturePtr(GCC_NEW Texture(frame)));
		}
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("name");
		serializer.writer.String(this->name.c_str());

		serializer.writer.String("frames");
		serializer.writer.StartArray();

		for (auto texture : frames) {
			texture->serialize(serializer);
		}

		serializer.writer.EndArray();

		serializer.writer.EndObject();
	}
};

class AnimationSet : public Serializable {
public:
	std::unordered_map<std::string, AnimationPtr> animations;
	std::string spritesheet;
	std::string name;
	std::string defaultAnimationName;
	int fps;

	AnimationSet() {}

	AnimationSet(const AnimationSet& animationSet) : animations(animationSet.animations), spritesheet(animationSet.spritesheet), name(animationSet.name), defaultAnimationName(animationSet.defaultAnimationName), fps(animationSet.fps) {}

	AnimationSet(const rapidjson::Value& root) {
		this->spritesheet = root["spritesheet"].GetString();
		this->name = root["name"].GetString();

		LoadAssetEventData* eventData = GCC_NEW LoadAssetEventData(SDL_GetTicks(), this->spritesheet, this->name);
		EventManager::getInstance().pushEvent(eventData);

		this->defaultAnimationName = root["defaultAnimationName"].GetString();
		this->fps = root["fps"].GetInt();

		auto it = root["animations"].Begin();

		while (it != root["animations"].End()) {
			const rapidjson::Value& animation = *it;
			this->animations[animation["name"].GetString()] = AnimationPtr(GCC_NEW Animation(animation));
			it++;
		}
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("spritesheet");
		serializer.writer.String(this->spritesheet.c_str());

		serializer.writer.String("name");
		serializer.writer.String(this->name.c_str());

		serializer.writer.String("defaultAnimationName");
		serializer.writer.String(this->defaultAnimationName.c_str());

		serializer.writer.String("fps");
		serializer.writer.Int(this->fps);

		serializer.writer.String("animations");
		serializer.writer.StartObject();

		for (auto entry : this->animations) {
			serializer.writer.String(entry.second->name.c_str());
			entry.second->serialize(serializer);
		}

		serializer.writer.EndObject();

		serializer.writer.EndObject();
	}
};

class AnimationHandler : public Serializable {
public:
	AnimationHandler(AnimationSetPtr animationSet, int fps) {
		this->fps = fps;
		this->state = AnimationState::STOPPED;
		this->timePerFrame = (1000 / fps);

		this->setAnimationSet(animationSet);
	}

	AnimationHandler(const rapidjson::Value& root) {
		const rapidjson::Value& animationSet = root["animationSet"];
		this->animationSet = AnimationSetPtr(GCC_NEW AnimationSet(animationSet));
		this->fps = root["fps"].GetInt();

		this->setAnimation(root["currentAnimtionName"].GetString());

		this->timePerFrame = (1000 / this->fps);

		this->state = AnimationState::STOPPED;
	}

	void setAnimation(std::string animationName) {
		this->currentAnimtionName = animationName;
	}

	void setAnimationSet(AnimationSetPtr animationSet) {
		this->currentFrame = 0;
		this->animationSet = animationSet;
		this->currentAnimtionName = this->animationSet->defaultAnimationName;
		this->frameTime = 0;
	}

	void play() {
		this->state = AnimationState::PLAYING;
	}

	void loop() {
		this->state = AnimationState::LOOPING;
	}

	void stop() {
		this->currentFrame = 0;
		this->state = AnimationState::STOPPED;
	}

	void update(Uint32 delta) {
		if (AnimationState::STOPPED == state) {
			return;
		}

		this->frameTime += delta;
		if (this->frameTime < this->timePerFrame) {
			return;
		}

		AnimationPtr currentAnimation = animationSet->animations[currentAnimtionName];

		if (++this->currentFrame >= currentAnimation->frames.size()) {
			this->currentFrame = 0;
			if (this->state != AnimationState::LOOPING) {
				stop();
			}
		}

		this->frameTime = 0;
	}

	TexturePtr getCurrentFrame() {
		AnimationPtr currentAnimation = animationSet->animations[currentAnimtionName];
		TexturePtr texture = currentAnimation->frames[this->currentFrame];
		return texture;
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("animationSet");
		this->animationSet->serialize(serializer);

		serializer.writer.String("fps");
		serializer.writer.Int(this->fps);

		serializer.writer.String("currentAnimtionName");
		serializer.writer.String(this->currentAnimtionName.c_str());;

		serializer.writer.EndObject();
	}

	AnimationSetPtr animationSet;
	int fps;
	int currentFrame;
	std::string currentAnimtionName;
	AnimationState state;
	Uint32 frameTime;
	Uint32 timePerFrame;
};

class AnimationDrawable : public Drawable {
public:
	AnimationHandlerPtr animationHandler;

	AnimationDrawable(AnimationHandlerPtr animationHandler) : Drawable(-1, -1) {
		this->animationHandler = animationHandler;
	}

	AnimationDrawable(float width, float height, AnimationHandlerPtr animationHandler) : Drawable(width, height) {
		this->animationHandler = animationHandler;
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override {
		if (animationHandler->state == AnimationState::STOPPED) {
			return;
		}

		graphicsRef.renderTexture(this->animationHandler->getCurrentFrame(), position.x, position.y, width, height, angle, mColor->r, mColor->g, mColor->b, mColor->a);
	}

	void onSerialize(Serializer& serializer) const override {

	}
};

class AnimationComponent : public Component {
public:
	AnimationDrawablePtr aninmationDrawable;

	AnimationComponent(unsigned long entityId, AnimationDrawablePtr aninmationDrawable) : Component(entityId, ComponentType::ANIMATION_COMPONENT) {
		this->aninmationDrawable = aninmationDrawable;
	}

	AnimationComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::ANIMATION_COMPONENT) {
		//this->animationHandler = AnimationHandlerPtr(new AnimationHandler(root["animationHandler"]));
	}

	float getAngle() {
		return this->aninmationDrawable->getAngle();
	}

	void setAngle(float angle) {
		this->aninmationDrawable->setAngle(angle);
	}

	Uint8 getZOrder() {
		return aninmationDrawable->getDrawDepth();
	}

	void setZOrder(Uint8 zOrder) {
		aninmationDrawable->setDrawDepth(zOrder);

		EntityZOrderSetEventData* eventData = GCC_NEW EntityZOrderSetEventData(entityId, SDL_GetTicks());
		EventManager::getInstance().pushEvent(eventData);
	}

	void setSize(float width, float height) {
		this->aninmationDrawable->setSize(width, height);
	}

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		this->aninmationDrawable->setColor(r, g, b, a);
	}

	void setAnimationSet(AnimationSetPtr animationSet) {
		this->aninmationDrawable->animationHandler->setAnimationSet(animationSet);
	}

	void setAnimation(const string& name) {
		this->aninmationDrawable->animationHandler->setAnimation(name);
	}

	void play() {
		this->aninmationDrawable->animationHandler->play();
	}

	void loop() {
		this->aninmationDrawable->animationHandler->loop();
	}

	void stop() {
		this->aninmationDrawable->animationHandler->stop();
	}

	AnimationState getAnimationState() {
		return this->aninmationDrawable->animationHandler->state;
	}

	void serialize(Serializer& serializer) const override {

		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint((Uint8) this->componentId);

		serializer.writer.String("aninmationDrawable");
		this->aninmationDrawable->serialize(serializer);

		serializer.writer.EndObject();
	}
};

AnimationSetPtr loadAsepriteAnimation(const string& path);

#endif // !__ANIMATION_H__
