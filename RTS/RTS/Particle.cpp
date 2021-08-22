#include"Particle.h"

void ParticleCloudDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	if (!this->playing) {
		return;
	}

	for (ParticlePtr particle : particles) {
		Vector2f pos = *particle->position + position;
		graphicsRef.renderTexture(particleCloud->particleTextures[particle->frame], pos.x, pos.y, this->width, this->height, 0, 255, 255, 0, particle->fade * 255);
	}
}

void ParticleCloudDrawable::update(Uint32 delta) {
	if (!this->playing) {
		return;
	}

	if (particles.size() < MAX_PARTICLES) {
		for (int count = 0; count < GENERATED_PER_FRAME; count++) {
			int range = (int) round(particleCloud->endAngle - particleCloud->startAngle);
			float angle = rand() % range + particleCloud->startAngle;
			angle = M_PI / 180.0f * angle;
			ParticlePtr particle = std::make_shared<Particle>();
			particle->lifespan = 0;
			particle->velocity = std::make_shared<Vector2f>(std::sin(angle), std::cos(angle));
			particle->position = std::make_shared<Vector2f>(0, 0);

			particles.push_back(particle);
		}
	}

	for (ParticlePtr particle : particles) {
		particle->lifespan += delta;
		if (particle->lifespan > particleCloud->particleLifeMillis) {
			continue;
		}


		float lifespan = (float)particle->lifespan / (float)particleCloud->particleLifeMillis;
		Vector2f velocity = particle->velocity;
		Vector2f scaledVelocity = velocity * (delta / 1000.0f) * particleCloud->speed;
		*particle->position += scaledVelocity;

		if (particleCloud->gravity != nullptr) {
			velocity *= particleCloud->speed;
			velocity += *particleCloud->gravity;
			velocity.normalize();
			particle->velocity->set(velocity);
		}

		particle->frameTime += delta;
		if (particle->frameTime > particle->timePerFrame) {
			particle->frame += 1;
			particle->frame %= particleCloud->particleTextures.size();
			particle->frameTime = 0;
		}

		particle->fade = 1.0f - (particleCloud->fade * lifespan);
	}

	while (!particles.empty() && particles.front()->lifespan > particleCloud->particleLifeMillis) {
		particles.remove(particles.front());
	}
}

DrawableType  ParticleCloudDrawable::getType() {
	return DrawableType::PARTICLE;
}

void ParticleCloudDrawable::play() {
	this->playing = true;
}

void ParticleCloudDrawable::stop() {
	this->playing = false;
	this->particles.clear();
}

void ParticleCloudComponent::setParticleLifeMillis(Uint32 time) {
	this->particleCloudDrawable->particleCloud->particleLifeMillis = time;
}

void  ParticleCloudComponent::setParticleMaxRadius(float radius) {
	this->particleCloudDrawable->particleCloud->spreadDist = radius;
}

void  ParticleCloudComponent::setSpeed(float pps) {
	this->particleCloudDrawable->particleCloud->speed = pps;
}

void  ParticleCloudComponent::setFade(float fade) {
	this->particleCloudDrawable->particleCloud->fade = fade;
}

void  ParticleCloudComponent::setGravity(float x, float y) {
	this->particleCloudDrawable->particleCloud->gravity->set(x, y);
}

void  ParticleCloudComponent::setSpawnAngleRange(float start, float end) {
	this->particleCloudDrawable->particleCloud->startAngle = start;
	this->particleCloudDrawable->particleCloud->endAngle = end;
}

void ParticleCloudComponent::play() {
	this->particleCloudDrawable->play();
}

void ParticleCloudComponent::stop() {
	this->particleCloudDrawable->stop();
}