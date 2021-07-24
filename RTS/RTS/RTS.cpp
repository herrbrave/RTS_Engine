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

	/*
	mEntity = mEntityFactory->createPhysicsEntity(512, 348, 20, 20);
	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	graphicsSystem->addTexture("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", "Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png");
	ParticleCloudPtr particleCloud = std::make_shared<ParticleCloud>();

	particleCloud->particleLifeMillis = 5000;
	particleCloud->spreadDist = 348;
	particleCloud->gravity = std::make_shared<Vector2f>(0.0f, -2.7f);

	particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 0, 0, 16, 16));
	particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 16, 0, 16, 16));
	particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 32, 0, 16, 16));
	particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 48, 0, 16, 16));
	particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 64, 0, 16, 16));

	ParticleCloudDrawablePtr particleCloudDrawable = std::make_shared<ParticleCloudDrawable>(particleCloud);
	ParticleSystemPtr particleSystem = makeShared(mSystemManager->getSystemByType<ParticleSystem>(SystemType::PARTICLE));
	particleSystem->registerParticleEmitter(mEntity->id, particleCloudDrawable);

	DrawableComponentPtr drawableComponent = std::make_shared<DrawableComponent>(mEntity->id, particleCloudDrawable);
	graphicsSystem->registerDrawable(mEntity->id, particleCloudDrawable);
	mEntity->addComponent(drawableComponent);
	*/
	
	mMap = mMapFactory->createMap("Assets/HackNSlasher/maps/dungeon_test.json");

	//mEntity = mEntityFactory->createDefault(25, 25, 50, 50, 255, 0, 0, 255);
	//applyScript(mSystemManager, mEntity->id, "Games/test/sweep_test.lua");

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

	mSystemManager->update(delta);

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