#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include"Graphics.h"
#include"Texture.h"

#include<vector>

enum class AnimationState {
	PLAYING = 0,
	STOPPED,
};

typedef struct {
	std::vector<Texture*>  frames;
	std::string name;
} Animation;

typedef struct {
	std::unordered_map<std::string, Animation*> animations;
	std::string defaultAnimationName;
	int fps;
} AnimationSet;

class AnimationHandler {
public:
	AnimationHandler(TextureDrawable* textureDrawable, AnimationSet* animationSet, int fps) {
		mTextureDrawable = textureDrawable;
		mFps = fps;
		mState = AnimationState::STOPPED;
		mTimePerFrame = (1000 / mFps);

		setAnimationSet(animationSet);
	}

	void setAnimation(std::string animationName) {
		mCurrentAnimationName = animationName;

		Animation* currentAnimation = mAnimationSet->animations[mCurrentAnimationName];
		mTextureDrawable->setTexture(currentAnimation->frames[0]);
	}

	void setAnimationSet(AnimationSet* animationSet) {
		mAnimationSet.reset(animationSet);
		mCurrentAnimationName = mAnimationSet->defaultAnimationName;
		mCurrentFrame = 0;
		mFrameTime = 0;

		Animation* currentAnimation = mAnimationSet->animations[mCurrentAnimationName];
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

		Animation* currentAnimation = mAnimationSet->animations[mCurrentAnimationName];

		if (++mCurrentFrame >= currentAnimation->frames.size()) {
			mCurrentFrame = 0;
		}
		mTextureDrawable->setTexture(currentAnimation->frames[mCurrentFrame]);
		mFrameTime = 0;
	}

private:
	TextureDrawable* mTextureDrawable;
	std::unique_ptr<AnimationSet> mAnimationSet;
	int mFps;
	int mCurrentFrame;
	std::string mCurrentAnimationName;
	AnimationState mState;
	Uint32 mFrameTime;
	Uint32 mTimePerFrame;
};

class AnimationComponent : public Component {
public:
	AnimationHandler* animationHandler;

	AnimationComponent(unsigned long entityId, AnimationHandler* animationHandler) : Component(entityId, ComponentType::ANIMATION_COMPONENT) {
		this->animationHandler = animationHandler;
	}

	void serialize(Serializer& serializer) const override {

	}
};

#endif // !__ANIMATION_H__
