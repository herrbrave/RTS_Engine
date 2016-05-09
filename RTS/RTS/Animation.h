#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include"Graphics.h"
#include"Texture.h"

#include<vector>

enum class AnimationState {
	PLAYING = 0,
	STOPPED = 1,
};

typedef struct {
	std::vector<TexturePtr>  frames;
	std::string name;
} Animation;
typedef shared_ptr<Animation> AnimationPtr;
typedef weak_ptr<Animation> WeakAnimationPtr;

typedef struct {
	std::unordered_map<std::string, AnimationPtr> animations;
	std::string defaultAnimationName;
	int fps;
} AnimationSet;
typedef shared_ptr<AnimationSet> AnimationSetPtr;
typedef weak_ptr<AnimationSet> WeakAnimationSetPtr;

class AnimationHandler;
typedef shared_ptr<AnimationHandler> AnimationHandlerPtr;
typedef weak_ptr<AnimationHandler> WeakAnimationHandlerPtr;

class AnimationComponent;
typedef shared_ptr<AnimationComponent> AnimationComponentPtr;
typedef weak_ptr<AnimationComponent> WeakAnimationComponentPtr;

class AnimationHandler {
public:
	AnimationHandler(shared_ptr<TextureDrawable> textureDrawable, AnimationSetPtr animationSet, int fps) {
		mTextureDrawable = textureDrawable;
		mFps = fps;
		mState = AnimationState::STOPPED;
		mTimePerFrame = (1000 / mFps);

		setAnimationSet(animationSet);
	}

	void setAnimation(std::string animationName) {
		mCurrentAnimationName = animationName;

		AnimationPtr currentAnimation = mAnimationSet->animations[mCurrentAnimationName];
		mTextureDrawable->setTexture(currentAnimation->frames[0]);
	}

	void setAnimationSet(AnimationSetPtr animationSet) {
		mAnimationSet = animationSet;
		mCurrentAnimationName = mAnimationSet->defaultAnimationName;
		mCurrentFrame = 0;
		mFrameTime = 0;

		AnimationPtr currentAnimation = mAnimationSet->animations[mCurrentAnimationName];
		mTextureDrawable->setTexture(currentAnimation->frames[0]);
	}

	void play() {
		mState = AnimationState::PLAYING;
	}

	void stop() {
		mCurrentFrame = 0;
		mState = AnimationState::STOPPED;
	}

	void update(Uint32 delta) {
		if (AnimationState::STOPPED == mState) {
			return;
		}

		mFrameTime += delta;
		if (mFrameTime < mTimePerFrame) {
			return;
		}

		AnimationPtr currentAnimation = mAnimationSet->animations[mCurrentAnimationName];

		if (++mCurrentFrame >= currentAnimation->frames.size()) {
			mCurrentFrame = 0;
		}
		mTextureDrawable->setTexture(currentAnimation->frames[mCurrentFrame]);
		mFrameTime = 0;
	}

private:
	shared_ptr<TextureDrawable> mTextureDrawable;
	AnimationSetPtr mAnimationSet;
	int mFps;
	int mCurrentFrame;
	std::string mCurrentAnimationName;
	AnimationState mState;
	Uint32 mFrameTime;
	Uint32 mTimePerFrame;
};

class AnimationComponent : public Component {
public:
	AnimationHandlerPtr animationHandler;

	AnimationComponent(unsigned long entityId, AnimationHandlerPtr animationHandler) : Component(entityId, ComponentType::ANIMATION_COMPONENT) {
		this->animationHandler = animationHandler;
	}

	void serialize(Serializer& serializer) const override {

	}
};

#endif // !__ANIMATION_H__
