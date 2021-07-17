#pragma once

#include"Graphics.h"

#include<list>

using std::list;

const static int MAX_PARTICLES = 1000;
const static int GENERATED_PER_FRAME = 2;

class Particle;
typedef shared_ptr<Particle> ParticlePtr;
typedef weak_ptr<Particle> WeakParticlePtr;
typedef list<ParticlePtr> Particles;

class ParticleCloud;
typedef shared_ptr<ParticleCloud> ParticleCloudPtr;
typedef weak_ptr<ParticleCloud> WeakParticleCloudPtr;

class ParticleCloudDrawable;
typedef shared_ptr<ParticleCloudDrawable> ParticleCloudDrawablePtr;
typedef weak_ptr<ParticleCloudDrawable> WeakParticleCloudDrawablePtr;

class Particle {
public:
	Vector2fPtr velocity;
	Uint32 lifespan;
	int frame = 0;
	int frameTime = 0;
	int timePerFrame = 100;
};

class ParticleCloud {
public:
	Uint32 particleLifeMillis;
	float spreadDist;
	vector<TexturePtr> particleTextures;
	float fade = 0.2f;
};

class ParticleCloudDrawable : public Drawable {
public:
	ParticleCloudPtr particleCloud;
	Particles particles;
	Uint32 lastTime;

	ParticleCloudDrawable(ParticleCloudPtr particleCloud) : Drawable(0, 0), particleCloud(particleCloud) {
		this->lastTime = SDL_GetTicks();
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;
protected:
	void onSerialize(Serializer& serializer) const override {

	}
};