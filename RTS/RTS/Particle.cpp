#include"Particle.h"

void ParticleCloudDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	for (ParticlePtr particle : particles) {
		Vector2f pos = *particle->position + position;
		graphicsRef.renderTexture(particleCloud->particleTextures[particle->frame], pos.x, pos.y, 20, 20, 0, 255, 255, 0, particle->fade * 255);
	}
}

void ParticleCloudDrawable::update(Uint32 delta) {
	if (particles.size() < MAX_PARTICLES) {
		for (int count = 0; count < GENERATED_PER_FRAME; count++) {
			float angle = rand() % 360;
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