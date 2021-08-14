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

	mSystemManager = std::make_shared<SystemManager>(config);

	mEntityFactory = std::make_shared<EntityFactory>(mSystemManager);

	mWidgetFactory = std::make_shared<WidgetFactory>("Assets/ui/ui.json", mSystemManager);

	mSoundControllerFactory = std::make_shared<SoundControllerFactory>(mSystemManager);

	mLuaScriptFactory = std::make_shared<LuaScriptFactory>(mEntityFactory, mWidgetFactory, mSystemManager);

	mMapFactory = std::make_shared<MapFactory>(mEntityFactory, mLuaScriptFactory, mSystemManager);

	worldFactory = std::make_shared<WorldFactory>(mSystemManager, mEntityFactory);

	// world = worldFactory->createWorldFromTMXMap("Assets/test/grid_test.json");
	//Serializer serializer;
	//world->serialize(serializer);
	//serializer.writeTo("world.json");b

	//mMap = mMapFactory->createTMXMap("Assets/test/grid_test.json");

	//mEntity = mEntityFactory->createDefault(25, 25, 50, 50, 0, 0, 0, 255);
	//applyScript(mSystemManager, mEntity->id, "Games/test/inventory_test.lua");

	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	graphicsSystem->addTexture("Assets/HackNSlasher/Items/Weapons.png", "Assets/HackNSlasher/Items/Weapons.png");
	graphicsSystem->addTexture("Assets/ui/ui_sheet.png", "Assets/ui/ui_sheet.png");
	TexturePtr texture = std::make_shared<Texture>("Assets/HackNSlasher/Items/Weapons.png", 32, 0, 16, 16);

	mWidgetFactory->createTextBox(512, 348, 700, 25, "", 14);

	/*
	graphicsSystem->addTexture("test_panel", 512, 512);
	graphicsSystem->addTexture("Assets/HackNSlasher/Items/Weapons.png", "Assets/HackNSlasher/Items/Weapons.png");
	graphicsSystem->addTexture("Assets/HackNSlasher/Menu/Menu.png", "Assets/HackNSlasher/Menu/Menu.png");
	ItemPanelDrawablePtr panel = std::make_shared<ItemPanelDrawable>("test_panel", 512.0f, 512.0f);
	EntityPtr p = mEntityFactory->createPhysicsEntity(512, 348, 512, 512, false);

	ItemPanelComponentPtr drawableComponent = std::make_shared<ItemPanelComponent>(p->id, panel);
	p->addComponent(drawableComponent);
	graphicsSystem->registerDrawable(p->id, panel);

	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			ItemPtr item = std::make_shared<Item>();
			item->texture.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Menu/Menu.png", 64, 84, 28, 28));
			item->texture.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Items/Weapons.png", x * 16, y * 16, 16, 16));
			item->name = std::to_string(x) + ", " + std::to_string(y);
			drawableComponent->addItem(item);
		}
	}
	*/

	graphicsSystem->initialize();
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

	mLuaScriptFactory->clean();

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