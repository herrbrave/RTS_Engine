#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	GraphicsConfig config;
	config.setTitle("RTS");
	config.setWindowWidth(1024);
	config.setWindowHeight(768);

	mGraphicsSystem.reset(new GraphicsSystem(&config));

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });

	mCamera = std::shared_ptr<Camera>(new Camera{ 0, 0, 800, 600 });
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
				mCamera->x -= 10;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				mCamera->x += 10;
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				mCamera->y -= 10;
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				mCamera->y += 10;
			}
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			int x, y;
			SDL_GetMouseState(&x, &y);
		}
	}
}

void RTS::update(){
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