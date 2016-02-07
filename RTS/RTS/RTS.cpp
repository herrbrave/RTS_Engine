#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	GraphicsConfig config;
	config.setTitle("RTS");
	config.setWindowWidth(1024);
	config.setWindowHeight(768);

	mPhysicsSystem.reset(new PhysicsSystem());
	mGraphicsSystem.reset(new GraphicsSystem(&config, mPhysicsSystem.get()));

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });

	mCamera = std::shared_ptr<Camera>(new Camera{ 0, 0, 800, 600 });

	mUnitFactory.reset(new UnitFactory(mGraphicsSystem.get(), mPhysicsSystem.get()));
	mUnit.reset(mUnitFactory->create());
	mUnit->getBody()->setSpeed(10);
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
				mUnit->getBody()->setVelocity(&vector2f{ -1, 0 });
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				mUnit->getBody()->setVelocity(&vector2f{ 1, 0 });
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, -1 });
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, 1 });
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_LEFT) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, 0 });
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, 0 });
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, 0 });
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				mUnit->getBody()->setVelocity(&vector2f{ 0, 0 });
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