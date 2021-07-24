#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	srand(time(NULL));

	GraphicsConfig* config = GCC_NEW GraphicsConfig();
	config->setWindowWidth(1024);
	config->setWindowHeight(768);
	config->setWindowX(SDL_WINDOWPOS_CENTERED);
	config->setWindowY(SDL_WINDOWPOS_CENTERED);
	config->setFont("Digital_tech.otf");

	mSystemManager = std::make_shared<SystemManager>(config);

	mEntityFactory = std::make_shared<EntityFactory>(mSystemManager);

	mWidgetFactory = std::make_shared<WidgetFactory>("Assets/BlueButton.json", "Assets/Panel.json", mSystemManager);

	mSoundControllerFactory = std::make_shared<SoundControllerFactory>(mSystemManager);

	mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS)->addFont("Digital_tech.otf", "Digital_tech", 20);

	mTileFactory = std::make_shared<TileFactory>(mSystemManager);

	mLuaScriptFactory = std::make_shared<LuaScriptFactory>(mEntityFactory, mWidgetFactory, mSystemManager);

	mMapFactory = std::make_shared<MapFactory>(mTileFactory, mLuaScriptFactory, mSystemManager);
	
	mMap = mMapFactory->createMap("Assets/HackNSlasher/maps/dungeon_test.json");

	//mEntity = mEntityFactory->createDefault(25, 25, 50, 50, 255, 0, 0, 255);
	//applyScript(mSystemManager, mEntity->id, "Games/test/sweep_test.lua");
}

void RTS::handleEvents() {
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

		InputSystemPtr inputSystem = mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT);
		inputSystem->handleEvent(event);
	}
}

void RTS::update() {
	Uint32 lastTime(SDL_GetTicks());
	Uint32 delta(lastTime - mLastTime);

	mSystemManager->update(delta);

	EventManager::getInstance().update();

	mLastTime = lastTime;
}

void RTS::draw() {
	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	graphicsSystem->draw();
}
void RTS::delay(Uint32 frameTime) {
	if (frameTime < 33) {
		SDL_Delay(Uint32(33) - frameTime);
	}
}

void RTS::teardown() {
	TTF_Quit();
	SDL_Quit();
}