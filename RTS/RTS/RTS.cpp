#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	GraphicsConfig config;
	config.setWindowWidth(1024);
	config.setWindowHeight(768);

	mPhysicsSystem.reset(new PhysicsSystem());
	mGraphicsSystem.reset(new GraphicsSystem(&config, mPhysicsSystem.get()));

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });

	mUnitFactory.reset(new UnitFactory(mGraphicsSystem.get(), mPhysicsSystem.get()));
	mUnit.reset(mUnitFactory->create());
	mUnit->getBody()->setSpeed(100);

	mGraphicsSystem->getCamera()->position->x = -100;
	mGraphicsSystem->getCamera()->position->y = -100;
}

void RTS::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			mIsPlaying = false;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_LEFT) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.x = -1;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.x = 1;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.y = -1;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.y = 1;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE) {
				mIsPlaying = false;
			}
		}
		else if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_LEFT) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.x = 0;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.x = 0;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.y = 0;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				vector2f velocity(*mUnit->getBody()->getVelocity());
				velocity.y = 0;
				velocity.normalize();
				mUnit->getBody()->setVelocity(&velocity);
			}
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			int x, y;
			SDL_GetMouseState(&x, &y);
		}
	}
}

void RTS::update(){
	Uint32 lastTime(SDL_GetTicks());
	mPhysicsSystem->update(lastTime - mLastTime);
	mLastTime = lastTime;
}

void RTS::draw(){
	mGraphicsSystem->draw();
}
void RTS::delay(Uint32 frameTime){
	if (frameTime < 33) {
		SDL_Delay(Uint32(33) - frameTime);
	}
}

void RTS::teardown(){
	TTF_Quit();
	SDL_Quit();
}