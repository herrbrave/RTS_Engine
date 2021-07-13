#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	GraphicsConfig* config = GCC_NEW GraphicsConfig();
	config->setWindowWidth(1024);
	config->setWindowHeight(768);
	config->setWindowX(SDL_WINDOWPOS_CENTERED);
	config->setWindowY(SDL_WINDOWPOS_CENTERED);
	config->setFont("Digital_tech.otf");
	
	auto start = std::chrono::high_resolution_clock::now();

	mSystemManager.reset(GCC_NEW SystemManager(config));

	auto end = std::chrono::high_resolution_clock::now();
	auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "System Manager Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	mEntityFactory.reset(GCC_NEW EntityFactory(mSystemManager));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Entity Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	mWidgetFactory.reset(GCC_NEW WidgetFactory("Assets/BlueButton.json", "Assets/Panel.json", mSystemManager));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Widget Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	mSoundControllerFactory.reset(GCC_NEW SoundControllerFactory(mSystemManager));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Sound Controller Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS))->addFont("Digital_tech.otf", "Digital_tech", 20);


	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Font Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	mTileFactory.reset(GCC_NEW TileFactory(mSystemManager));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Tile Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();

	mLuaScriptFactory.reset(GCC_NEW LuaScriptFactory(WeakEntityFactoryPtr(mEntityFactory), WeakWidgetFactoryPtr(mWidgetFactory), WeakSystemManagerPtr(mSystemManager)));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Lua Script Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	mMapFactory.reset(GCC_NEW MapFactory(mTileFactory, mLuaScriptFactory, mSystemManager));

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Map Factory Setup: " << res.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();

	//mEntity = mEntityFactory->createFromSerialization("test_serialization.json");

	mMap = mMapFactory->createMap("Assets/orksvhumans/test.json");

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Load Map: " << res.count() << std::endl;
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

		InputSystemPtr inputSystem(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT));
		inputSystem->handleEvent(event);
	}
}

void RTS::update() {
	Uint32 lastTime(SDL_GetTicks());
	Uint32 delta(lastTime - mLastTime);

	LuaScriptSystemPtr luaScriptSystem = makeShared<LuaScriptSystem>(mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
	luaScriptSystem->update(delta);

	PhysicsSystemPtr physicsSystem(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	physicsSystem->update(delta);

	AnimationSystemPtr animationSystem(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	animationSystem->update(delta);

	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	entitySystem->update(delta);

	EventManager::getInstance().update();

	mLastTime = lastTime;
}

void RTS::draw() {
	GraphicsSystemPtr graphicsSystem(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
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