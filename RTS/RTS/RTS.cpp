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
	config->setFont("Digital_tech.otf");

	mSystemManager.reset(new SystemManager(config));

	mEntityFactory.reset(new EntityFactory(mSystemManager.get()));

	mWidgetFactory.reset(new WidgetFactory("Assets/Button.json", "Assets/Panel.json", mSystemManager.get()));
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

		handleInput(event, mSystemManager.get());
	}
}

void RTS::update(){
	Uint32 lastTime(SDL_GetTicks());
	updatePhysicsSystem(lastTime - mLastTime, mSystemManager.get());
	mLastTime = lastTime;
}

void RTS::draw(){
	drawGraphicsSystem(mSystemManager.get());
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