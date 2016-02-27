#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	GraphicsConfig* config = new GraphicsConfig();
	config->setWindowWidth(1024);
	config->setWindowHeight(768);
	config->setWindowX(SDL_WINDOWPOS_CENTERED);
	config->setWindowY(SDL_WINDOWPOS_CENTERED);

	mAssetSystem.reset(new AssetSystem());
	mEntitySystem.reset(new EntitySystem());
	mPhysicsSystem.reset(new PhysicsSystem());
	mGraphicsSystem.reset(new GraphicsSystem(mAssetSystem.get(), config, mPhysicsSystem.get()));

	mEntityFactory.reset(new EntityFactory(mEntitySystem.get(), mGraphicsSystem.get(), mPhysicsSystem.get()));
	mTileFactory.reset(new TileFactory(mEntitySystem.get(), mGraphicsSystem.get(), mPhysicsSystem.get()));

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });
}

void RTS::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			mIsPlaying = false;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				mIsPlaying = false;
			}
		}
		else if (event.type == SDL_KEYUP) {

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