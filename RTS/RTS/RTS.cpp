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

	mSystemManager.reset(new SystemManager(config));

	mEntityFactory.reset(new EntityFactory(mSystemManager.get()));

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });

	mWidgetFactory.reset(new WidgetFactory("Assets/Button.json", mSystemManager.get()));
	std::function<void()> func([=](){ mEntities.push_back(mEntityFactory->createDefault()); });
	mButton.reset(mWidgetFactory->createButton(func, 500, 500, 100, 40));
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
			MouseEvent evt;
			evt.action = MouseAction::CLICK_UP;
			evt.button = ((event.button.button == SDL_BUTTON_LEFT) ? MouseButton::LEFT : MouseButton::RIGHT);
			evt.position->x = x;
			evt.position->y = y;

			ButtonComponent* comp = reinterpret_cast<ButtonComponent*>(mButton->componentContainer->getComponentByType(ComponentType::BUTTON_COMPONENT));
			comp->onMouseEvent(evt, mSystemManager.get());
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			MouseEvent evt;
			evt.action = MouseAction::CLICK_DOWN;
			evt.button = ((event.button.button == SDL_BUTTON_LEFT) ? MouseButton::LEFT : MouseButton::RIGHT);
			evt.position->x = x;
			evt.position->y = y;

			ButtonComponent* comp = reinterpret_cast<ButtonComponent*>(mButton->componentContainer->getComponentByType(ComponentType::BUTTON_COMPONENT));
			comp->onMouseEvent(evt, mSystemManager.get());
		}
		else if (event.type == SDL_MOUSEMOTION) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			MouseEvent evt;
			evt.action = MouseAction::MOVE;
			evt.button = ((event.button.button == SDL_BUTTON_LEFT) ? MouseButton::LEFT : MouseButton::RIGHT);
			evt.position->x = x;
			evt.position->y = y;

			ButtonComponent* comp = reinterpret_cast<ButtonComponent*>(mButton->componentContainer->getComponentByType(ComponentType::BUTTON_COMPONENT));
			comp->onMouseEvent(evt, mSystemManager.get());
		}
	}
}

void RTS::update(){
	Uint32 lastTime(SDL_GetTicks());
	mSystemManager->physicsSystem->update(lastTime - mLastTime);
	mLastTime = lastTime;
}

void RTS::draw(){
	mSystemManager->graphicsSystem->draw();
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