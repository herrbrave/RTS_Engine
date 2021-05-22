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

	mSystemManager.reset(GCC_NEW SystemManager(config));
	mEntityFactory.reset(GCC_NEW EntityFactory(mSystemManager));
	mWidgetFactory.reset(GCC_NEW WidgetFactory("Assets/BlueButton.json", "Assets/Panel.json", mSystemManager));
	mSoundControllerFactory.reset(GCC_NEW SoundControllerFactory(mSystemManager));

	makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS))->addFont("Digital_tech.otf", "Digital_tech", 20);

	mTileFactory.reset(GCC_NEW TileFactory(mSystemManager));

	mLuaScriptFactory.reset(GCC_NEW LuaScriptFactory(WeakEntityFactoryPtr(mEntityFactory), WeakWidgetFactoryPtr(mWidgetFactory), WeakSystemManagerPtr(mSystemManager)));
	mMapFactory.reset(GCC_NEW MapFactory(mTileFactory, mLuaScriptFactory, mSystemManager));

	EntityBuilder entityBuilder(mSystemManager, mLuaScriptFactory);

	mEntity = entityBuilder.withPhysics(-1, -1, 1, 1, false).withScript("Games/test/collision_test.lua").build();
	//mEntity = mEntityFactory->createFromSerialization("test_serialization.json");

	//mMap = mMapFactory->createMap("Assets/rts/rts_test_map.json");
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