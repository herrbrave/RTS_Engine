#include"Particle.h"

void ParticleCloudDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	Uint32 time = SDL_GetTicks();
	Uint32 diff = time - lastTime;

	if (particles.size() < MAX_PARTICLES) {
		for (int count = 0; count < GENERATED_PER_FRAME; count++) {
			float angle = rand() % 360;
			angle = M_PI / 180.0f * angle;
			ParticlePtr particle = std::make_shared<Particle>();
			particle->lifespan = 0;
			particle->velocity = std::make_shared<Vector2f>(std::sin(angle), std::cos(angle));

			particles.push_back(particle);
		}
	}

	for (ParticlePtr particle : particles) {
		particle->lifespan += diff;
		if (particle->lifespan > particleCloud->particleLifeMillis) {
			continue;
		}


		float lifespan = (float) particle->lifespan / (float) particleCloud->particleLifeMillis;
		float dist = lifespan * (float) particleCloud->spreadDist;
		Vector2f velocity = particle->velocity;
		Vector2f scaledVelocity = velocity * dist;
		Vector2f pos = position + scaledVelocity;

		particle->frameTime += diff;
		if (particle->frameTime > particle->timePerFrame) {
			particle->frame += 1;
			particle->frame %= particleCloud->particleTextures.size();
			particle->frameTime = 0;
		}
		
		float fade = 1.0f - (particleCloud->fade * lifespan);
		graphicsRef.renderTexture(particleCloud->particleTextures[particle->frame], pos.x, pos.y, 20, 20, 0, 255, 255, 0, fade * 255);
	}

	while (!particles.empty() && particles.front()->lifespan > particleCloud->particleLifeMillis) {
		particles.remove(particles.front());
	}

	lastTime = time;
}