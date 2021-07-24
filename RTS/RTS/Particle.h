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

class ParticleEmitter;
typedef shared_ptr<ParticleEmitter> ParticleEmitterPtr;
typedef weak_ptr<ParticleEmitter> WeakParticleEmitterPtr;

class ParticleCloudDrawable;
typedef shared_ptr<ParticleCloudDrawable> ParticleCloudDrawablePtr;
typedef weak_ptr<ParticleCloudDrawable> WeakParticleCloudDrawablePtr;

class Particle {
public:
	Vector2fPtr velocity;
	Vector2fPtr position;
	Uint32 lifespan;
	int frame = 0;
	int frameTime = 0;
	int timePerFrame = 100;
	float fade = 1.0;
};

class ParticleCloud {
public:
	Uint32 particleLifeMillis;
	float spreadDist;
	float speed = 45.0f;
	vector<TexturePtr> particleTextures;
	float fade = 0.9f;
	Vector2fPtr gravity{ nullptr };
	float startAngle = 90.0f;
	float endAngle = 270.0f;
};

class ParticleEmitter {
public:
	virtual void update(Uint32 delta) = 0;
};

class ParticleCloudDrawable : public Drawable, public ParticleEmitter {
public:
	ParticleCloudPtr particleCloud;
	Particles particles;

	ParticleCloudDrawable(ParticleCloudPtr particleCloud) : Drawable(0, 0), particleCloud(particleCloud) {

	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void update(Uint32 delta) override;

	DrawableType getType() override;

protected:
	void onSerialize(Serializer& serializer) const override {

	}
};

class ParticleCloudComponent : public Component {
public:
	ParticleCloudDrawablePtr particleCloudDrawable;

	ParticleCloudComponent(unsigned long entityId, ParticleCloudDrawablePtr particleCloud) : Component(entityId, ComponentType::PARTICLE_COMPONENT), particleCloudDrawable(particleCloud) {

	}

	void setParticleLifeMillis(Uint32 time);
	void setParticleMaxRadius(float radius);
	void setSpeed(float pps);
	void setFade(float fade);
	void setGravity(float x, float y);
	void setSpawnAngleRange(float start, float end);
};
